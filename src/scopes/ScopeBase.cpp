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

#include <unity/scopes/ScopeBase.h>

namespace unity
{

namespace scopes
{

//! @cond

ScopeBase::ScopeBase()
{
}

ScopeBase::~ScopeBase() noexcept
{
}

//! @endcond

void ScopeBase::run()
{
    // Intentionally empty: default "do nothing" implementation.
}

void ScopeBase::runtime_version(int& v_major, int& v_minor, int& v_micro) noexcept
{
    v_major = unity::scopes::major_version();
    v_minor = unity::scopes::minor_version();
    v_micro = unity::scopes::micro_version();
}

} // namespace scopes

} // namespace unity