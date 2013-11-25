/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <scopes/internal/ObjectProxyImpl.h>

#include <scopes/internal/MWObjectProxy.h>
#include <scopes/ObjectProxy.h>

using namespace std;

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

ObjectProxyImpl::ObjectProxyImpl(MWProxy const& mw_proxy) :
    mw_proxy_(mw_proxy)
{
}

ObjectProxyImpl::~ObjectProxyImpl() noexcept
{
}

Proxy ObjectProxyImpl::create(MWProxy const&)
{
    return make_shared<ObjectProxy>();
}

MWProxy ObjectProxyImpl::proxy() const
{
    return mw_proxy_;
}

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity
