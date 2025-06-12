//
// TextMIDITools Version 1.0.98
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#include <utility>
#include <vector>

#include "MidiString.h"

using std::string;
using std::vector;

namespace
{
    const std::vector < std::pair < char, std::string >> ControlCharToString
    {
        { '\a', "\\a"},           // bell
        { '\b', "\\b"},           // backspace
        { '\t', "\\t"},           // horz tab
        { '\n', "\\n"},           // line feed
        { '\v', "\\v"},           // vert tab
        { '\r', "\\r"},           // carriage return
        { '"', "\\\""}            // double quote
    };

    void escape(string & str)
    {
        string::size_type pos
        {
        };

        // if the string is not zero-length remove trailing ASCII nul bytes.
        // The && operator has a short-circuit, so the check for empty is first
        // and makes back() valid.
        while (!str.empty() && (0x0 == str.back()))
        {
            str.pop_back();
        }
        for (auto ctrl_char_to_str : ControlCharToString)
        {
            pos = 0;
            while ((pos < str.size()) && ((pos = str.find(ctrl_char_to_str.first, pos)) != str.npos))
            {
                str.replace(pos, sizeof ctrl_char_to_str.first, ctrl_char_to_str.second);
                pos += ctrl_char_to_str.second.size();
            }
        }
    }

    void de_escape(string & str)
    {
        // now the trailing total quotes have been removed.
        string::size_type p
        {
        };
        for (auto ctrl_char_to_str : ControlCharToString)
        {
            p = 0;
            while ((p = str.find(ctrl_char_to_str.second, p)) != string::npos)
            {
                str.replace(p, ctrl_char_to_str.second.size(), string{} += ctrl_char_to_str.first);
            }
        }
    }

    void strip_quotes(string & str)
    {
        //
        // We want to erase double quotes that encase the entire string,
        // but not erase double quotes that do not encase the entire string.
        if (!str.empty())
        {
            // One of the tests had this string, a slash that miditext quoted.
            // It's possible that this was a bug in textmidi/miditext.
            if (str == string(R"("\")"))
            {
                str = R"(\)";
            }
            else
            {
                // erase a single double quote by itself.
                // A test file had this I recall.
                if (str == "\"")
                {
                    str.clear();
                }
                else
                {
                    if ((str.size() > 1)
                        && ('\"' == str.front()) && ('\"' == str.back()) && ('\\' != str[str.size() - 2]))
                    {
                        str.erase(0, 1);
                        str.erase(str.size() - 1);
                    }
                }
            }
        }
    }

}                               // namespace

void textmidi::make_raw_string(string & str) noexcept
{
    strip_quotes(str);
    de_escape(str);
}

void textmidi::make_human_string(string & str) noexcept
{
    escape(str);
}
