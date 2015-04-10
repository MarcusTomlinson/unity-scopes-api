/*
 * Copyright (C) 2014 Canonical Ltd
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

#include <unity/scopes/internal/Utils.h>
#include <unity/scopes/ScopeExceptions.h>
#include <unity/UnityExceptions.h>

#include <boost/filesystem.hpp>

#include <iomanip>
#include <locale>
#include <mutex>

#include <sys/stat.h>

using namespace std;

namespace unity
{

namespace scopes
{

namespace internal
{

VariantMap::const_iterator find_or_throw(string const& context, VariantMap const& var, string const& key)
{
    auto it = var.find(key);
    if (it == var.end())
    {
        throw unity::scopes::NotFoundException(context + ": missing element", key);
    }
    return it;
}

string to_percent_encoding(string const& str)
{
    ostringstream result;
    for (auto const& c: str)
    {
        if ((!isalnum(c)))
        {
            result << '%' << setw(2) << setfill('0') << hex << uppercase << static_cast<int>(static_cast<unsigned char>(c)) << nouppercase;
        }
        else
        {
            result << c;
        }
    }
    return result.str();
}

string from_percent_encoding(string const& str)
{
    ostringstream result;
    for (auto it = str.begin(); it != str.end(); it++)
    {
        auto c = *it;
        if (c == '%')
        {
            bool valid = false;
            // take two characters and covert them from hex to actual char
            if (++it != str.end())
            {
                c = *it;
                if (++it != str.end())
                {
                    string const hexnum { c, *it };
                    try
                    {
                        auto k = stoi(hexnum, nullptr, 16);
                        result << static_cast<char>(k);
                        valid = true;
                    }
                    catch (logic_error const& e) // covers both invalid_argument and out_of_range
                    {
                        stringstream err;
                        err << "from_percent_encoding(): unsupported conversion of '" << hexnum << "'";
                        throw unity::InvalidArgumentException(err.str());
                    }
                }
            }
            if (!valid)
            {
                throw unity::InvalidArgumentException("from_percent_encoding(): too few characters for percent-encoded value");
            }
        }
        else
        {
            result << c;
        }
    }
    return result.str();
}

string uncamelcase(string const& str)
{
    const locale loc("");
    if (str.size() == 0)
    {
        return str;
    }
    auto it = str.begin();
    int previous_is_lower = islower(*it);
    stringstream res;
    res << tolower(*it, loc);
    ++it;
    while (it != str.end())
    {
        if (isupper(*it) && previous_is_lower)
        {
            res << "-";
        }
        previous_is_lower = islower(*it);
        res << tolower(*it, loc);
        ++it;
    }
    return res.str();
}

template<>
bool convert_to<bool>(string const& val, Variant& out)
{
    if (val == "true")
    {
        out = Variant(true);
        return true;
    }
    if (val == "false")
    {
        out = Variant(false);
        return true;
    }
    return false;
}

int safe_system_call(string const& command)
{
    static mutex system_mutex;
    lock_guard<mutex> lock(system_mutex);
    return system(command.c_str());
}

// Recursively create the directories in path, setting permissions to the specified mode
// (regardless of the setting of umask). If one or more directories already exist, they
// are left unchanged (including their permissions). If a directory cannot be created,
// fail silently.

void make_directories(string const& path_name, mode_t mode)
{
    using namespace boost::filesystem;

    // We can't use boost::create_directories() here because that does not allow control over permissions.
    auto abs = absolute(path_name);
    path path_so_far = "";
    path::iterator it = abs.begin();
    ++it; // No point in trying to create /
    while (it != abs.end())
    {
        path_so_far += "/";
        path_so_far += *it++;
        string p = path_so_far.native();
        if (mkdir(p.c_str(), mode) != 0)
        {
            if (errno == EEXIST)
            {
                continue;
            }
            return;  // No point in continuing, we'd fail on all subsequent iterations.
        }
        // We just created the dir, make sure it has the requested permissions,
        // not the permissions modified by umask.
        chmod(p.c_str(), mode);
    }
}

} // namespace internal

} // namespace scopes

} // namespace unity
