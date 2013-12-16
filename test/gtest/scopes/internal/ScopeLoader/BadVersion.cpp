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

// Force Version.h to use the specified version, so we get coverage on the version check.

#define UNITY_SCOPES_VERSION_MAJOR 666

#include "Counters.h"

#include <scopes/internal/ScopeLoader.h>

#include <iostream>

class EXPORT MyScope : public unity::api::scopes::ScopeBase
{
public:
    MyScope() {}
    virtual ~MyScope() noexcept {}

    virtual int start(std::string const&, unity::api::scopes::RegistryProxy const&) override
    {
        inc_start();
        return VERSION;
    }

    virtual void stop() override
    {
        inc_stop();
    }

    virtual void run() override
    {
        inc_run();
    }

    virtual unity::api::scopes::QueryBase::UPtr create_query(std::string const&,
                                                             unity::api::scopes::VariantMap const&) override
    {
        return nullptr;
    }
};

extern "C"
{

    EXPORT
    unity::api::scopes::ScopeBase*
    // cppcheck-suppress unusedFunction
    UNITY_API_SCOPE_CREATE_FUNCTION()
    {
        inc_create();
        return new MyScope;
    }

    EXPORT
    void
    // cppcheck-suppress unusedFunction
    UNITY_API_SCOPE_DESTROY_FUNCTION(unity::api::scopes::ScopeBase* scope_base)
    {
        inc_destroy();
        delete scope_base;
    }

}