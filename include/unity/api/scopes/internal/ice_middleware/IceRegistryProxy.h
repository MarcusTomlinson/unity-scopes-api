/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#ifndef UNITY_API_SCOPES_INTERNAL_ICEMIDDLEWARE_ICEREGISTRYPROXY_H
#define UNITY_API_SCOPES_INTERNAL_ICEMIDDLEWARE_ICEREGISTRYPROXY_H

#include <slice/unity/api/scopes/internal/ice_middleware/Registry.h>
#include <unity/api/scopes/internal/MWRegistryProxy.h>

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

namespace ice_middleware
{

// Client-side registry proxy for Ice. The implementation forwards the invocations via Ice,
// and translates the parameters and return value between the Ice types and the public types.

class IceRegistryProxy : public MWRegistryProxy
{
public:
    UNITY_DEFINES_PTRS(IceRegistryProxy);

    IceRegistryProxy(MiddlewareBase* mw_base, Ice::ObjectPrx const& p) noexcept;
    ~IceRegistryProxy() noexcept;

    // Remote operations.
    virtual ScopeProxy::SPtr find(std::string const& scope_name) override;
    virtual ScopeMap list() override;

private:
    middleware::RegistryPrx proxy_; // Ice proxy for the registry
};

} // namespace ice_middleware

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity

#endif
