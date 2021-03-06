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

#if !defined(UNITY_SCOPES_INTERNAL_LTTNG_UNITYSCOPESTP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define UNITY_SCOPES_INTERNAL_LTTNG_UNITYSCOPESTP_H

#undef TRACEPOINT_PROVIDER
#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_PROVIDER unity_scopes
#define TRACEPOINT_INCLUDE <unity/scopes/internal/lttng/UnityScopes_tp.h>

#include <unity/scopes/internal/lttng/SimpleTracepoint.h>
#include <stdint.h>

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

SIMPLE_TRACEPOINT(
  trace,
  TRACE_DEBUG,
  stp_integer(int, value)
)

#if __clang__
#pragma clang diagnostic pop
#endif

#endif /* UNITY_SCOPES_INTERNAL_LTTNG_UNITYSCOPES_TP_H */
