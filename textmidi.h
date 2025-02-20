//
// TextMIDITools Version 1.0.89
//
// textmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(TEXTMIDI_H)
#    define  TEXTMIDI_H

#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include "Midi.h"

namespace textmidi
{
    struct TextMidiFeatures
    {
      public:
        TextMidiFeatures() = default;
        static std::unique_ptr<TextMidiFeatures>& me()
        {
            if (!me_)
            {
                me_ = std::make_unique<TextMidiFeatures>();
            }
            return me_;
        }

        TextMidiFeatures(const TextMidiFeatures& ) = delete;

        void text_filename(const std::string& text_filename)
        {
            this->text_filename_ = text_filename;
        }

        void line_ctr(int line_ctr)
        {
            this->line_ctr_ = line_ctr;
        }

        void detache(std::uint32_t detache)
        {
            this->detache_ = detache;
        }

        void note_off_select(bool note_off_select)
        {
            this->note_off_select_ = note_off_select;
        }

        void verbose(bool verbose)
        {
            this->verbose_ = verbose;
        }

        std::string text_filename() const
        {
            return this->text_filename_;
        }

        std::ofstream& midi_filestr()
        {
            return this->midi_filestr_;
        }

        constexpr int line_ctr() const
        {
            return this->line_ctr_;
        }

        void inc_line_ctr()
        {
            ++(this->line_ctr_);
        }

        constexpr std::uint32_t detache() const
        {
            return this->detache_;
        }

        constexpr bool note_off_select() const
        {
            return this->note_off_select_;
        }

        constexpr bool verbose() const
        {
            return this->verbose_;
        }
      private:
        static std::unique_ptr<TextMidiFeatures> me_;

        // The input file name, selected by command-line options, is used in error
        // messages.
        std::string text_filename_{"unknown.txt"};
        // The ofstream of the binary standard MIDI output file.
        std::ofstream midi_filestr_{};
        // line_ctr counts the lines in the input text file so that error messages can
        // refer to the line on which the error was encountered.
        int line_ctr_{1};
        // detache is set by a command-line argument for the separation of consecutive
        // notes.  Defaults to 10.
        std::uint32_t detache_{};
        // lazy_note_off_select is set by a command-line argument.  If true, then
        // in LAZY or BRIEF mode, note_off events (with the global velocity) are
        // written as the endings of notes; else note_ons (with a zero velocity) are
        // used to end notes.
        bool note_off_select_{};
        // Verbose flag taken from command-line options
        bool verbose_{};
    };
}

extern std::unique_ptr<midi::RunningStatusBase> running_status;

#endif
