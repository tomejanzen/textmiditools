//
// TextMIDITools Version 1.0.96
//
// textmidi 1.0.6
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(OPTIONNAME_H)
#    define  OPTIONNAME_H

#include <string>
#include <string_view>

namespace textmidi {

class OptionName
{
  public:
    constexpr OptionName(const std::string& option, const std::string& text,
        char letter = 0)
      : option_{option},
        text_{text},
        registered_name_{option + (letter ? (std::string{","} + letter) : "")}
    {
    }
    constexpr std::string registered_name() const
    {
        return registered_name_;
    }
    constexpr std::string option() const
    {
        return option_;
    }
    constexpr std::string text() const
    {
        return text_;
    }
 private:
    const std::string option_;
    const std::string text_;
    const std::string registered_name_;
};

} // namespace textmidi

#endif
