%{
/*
** TextMIDITools Version 1.0.59
** Copyright © 2023 Thomas E. Janzen
** License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
** This is free software: you are free to change and redistribute it.
** There is NO WARRANTY, to the extent permitted by law.
**
** textmidi converts text files that are written in textmidi MIDI language
** into standard MIDI file binary format files.
**
** HISTORY
**    1999: first version
**    2003: enhanced to use to sequence the Schumann Quintet,
**          which was submitted to the classical MIDI archive.
**    2021: C++ upgrades, fuller MIDI implementation to MIDI 1.1
**
** textmidi.l is a flex tokenizer specification for recognizing textmidi language
** in order to write binary MIDI files for musical purposes.
**
*/
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */
#include <endian.h>

#include <climits>
#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <array> // scale step constant
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>   // MIDI codes and textmidi strings
#include <regex> // recognize note names
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <utility> // pair
#include <tuple>
#include <stdexcept>
#include <ranges>
#include <concepts>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp> // to_upper
#include <boost/preprocessor/stringize.hpp>

#include "Midi.h" // MIDI codes and MIDI file header
#include "MidiMaps.h" // string-number maps
#include "TextmidiUtils.h"
#include "MIDIKeyString.h"
#include "MidiString.h"
#include "textmidi.h" // For this file's interfaces to main.cc

using namespace std;
using namespace boost;
using namespace boost::algorithm; // for to_upper(string)
using namespace midi;
using namespace textmidi;

#if 0
extern "C" int32_t yywrap (void) {};
#endif

// running_status is a MIDI trick to avoid repeating events such as NOTE_ON.
// "Meta events and sysex events cancel any running status that was in effect."
// RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
RunningStatus running_status{};

namespace
{
    // The last MIDI key (note) that was played, init to middle C
    constexpr int middle_c{60};
    int last_key{middle_c};
    uint16_t track_counter{}; // Count the tracks in order to set MIDI header
    MidiStreamVector trackchunk; // One track's data bytes.
    uint32_t accum_delay{}; // Delay accumulated between events.
    double global_total_track_ticks{}; // accum. tracks ticks to print for "ticks" cmd
    uint32_t total_beats{}; // beat to print for errors.
    // To turn off notes using NOTE_ON, use a zero velocity.
    constexpr MidiStreamAtom note_off_velocity{};
    constexpr double DotLength(0.5);
    constexpr double DoubleDotLength(0.25);
    // Map textmidi strings to MIDI file format codes.

    int global_ticks_per_quarter{384};
    // global_channel is part of implementing MIDI running status
    // because channel is part of a command.
    uint32_t global_channel{}; // [0..15]

    // keyevent_to_keynumber
    // converts a string keyevent, which can include tie-in and tie-out,
    // may be a note name such as "C4" or may be a key number as in "K60"
    // or may be a delta key as in "K+2".
    int keyevent_to_keynumber(const string& keyevent,
                              bool& tie_in, bool& tie_out, bool& is_delta)
    {
        int keynumber{MiddleC};
        smatch matches{};
        //
        // Hard Lessons: you can't break a line with a backslash in the middle of a
        // Raw regular expression
        const regex keyevent_re{R"((-?)(((([A-Ga-g])(([b#x]?)|(bb)))(-?)([[:digit:]]))|(([Kk])([+-]?)([[:digit:]]+)))(-?))"};
        const auto mat{regex_match(keyevent, matches, keyevent_re)};
        if (!mat)
        {
            const string errstr{(string{"Improper keyevent: "} += keyevent) += '\n'};
            cerr << errstr;
        }
        else
        {
            constexpr int match_tie_in{1};
            constexpr int match_note_or_key{2};
            constexpr int match_tie_out{15};
            tie_in  = !matches[match_tie_in].str().empty();
            tie_out = !matches[match_tie_out].str().empty();

            if (!matches[match_note_or_key].str().empty())
            {
                tie(keynumber, is_delta)
                    = pitchname_to_keynumber(matches[match_note_or_key]);
            }
        }
        return keynumber;
    }

    // KeyTie is a class that can hold the note event as a key number
    // and its ties in and out.
    struct KeyTie
    {
        KeyTie (string& key_name)
            : tie_in_{},
              key_{},
              tie_out_{}
        {
            bool is_delta{};
            int signed_key{keyevent_to_keynumber(key_name,
                             tie_in_, tie_out_, is_delta)};
            if (is_delta)
            {
                key_ = static_cast<unsigned>(last_key + signed_key);
                last_key = static_cast<int>(key_);
            }
            else
            {
                key_ = static_cast<unsigned>(signed_key);
                last_key = static_cast<int>(key_);
            }
            if (tie_in_)
            {
                key_name.erase(0, 1);
            }
            if (tie_out_)
            {
                key_name.erase(key_name.size() - 1);
            }
        }

        bool tie_in_;
        uint32_t key_;
        bool tie_out_;
    };

    //
    // int_to_vec
    // converts numbers into MIDI 1.1 spec extended values,
    // which can be a vector of bytes.
    // The first byte is the most-significant and has the 2^7 bit set.
    // Note that MIDI does not specify variable length numbers of more than 28 bits.
    template <integral IntType>
    MidiStreamVector int_to_vec(IntType i)
    {
        MidiStreamVector numvec{};
        if (i)
        {
            auto j{i};
            int lg2{};
            while (j != 0)
            {
                j >>= 1;
                ++lg2;
            }
            constexpr int bits_per_bite{7};
            int byte_qty{1};
            if (lg2 % bits_per_bite)
            {
                byte_qty = (lg2 + bits_per_bite) / bits_per_bite;
            }
            else
            {
                byte_qty = lg2 / bits_per_bite;
            }
            for (int groupof7{}; groupof7 < byte_qty; ++groupof7)
            {
                unsigned char temp{};
                temp = i >> ((byte_qty - groupof7 - 1) * bits_per_bite) & 0x7F;
                if (groupof7 != (byte_qty - 1))
                {
                    temp |= 0x80;
                }
                numvec.push_back(temp);
            }
        }
        else
        {
            numvec.push_back(static_cast<MidiStreamAtom>(0));
        }
        return numvec;
    }

    //
    // int24_to_vec converts 24-bit numbers to MIDI extended values.
    MidiStreamVector int24_to_vec(int i)
    {
        MidiStreamVector numvec;
        MidiStreamAtom tempbyte{};
        constexpr MidiStreamAtom byte_mask{0xFF};
        for (int ctr{2}; ctr >= 0; --ctr)
        {
            tempbyte = (i >> (bits_per_byte * ctr) ) & byte_mask;
            numvec.push_back(tempbyte);
        }
        return numvec;
    }

    //
    // write_delay writes the time delay in MIDI ticks.
    void write_delay()
    {
        // Delays must occur between events even if delay is zero.
        const MidiStreamVector delayvec = int_to_vec(accum_delay);
        ranges::copy(delayvec, back_inserter(trackchunk));
        total_beats += (accum_delay / global_ticks_per_quarter);
        accum_delay = 0;
    }

    //
    // program_1_to_0
    // transforms a textmidi value (usu. program or program) in (1..16)
    // to a MIDI binary program in (0..15).
    void program_1_to_0(uint32_t& program)
    {
        if (0 == program)
        {
            const string errstr{((string{textmidi_features->text_filename_} += ':')
                += lexical_cast<string>(textmidi_features->line_ctr_))
                += " Program was zero (0) and left as zero; textmidi program numbers must be in [1..16]\n"};
            cerr << errstr;
        }
        else
        {
            --program;
        }
    }

    //
    // channel_1_to_0
    // transforms a textmidi value (usu. channel or program) in (1..16)
    // to a MIDI binary channel in (0..15).
    void channel_1_to_0(uint32_t& channel)
    {
        if (0 == channel)
        {
            const string errstr{((string{textmidi_features->text_filename_} += ':')
                += lexical_cast<string>(textmidi_features->line_ctr_))
                += " Channel was zero (0) and left as zero; channels must be in [1..16]\n"};
            cerr << errstr;
        }
        else
        {
            channel = (((channel > 0) ? (channel - 1) : channel) & channel_mask);
        }
        global_channel = channel;
    }

    uint32_t delay_and_update_total_ticks(double thenumerator,
            double thedivisor, bool dotted, bool double_dotted)
    {
        // This routine handles full note rhythm value.
        // detache adjustments are done in the client
        // after calling this function.
        double ticks{};
        if (thedivisor != 0.0)
        {
          // Musical values are in whole notes, as a 1/4 note
          // is 1/4 of a whole note, but MIDI specifies ticks per quarter.
          ticks = (static_cast<double>(QuartersPerWhole) * thenumerator
                 * static_cast<double>(global_ticks_per_quarter)) / thedivisor;
        }
        if (dotted) // dotted notes
        {
            auto save_ticks{ticks};
            // This looks backwards but
            // We are here because we know there is a dot at the end.
            // If there is a dot in front of it we know it was a
            // double-dot; hence the proper lengths get added.
            ticks += (save_ticks * DotLength);
            if (double_dotted) // double dotted notes
            {
                ticks += (save_ticks * DoubleDotLength);
            }
        }
        // We update total ticks with the full duration
        // although detache might get subtracted in the caller.
        // So global_total_track_ticks includes the detache value.
        const auto temp_total_track_ticks = global_total_track_ticks + ticks;
        // To address the running sums problem.
        // Tracks were long or short due to accumulating rounding errors.
        if ((temp_total_track_ticks - trunc(temp_total_track_ticks)) >= 0.5)
        {
            ticks += 0.5; // put it over the next integer value up.
        }
        // Now it's OK to use round().
        ticks = round(ticks);
        global_total_track_ticks += ticks;
        return ticks;
    }

    class SysExEventSequence
    {
      public:
        SysExEventSequence()
          : sysex_seq_{},
            subid_{},
            device_id_{},
            subid1_{},
            remaining_{}
        {
        }
        void clear()
        {
            sysex_seq_.clear();
            subid_.clear();
            device_id_.clear();
            subid1_.clear();
            remaining_.clear();
        }
        void write_length()
        {
        }
        const MidiStreamVector& sysex_seq() const
        {
            return sysex_seq_;
        }
        void subid(MidiStreamVector subid)
        {
            subid_ = subid;
        }
        void device_id(const MidiStreamVector& device_id)
        {
            device_id_ = device_id;
        }
        void subid1(const MidiStreamVector& subid1)
        {
            subid1_ = subid1;
        }
        void remaining(const MidiStreamVector& remaining)
        {
            remaining_ = remaining;
        }
        void build_msg()
        {
            // sysex=subid.size() + device_id.size() + subid1=1 + subid2=1 + remaining.size() + end_of_sysex=1
            const size_t data_length{subid_.size() + device_id_.size() + subid1_.size() + remaining_.size() + 1};
            const MidiStreamVector length_seq{int_to_vec(data_length)};

            sysex_seq_.reserve(data_length + length_seq.size());

            sysex_seq_.push_back(start_of_sysex[0]);
            ranges::copy(length_seq, back_inserter(sysex_seq_));
            ranges::copy(subid_, back_inserter(sysex_seq_));
            ranges::copy(device_id_, back_inserter(sysex_seq_));
            ranges::copy(subid1_, back_inserter(sysex_seq_));
            ranges::copy(remaining_, back_inserter(sysex_seq_));
            sysex_seq_.push_back(end_of_sysex[0]);
        }
        void append(MidiStreamVector& track)
        {
            ranges::copy(sysex_seq_, back_inserter(track));
            this->clear();
        }
      private:
        MidiStreamVector sysex_seq_;
        MidiStreamVector subid_;
        MidiStreamVector device_id_;
        MidiStreamVector subid1_;
        MidiStreamVector remaining_;
    };

    struct HexOrDec
    {
        int operator()(const std::string& str) const
        {
            int i{};
            // if a hexadecimal byte
            if (str.substr(0, 2) == "0x")
            {
                istringstream iss{str};
                iss >> hex >> i;
            }
            else // else is decimal
            {
                i = lexical_cast<unsigned>(str);
            }
            return i;
        }
    };

    SysExEventSequence sysex_event_sequence{};
}

namespace lazy
{
    bool note_off_select{}; // set in main.cc as option
    uint32_t dynamic{64};
    vector<KeyTie> chord{};
}

%}
/* definitions */
DYNAMIC (pppp|ppp|pp|p|mp|mf|forte|ff|fff|ffff|PPPP|PPP|PP|P|MP|MF|FORTE|FF|FFF|FFFF)[-\+]?
DIGIT [[:digit:]]
SPACE [\t ]*
STARTLINE_OR_SPACE (^|{SPACE})
SOMESPACE ([\t ]+)
LETTER [a-zA-Z]
INTEGER ([-+]?{DIGIT}+)
UINTEGER ({DIGIT}+)
DECBYTE [12]?{DIGIT}{1,2}
XINTEGER 0x([[:xdigit:]])+
HEXBYTE 0x([[:xdigit:]]){1,2}
UXINTEGER ({UINTEGER}|{XINTEGER})
BYTE ({DECBYTE}|{HEXBYTE})
FLOAT {UINTEGER}(([.][[:digit:]]+)?)
BYTE_SEQ {BYTE}({SOMESPACE}{BYTE})*
ZERO_HEX_BYTE (0x00|0x0)
THIRTYTWO_BYTE (0x20|32)
PANVALUE ({INTEGER}|LEFT|CENTER|RIGHT)
SMPTE_CODE ([[:print:]]+{SOMESPACE}{UINTEGER}:{UINTEGER}:{UINTEGER}:{UINTEGER}:{UINTEGER})

ONOFF (ON|OFF)
SLASH [\/]
RHYTHM ({UINTEGER}({SLASH}{INTEGER})?[.]{0,2})
NOTE_LETTER [A-Ga-g]
OCTAVE_DIGIT [0-9]
ACCIDENTAL (([b#x]?)|(bb))
NON_NOTE_LETTER ([^[A-Ga-g \t]])
NOTE_NAME ({NOTE_LETTER}{ACCIDENTAL})
PITCH_NAME (({NOTE_LETTER}{ACCIDENTAL}(-?){OCTAVE_DIGIT})|(([Kk])([+-]?)([[:digit:]]+)))
KEY_COMMAND ([-]?K{INTEGER}[-]?)
LAZY_PITCH_NAME ((-?)(({NOTE_LETTER}{ACCIDENTAL}(-?){OCTAVE_DIGIT})|(([Kk])([+-]?)([[:digit:]]+)))(-?))
STRING .*
LINE (.*)
LETTERUNDER ({LETTER}|_)
ALPHANUM ({LETTER}|{DIGIT})
ALPHANUMUNDER ({ALPHANUM}|_)
IDENT ({LETTERUNDER}{ALPHANUMUNDER}*)
ASSIGNMENT ({IDENT}{SPACE}={SPACE}{STRING})
TEXT_META (TEXT|COPYRIGHT|TRACK|INSTRUMENT|LYRIC|MARKER|CUE_POINT|PROGRAM_NAME|DEVICE_NAME|TEXT_0A|TEXT_0B|TEXT_0C|TEXT_0D|TEXT_0E|TEXT_0F|TICKS)
CONTROL_FUNCTION (BANK_SELECT|MODULATION|BREATH|UNDEFINED_([[:digit:]]){2}|FOOT_CONTROLLER|PORTAMENTO_TIME|DATA_ENTRY_MSB|CHANNEL_VOLUME|BALANCE|PAN|EXPRESSION|EFFECT_1|EFFECT_2|GENERAL_PURPOSE_1|GENERAL_PURPOSE_2|GENERAL_PURPOSE_3|GENERAL_PURPOSE_4|DAMPER|PORTAMENTO_ON_OFF|SOSTENUTO|SOFTPEDAL|LEGATO_FOOT|HOLD_2|SOUND_VARIATION|TIMBRE_INTENSITY|RELEASE_TIME|ATTACK_TIME|BRIGHTNESS|DECAY_TIME|VIBRATO_RATE|VIBRATO_DEPTH|SOUND_CONTROLLER_9|SOUND_CONTROLLER_10|GENERAL_PURPOSE_5|GENERAL_PURPOSE_6|GENERAL_PURPOSE_7|GENERAL_PURPOSE_8|PORTAMENTO|HIRES_VELOCITY_MSB|REVERB_SEND_LEVEL|TREMOLO_DEPTH|CHORUS_SEND_LEVEL|CELESTE_DEPTH|PHASER_DEPTH|DATA_INCREMENT|DATA_DECREMENT|NON_REGISTERED_PARAMETER_LSB|NON_REGISTERED_PARAMETER_MSB|REGISTERED_PARAMETER_LSB|REGISTERED_PARAMETER_MSB|ALL_SOUND_OFF|RESET_ALL_CONTROLLERS|LOCAL_CONTROL|ALL_NOTES_OFF|OMNI_OFF|OMNI_ON|MONO_ON|POLY_ON|LSB_([[:digit:]]{2}))
CONTROL_VALUE (AZIMUTH|ELEVATION|GAIN|DISTANCE|MAXIMUM_DISTANCE|GAIN_AT_MAX_DISTANCE|REFERENCE_DISTANCE_RATIO|PAN_SPREAD_ANGLE|ROLL_ANGLE)

LBRACE [{]
RBRACE [}]
LEFTBRACKET "["
RIGHTBRACKET "]"
COMMENTCHAR ";"
COMMENT ({COMMENTCHAR}{STRING}\n)
skip [ \t,]*

FILEHEADERCONTEXT (FILEHEADER{SPACE})
FILEFORMAT (MONOTRACK|MULTITRACK|MULTISEQUENCE)
STARTTRACKTOKEN (STARTTRACK|\{)
END_OF_TRACKTOKEN (END_OF_TRACK|\})

ALTERNATE_CONTROL_CHANNEL (ALL_SOUND_OFF|RESET_ALL_CONTROLLERS|ALL_NOTES_OFF|OMNI_OFF|OMNI_ON|POLY_ON)
ALTERNATE_CONTROL_CHANNEL_INT (BREATH|DAMPER|SOFTPEDAL|MONO_ON)
ALTERNATE_CONTROL_CHANNEL_ONOFF (LEGATO_FOOT|PORTAMENTO|SOSTENUTO|LOCAL_CONTROL)

PATCH_TYPE (GM1|GM2|DLS)

SYSEX_SUBID (NON_COMMERCIAL|NON_REALTIME|REALTIME)
SYSEX_NONRT_SUBID1 (NONRT_SAMPLE_DUMP_HEADER|NONRT_SAMPLE_DATA_PACKET|NONRT_SAMPLE_DUMP_REQUEST|NONRT_TIMECODE|NONRT_SAMPLE_DUMP_EXTENSIONS|NONRT_GENERAL_INFO|NONRT_FILE_DUMP|NONRT_TUNING_STD|NONRT_GM|NONRT_END_OF_FILE|NONRT_WAIT|NONRT_CANCEL|NONRT_NAK|NONRT_ACK)
SYSEX_RT_SUBID1 (RT_TIMECODE|RT_SHOW_CONTROL|RT_NOTATION_INFORMATION|RT_DEVICE_CONTROL|RT_MTC_CUEING|RT_MACHINE_CONTROL_COMMANDS|RT_MACHINE_CONTROL_RESPONSES|RT_TUNING_STD)
DEVICE_ID (ALL_CALL|{BYTE})

%s FILE_HEADER_STATE
%s LAZY_STATE
%s DETAIL_STATE
%s TEXT_META_STATE
%s TICKS_STATE
%s SYSEX_SUBID_STATE
%s SYSEX_DEVICE_ID_STATE
%s SYSEX_SUBID1_STATE
%s SYSEX_REMAINDER_STATE

%option noyywrap

%%
<INITIAL>{FILEHEADERCONTEXT} {
    BEGIN(FILE_HEADER_STATE);
  }

<FILE_HEADER_STATE>{INTEGER}{SOMESPACE}{INTEGER}{SOMESPACE}{FILEFORMAT} {
    MidiHeader midi_header{};
    istringstream iss(yytext);
    string format_str;
    iss >> midi_header.ntrks_ >> midi_header.division_ >> format_str;
    to_upper(format_str);
    const auto format = format_map(format_str);
    if (format)
    {
        midi_header.format_ = *format;
    }
    else
    {
        string errstr{((((string{textmidi_features->text_filename_} += ':')
            += lexical_cast<string>(textmidi_features->line_ctr_))
            += "File format was: ") += format_str) += " but must be in {"};
        for (const auto& fmp : midi::format_map.string_num_map())
        {
            (errstr += fmp.first) += ", ";
        }
        errstr += "} or omitted for a default of MULTITRACK\n";
        cerr << errstr;
    }
    if ((MIDI_Format::MonoTrack == midi_header.format_)
        && (midi_header.ntrks_ > 1))
    {
        const string errstr{((((string{textmidi_features->text_filename_} += ':')
            += lexical_cast<string>(textmidi_features->line_ctr_))
            +=  " File format was MONOTRACK which allows only one track but there are ")
            += lexical_cast<string>(midi_header.ntrks_)) += '\n'};
        cerr << errstr;
    }
    global_ticks_per_quarter = midi_header.division_;
    midi_header.swapbytes();
    textmidi_features->midi_filestr_.write(io_bytes(midi_header), sizeof(MidiHeader));
    BEGIN(DETAIL_STATE);
  }

<FILE_HEADER_STATE>{INTEGER}{SOMESPACE}{INTEGER} {
    MidiHeader midi_header{};
    midi_header.format_ = MIDI_Format::MultiTrack;
    istringstream iss(yytext);
    iss >> midi_header.ntrks_ >> midi_header.division_;
    global_ticks_per_quarter = midi_header.division_;
    midi_header.swapbytes();
    textmidi_features->midi_filestr_.write(io_bytes(midi_header), sizeof(MidiHeader));
    BEGIN(DETAIL_STATE);
  }


<DETAIL_STATE>LAZY|BRIEF {
    BEGIN(LAZY_STATE);
  }

<LAZY_STATE>END_LAZY|DETAIL|END_BRIEF {
    BEGIN(DETAIL_STATE);
  }

<DETAIL_STATE>{STARTTRACKTOKEN} {
    ++track_counter;
    if (textmidi_features->verbose_)
    {
        clog << "Found " << yytext << '\n';
    }
    trackchunk.clear();
    trackchunk.reserve(33000); // about the longest track I've seen
    trackchunk.push_back('M');
    trackchunk.push_back('T');
    trackchunk.push_back('r');
    trackchunk.push_back('k');
    trackchunk.push_back(0);
    trackchunk.push_back(0);
    trackchunk.push_back(0);
    trackchunk.push_back(0);
    total_beats = 0;
    global_total_track_ticks = 0.0;
    // Invalidate running_status at the start of a track.
    running_status.clear();
  }

<DETAIL_STATE>SEQUENCER_SPECIFIC{SOMESPACE}{BYTE_SEQ} {
    write_delay();
    istringstream iss{yytext};
    string next_word{};
    iss >> next_word;
    vector<string> byte_strings;
    while (iss)
    {
        iss >> next_word;
        if (iss && !next_word.empty())
        {
            byte_strings.push_back(next_word);
        }
    }
    vector<unsigned> bytes{};
    bytes.reserve(byte_strings.size());
    ranges::transform(byte_strings, back_inserter(bytes), HexOrDec());

    trackchunk.push_back(meta_prefix[0]);
    trackchunk.push_back(sequencer_specific_prefix[0]);
    const MidiStreamVector length_seq = int_to_vec(bytes.size());
    ranges::copy(length_seq, back_inserter(trackchunk));
    ranges::copy(bytes, back_inserter(trackchunk));
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
}

<DETAIL_STATE>SEQUENCE_NUMBER{SOMESPACE}{UINTEGER} {
    write_delay();
    istringstream iss(yytext);
    string meta_name{};
    int sequence_number{};
    iss >> meta_name >> sequence_number;
    trackchunk.push_back(meta_prefix[0]);
    trackchunk.push_back(sequence_number_prefix[0]);
    trackchunk.push_back(sequence_number_prefix[1]);
    MidiStreamAtom msb{static_cast<MidiStreamAtom>((sequence_number >> 8) & octet_mask)};
    MidiStreamAtom lsb{static_cast<MidiStreamAtom> (sequence_number       & octet_mask)};
    trackchunk.push_back(msb);
    trackchunk.push_back(lsb);
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>UNKNOWN_META{SOMESPACE}{BYTE_SEQ} {
    write_delay();

    istringstream iss{yytext};
    string next_word;
    iss >> next_word; // "UNKNOWN_META"
    iss >> next_word; // an unassigned meta code

    unsigned meta_code{};
    meta_code = HexOrDec()(next_word);

    vector<string> byte_strings{};
    while (iss)
    {
        iss >> next_word;
        if (iss && !next_word.empty())
        {
            byte_strings.push_back(next_word);
        }
    }

    const MidiStreamVector len_vec = int_to_vec(byte_strings.size());

    vector<unsigned> bytes{};
    bytes.reserve(byte_strings.size());
    ranges::transform(byte_strings, back_inserter(bytes), HexOrDec());
    // push the data byters onto the MIDI stream.
    trackchunk.push_back(meta_prefix[0]);
    trackchunk.push_back(meta_code);
    ranges::copy(len_vec, back_inserter(trackchunk));

    ranges::copy(bytes, back_inserter(trackchunk));
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>{TEXT_META}{SOMESPACE} {
    write_delay();
    istringstream iss(yytext);
    string meta_name{};
    iss >> meta_name;
    to_upper(meta_name);

    const auto code = text_meta_map(meta_name);
    if (code)
    {
        trackchunk.push_back(meta_prefix[0]);
        trackchunk.push_back(*code);
        BEGIN(TEXT_META_STATE);
    }
    else
    {
        if (string_view("TICKS") == meta_name)
        {
            trackchunk.push_back(meta_prefix[0]);
            trackchunk.push_back(text_0F_prefix[0]);
            running_status.clear();
            BEGIN(TICKS_STATE);
        }
        else
        {
            const string errstr{((((string{textmidi_features->text_filename_} += ':')
                += lexical_cast<string>(textmidi_features->line_ctr_))
                += " Invalid meta event name ") += meta_name) += '\n'};
            cerr << errstr;
        }
    }
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<TEXT_META_STATE>{LINE} {
    string yystring{yytext};
    make_raw_string(yystring);
    MidiStreamVector len_vec = int_to_vec(yystring.length());
    ranges::copy(len_vec,  back_inserter(trackchunk));
    ranges::copy(yystring, back_inserter(trackchunk));
    ++textmidi_features->line_ctr_;

    BEGIN(DETAIL_STATE);
  }

<TICKS_STATE>{LINE} {
    string yystring{yytext};
    string ticks_str{};
    (((ticks_str += "Ticks: ")
        += lexical_cast<string>(global_total_track_ticks)) += ' ') += yytext;
    make_raw_string(ticks_str);
    if (textmidi_features->verbose_)
    {
        cout << ticks_str << '\n';
    }
    MidiStreamVector len_vec = int_to_vec(ticks_str.length());
    ranges::copy(len_vec,  back_inserter(trackchunk));
    ranges::copy(ticks_str, back_inserter(trackchunk));
    ++textmidi_features->line_ctr_;

    BEGIN(DETAIL_STATE);
  }

<DETAIL_STATE,LAZY_STATE>{END_OF_TRACKTOKEN} {
    write_delay();
    trackchunk.push_back(meta_prefix[0]);
    trackchunk.push_back(end_of_track_prefix[0]);
    trackchunk.push_back(end_of_track_prefix[1]);
    const auto sizetemp{trackchunk.size() - 8};
    trackchunk[4] = (sizetemp >> 24) & 0xFF;
    trackchunk[5] = (sizetemp >> 16) & 0xFF;
    trackchunk[6] = (sizetemp >>  8) & 0xFF;
    trackchunk[7] = (sizetemp >>  0) & 0xFF;
    ranges::copy(trackchunk, ostream_iterator<MidiStreamAtom>(textmidi_features->midi_filestr_));
    accum_delay = 0;
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
    BEGIN(DETAIL_STATE);
  }

<DETAIL_STATE>TEMPO{SPACE}{FLOAT} {
    write_delay();
    istringstream iss(yytext);
    string discard{};
    double mmtempo{60.0};
    iss >> discard >> mmtempo;
    // convert ticks per minute to useconds per quarter
    const uint32_t tempo{static_cast<uint32_t>(std::round((SecondsPerMinute * UsecPerSecond) / mmtempo))};

    const MidiStreamVector tempovec{int24_to_vec(tempo)};
    trackchunk.push_back(meta_prefix[0]);

    ranges::copy(tempo_prefix, back_inserter(trackchunk));
    ranges::copy(tempovec, back_inserter(trackchunk));
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>DELAY{SPACE}{INTEGER} {
    istringstream iss(yytext);
    string discard{};
    uint32_t delay{};
    iss >> discard >> delay;
    accum_delay += delay;
    global_total_track_ticks += static_cast<double>(delay);
  }

<DETAIL_STATE>NOTE_ON{SOMESPACE}{INTEGER}{SOMESPACE}{PITCH_NAME}{SOMESPACE}{INTEGER} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    uint32_t channel{}, // [1..16]
             key{},
             velocity{};
    string pitch_name;
    iss >> discard >> channel >> pitch_name >> velocity;

    channel_1_to_0(channel);

    bool is_delta{};
    int signed_key{};
    tie(signed_key, is_delta) = pitchname_to_keynumber(pitch_name);
    if (is_delta)
    {
        key = static_cast<unsigned>(last_key + signed_key);
        last_key = static_cast<int>(key);
    }
    else
    {
        key = static_cast<unsigned>(signed_key);
        last_key = static_cast<int>(key);
    }

    running_status(channel | note_on[0], trackchunk);

    key      &= byte7_mask;
    velocity &= byte7_mask;
    trackchunk.push_back(key);
    trackchunk.push_back(velocity);
  }

<DETAIL_STATE>NOTE_OFF{SPACE}{INTEGER}{SPACE}{PITCH_NAME}{SPACE}{INTEGER} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    uint32_t channel{},
             key{},
             velocity{};
    string pitch_name;
    iss >> discard >> channel >> pitch_name >> velocity;
    channel_1_to_0(channel);

    bool is_delta{};
    int signed_key{};
    tie(signed_key, is_delta) = pitchname_to_keynumber(pitch_name);
    if (is_delta)
    {
        key = static_cast<unsigned>(last_key + signed_key);
        last_key = static_cast<int>(key);
    }
    else
    {
        key = static_cast<unsigned>(signed_key);
        last_key = static_cast<int>(key);
    }
    running_status(channel | note_off[0], trackchunk);

    key &= byte7_mask;
    velocity &= byte7_mask;
    trackchunk.push_back(key);
    trackchunk.push_back(velocity);
  }


<DETAIL_STATE>POLY_KEY_PRESSURE{SPACE}{INTEGER}{SPACE}{PITCH_NAME}{SPACE}{INTEGER} {
    istringstream iss(yytext);
    string discard;
    uint32_t channel{},
             key{},
             pressure{};
    string pitch_name;
    iss >> discard >> channel >> pitch_name >> pressure;
    channel_1_to_0(channel);

    bool is_delta{};
    int signed_key{};
    tie(signed_key, is_delta) = pitchname_to_keynumber(pitch_name);
    if (is_delta)
    {
        key = static_cast<unsigned>(last_key + signed_key);
        last_key = static_cast<int>(key);
    }
    else
    {
        key = static_cast<unsigned>(signed_key);
        last_key = static_cast<int>(key);
    }

    write_delay();
    running_status(channel | polyphonic_key_pressure[0], trackchunk);

    key &= byte7_mask;
    pressure &= byte7_mask;
    trackchunk.push_back(key);
    trackchunk.push_back(pressure);
  }

<DETAIL_STATE>CONTROL{SPACE}{BYTE}{SPACE}({BYTE}|{CONTROL_FUNCTION}){SPACE}({CONTROL_VALUE}|{BYTE}|{ONOFF}) {
    write_delay();

    istringstream iss(yytext);
    string discard;
    uint32_t channel{};

    string control_id_str{},
           control_value_str{};
    iss >> discard >> channel >> control_id_str >> control_value_str;

    channel_1_to_0(channel);
    running_status(channel | control[0], trackchunk);

    string control_id_strv{control_id_str};
    to_upper(control_id_strv);
    const auto id_code = control_function_map(control_id_strv);
    int32_t control_id{};
    if (id_code)
    {
        control_id = *id_code;
    }
    else
    {
        control_id = HexOrDec()(control_id_str);
    }
    control_id &= byte7_mask;
    trackchunk.push_back(control_id);

    bool was_mapped{};
    auto upper_control_value_str{control_value_str};
    to_upper(upper_control_value_str);
    const auto on_off_code = control_on_off_map(upper_control_value_str);
    int32_t control_value{};
    if (on_off_code)
    {
        control_value = *on_off_code;
        was_mapped = true;
    }
    else
    {
        if (control_registered_parameter_lsb[0] == control_id)
        {
            if (const auto control_value_code = parm_3d_map(upper_control_value_str))
            {
                control_value = static_cast<int>(*control_value_code);
                was_mapped = true;
            }
            else
            {
                if (const auto control_value_code = parm_00_map(upper_control_value_str))
                {
                    control_value = static_cast<int>(*control_value_code);
                    was_mapped = true;
                }
            }
        }
        // handle MIDI 3D
        if (!was_mapped)
        {
            control_value = HexOrDec()(control_value_str);
        }
    }
    // If "PAN" was used, add Excess64
    if (id_code && (control_pan[0] == control_id))
    {
        control_value += PanExcess64;
    }
    control_value &= byte7_mask;
    trackchunk.push_back(control_value);
  }

<DETAIL_STATE>CONTROL{SPACE}{INTEGER}{SPACE}PAN{SPACE}({INTEGER}|{PANVALUE}) {
    write_delay();

    istringstream iss(yytext);
    string discard;

    const auto control_id{control_pan[0]};
    int32_t control_value{};
    string control_value_str{};
    string discard_control_id_str;
    uint32_t channel{};
    iss >> discard >> channel >> discard_control_id_str >> control_value_str;
    channel_1_to_0(channel);
    running_status(channel | control[0], trackchunk);
    trackchunk.push_back(control_id);

    to_upper(control_value_str);
    const auto position = pan_map(control_value_str);
    if (position)
    {
        control_value = *position;
    }
    else
    {
        control_value = lexical_cast<int32_t>(control_value_str);
    }
    control_value += PanExcess64;
    control_value &= byte7_mask;
    trackchunk.push_back(control_value);
  }

<DETAIL_STATE>{ALTERNATE_CONTROL_CHANNEL}{SPACE}{INTEGER} {
    write_delay();

    string control_id_str{};
    int32_t control_id{};
    uint32_t channel{};
    istringstream iss{yytext};
    iss >> control_id_str >> channel;
    to_upper(control_id_str);
    channel_1_to_0(channel);
    running_status(channel | control[0], trackchunk);

    const auto id = control_function_map(control_id_str);
    if (id)
    {
        control_id = *id;
    }
    trackchunk.push_back(control_id);
    trackchunk.push_back(0);
  }

<DETAIL_STATE>{ALTERNATE_CONTROL_CHANNEL_INT}{SPACE}{INTEGER}{SPACE}{UXINTEGER} {
    write_delay();

    string control_id_str;
    uint32_t channel{};
    string control_value_str{};
    istringstream iss(yytext);
    iss >> control_id_str >> channel >> control_value_str;
    channel_1_to_0(channel);
    running_status(channel | control[0], trackchunk);

    to_upper(control_id_str);
    const auto id = control_function_map(control_id_str);
    int32_t control_id{};
    if (id)
    {
        control_id = *id;
    }
    trackchunk.push_back(control_id);

    int32_t control_value{};
    control_value = HexOrDec()(control_value_str);
    control_value &= byte7_mask;
    trackchunk.push_back(control_value);
  }

<DETAIL_STATE>{ALTERNATE_CONTROL_CHANNEL_ONOFF}{SPACE}{INTEGER}{SPACE}({UXINTEGER}|{ONOFF}) {
    write_delay();

    string control_id_str;
    uint32_t channel{};
    string control_value_str{};
    istringstream iss(yytext);
    iss >> control_id_str >> channel >> control_value_str;
    // Can't upcase the "x" in hexadecimal 0x00 and use stream conversion on it.
    auto upper_control_value_str{control_value_str};
    to_upper(upper_control_value_str);
    to_upper(control_id_str);
    channel_1_to_0(channel);
    running_status(channel | control[0], trackchunk);

    // Can't fail; the grammar would not have qualified an invalid name.
    const auto id = control_function_map(control_id_str);
    trackchunk.push_back(*id);

    int control_value{};
    const auto val = control_on_off_map(upper_control_value_str);
    if (val)
    {
        control_value = *val;
    }
    else
    {
        control_value = HexOrDec()(control_value_str);
    }
    trackchunk.push_back(control_value);
  }

<DETAIL_STATE>PAN{SPACE}{INTEGER}{SPACE}{PANVALUE} {
    write_delay();

    string discard{};
    string panstring{};
    uint32_t channel{};
    istringstream iss(yytext);
    iss >> discard >> channel >> panstring;
    channel_1_to_0(channel);
    running_status(channel | control[0], trackchunk);
    trackchunk.push_back(control_pan[0]);

    to_upper(panstring);
    const auto pan = pan_map(panstring);
    int32_t pancode{};
    if (pan)
    {
        pancode = *pan;
    }
    else
    {
        pancode = lexical_cast<int>(panstring);
    }
    pancode += PanExcess64;
    pancode &= byte7_mask;
    trackchunk.push_back(pancode);
  }

<DETAIL_STATE>PROGRAM{SPACE}{INTEGER}{SPACE}{INTEGER} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    uint32_t channel{},
             theprogram{};
    iss >> discard >> channel >> theprogram;
    channel_1_to_0(channel);
    running_status(channel | program[0], trackchunk);

    program_1_to_0(theprogram);
    theprogram &= byte7_mask;
    trackchunk.push_back(theprogram);
  }

<DETAIL_STATE>CHANNEL_PRESSURE{SPACE}{INTEGER}{SPACE}{INTEGER} {
    write_delay();
    istringstream iss(yytext);
    string discard;
    uint32_t channel{},
             pressure{};
    iss >> discard >> channel >> pressure;
    channel_1_to_0(channel);
    running_status(channel | channel_pressure[0], trackchunk);

    pressure &= byte7_mask;
    trackchunk.push_back(pressure);
  }

<DETAIL_STATE>PITCH_WHEEL{SPACE}{INTEGER}{SPACE}{INTEGER} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    uint32_t channel {},
             bend{};
    iss >> discard >> channel >> bend;
    channel_1_to_0(channel);

    running_status(channel | pitch_wheel[0], trackchunk);

    uint32_t bendlsb{},
             bendmsb{};
    bendlsb = bend & byte7_mask;
    trackchunk.push_back(bendlsb);
    bendmsb = (bend >> byte7_shift) & byte7_mask;
    trackchunk.push_back(bendmsb);
  }

<DETAIL_STATE>TIME_SIGNATURE{SPACE}{INTEGER}{SPACE}{INTEGER}{SPACE}{INTEGER} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    uint32_t numer{},
             denom{},
             ticksperbeat{};
    constexpr uint32_t thirtysecsperqtr{8};
    iss >> discard >> numer >> denom >> ticksperbeat;
    // convert ticks per minute to useconds per quarter
    uint32_t denom_pwr{};
    while (denom >>= 1)
    {
      ++denom_pwr;
    }

    trackchunk.push_back(meta_prefix[0]);

    ranges::copy(time_signature_prefix, back_inserter(trackchunk));
    trackchunk.push_back(numer);
    trackchunk.push_back(denom_pwr);
    auto tempticksperbeat{ticksperbeat};
    while (tempticksperbeat > numeric_limits<MidiStreamAtom>().max())
    {
        tempticksperbeat /= 2;
    }
    trackchunk.push_back(tempticksperbeat);
    trackchunk.push_back(thirtysecsperqtr);
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>KEY_SIGNATURE{SPACE}{NOTE_NAME} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    string key_sig_name;
    iss >> discard >> key_sig_name;
    int32_t themode{};
    int32_t accidentals;
    // tie is C++ tie for 2 returns, not musical tie.
    tie(accidentals, themode) = key_sig_name_to_accidentals(key_sig_name);

    trackchunk.push_back(meta_prefix[0]);
    trackchunk.push_back(key_signature_prefix[0]);
    trackchunk.push_back(key_signature_prefix[1]);
    trackchunk.push_back(static_cast<signed char>(accidentals));
    trackchunk.push_back(themode);
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>XMF_PATCH_TYPE{SPACE}{PATCH_TYPE} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    string patch_type_str;
    iss >> discard >> patch_type_str;

    to_upper(patch_type_str);
    // This can't fail because the grammar has validated the line.
    const MidiStreamAtom xmf_patch_type{
        static_cast<MidiStreamAtom>(xmf_patch_type_map.at(patch_type_str))};

    trackchunk.push_back(meta_prefix[0]);
    ranges::copy(xmf_patch_type_prefix, back_inserter(trackchunk));
    trackchunk.push_back(xmf_patch_type);
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>SMPTE_OFFSET{SPACE}{SMPTE_CODE} {
    write_delay();

    string yystring{yytext},
           discard;
    const regex smpteoffset_re{R"(SMPTE_OFFSET([[:space:]]+)(\w+)([[:space:]])+([[:digit:]]{2,4}):([[:digit:]]{2}):([[:digit:]]{2}):([[:digit:]]{2}):([[:digit:]]{2}))"};
    constexpr int fps_match{2};
    constexpr int hours_match{4};
    constexpr int minutes_match{5};
    constexpr int ff_match{8};

    //const regex smpteoffset_re{R"(SMPTE_OFFSET[[:space:]]+.*)"};
    smatch matches{};
    const auto mat{regex_match(yystring, matches, smpteoffset_re)};
    if (!mat)
    {
        cerr << "Improper SMPTE string: " << yystring << '\n';
    }
    else
    {
        // values in [2]..
        MidiStreamVector offsetvalues;
        MidiStreamAtom fps{};
        string fps_str(matches[fps_match].str());
        to_upper(fps_str);
        const auto the_fps = smpte_fps_map(fps_str);
        if (the_fps)
        {
            fps = *the_fps;
        }
        int hours{lexical_cast<int>(matches[hours_match])};
        hours |= (fps << smpte_fps_shift);
        offsetvalues.push_back(hours);
        for (auto i{minutes_match}; i < (ff_match + 1); ++i)
        {
            offsetvalues.push_back(lexical_cast<int>(matches[i]));
        }
        trackchunk.push_back(meta_prefix[0]);
        trackchunk.push_back(smpte_prefix[0]);
        trackchunk.push_back(smpte_prefix[1]);
        for (auto offsetval : offsetvalues)
        {
            trackchunk.push_back(offsetval);
        }
    }
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>MIDI_CHANNEL{SPACE}{UINTEGER} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    uint32_t channel{};
    iss >> discard >> channel;
    channel_1_to_0(channel);

    trackchunk.push_back(meta_prefix[0]);
    trackchunk.push_back(midi_channel_prefix[0]);
    trackchunk.push_back(midi_channel_prefix[1]);
    trackchunk.push_back(channel);
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>MIDI_PORT{SPACE}{UINTEGER} {
    write_delay();

    istringstream iss(yytext);
    string discard;
    int32_t port{};
    iss >> discard >> port;

    trackchunk.push_back(meta_prefix[0]);
    trackchunk.push_back(midi_port_prefix[0]);
    trackchunk.push_back(midi_port_prefix[1]);
    trackchunk.push_back(port);
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>SYSEX{SOMESPACE}{BYTE_SEQ} {
    write_delay();

    istringstream iss{yytext};
    string next_word{};
    iss >> next_word;
    vector<string> byte_strings;
    while (iss)
    {
        iss >> next_word;
        if (iss && !next_word.empty())
        {
            byte_strings.push_back(next_word);
        }
    }
    vector<unsigned> byte_ints{};
    byte_ints.reserve(byte_strings.size());
    ranges::transform(byte_strings, back_inserter(byte_ints), HexOrDec());
    // push the length of data plus the F7 at end
    // of the MIDI file stream.
    MidiStreamVector bytes{};
    bytes.reserve(byte_ints.size());
    ranges::transform(byte_ints, back_inserter(bytes),
        [](int i) { return static_cast<MidiStreamAtom>(i); } );
    sysex_event_sequence.remaining(bytes);
    sysex_event_sequence.build_msg();
    sysex_event_sequence.append(trackchunk);
    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
  }

<DETAIL_STATE>SYSEX{SOMESPACE}{SYSEX_SUBID} {
    write_delay();
    string str{};
    istringstream iss{yytext};
    iss >> str;
    iss >> str;
    MidiStreamVector vec;
    // this can't fail because flex's analyzer
    // has already identified this as a subid.
    vec.push_back(sysex_subid_map.at(str));
    sysex_event_sequence.subid(vec);
    BEGIN(SYSEX_DEVICE_ID_STATE);
  }

<SYSEX_SUBID_STATE>{SOMESPACE}({ZERO_HEX_BYTE}{SOMESPACE}({ZERO_HEX_BYTE}|{THIRTYTWO_BYTE}))?{SOMESPACE}{XINTEGER} {
    string str{yytext};
    istringstream iss{yytext};
    string next_word{};
    vector<string> byte_strings{};
    while (iss)
    {
        iss >> next_word;
        if (iss && !next_word.empty())
        {
            byte_strings.push_back(next_word);
        }
    }
    MidiStreamVector bytes{};
    bytes.reserve(byte_strings.size());
    ranges::transform(byte_strings, back_inserter(bytes), HexOrDec());
    sysex_event_sequence.subid(bytes);
    BEGIN(SYSEX_SUBID1_STATE);
  }

<SYSEX_DEVICE_ID_STATE>{SOMESPACE}{DEVICE_ID} {
    unsigned int xb{};
    string yystr(yytext);
    const auto nonspace{yystr.find_first_not_of(" \t")};
    const string token{yystr.substr(nonspace)};
    string token_upper{token};
    to_upper(token_upper);
    const auto id = sysex_device_id_map(token_upper);
    if (id)
    {
        xb = *id;
    }
    else
    {
        // is it hexadecimal?
        xb = HexOrDec()(token);
    }
    MidiStreamVector xbvec{static_cast<uint8_t>(xb)};
    sysex_event_sequence.device_id(xbvec);
    BEGIN(SYSEX_SUBID1_STATE);
  }

<SYSEX_SUBID1_STATE>{SOMESPACE}{UXINTEGER} {
    string yystr(yytext);
    const auto pos{yystr.find_first_not_of(" \t")};
    const string token{yystr.substr(pos)};
    const int xb{HexOrDec()(token)};
    MidiStreamVector xbvec{static_cast<uint8_t>(xb)};
    sysex_event_sequence.subid1(xbvec);
    BEGIN(SYSEX_REMAINDER_STATE);
  }

<SYSEX_SUBID1_STATE>{SOMESPACE}{SYSEX_NONRT_SUBID1} {
    string yystr{yytext};
    const auto pos{yystr.find_first_not_of(" \t")};
    const string token{yystr.substr(pos)};
    const MidiStreamVector token_vec{sysex_nonrt_id1_map.at(token)};
    sysex_event_sequence.subid1(token_vec);
    BEGIN(SYSEX_REMAINDER_STATE);
  }

<SYSEX_SUBID1_STATE>{SOMESPACE}{SYSEX_RT_SUBID1} {
    string yystr{yytext};
    const auto pos{yystr.find_first_not_of(" \t")};
    const string token{yystr.substr(pos)};
    const MidiStreamVector token_vec{sysex_rt_id1_map.at(token)};
    sysex_event_sequence.subid1(token_vec);
    BEGIN(SYSEX_REMAINDER_STATE);
  }

<SYSEX_REMAINDER_STATE>{SOMESPACE}{BYTE_SEQ} {
    istringstream iss{yytext};
    string yystr{yytext};
    const auto pos{yystr.find_first_not_of(" \t")};
    yystr.erase(0, pos);
    string next_word{};
    vector<string> byte_strings;
    while (iss)
    {
        iss >> next_word;
        if (iss && !next_word.empty())
        {
            byte_strings.push_back(next_word);
        }
    }
    MidiStreamVector bytes{};
    bytes.reserve(byte_strings.size());
    ranges::transform(byte_strings, back_inserter(bytes), HexOrDec());
    sysex_event_sequence.remaining(bytes);
    sysex_event_sequence.build_msg();
    sysex_event_sequence.append(trackchunk);

    // "Meta events and sysex events cancel any running status that was in effect."
    // RP-001_v1-0_Standard_MIDI_Files_Specification_96-1-4.pdf page 7 bottom.
    running_status.clear();
    BEGIN(DETAIL_STATE);
  }

<DETAIL_STATE>SYSEXRAW{SOMESPACE}{BYTE_SEQ} {
    write_delay();

    string next_word{};
    istringstream iss{yytext};
    iss >> next_word;
    vector<string> byte_strings;
    while (iss)
    {
        iss >> next_word;
        if (iss && !next_word.empty())
        {
            byte_strings.push_back(next_word);
        }
    }

    vector<unsigned> bytes{};
    bytes.reserve(byte_strings.size());
    ranges::transform(byte_strings, back_inserter(bytes), HexOrDec());

    // pushd the length of data plus the F7 at end
    // of the MIDI file stream.
    trackchunk.push_back(end_of_sysex[0]); // Raw SYSEX *starts* with F7
    // length of data; no implied terminating F0
    const MidiStreamVector length_seq = int_to_vec(bytes.size());
    ranges::copy(length_seq, back_inserter(trackchunk));
    ranges::copy(bytes, back_inserter(trackchunk));
  }

<LAZY_STATE>{DYNAMIC} {
    string dynamic_str{yytext};

    to_lower(dynamic_str);
    // This use of the map must work because we can only
    // be in here if the dynamic marking is one of the ones
    // in the map; we will not call contains().
    const auto dynamic{dynamics_map.at(dynamic_str)};
    lazy::dynamic = dynamic % 128;
  }

<LAZY_STATE>vel{SPACE}{INTEGER} {
    string velocity_str(yytext), directive;
    string intstr;
    istringstream iss(velocity_str);
    iss >> directive >> intstr;
    int64_t vel_number{boost::lexical_cast<int64_t>(intstr)};
    int64_t temp_dynamic{lazy::dynamic};
    switch (intstr[0])
    {
      case '+': // delta change
      case '-': // delta change
        temp_dynamic += vel_number;
        break;
      [[likely]] default:  // absolute setting
        temp_dynamic = vel_number;
        break;
    }
    lazy::dynamic = static_cast<uint32_t>(temp_dynamic) % 128;
  }

<LAZY_STATE>chan{SPACE}{INTEGER} {
    string channel_str(yytext),
           discard;
    uint32_t channel{1U}; // text channels are in [1..16]
    istringstream iss(channel_str);
    iss >> discard >> channel;
    channel_1_to_0(channel);

    if (running_status.channel() != global_channel)
    {
        running_status.clear();
    }
  }

<LAZY_STATE>{STARTLINE_OR_SPACE}{LAZY_PITCH_NAME} {
    string note_name{};
    istringstream iss(yytext);
    iss >> note_name;
    KeyTie keytie{note_name};

    // write note event
    if (!keytie.tie_in_)
    {
        write_delay();
        running_status((global_channel & channel_mask) | note_on[0], trackchunk);
        trackchunk.push_back(keytie.key_);
        trackchunk.push_back(lazy::dynamic);
    /* Recognize chan {channel} */
    }
    lazy::chord.push_back(keytie);    // save key number in lazy::chord;
  }

<LAZY_STATE>{NON_NOTE_LETTER}{LAZY_PITCH_NAME} {
    const string errstr{((((((((((string{textmidi_features->text_filename_} += ':')
        += lexical_cast<string>(textmidi_features->line_ctr_))
        += " Non Key Letter before Key Name: ")
        += BOOST_PP_STRINGIZE(__LINE__))
        += ' ') += yytext) += " in track ") += lexical_cast<string>(track_counter))
        += " at beat ") += lexical_cast<string>(total_beats + (accum_delay / global_ticks_per_quarter)))
        += '\n'};
    cerr << errstr;
    exit(EXIT_SUCCESS);
  }

<DETAIL_STATE>(-{PITCH_NAME})|({PITCH_NAME}-) {
    const string errstr{((((((((((string{textmidi_features->text_filename_} += ':')
        += lexical_cast<string>(textmidi_features->line_ctr_))
        += " Ties are not allowed in DETAIL mode: ")
        += BOOST_PP_STRINGIZE(__LINE__))
        += ' ') += yytext) += " in track ") += lexical_cast<string>(track_counter))
        += " at beat ") += lexical_cast<string>(total_beats + (accum_delay / global_ticks_per_quarter)))
        += '\n'};
    cerr << errstr;
    exit(EXIT_SUCCESS);
  }

<DETAIL_STATE>{NON_NOTE_LETTER}{PITCH_NAME} {
    const string errstr{((((((((((string{textmidi_features->text_filename_} += ':')
        += lexical_cast<string>(textmidi_features->line_ctr_))
        += " Non-Note Letter before Note Name: ")
        += BOOST_PP_STRINGIZE(__LINE__))
        += ' ') += yytext) += " in track ") += lexical_cast<string>(track_counter))
        += " at beat ") += lexical_cast<string>(total_beats + (accum_delay / global_ticks_per_quarter)))
        += '\n'};
    cerr << errstr;
    exit(EXIT_SUCCESS);
  }

<LAZY_STATE>{SPACE}{NOTE_NAME}{SOMESPACE}{LAZY_PITCH_NAME} {
    const string errstr{((((((((((string{textmidi_features->text_filename_} += ':')
        += lexical_cast<string>(textmidi_features->line_ctr_))
        += " Key Name without octave: ")
        += BOOST_PP_STRINGIZE(__LINE__))
        += ' ') += yytext) += " in track ") += lexical_cast<string>(track_counter))
        += " at beat ") += lexical_cast<string>(total_beats + (accum_delay / global_ticks_per_quarter)))
        += '\n'};
    cerr << errstr;
  }

<LAZY_STATE>{SPACE}{NOTE_NAME}{SOMESPACE}{RHYTHM} {
    const string errstr{((((((((((string{textmidi_features->text_filename_} += ':')
        += lexical_cast<string>(textmidi_features->line_ctr_))
        += " Key Name without octave: ")
        += BOOST_PP_STRINGIZE(__LINE__))
        += ' ') += yytext) += " in track ") += lexical_cast<string>(track_counter))
        += " at beat ") += lexical_cast<string>(total_beats + (accum_delay / global_ticks_per_quarter)))
        += '\n'};
    cerr << errstr;
  }

<LAZY_STATE>{SPACE}{RHYTHM} {
    const string rhythmstr{yytext};
    istringstream iss(rhythmstr);
    double num1{1.0},
           num2{1.0},
           thedivisor{1.0},
           thenumerator{1.0};
    uint32_t total_duration{},
             note_duration{};
    iss >> num1;

    // a quarter note is "4" or "1/4".  A double whole note must be "2/1".
    const size_t slashi{rhythmstr.find('/')};
    if (slashi != rhythmstr.npos)
    {
        // we have a ratio
        string num2str{rhythmstr, slashi + 1};
        istringstream num2stream{num2str};
        num2stream >> num2;
        thenumerator = num1;
        thedivisor   = num2;
    }
    else
    {
        // we have a single number, which we divide into 1.0
        thedivisor = num1;
        if (thedivisor > (global_ticks_per_quarter * QuartersPerWhole))
        {
            const string errstr{((((((string{textmidi_features->text_filename_}
                += ':') += lexical_cast<string>(textmidi_features->line_ctr_))
                += " A duration denominator that is > ticks per whole! ")
                += lexical_cast<string>(thedivisor)) += '>')
                += lexical_cast<string>(global_ticks_per_quarter * QuartersPerWhole))
                += '\n' };
            cerr << errstr;
        }
    }
    total_duration = delay_and_update_total_ticks(thenumerator, thedivisor,
            ('.' == rhythmstr[rhythmstr.size() - 1]),
            ('.' == rhythmstr[rhythmstr.size() - 2]));
    //
    // Subtract detache, to be added back in as a "rest".
    note_duration
        = total_duration - ((static_cast<double>(textmidi_features->detache_) < total_duration) ? textmidi_features->detache_ : 0);
    // write note offs for each note in the chord with 0 delays
    accum_delay += note_duration;

    for (const auto& keytie : lazy::chord)
    {
        if (!keytie.tie_out_)
        {
            write_delay(); // wait the duration of the note

            uint32_t channel{global_channel & channel_mask};
            const MidiStreamAtom note_cmd{textmidi_features->note_off_select_ ? note_off[0] : note_on[0]};
            const auto status_byte{channel | note_cmd};
            running_status(status_byte, trackchunk);

            trackchunk.push_back(keytie.key_ & byte7_mask);

            // If we are using a note_off, then use the prevailing dynamic; else 0.
            const MidiStreamAtom velocity{textmidi_features->note_off_select_
                ? static_cast<MidiStreamAtom>(lazy::dynamic) : note_off_velocity};
            trackchunk.push_back(velocity);
        }
    }
    lazy::chord.clear();
    if (static_cast<double>(textmidi_features->detache_) < total_duration)
    {
        accum_delay += textmidi_features->detache_;
    }
    BEGIN(LAZY_STATE);
  }

<LAZY_STATE>(r|R){SPACE}{RHYTHM} {
    string rhythmstr{yytext},
           discard{};

    istringstream stringstream{rhythmstr};
    double num1{1.0},
           num2{1.0},
           thedivisor{1.0},
           thenumerator{1.0};
    uint32_t total_duration{240};
    stringstream >> discard >> num1;
    size_t slashi{rhythmstr.find('/')};
    if (slashi != rhythmstr.npos)
    {
        // we have a ratio
        string num2str{rhythmstr, slashi + 1};
        istringstream num2stream{num2str};
        num2stream >> num2;
        thenumerator = num1;
        thedivisor   = num2;
    }
    else
    {
        // we have a single number, which we divide into 1.0
        thedivisor = num1;
        if (thedivisor > (global_ticks_per_quarter * QuartersPerWhole))
        {
            const string errstr{((((((string{textmidi_features->text_filename_} += ':')
                += lexical_cast<string>(textmidi_features->line_ctr_))
                += " A duration denominator that is > ticks per whole! ")
                += lexical_cast<string>(thedivisor)) += '>')
                += lexical_cast<string>(global_ticks_per_quarter * QuartersPerWhole)) += '\n' };
            cerr << errstr;
        }
    }
    total_duration = delay_and_update_total_ticks(thenumerator, thedivisor,
            ('.' == rhythmstr[rhythmstr.size() - 1]),
            ('.' == rhythmstr[rhythmstr.size() - 2]));
    accum_delay += total_duration;
  }

<DETAIL_STATE,LAZY_STATE>{COMMENT} {
    ++textmidi_features->line_ctr_;
  }
^{SOMESPACE}
{skip}* {
        }
(\n) {
    ++textmidi_features->line_ctr_;
  }
<<EOF>> {
    if (textmidi_features->verbose_)
    {
        clog << "Found " << track_counter
             << " tracks and will set MIDI header accordingly.\n";
    }
    textmidi_features->midi_filestr_.seekp(10);
    track_counter = htobe16(track_counter);
    textmidi_features->midi_filestr_.write(io_bytes(track_counter), sizeof(track_counter));
    yyterminate();
  }
.

%%

