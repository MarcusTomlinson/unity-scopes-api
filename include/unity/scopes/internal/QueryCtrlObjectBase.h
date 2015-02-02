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

#pragma once

#include <unity/scopes/internal/AbstractObject.h>

namespace unity
{

namespace scopes
{

namespace internal
{

struct InvokeInfo;

class QueryCtrlObjectBase : public AbstractObject
{
public:
    UNITY_DEFINES_PTRS(QueryCtrlObjectBase);

    virtual void cancel(InvokeInfo const& info) = 0;
    virtual void destroy(InvokeInfo const& info) = 0;
};

} // namespace internal

} // namespace scopes

} // namespace unity
