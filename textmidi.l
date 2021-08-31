%{
/*
** textmidi.l 1.0
** Copyright © 2021 Thomas E. Janzen
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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "Midi.h" // MIDI codes and MIDI file header
#include "MIDIKeyString.h"
#include "textmidi.h" // For this file's interfaces to main.cc

using namespace std;
using namespace boost;
using namespace boost::algorithm; // for to_upper(string)
using namespace textmidi;

#if 0
extern "C" int32_t yywrap (void) {};
#endif

string text_filename{}; // set in main.cc as option
uint32_t line_ctr{1};   // printed in main.cc as option
uint32_t detache{10};   // set from cmd line; separation between notes
ofstream midi_filestr{}; // set in main.cc
bool verbose{};          // set in main.cc as option

namespace
{
    // The last MIDI key (note) that was played, init to middle C
    constexpr int middle_c{60};
    int last_key{middle_c};
    uint16_t track_counter{}; // Count the tracks in order to set MIDI header
    vector<uint8_t> trackchunk; // One track's data bytes.
    uint32_t accum_delay{}; // Delay accumulated between events.
    double global_total_track_ticks{}; // accum. tracks ticks to print for "ticks" cmd
    uint32_t total_beats{}; // beat to print for errors.
    // running_status is a MIDI trick to avoid repeating events such as NOTE_ON.
    uint8_t running_status{};
    // To turn off notes using NOTE_ON, use a zero velocity.
    constexpr uint8_t note_off_velocity{};
    constexpr double DotLength(0.5);
    constexpr double DoubleDotLength(0.25);
    // Map textmidi strings to MIDI file format codes.
    const map<const string_view, uint8_t> text_meta_map{
                  {"TEXT",         text_prefix},
                  {"COPYRIGHT",    copyright_prefix},
                  {"TRACK",        track_name_prefix},
                  {"INSTRUMENT",   instrument_name_prefix},
                  {"LYRIC",        lyric_prefix},
                  {"MARKER",       marker_prefix},
                  {"CUE_POINT",    cue_point_prefix}
              };
    const map<string_view, uint8_t> text_control_map{
        {"RESETALL",     control_reset_all_ctrl},
        {"ALLNOTES_OFF", control_all_notes_off},
        {"ALL_SOUND_OFF", control_all_sound_off},
        {"BREATH",       control_breath},
        {"DAMPER",       control_damper},
        {"SOFTPEDAL",    control_softpedal},
        {"PORTAMENTO",   control_portamento},
        {"SOSTENUTO",    control_sostenuto},
    };
    // PORTAMENTO and SOSTENUTO have an "on" or "off" value.
    const map<string, uint8_t> OnOffMap{{"OFF", control_off}, {"ON", control_full}};

    int global_ticks_per_quarter{384};
    // global_channel is part of implementing MIDI running status
    // because channel is part of a command.
    uint32_t global_channel{}; // [0..15]

    // MIDI Pan is actually excess 64,
    // but textmidi treats it as -64..0..63, where 0 is center.
    // In addition we allow the words "left", "center", and "right".
    const map<string_view, int> PanStringToValue{{"left", 0},
                                                 {"center", 64},
                                                 {"right", 127}};

    // keyevent_to_keynumber
    // converts a string keyevent, which can include tie-in and tie-out,
    // may be a note name such as "C4" or may be a key number as in "K60"
    // or may be a delta key as in "K+2".
    int keyevent_to_keynumber(const string& keyevent,
                              bool& tie_in, bool& tie_out, bool& is_delta)
    {
        int keynumber{};
        smatch matches{};
        //
        // Hard Lessons: you can't break a line with a backslash in the middle of a
        // Raw regular expression
        const regex keyevent_re{R"((-?)(((([A-Ga-g])(([b#x]?)|(bb)))(-?)([0-9]))|(([Kk])([+-]?)([[:digit:]]+)))(-?))"};
        auto sts{regex_match(keyevent, matches, keyevent_re)};
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


#if 0
    char
    tolowerf(char anycase)
    {
        return tolower(anycase);
    }
#endif

    //
    // int_to_vec
    // converts numbers into MIDI 1.1 spec extended values,
    // which can be a vector of bytes.
    // The first byte is the most-significant and has the 2^7 bit set.
    vector<uint8_t> int_to_vec(int i)
    {
        constexpr size_t bits_per_bite{7};
        int64_t bites{(((sizeof i) * bits_per_byte) / bits_per_bite) + 1};

        constexpr uint8_t dataflag{0x80};
        vector<uint8_t> numvec{};
        for (int64_t ctr{bites - 1}; ctr >= 0; --ctr)
        {
            uint8_t tempbite{static_cast<uint8_t>((i >> (7 * ctr) ) & byte7_mask)};
            if ((tempbite != 0) || (0 == ctr))
            {
                if (ctr > 0)
                {
                    tempbite |= dataflag;
                }
                numvec.push_back(tempbite);
            }
        }
        numvec.back() &= ~dataflag;
        return numvec;
    }

    //
    // int24_to_vec converts 24-bit numbers to MIDI extended values.
    vector<uint8_t> int24_to_vec(int i)
    {
        vector<uint8_t> numvec;
        uint8_t tempbyte{};
        constexpr uint8_t byte_mask{0xFF};
        for (int ctr{2}; ctr >= 0; --ctr)
        {
            tempbyte = (i >> (bits_per_byte * ctr) ) & byte_mask;
            numvec.push_back(tempbyte);
        }
        return numvec;
    }

    // In MIDI it is not necessary to repeat commands such as NOTE_ON
    // if the command and channel are the same in successive commands.
    // This requires keeping track of the command and channel.
    void check_running_status(const int32_t status_byte)
    {
        if (status_byte != running_status)
        {
            trackchunk.push_back(status_byte);
            running_status = status_byte;
        }
    }

    //
    // write_delay writes the time delay in MIDI ticks.
    void write_delay()
    {
        // Delays must occur between events even if delay is zero.
        vector<uint8_t> delayvec{int_to_vec(accum_delay)};
        trackchunk.insert(trackchunk.end(), delayvec.begin(), delayvec.end());
        total_beats += (accum_delay / global_ticks_per_quarter);
        accum_delay = 0;
    }

    //
    // shift_channel
    // transforms a textmidi channel in (1..16)
    // to a MIDI binary channel in (0..15).
    void shift_channel(uint32_t& channel)
    {
        if (0 == channel)
        {
            cerr << text_filename << ':' << line_ctr
                 << " Channel was zero (0) and left as zero; "
                 << "channels must be in [1..16]\n";
        }
        global_channel
            = channel = (((channel > 0) ? (channel - 1) : channel) & channel_mask);
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
                 * static_cast<double>(global_ticks_per_quarter))
                 / static_cast<double>(thedivisor);
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
DIGIT [0-9]
SPACE [\t ]*
STARTLINE_OR_SPACE (^|{SPACE})
SOMESPACE ([\t ]+)
LETTER [a-zA-Z]
INTEGER ([-+]?{DIGIT}+)
UINTEGER ({DIGIT}+)
XINTEGER 0x([0-9A-Fa-f])+
UXINTEGER ({UINTEGER}|{XINTEGER})
SYSEX_BYTES ({SOMESPACE}{UXINTEGER})+
PANVALUE ({INTEGER}|LEFT|CENTER|RIGHT)
SMPTE_CODE ({UINTEGER}:{UINTEGER}:{UINTEGER}:{UINTEGER}:{UINTEGER})

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
QUOTED_STRING ([^"]+)
STRING .*
LINE ([^"\n]*)\n
LETTERUNDER ({LETTER}|_)
ALPHANUM ({LETTER}|{DIGIT})
ALPHANUMUNDER ({ALPHANUM}|_)
IDENT ({LETTERUNDER}{ALPHANUMUNDER}*)
ASSIGNMENT ({IDENT}{SPACE}={SPACE}{STRING})
TEXT_META (TEXT|COPYRIGHT|TRACK|INSTRUMENT|LYRIC|MARKER|CUE_POINT)
CONTROL_CHANNEL (RESETALL|ALLNOTES_OFF|ALL_SOUND_OFF)
CONTROL_CHANNEL_INT (BREATH|DAMPER|SOFTPEDAL)
CONTROL_CHANNEL_ONOFF (PORTAMENTO|SOSTENUTO)
LEFTBRACKET "["
RIGHTBRACKET "]"
COMMENTCHAR ";"
COMMENT ({COMMENTCHAR}{STRING}\n)
skip [ \t,]*

FILEHEADERCONTEXT (FILEHEADER{SPACE})
FILEFORMAT (MONOTRACK|MULTITRACK|MULTISEQUENCE)
STARTTRACKTOKEN (STARTTRACK|\{)
END_OF_TRACKTOKEN (END_OF_TRACK|\})

%s FILE_HEADER_STATE
%s LAZY_STATE
%s DETAIL_STATE
%s TEXT_META_STATE
%s QUOTE_STATE
%option noyywrap

%%
<INITIAL>{FILEHEADERCONTEXT} {
    /* Recognize FILEHEADER */
    BEGIN(FILE_HEADER_STATE);
  }

<FILE_HEADER_STATE>{INTEGER}{SOMESPACE}{INTEGER}{SOMESPACE}{FILEFORMAT} {
    /* After FILEHEADER, recognize {number of tracks (discarded)} {ticks per quarter note} {MONOTRACK|MULTITRACK|MULTISEQUENCE} */
    MidiHeader midi_header{};
    istringstream stringstream(yytext);
    string format_str;
    stringstream >> midi_header.ntrks_ >> midi_header.division_ >> format_str;
    to_upper(format_str);
    auto fmf{textmidi::format_map.find(format_str)};
    if (fmf != textmidi::format_map.end())
    {
        midi_header.format_ = fmf->second;
    }
    else
    {
        cerr << text_filename << ':' << line_ctr
             << " File format was: " << format_str << " but must be in {";
        for (const auto& fmp : textmidi::format_map)
        {
            cerr << fmp.first << ", ";
        }
        cerr << "} or omitted for a default of MULTITRACK\n";
    }
    if ((MIDI_Format::MonoTrack == midi_header.format_)
        && (midi_header.ntrks_ > 1))
    {
        cerr << text_filename << ':' << line_ctr
             << " File format was MONOTRACK which allows only one track"
             << " but there are " << midi_header.ntrks_ << '\n';
    }
    global_ticks_per_quarter = midi_header.division_;
    midi_header.swap();
    midi_filestr.write(io_bytes(midi_header), sizeof(MidiHeader));
    BEGIN(DETAIL_STATE);
  }

<FILE_HEADER_STATE>{INTEGER}{SOMESPACE}{INTEGER} {
    /* Recognize {number of tracks (discarded)} {ticks per quarter} */
    MidiHeader midi_header{};
    midi_header.format_ = MIDI_Format::MultiTrack;
    istringstream stringstream(yytext);
    stringstream >> midi_header.ntrks_ >> midi_header.division_;
    global_ticks_per_quarter = midi_header.division_;
    midi_header.swap();
    midi_filestr.write(io_bytes(midi_header), sizeof(MidiHeader));
    BEGIN(DETAIL_STATE);
  }


<DETAIL_STATE>LAZY|BRIEF {
    /* Enter LAZY mode. */
    BEGIN(LAZY_STATE);
  }

<LAZY_STATE>END_LAZY|DETAIL|END_BRIEF {
    /* Enter DETAIL mode. */
    BEGIN(DETAIL_STATE);
  }

<DETAIL_STATE>{STARTTRACKTOKEN} {
    /* Recognize STARTTRACK */
    ++track_counter;
    trackchunk.clear();
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
    running_status = 0;
  }

<DETAIL_STATE>{TEXT_META}{SOMESPACE} {
    /* Recognize (TEXT|COPYRIGHT|TRACK|INSTRUMENT|LYRIC|MARKER|CUE_POINT) */
    /* This may be followed by a double-quoted string, or a non-quoted string */
    /* to the end of the line. */
    write_delay();
    istringstream stringstream(yytext);
    string meta_name{};
    stringstream >> meta_name;
    auto prefix{text_meta_map.find(meta_name)};
    if (prefix != text_meta_map.end())
    {
        trackchunk.push_back(meta_prefix);
        trackchunk.push_back(prefix->second);
    }
    else
    {
        cerr << text_filename << ':' << line_ctr
             << " Invalid meta event name " << meta_name << '\n';

    }
    BEGIN(TEXT_META_STATE);
  }

<TEXT_META_STATE>[\"] {
    /* Recognize start of a quoted string. */
    BEGIN(QUOTE_STATE);
  }

<TEXT_META_STATE>{LINE} {
    /* Recognize the remainder of the line */
    string yystring{yytext};
    if (!yystring.empty() && ('\n' == yystring[yystring.length() - 1]))
    {
        yystring.erase(yystring.length() - 1);
    }
    vector<uint8_t> len_vec{int_to_vec(yystring.length())};
    trackchunk.insert(trackchunk.end(), len_vec.begin(), len_vec.end());
    trackchunk.insert(trackchunk.end(), yystring.begin(), yystring.end());
    BEGIN(DETAIL_STATE);
  }

<QUOTE_STATE>{QUOTED_STRING} {
    /* Recognize start of a quoted string. */
    string yystring{yytext};
    if (!yystring.empty() && ('\n' == yystring[yystring.length() - 1]))
    {
        yystring.resize(yystring.length() - 1);
    }
    vector<uint8_t> len_vec{int_to_vec(yystring.length())};
    trackchunk.insert(trackchunk.end(), len_vec.begin(), len_vec.end());
    trackchunk.insert(trackchunk.end(), yystring.begin(), yystring.end());
  }

<QUOTE_STATE>["] {
    /* Recognize end of quoted string and return to DETAIL state. */
    BEGIN(DETAIL_STATE);
  }

<DETAIL_STATE,LAZY_STATE>{END_OF_TRACKTOKEN} {
    /* Recognize the end of a track. */
    write_delay();
    trackchunk.push_back(meta_prefix);
    trackchunk.push_back(end_of_track_prefix[0]);
    trackchunk.push_back(end_of_track_prefix[1]);
    const auto sizetemp{trackchunk.size() - 8};
    trackchunk[4] = (sizetemp >> 24) & 0xFF;
    trackchunk[5] = (sizetemp >> 16) & 0xFF;
    trackchunk[6] = (sizetemp >>  8) & 0xFF;
    trackchunk[7] = (sizetemp >>  0) & 0xFF;
    copy(trackchunk.begin(), trackchunk.end(),
         ostream_iterator<uint8_t>(midi_filestr));
    accum_delay = 0;
    BEGIN(DETAIL_STATE);
  }

<DETAIL_STATE>TEMPO{SPACE}{INTEGER} {
    /* Recognize TEMPO {num} */
    write_delay();
    istringstream stringstream(yytext);
    string discard{};
    uint32_t mmtempo{60};
    stringstream >> discard >> mmtempo;
    running_status = 0;
    // convert ticks per minute to useconds per quarter
    uint32_t tempo{(60 * 1000000) / mmtempo};

    vector<uint8_t> tempovec{int24_to_vec(tempo)};
    trackchunk.push_back(meta_prefix);

    trackchunk.insert(trackchunk.end(), &tempo_prefix[0], &tempo_prefix[2]);
    trackchunk.insert(trackchunk.end(), tempovec.begin(), tempovec.end());
  }

<DETAIL_STATE>DELAY{SPACE}{INTEGER} {
    /* Recognize DELAY num (the number of ticks) */
    istringstream stringstream(yytext);
    string discard{};
    uint32_t delay{};
    stringstream >> discard >> delay;
    accum_delay += delay;
    global_total_track_ticks += static_cast<double>(delay);
  }

<DETAIL_STATE>NOTE_ON{SOMESPACE}{INTEGER}{SOMESPACE}{PITCH_NAME}{SOMESPACE}{INTEGER} {
    /* Recognize NOTE_ON {chan} {pitch} {velocity num}. */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    uint32_t channel{}, // [1..16]
             key{},
             velocity{};
    string pitch_name;
    stringstream >> discard >> channel >> pitch_name >> velocity;

    shift_channel(channel);

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

    check_running_status(channel | note_on);

    key      &= byte7_mask;
    velocity &= byte7_mask;
    trackchunk.push_back(key);
    trackchunk.push_back(velocity);
  }

<DETAIL_STATE>NOTE_OFF{SPACE}{INTEGER}{SPACE}{PITCH_NAME}{SPACE}{INTEGER} {
    /* Recognize NOTE_OFF {channel} {Pitch} {velocity}. */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    uint32_t channel{},
             key{},
             velocity{};
    string pitch_name;
    stringstream >> discard >> channel >> pitch_name >> velocity;
    shift_channel(channel);

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
    check_running_status(channel | note_off);

    key &= byte7_mask;
    velocity &= byte7_mask;
    trackchunk.push_back(key);
    trackchunk.push_back(velocity);
  }


<DETAIL_STATE>POLY_KEY_PRESSURE{SPACE}{INTEGER}{SPACE}{PITCH_NAME}{SPACE}{INTEGER} {
    /* Recognize POLY_KEY_PRESSURE {channel} {pitchname} {velocity num} */
    istringstream stringstream(yytext);
    string discard;
    uint32_t channel{},
             key{},
             pressure{};
    string pitch_name;
    stringstream >> discard >> channel >> pitch_name >> pressure;
    shift_channel(channel);

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
    check_running_status(channel | poly_key_pressure);

    key &= byte7_mask;
    pressure &= byte7_mask;
    trackchunk.push_back(key);
    trackchunk.push_back(pressure);
  }


<DETAIL_STATE>CONTROL{SPACE}{INTEGER}{SPACE}{INTEGER}{SPACE}{INTEGER} {
    /* Recognize CONTROL {channel} {control_id} {control_value} */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    uint32_t channel{};
    int32_t control_id{},
            control_value{};
    stringstream >> discard >> channel >> control_id >> control_value;
    shift_channel(channel);

    check_running_status(channel | control);

    control_id &= byte7_mask;
    trackchunk.push_back(control_id);
    control_value &= byte7_mask;
    trackchunk.push_back(control_value);
  }

<DETAIL_STATE>{CONTROL_CHANNEL}{SPACE}{INTEGER} {
    /* Recognize {RESETALL|ALLNOTES_OFF|ALL_SOUND_OFF} {channel} */
    write_delay();

    string namestr{};
    int32_t control_id{};
    uint32_t channel{};
    istringstream stringstream{yytext};
    stringstream >> namestr >> channel;
    shift_channel(channel);

    try
    {
        control_id = text_control_map.at(namestr);
    }
    catch (out_of_range &oor)
    {
        cerr << "unknown control channel: " << text_filename << ':' << line_ctr
             << ' ' << namestr<< oor.what() << '\n';
        exit(EXIT_SUCCESS);
    }

    check_running_status(channel | control);
    trackchunk.push_back(control_id);
    trackchunk.push_back(0);
  }

<DETAIL_STATE>{CONTROL_CHANNEL_INT}{SPACE}{INTEGER}{SPACE}{INTEGER} {
    /* Recognize {BREATH|DAMPER|SOFTPEDAL} {channel} {control_value_num} */
    write_delay();

    string namestr;
    uint32_t channel{};
    int32_t control_id{},
        control_value{};
    istringstream stringstream(yytext);
    stringstream >> namestr >> channel >> control_value;
    shift_channel(channel);
    try
    {
        control_id = text_control_map.at(namestr);
    }
    catch (out_of_range &oor)
    {
        cerr << "unknown control channel: " << text_filename << ':' << line_ctr
             << ' ' << namestr<< oor.what() << '\n';
        exit(EXIT_SUCCESS);
    }

    check_running_status(channel | control);
    trackchunk.push_back(control_id);
    control_value &= byte7_mask;
    trackchunk.push_back(control_value);
  }

<DETAIL_STATE>{CONTROL_CHANNEL_ONOFF}{SPACE}{INTEGER}{SPACE}{ONOFF} {
    /* Recognize {PORTAMENTO|SOSTENUTO} {channel num} {ON|OFF} */
    write_delay();

    string namestr;
    uint32_t channel{};
    string onoff{};
    istringstream stringstream(yytext);
    stringstream >> namestr >> channel >> onoff;
    shift_channel(channel);
    int32_t control_id{};
    try
    {
        control_id = text_control_map.at(namestr);
    }
    catch (out_of_range &oor)
    {
        cerr << "unknown control : " << text_filename << ':' << line_ctr
             << ' ' << namestr << ' ' << oor.what() << '\n';
        exit(EXIT_SUCCESS);
    }

    uint8_t control_value{};
    try
    {
        control_value = OnOffMap.at(onoff);
    }
    catch (out_of_range &oor)
    {
        cerr << "unknown control on/off: " << text_filename << ':' << line_ctr
             << ' ' << onoff << ' ' << oor.what() << '\n';
        exit(EXIT_SUCCESS);
    }

    check_running_status(channel | control);
    trackchunk.push_back(control_id);
    trackchunk.push_back(control_value);
  }

<DETAIL_STATE>PAN{SPACE}{INTEGER}{SPACE}{PANVALUE} {
    /* Recognize PAN {channel num} {LEFT|CENTER|RIGHT|-64 to 63} */
    string namestr{};
    string panstring{};
    int32_t pancode{};
    write_delay();

    uint32_t channel{};
    istringstream stringstream(yytext);
    stringstream >> namestr >> channel >> panstring;
    to_lower(panstring);
    shift_channel(channel);

    auto pstvi{PanStringToValue.find(panstring)};
    if (PanStringToValue.end() == pstvi)
    {
        pancode = lexical_cast<int>(panstring);
        pancode += PanExcess64;
    }
    else
    {
        pancode = pstvi->second;
    }
    check_running_status(channel | control);
    trackchunk.push_back(control_pan);
    pancode &= byte7_mask;
    trackchunk.push_back(pancode);
  }

<DETAIL_STATE>PROGRAM{SPACE}{INTEGER}{SPACE}{INTEGER} {
    /* Recognize PROGRAM {channel num} {program number} */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    uint32_t channel {},
             theprogram {};
    stringstream >> discard >> channel >> theprogram;
    shift_channel(channel);

    check_running_status(channel | program);
    --theprogram;
    theprogram &= byte7_mask;
    trackchunk.push_back(theprogram);
  }

<DETAIL_STATE>CHANNEL_PRESSURE{SPACE}{INTEGER}{SPACE}{INTEGER} {
    /* Recognize CHANNEL_PRESSURE {channel} {pressure}*/
    write_delay();
    istringstream stringstream(yytext);
    string discard;
    uint32_t channel {},
                 pressure{};
    stringstream >> discard >> channel >> pressure;
    shift_channel(channel);

    // looks like a kluge. Does channel_pressure participate in running_status?
    running_status = 0;
    check_running_status(channel | channel_pressure);

    pressure &= byte7_mask;
    trackchunk.push_back(pressure);
  }

<DETAIL_STATE>PITCH_WHEEL{SPACE}{INTEGER}{SPACE}{INTEGER} {
    /* Recognize PITCH_WHEEL {channel} {bend num} */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    uint32_t channel {},
             bend{};
    stringstream >> discard >> channel >> bend;
    shift_channel(channel);

    const auto status_byte{channel | pitch_wheel};
    check_running_status(status_byte);

    uint32_t bendlsb{},
             bendmsb{};
    bendlsb = bend & byte7_mask;
    trackchunk.push_back(bendlsb);
    bendmsb = (bend >> 7) & byte7_mask;
    trackchunk.push_back(bendmsb);
  }

<DETAIL_STATE>TIME_SIGNATURE{SPACE}{INTEGER}{SPACE}{INTEGER}{SPACE}{INTEGER} {
    /* Recognize TIME_SIGNATURE {beats per bar} {beats per measure} {MIDI ticks per beat} */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    uint32_t numer{},
             denom{},
             ticksperbeat{};
    constexpr uint32_t thirtysecsperqtr{8};
    stringstream >> discard >> numer >> denom >> ticksperbeat;
    running_status = 0;
    // convert ticks per minute to useconds per quarter
    uint32_t denom_pwr{};
    while (denom >>= 1)
    {
      ++denom_pwr;
    }

    trackchunk.push_back(meta_prefix);

    trackchunk.insert(trackchunk.end(),
        &time_signature_prefix[0], &time_signature_prefix[2]);
    trackchunk.push_back(numer);
    trackchunk.push_back(denom_pwr);
    auto tempticksperbeat{ticksperbeat};
    while (tempticksperbeat > numeric_limits<uint8_t>().max())
    {
        tempticksperbeat /= 2;
    }
    trackchunk.push_back(tempticksperbeat);
    trackchunk.push_back(thirtysecsperqtr);
  }

<DETAIL_STATE>KEY_SIGNATURE{SPACE}{NOTE_NAME} {
    /* Recognize KEY_SIGNATURE {key as in a for a minor, or Db for D-flat Major */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    string key_sig_name;
    stringstream >> discard >> key_sig_name;
    int32_t themode{};
    int32_t accidentals;
    // tie is C++ tie for 2 returns, not musical tie.
    tie(accidentals, themode) = key_sig_name_to_accidentals(key_sig_name);

    trackchunk.push_back(meta_prefix);
    trackchunk.push_back(key_signature_prefix[0]);
    trackchunk.push_back(key_signature_prefix[1]);
    trackchunk.push_back(static_cast<signed char>(accidentals));
    trackchunk.push_back(themode);
  }

<DETAIL_STATE>SMPTE_OFFSET{SPACE}{SMPTE_CODE} {
    /* Recognize SMPTE_OFFSET HH:MM:SS:FR:FF (hours:minutes:seconds:frames: 1/100's of frames) */
    write_delay();

    string yystring{yytext},
           discard;
    size_t colonpos{};
    vector<int> offsets{0};
    while ((colonpos = yystring.find(':', colonpos)) != yystring.npos)
    {
        ++colonpos;
        offsets.push_back(colonpos);
    }
    offsets.push_back(colonpos);
    vector<int> offsetvalues;
    for (auto i{0U}; i < (offsets.size() - 1); ++i)
    {
        string str{yystring.substr(offsets[i],
            offsets[i + 1] - offsets[i] - 1)};
        offsetvalues.push_back(lexical_cast<int>(str));
    }
    trackchunk.push_back(meta_prefix);
    trackchunk.push_back(smpte_prefix[0]);
    trackchunk.push_back(smpte_prefix[1]);
    for (auto offsetval : offsetvalues)
    {
        trackchunk.push_back(offsetval);
    }
  }

<DETAIL_STATE>MIDI_CHANNEL{SPACE}{UINTEGER} {
    /* Recognize MIDI_CHANNEL {channel} */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    uint32_t channel{};
    stringstream >> discard >> channel;
    shift_channel(channel);

    trackchunk.push_back(meta_prefix);
    trackchunk.push_back(midi_channel_prefix[0]);
    trackchunk.push_back(midi_channel_prefix[1]);
    trackchunk.push_back(channel);
  }

<DETAIL_STATE>MIDI_PORT{SPACE}{UINTEGER} {
    /* Recognize MIDI_PORT {port} */
    write_delay();

    istringstream stringstream(yytext);
    string discard;
    int32_t port{};
    stringstream >> discard >> port;

    trackchunk.push_back(meta_prefix);
    trackchunk.push_back(midi_port_prefix[0]);
    trackchunk.push_back(midi_port_prefix[1]);
    trackchunk.push_back(port);
  }

<DETAIL_STATE>SYSEX{SYSEX_BYTES} {
    /* Recognize SYSEX {byte}... */
    /* The textmidi text file should not have the starting F0 or length */
    /* or the trailing F7. */
    /* textmidi will add the SYSEX (0xF0) and the length at the front */
    /* as well as well the end-of-sysex (F7) at the end. */
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
    vector<unsigned> bytes(byte_strings.size());
    transform(byte_strings.begin(),
        byte_strings.end(), bytes.begin(), [](const string& str)
        {
            unsigned i{};
            if (str.substr(0, 2) == "0x")
            {
                istringstream iss{str};
                iss >> hex >> i;
            }
            else
            {
                i = lexical_cast<unsigned>(str);
            }
            return i;
        }
    );
    // pushd the length of data plus the F7 at end
    // of the MIDI file stream.
    trackchunk.push_back(start_of_sysex);
    auto length_seq{int_to_vec(bytes.size() + 1)};
    trackchunk.insert(trackchunk.end(), length_seq.begin(), length_seq.end());
    if (bytes.size() > 0)
    {
        for (auto bi{bytes.begin()}; bi != bytes.end(); ++bi)
        {
            trackchunk.push_back(*bi);
        }
    }
    trackchunk.push_back(end_of_sysex);
  }

<DETAIL_STATE>SYSEXRAW{SYSEX_BYTES} {
    /* Recognize SYSEXRAW {byte}... */
    /* The textmidi text file should not have the starting F7 or length. */
    /* textmidi will add the 0xF7 and the length at the front */
    /* There is NO end-of-sysex (F7) at the end. */
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
    vector<unsigned> bytes(byte_strings.size());
    transform(byte_strings.begin(),
         byte_strings.end(), bytes.begin(), [](const string& str)
        {
            unsigned i{};
            if (str.substr(0, 2) == "0x")
            {
                istringstream iss{str};
                iss >> hex >> i;
            }
            else
            {
                i = lexical_cast<unsigned>(str);
            }
            return i;
        }
    );
    // pushd the length of data plus the F7 at end
    // of the MIDI file stream.
    trackchunk.push_back(end_of_sysex); // Raw SYSEX *starts* with F7
    // length of data; no implied terminating F0
    auto length_seq{int_to_vec(bytes.size())};
    trackchunk.insert(trackchunk.end(), length_seq.begin(), length_seq.end());
    if (bytes.size() > 0)
    {
        for (auto bi{bytes.begin()}; bi != bytes.end(); ++bi)
        {
            trackchunk.push_back(*bi);
        }
    }
  }

<DETAIL_STATE,LAZY_STATE>ticks{SPACE}{STRING} {
    /* Recognize ticks {string to end of line} */
    /* This will print to the screen, the accumulated ticks at this point
    ** in the current track for auditing purposes (such as tracks
    ** being different lengths in ticks). */
    string ticks_str{};
    (((ticks_str += "Ticks: ")
        += lexical_cast<string>(global_total_track_ticks)) += ' ') += yytext;
    cout << ticks_str << '\n';
    {
      write_delay();
      trackchunk.push_back(meta_prefix);
      trackchunk.push_back(text_prefix);
      vector<uint8_t> len_vec{int_to_vec(ticks_str.length())};
      trackchunk.insert(trackchunk.end(), len_vec.begin(), len_vec.end());
      trackchunk.insert(trackchunk.end(), ticks_str.begin(), ticks_str.end());
    }
  }

<LAZY_STATE>{DYNAMIC} {
    /* Recognize a dynamic as in pppp..mf..forte..ff..ffff */
    string note_name{yytext};
    uint32_t dynamic = 64;

    to_lower(note_name);
    if (note_name == "pppp")
    {
      dynamic = 10;
    }

    if (note_name == "ppp")
    {
      dynamic = 25;
    }

    if (note_name == "pp")
    {
      dynamic = 40;
    }

    if (note_name == "p")
    {
      dynamic = 50;
    }

    if (note_name == "mp")
    {
      dynamic = 62;
    }

    if (note_name == "mf")
    {
      dynamic = 75;
    }

    if (note_name == "forte")
    {
      dynamic = 90;
    }

    if (note_name == "ff")
    {
      dynamic = 110;
    }

    if (note_name == "fff")
    {
      dynamic = 120;
    }

    if (note_name == "ffff")
    {
      dynamic = 127;
    }

    lazy::dynamic = dynamic % 128;
  }

<LAZY_STATE>vel{SPACE}{INTEGER} {
    // A velocity value with a sign, + or -,
    // is a delta change in the current lazy::dynamic.
    // Otherwise it is an absolute setting for lazy dynamic.
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
    /* Recognize chan {channel} */
    string channel_str(yytext),
           discard;
    uint32_t channel{1U}; // text channels are in [1..16]
    istringstream stringstream(channel_str);
    stringstream >> discard >> channel;
    shift_channel(channel);

    if ((running_status & channel_mask) != global_channel)
    {
        running_status = static_cast<std::uint8_t>(0);
    }
  }

<LAZY_STATE>{STARTLINE_OR_SPACE}{LAZY_PITCH_NAME} {
    /* Recognize pitch name with optional ties */
    string note_name{};
    istringstream iss(yytext);
    iss >> note_name;
    KeyTie keytie{note_name};

    // write note event
    if (!keytie.tie_in_)
    {
      write_delay();
      check_running_status((global_channel & channel_mask) | note_on);
      trackchunk.push_back(keytie.key_);
      trackchunk.push_back(lazy::dynamic);
    }
    lazy::chord.push_back(keytie);    // save key number in lazy::chord;
  }

<LAZY_STATE>{NON_NOTE_LETTER}{LAZY_PITCH_NAME} {
    /* Recognize error grammar bad note letter */
    // ERROR
    cerr << text_filename << ':' << line_ctr <<
      " Non Key Letter before Key Name: " << __LINE__ << ' ' << yytext <<
      " in track " << track_counter << " at beat " <<
      (total_beats + (accum_delay / global_ticks_per_quarter)) << endl;
    exit(EXIT_SUCCESS);
  }

<DETAIL_STATE>(-{PITCH_NAME})|({PITCH_NAME}-) {
    /* Recognize error grammar */
    // ERROR
    cerr << text_filename << ':' << line_ctr <<
      " Ties are not allowed in DETAIL mode: " << __LINE__ << ' ' << yytext <<
      " in track " << track_counter << " at beat " <<
      (total_beats + (accum_delay / global_ticks_per_quarter)) << endl;
    exit(EXIT_SUCCESS);
  }

<DETAIL_STATE>{NON_NOTE_LETTER}{PITCH_NAME} {
    /* Recognize error grammar */
    // ERROR
    cerr << text_filename << ':' << line_ctr <<
      " Non Note Letter before Note Name: " << __LINE__ << ' ' << yytext <<
      " in track " << track_counter << " at beat " <<
      (total_beats + (accum_delay / global_ticks_per_quarter)) << endl;
    exit(EXIT_SUCCESS);
  }

<LAZY_STATE>{SPACE}{NOTE_NAME}{SOMESPACE}{LAZY_PITCH_NAME} {
    /* Recognize error grammar */
    // ERROR
    cerr << text_filename << ':' << line_ctr <<
      " Key Name without octave: " << __LINE__ << ' ' << yytext <<
      " in track " << track_counter << " at beat " <<
      (total_beats + (accum_delay / global_ticks_per_quarter)) << endl;
  }

<LAZY_STATE>{SPACE}{NOTE_NAME}{SOMESPACE}{RHYTHM} {
    /* Recognize error grammar */
    // ERROR
    cerr << text_filename << ':' << line_ctr <<
      " Key Name without octave: " << __LINE__ << ' ' << yytext <<
      " in track " << track_counter << " at beat " <<
      (total_beats + (accum_delay / global_ticks_per_quarter)) << endl;
  }

<LAZY_STATE>{SPACE}{RHYTHM} {
    /* Recognize a duration fraction in LAZY mode.  */
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
        cerr << text_filename << ':' << line_ctr
             << " A duration denominator that is > ticks per whole! "
             << thedivisor << '>' << (global_ticks_per_quarter * QuartersPerWhole) << '\n';
      }
    }
    total_duration = delay_and_update_total_ticks(thenumerator, thedivisor,
            ('.' == rhythmstr[rhythmstr.size() - 1]),
            ('.' == rhythmstr[rhythmstr.size() - 2]));
    //
    // Subtract detache, to be added back in as a "rest".
    note_duration
        = total_duration - ((static_cast<double>(detache) < total_duration) ? detache : 0);
    // write note offs for each note in the chord with 0 delays
    accum_delay += note_duration;

    for (const auto& keytie : lazy::chord)
    {
      if (!keytie.tie_out_)
      {
        write_delay(); // wait the duration of the note

        uint32_t channel{global_channel & channel_mask};
        const uint8_t note_cmd{lazy::note_off_select ? note_off : note_on};
        const auto status_byte{channel | note_cmd};
        check_running_status(status_byte);

        trackchunk.push_back(keytie.key_ & byte7_mask);

        // If we are using a note_off, then use the prevailing dynamic; else 0.
        const uint8_t velocity{lazy::note_off_select
            ? static_cast<uint8_t>(lazy::dynamic) : note_off_velocity};
        trackchunk.push_back(velocity);
      }
    }
    remove_if(lazy::chord.begin(), lazy::chord.end(),
        [](const KeyTie& kt) { return !kt.tie_out_; });

    lazy::chord.clear();
    if (static_cast<double>(detache) < total_duration)
    {
        accum_delay += detache;
    }
    BEGIN(LAZY_STATE);
  }

<LAZY_STATE>(r|R){SPACE}{RHYTHM} {
    /* Recognize a rest. */
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
        cerr << text_filename << ':' << line_ctr
             << " A duration denominator that is > ticks per whole! "
             << thedivisor << '>' << (global_ticks_per_quarter * QuartersPerWhole) << '\n';
      }
    }
    total_duration = delay_and_update_total_ticks(thenumerator, thedivisor,
            ('.' == rhythmstr[rhythmstr.size() - 1]),
            ('.' == rhythmstr[rhythmstr.size() - 2]));
    accum_delay += total_duration;
  }

<DETAIL_STATE,LAZY_STATE>{COMMENT} {
    /* Recognize comments. */
    ++line_ctr;
  }
^{SOMESPACE}
{skip}*
(\n) {
    ++line_ctr;
  }
<<EOF>> {
    if (verbose)
    {
        clog << "Found " << track_counter
             << " tracks and will set MIDI header accordingly.\n";
    }
    midi_filestr.seekp(10);
    track_counter = htobe16(track_counter);
    midi_filestr.write(io_bytes(track_counter), sizeof(track_counter));
    yyterminate();
  }
.

%%

