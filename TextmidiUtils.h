//
// TextMIDITools Version 1.0.92
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(TEXTMIDIUTILS_H)
#    define  TEXTMIDIUTILS_H

namespace textmidi
{
    // Stroustrup, Bjarne.  "Programming Principles and Practice Using C++."
    // 2009.  Addison-Wesley.  Upper Saddle River, NJ. p. 388.
    template<typename T>
    char* io_bytes(T& i)
    {
        void* addr = &i;
        return static_cast<char*>(addr);
    }
} // namespace textmidi

#endif
