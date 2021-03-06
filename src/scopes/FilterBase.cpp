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
 * Authored by: Pawel Stolowski <pawel.stolowski@canonical.com>
*/

#include <unity/scopes/FilterBase.h>
#include <unity/scopes/internal/FilterBaseImpl.h>

namespace unity
{

namespace scopes
{

/// @cond
FilterBase::FilterBase(internal::FilterBaseImpl *pimpl)
    : p(pimpl)
{
}

VariantMap FilterBase::serialize() const
{
    return p->serialize();
}

FilterBase::SCPtr FilterBase::deserialize(VariantMap const& var) {
    return internal::FilterBaseImpl::deserialize(var);
}

FilterBase::~FilterBase() = default;
/// @endcond

void FilterBase::set_display_hints(int hints)
{
    p->set_display_hints(hints);
    p->validate_display_hints();
}

std::string FilterBase::id() const
{
    return p->id();
}

void FilterBase::set_title(std::string const& title)
{
    p->set_title(title);
}

std::string FilterBase::title() const
{
    return p->title();
}

int FilterBase::display_hints() const
{
    return p->display_hints();
}

std::string FilterBase::filter_type() const
{
    return p->filter_type();
}

FilterGroup::SCPtr FilterBase::filter_group() const
{
    return p->filter_group();
}

} // namespace scopes

} // namespace unity
