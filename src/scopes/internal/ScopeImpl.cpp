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

#include <unity/scopes/internal/ScopeImpl.h>

#include <unity/scopes/internal/ResultImpl.h>
#include <unity/scopes/internal/MiddlewareBase.h>
#include <unity/scopes/internal/MWScope.h>
#include <unity/scopes/internal/QueryCtrlImpl.h>
#include <unity/scopes/internal/ReplyObject.h>
#include <unity/scopes/Scope.h>
#include <unity/scopes/Result.h>
#include <unity/Exception.h>
#include <unity/scopes/internal/ActivationReplyObject.h>
#include <unity/scopes/internal/ResultReplyObject.h>
#include <unity/scopes/internal/PreviewReplyObject.h>

#include <cassert>
#include <iostream> // TODO: remove this once logging is added

using namespace std;

namespace unity
{

namespace scopes
{

namespace internal
{

ScopeImpl::ScopeImpl(MWScopeProxy const& mw_proxy, RuntimeImpl* runtime, std::string const& scope_name) :
    ObjectProxyImpl(mw_proxy),
    runtime_(runtime),
    scope_name_(scope_name)
{
    assert(runtime);
}

ScopeImpl::~ScopeImpl()
{
}

QueryCtrlProxy ScopeImpl::create_query(std::string const& query_string, std::string const& department_id, FilterState const& filter_state, VariantMap const& hints, SearchListener::SPtr const& reply) const
{
    Query query(scope_name_, query_string, department_id);
    query.set_filter_state(filter_state);
    return create_query(query, hints, reply);
}

QueryCtrlProxy ScopeImpl::create_query(std::string const& query_string, FilterState const& filter_state, VariantMap const& hints, SearchListener::SPtr const& reply) const
{
    Query query(scope_name_);
    query.set_query_string(query_string);
    query.set_filter_state(filter_state);
    return create_query(query, hints, reply);
}

QueryCtrlProxy ScopeImpl::create_query(string const& query_string, VariantMap const& hints, SearchListener::SPtr const& reply) const
{
    Query query(scope_name_);
    query.set_query_string(query_string);
    return create_query(query, hints, reply);
}

QueryCtrlProxy ScopeImpl::create_query(Query const& query, VariantMap const& hints, SearchListener::SPtr const& reply) const
{
    QueryCtrlProxy ctrl;
    ReplyObject::SPtr ro(make_shared<ResultReplyObject>(reply, runtime_, to_string()));
    try
    {
        MWReplyProxy rp = fwd()->mw_base()->add_reply_object(ro);

        // Forward the the create_query() method across the bus. This is a
        // synchronous twoway interaction with the scope, so it can return
        // the QueryCtrlProxy. This may block for some time, for example, if
        // the scope is not running and needs to be activated by the registry first.
        ctrl = fwd()->create_query(query, hints, rp);
        assert(ctrl);
    }
    catch (std::exception const& e)
    {
        try
        {
            ro->finished(ListenerBase::Error, e.what());
        }
        catch (...)
        {
        }
        throw;
    }
    return ctrl;
}

QueryCtrlProxy ScopeImpl::activate(Result const& result, VariantMap const& hints, ActivationListener::SPtr const& reply) const
{
    QueryCtrlProxy ctrl;
    ActivationReplyObject::SPtr ro(make_shared<ActivationReplyObject>(reply, runtime_, to_string()));
    try
    {
        MWReplyProxy rp = fwd()->mw_base()->add_reply_object(ro);

        // Forward the activate() method across the bus.
        ctrl = fwd()->activate(result.p->activation_target(), hints, rp);
        assert(ctrl);
    }
    catch (std::exception const& e)
    {
        // TODO: log error
        cerr << "activate(): " << e.what() << endl;
        try
        {
            ro->finished(ListenerBase::Error, e.what());
            throw;
        }
        catch (...)
        {
            cerr << "activate(): unknown exception" << endl;
        }
        throw;
    }
    return ctrl;
}

QueryCtrlProxy ScopeImpl::activate_preview_action(Result const& result, VariantMap const& hints, std::string const& action_id, ActivationListener::SPtr const& reply) const
{
    QueryCtrlProxy ctrl;
    try
    {
        // Create a middleware server-side object that can receive incoming
        // push() and finished() messages over the network.
        ActivationReplyObject::SPtr ro(make_shared<ActivationReplyObject>(reply, runtime_, to_string()));
        MWReplyProxy rp = fwd()->mw_base()->add_reply_object(ro);

        // Forward the activate() method across the bus.
        ctrl = fwd()->activate_preview_action(result.p->activation_target(), hints, action_id, rp);
        assert(ctrl);
    }
    catch (std::exception const& e)
    {
        // TODO: log error
        cerr << "activate_preview_action(): " << e.what() << endl;
        try
        {
            // TODO: if things go wrong, we need to make sure that the reply object
            // is disconnected from the middleware, so it gets deallocated.
            reply->finished(ListenerBase::Error, e.what());
            throw;
        }
        catch (...)
        {
            cerr << "activate_preview_action(): unknown exception" << endl;
        }
        throw;
    }
    return ctrl;
}

QueryCtrlProxy ScopeImpl::preview(Result const& result, VariantMap const& hints, PreviewListener::SPtr const& reply) const
{
    QueryCtrlProxy ctrl;
    PreviewReplyObject::SPtr ro(make_shared<PreviewReplyObject>(reply, runtime_, to_string()));
    try
    {
        // Create a middleware server-side object that can receive incoming
        // push() and finished() messages over the network.
        MWReplyProxy rp = fwd()->mw_base()->add_reply_object(ro);

        // Forward the the create_query() method across the bus. This is a
        // synchronous twoway interaction with the scope, so it can return
        // the QueryCtrlProxy. Because the Scope implementation has a separate
        // thread for create_query() calls, this is guaranteed not to block for
        // any length of time. (No application code other than the QueryBase constructor
        // is called by create_query() on the server side.)
        ctrl = fwd()->preview(result.p->activation_target(), hints, rp);
        assert(ctrl);
    }
    catch (std::exception const& e)
    {
        // TODO: log error
        cerr << "preview(): " << e.what() << endl;
        try
        {
            ro->finished(ListenerBase::Error, e.what());
            throw;
        }
        catch (...)
        {
            cerr << "preview(): unknown exception" << endl;
        }
        throw;
    }
    return ctrl;
}

ScopeProxy ScopeImpl::create(MWScopeProxy const& mw_proxy, RuntimeImpl* runtime, std::string const& scope_name)
{
    return ScopeProxy(new Scope(new ScopeImpl(mw_proxy, runtime, scope_name)));
}

MWScopeProxy ScopeImpl::fwd() const
{
    return dynamic_pointer_cast<MWScope>(proxy());
}

} // namespace internal

} // namespace scopes

} // namespace unity
