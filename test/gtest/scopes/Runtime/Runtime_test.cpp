/*
* Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <mutex>

#include <scopes/CategorisedResult.h>
#include <scopes/ListenerBase.h>
#include <scopes/Runtime.h>
#include <scopes/internal/RuntimeImpl.h>
#include <scopes/internal/MWScope.h>
#include <scopes/internal/ScopeImpl.h>
#include <unity/UnityExceptions.h>

#include <gtest/gtest.h>

using namespace std;
using namespace unity::api::scopes;

TEST(Runtime, basic)
{
    Runtime::UPtr rt = Runtime::create("Runtime.ini");
    EXPECT_TRUE(rt->registry().get() != nullptr);
    rt->destroy();
}

class Receiver : public SearchListener
{
public:
    virtual void push(CategorisedResult result) override
    {
        EXPECT_EQ("uri", result.uri());
        EXPECT_EQ("title", result.title());
        EXPECT_EQ("art", result.art());
        EXPECT_EQ("dnd_uri", result.dnd_uri());
        count_++;
    }
    virtual void finished(ListenerBase::Reason reason, string const& error_message) override
    {
        EXPECT_EQ(Finished, reason);
        EXPECT_EQ("", error_message);
        EXPECT_EQ(1, count_);
        // Signal that the query has completed.
        unique_lock<mutex> lock(mutex_);
        query_complete_ = true;
        cond_.notify_one();
    }
    void wait_until_finished()
    {
        unique_lock<mutex> lock(mutex_);
        cond_.wait(lock, [this] { return this->query_complete_; });
    }
private:
    bool query_complete_;
    mutex mutex_;
    condition_variable cond_;
    int count_;
};

TEST(Runtime, run_scope)
{
    // Spawn the test scope
    const char *const argv[] = {"./Runtime_TestScope", "Runtime.ini", NULL};
    pid_t pid;
    switch (pid = fork()) {
    case -1:
        FAIL();
    case 0: // child
        execv(argv[0], (char *const *)argv);
        FAIL();
    }

    // Parent: connect to scope and run a query
    auto rt = internal::RuntimeImpl::create("", "Runtime.ini");
    auto mw = rt->factory()->create("TestScope", "Zmq", "Zmq.ini");
    mw->start();
    auto proxy = mw->create_scope_proxy("TestScope");
    auto scope = internal::ScopeImpl::create(proxy, rt.get(), "TestScope");

    VariantMap hints;
    auto receiver = make_shared<Receiver>();
    auto ctrl = scope->create_query("test", hints, receiver);
    receiver->wait_until_finished();

    kill(pid, SIGTERM);
}
