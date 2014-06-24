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
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <unity/scopes/internal/SettingsSchema.h>

#include <unity/UnityExceptions.h>
#include <unity/util/FileIO.h>

#include <gtest/gtest.h>
#include <jsoncpp/json/json.h>

using namespace unity;
using namespace unity::scopes;
using namespace unity::scopes::internal;
using namespace std;

char const* ok_schema = R"delimiter(
{
    "settings":
    [
        {
            "id": "location",
            "displayName": "Location",
            "type": "string",
            "parameters": {
                "defaultValue": "London"
            }
        },
        {
            "id": "unitTemp",
            "displayName": "Temperature Units",
            "type": "list",
            "parameters": {
                "defaultValue": 1,
                "values": ["Celcius", "Fahrenheit"]
            }
        },
        {
            "id": "age",
            "displayName": "Age",
            "type": "number",
            "parameters": {
                "defaultValue": 23
            }
        },
        {
            "id": "enabled",
            "displayName": "Enabled",
            "type": "boolean",
            "parameters": {
                "defaultValue": true
            }
        },
        {
            "id": "string_no_default",
            "displayName": "string_no_default",
            "type": "string",
            "parameters": {
            }
        },
        {
            "id": "string_no_default2",
            "displayName": "string_no_default",
            "type": "string"
        },
        {
            "id": "list_no_default",
            "displayName": "list_no_default",
            "type": "list",
            "parameters": {
                "values": ["a", "b"]
            }
        },
        {
            "id": "number_no_default",
            "displayName": "number_no_default",
            "type": "number",
            "parameters": {
            }
        },
        {
            "id": "number_no_default2",
            "displayName": "number_no_default",
            "type": "number"
        },
        {
            "id": "boolean_no_default",
            "displayName": "boolean_no_default",
            "type": "boolean",
            "parameters": {
            }
        },
        {
            "id": "boolean_no_default2",
            "displayName": "boolean_no_default",
            "type": "boolean"
        }
    ]
}
)delimiter";

TEST(SettingsSchema, basic)
{
    SettingsSchema s(ok_schema);

    auto v = s.settings();

    EXPECT_EQ("London", v["location"].get_string());
    EXPECT_EQ(1, v["unitTemp"].get_int());
    EXPECT_EQ(23, v["age"].get_int());
    EXPECT_TRUE(v["enabled"].get_bool());

    EXPECT_EQ(Variant(), v["string_no_default"]);
    EXPECT_EQ(Variant(), v["string_no_default2"]);
    EXPECT_EQ(Variant(), v["list_no_default"]);
    EXPECT_EQ(Variant(), v["list_no_default2"]);
    EXPECT_EQ(Variant(), v["number_no_default"]);
    EXPECT_EQ(Variant(), v["number_no_default2"]);
    EXPECT_EQ(Variant(), v["boolean_no_default"]);
    EXPECT_EQ(Variant(), v["boolean_no_default2"]);
}

TEST(SettingsSchema, exceptions)
{
    try
    {
        SettingsSchema("");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): cannot parse schema: * Line 1, Column 1\n"
                     "  Syntax error: value, object or array expected.\n",
                     e.what());
    }

    try
    {
        SettingsSchema(R"({ "a": "b" })");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): missing \"settings\" definition",
                     e.what());
    }

    try
    {
        SettingsSchema(R"({ "settings": "b" })");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): value \"settings\" must be an array",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "displayName": "Location",
                        "type": "string"
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingSchema(): missing \"id\" definition",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "",
                        "type": "number"
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingSchema(): invalid empty \"id\" definition",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "someid",
                        "displayName": "Location"
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingSchema(): missing \"type\" definition, id = \"someid\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "someid": "id",
                        "displayName": "Location",
                        "type": 99
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingSchema(): missing \"id\" definition",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "displayName": "Location",
                        "type": 99
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid value type for \"type\" definition, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "displayName": "Location",
                        "type": "no_such_type"
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid \"type\" setting: "
                     "\"no_such_type\", id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "string",
                        "parameters": 99
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): expected value of type object "
                     "for \"parameters\", id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "boolean",
                        "parameters": {
                            "defaultValue": "true"
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid value type "
                     "for \"defaultValue\" definition, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "number",
                        "parameters": {
                            "defaultValue": true
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid value type "
                     "for \"defaultValue\" definition, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "list",
                        "parameters": {
                            "defaultValue": "hello",
                            "values": ["Celcius", "Fahrenheit"]
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid value type "
                     "for \"defaultValue\" definition, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "list"
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): missing \"parameters\" definition, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "list",
                        "parameters": {
                            "defaultValue": -1,
                            "values": "hello"
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid value type for \"values\" "
                     "definition, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "list",
                        "parameters": {
                            "defaultValue": -1,
                            "values": [ ]
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid empty \"values\" definition, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "list",
                        "parameters": {
                            "defaultValue": -1,
                            "values": [ 3, 7 ]
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid enumerator type, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "list",
                        "parameters": {
                            "defaultValue": -1,
                            "values": [ "a", "" ]
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid empty enumerator, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "list",
                        "parameters": {
                            "defaultValue": -1,
                            "values": [ "a", "b", "a" ]
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): duplicate enumerator \"a\", id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "list",
                        "parameters": {
                            "defaultValue": -1,
                            "values": [ "a", "b" ]
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): \"defaultValue\" out of range, id = \"x\"",
                     e.what());
    }

    try
    {
        SettingsSchema(R"(
            {
                "settings":
                [
                    {
                        "id": "x",
                        "type": "string",
                        "parameters": {
                            "defaultValue": true
                        }
                    }
                ]
            }
        )");
        FAIL();
    }
    catch (ResourceException const& e)
    {
        EXPECT_STREQ("unity::ResourceException: SettingsSchema(): invalid value type "
                     "for \"defaultValue\" definition, id = \"x\"",
                     e.what());
    }
}
