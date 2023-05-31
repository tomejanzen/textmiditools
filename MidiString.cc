//
// TextMIDITools Version 1.0.58
//
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#include "MidiString.h"

using namespace std;
using namespace textmidi;

void escape(string& str)
{
    string::size_type pos{};

    while ((str.size() > 0) && (0x0 == str[str.size() - 1]))
    {
        str.resize(str.size() - 1);
    }
    pos = 0;
    while ((pos < str.size()) && (pos = str.find('\a', pos)) != str.npos)
    {
        const char control_string[]{R"(\a)"};
        str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2});
        ++pos;
    }
    pos = 0;
    while ((pos < str.size()) && (pos = str.find('\b', pos)) != str.npos)
    {
        const char control_string[]{R"(\b)"};
        str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2});
        ++pos;
    }
    pos = 0;
    while ((pos < str.size()) && (pos = str.find('\t', pos)) != str.npos)
    {
        const char control_string[]{R"(\t)"};
        str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2});
        ++pos;
    }
    pos = 0;
    while ((pos < str.size()) && (pos = str.find('\n')) != str.npos)
    {
        str.replace(pos, 1, R"(\n)", 2);
        ++pos;
    }
    pos = 0;
    while ((pos < str.size()) && (pos = str.find('\v')) != str.npos)
    {
        str.replace(pos, 1, R"(\v)", 2);
        ++pos;
    }
    pos = 0;
    while ((pos < str.size()) && (pos = str.find('\f')) != str.npos)
    {
        str.replace(pos, 1, R"(\f)", 2);
        ++pos;
    }
    pos = 0;
    while ((pos < str.size()) && (pos = str.find('\r', pos)) != str.npos)
    {
        const char control_string[]{R"(\r)"};
        str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2});
        ++pos;
    }
    pos = 0;
    while ((pos < str.size()) && (pos = str.find('\"', pos)) != str.npos)
    {
        const char control_string[]{R"(\")"};
        str.replace(pos, string::size_type{1}, &control_string[0], string::size_type{2});
        pos += 2;
    }
}

void de_escape(string& str)
{
    // now the trailing total quotes have been removed.
    string::size_type p{};
    p = 0;
    while ((p = str.find("\\a", p, 2)) != string::npos)
    {
        str.replace(p, 2, "\a", 1);
    }
    p = 0;
    while ((p = str.find("\\b", p, 2)) != string::npos)
    {
        str.replace(p, 2, "\b", 1);
    }
    p = 0;
    while ((p = str.find("\\t", p, 2)) != string::npos)
    {
        str.replace(p, 2, "\t", 1);
    }
    p = 0;
    while ((p = str.find("\\n", p, 2)) != string::npos)
    {
        str.replace(p, 2, "\n", 1);
    }
    p = 0;
    while ((p = str.find("\\v", p, 2)) != string::npos)
    {
        str.replace(p, 2, "\v", 1);
    }
    p = 0;
    while ((p = str.find("\\f", p, 2)) != string::npos)
    {
        str.replace(p, 2, "\f", 1);
    }
    p = 0;
    while ((p = str.find("\\r", p, 2)) != string::npos)
    {
        str.replace(p, 2, "\r", 1);
    }
    p = 0;
    while ((p = str.find("\\\"", p, 2)) != string::npos)
    {
        str.replace(p, 2, "\"", 1);
    }
}


void strip_quotes(string& str)
{
    if (!str.empty())
    {
        if (str == string(R"("\")"))
        {
            str = R"(\)";
        }
        else
        {
            if ('\"' == str[0])
            {
                str.erase(0, 1);
            }
    
            if (str.size() >= 2)
            {
                // if a non-escape double quote at end
                if (('\"' == str[str.size() - 1]) && ('\\' != str[str.size() - 2]))
                {
                    str.erase(str.size() - 1);
                }
            }
            else
            {
                // erase a single double quote by itself.
                if (str == "\"")
                {
                    str.clear();
                }
            }
        }
    }
}


void add_quotes(string& str)
{
    string quoted("\"");
    (quoted += str) += '\"';
}

void strip_space(string& str)
{
    if (!str.empty())
    {
        string::size_type pos{};
        const string blanks{" \t\n\r"};
        if ((pos = str.find_last_not_of(blanks)) != str.npos)
        {
            if ((pos + 1) < str.size())
            {
                str.erase(pos + 1);
            }
        }
    }
}

void textmidi::make_raw_string(string& str)
{
    strip_quotes(str);
    de_escape(str);
}

void textmidi::make_human_string(string& str)
{
    escape(str);
    add_quotes(str);
}

