/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Marcus Tomlinson <marcus.tomlinson@canonical.com>
 */

#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/internal/MWRegistry.h>
#include <unity/scopes/internal/RegistryConfig.h>
#include <unity/scopes/internal/RegistryException.h>
#include <unity/scopes/internal/RuntimeImpl.h>
#include <unity/scopes/internal/smartscopes/SSRegistryObject.h>
#include <unity/scopes/internal/smartscopes/SSScopeObject.h>
#include <unity/scopes/ScopeExceptions.h>
#include <unity/scopes/SearchListener.h>

#include "../RaiiServer.h"

#include <memory>
#include <gtest/gtest.h>
#include <scope-api-testconfig.h>

using namespace testing;
using namespace unity::scopes;
using namespace unity::scopes::internal;
using namespace unity::scopes::internal::smartscopes;
using namespace unity::test::scopes::internal::smartscopes;

///! TODO: more tests

namespace
{

class smartscopesproxytest : public Test
{
public:
    smartscopesproxytest()
        : server_(FAKE_SSS_PATH)
        , reg_id_("SSRegistry")
        , scope_id_("SmartScope")
    {
        // Instantiate SS registry and scopes runtimes
        reg_rt_ = RuntimeImpl::create(reg_id_, SS_RUNTIME_PATH);
        scope_rt_ = RuntimeImpl::create(scope_id_, SS_RUNTIME_PATH);

        // Get registry config
        RegistryConfig reg_conf(reg_id_, reg_rt_->registry_configfile());
        std::string mw_kind = reg_conf.mw_kind();
        std::string mw_configfile = reg_conf.mw_configfile();

        // Get middleware handles from runtimes
        reg_mw_ = reg_rt_->factory()->find(reg_id_, mw_kind);
        scope_mw_ = scope_rt_->factory()->create(scope_id_, mw_kind, mw_configfile);

        // Instantiate a SS registry and scope objects
        reg_ = SSRegistryObject::SPtr(new SSRegistryObject(reg_mw_, scope_mw_->get_scope_endpoint(), 2, 2000, 60,
                                                           "http://127.0.0.1", server_.port_));
        scope_ = SSScopeObject::UPtr(new SSScopeObject(scope_id_, scope_mw_, reg_));

        // Add objects to the middlewares
        reg_mw_->add_registry_object(reg_rt_->registry_identity(), reg_);
        scope_mw_->add_dflt_scope_object(std::move(scope_));
    }

    ~smartscopesproxytest()
    {
        scope_mw_->stop();
        scope_mw_->wait_for_shutdown();

        reg_mw_->stop();
        reg_mw_->wait_for_shutdown();
    }

protected:
    RaiiServer server_;

    std::string reg_id_ = reg_id_;
    std::string scope_id_ = scope_id_;

    RuntimeImpl::UPtr reg_rt_;
    RuntimeImpl::UPtr scope_rt_;

    MiddlewareBase::SPtr reg_mw_;
    MiddlewareBase::SPtr scope_mw_;

    SSRegistryObject::SPtr reg_;
    SSScopeObject::UPtr scope_;
};

TEST_F(smartscopesproxytest, ss_registry)
{
    // locate should throw (direct)
    EXPECT_THROW(reg_->locate("Dummy Demo Scope"), RegistryException);

    // list scopes (direct)
    MetadataMap scopes = reg_->list();
    EXPECT_EQ(scopes.size(), 1);

    // visible scope (direct)
    ScopeMetadata meta = reg_->get_metadata("Dummy Demo Scope");
    EXPECT_EQ("Dummy Demo Scope", meta.scope_name());
    EXPECT_EQ("Dummy Demo Scope", meta.display_name());
    EXPECT_EQ("Dummy demo scope.", meta.description());

    // invisible scope (direct)
    EXPECT_THROW(reg_->get_metadata("Dummy Demo Scope 2"), NotFoundException);

    // locate should throw (via mw)
    MWRegistryProxy mw_reg = reg_mw_->create_registry_proxy(reg_id_, reg_mw_->get_scope_endpoint());
    EXPECT_THROW(mw_reg->locate("Dummy Demo Scope"), RegistryException);

    // list scopes (via mw)
    scopes = mw_reg->list();
    EXPECT_EQ(scopes.size(), 1);

    // visible scope (via mw)
    meta = mw_reg->get_metadata("Dummy Demo Scope");
    EXPECT_EQ("Dummy Demo Scope", meta.scope_name());
    EXPECT_EQ("Dummy Demo Scope", meta.display_name());
    EXPECT_EQ("Dummy demo scope.", meta.description());

    // invisible scope (via mw)
    EXPECT_THROW(mw_reg->get_metadata("Dummy Demo Scope 2"), NotFoundException);
}

class Receiver : public SearchListener
{
public:
    virtual void push(CategorisedResult result) override
    {
        if (count_ == 0)
        {
            EXPECT_EQ("URI", result.uri());
            EXPECT_EQ("Stuff", result.title());
            EXPECT_EQ("https://productsearch.ubuntu.com/imgs/amazon.png", result.art());
            EXPECT_EQ("", result.dnd_uri());
            EXPECT_EQ("cat1", result.category()->id());
            EXPECT_EQ("Category 1", result.category()->title());
            EXPECT_EQ("", result.category()->icon());
            EXPECT_EQ("", result.category()->renderer_template().data());
        }
        else if (count_ == 1)
        {
            EXPECT_EQ("URI2", result.uri());
            EXPECT_EQ("Things", result.title());
            EXPECT_EQ("https://productsearch.ubuntu.com/imgs/google.png", result.art());
            EXPECT_EQ("", result.dnd_uri());
            EXPECT_EQ("cat1", result.category()->id());
            EXPECT_EQ("Category 1", result.category()->title());
            EXPECT_EQ("", result.category()->icon());
            EXPECT_EQ("", result.category()->renderer_template().data());
        }

        count_++;
    }

    virtual void finished(ListenerBase::Reason reason, std::string const& error_message) override
    {
        EXPECT_EQ(Finished, reason);
        EXPECT_EQ("", error_message);
        EXPECT_EQ(2, count_);

        // signal wait_until_finished
        std::unique_lock<std::mutex> lock(mutex_);
        query_complete_ = true;
        cond_.notify_one();
    }

    void wait_until_finished()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return this->query_complete_; });
    }

private:
    int count_ = 0;
    bool query_complete_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

TEST_F(smartscopesproxytest, ss_scope)
{
    auto reply = std::make_shared<Receiver>();

    ScopeMetadata meta = reg_->get_metadata("Dummy Demo Scope");

    auto wait_thread = std::thread([&reply](){reply->wait_until_finished();});

    meta.proxy()->create_query("search_string", VariantMap(), reply);

    wait_thread.join();
}

} // namespace
