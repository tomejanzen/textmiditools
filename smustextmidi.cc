//
// TextMIDITools Version 1.0.89
//
// smustextmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// originally SMUSMIDI.C for the Commodore Amiga.
// Converts a DMCS .SMUS file to a standard MIDI file.
// Don't expect it to work on SMUS files from programs other than DMCS.
//
//  FACILITY:
//
//      SMUSMIDI file format convertor on Commodore (TM) Amiga (TM)
//      compiled with SAS/C V6.5
//
//  ABSTRACT:
//
//  smustextmidi converts SMUS files from DMCS (TM Electronic Arts)
//     Simple Music Score to textmidi text format, which textmidi
//     can convert into standard MIDI file format.
//
//  AUTHOR: Tom Janzen
//
//  CREATION DATE:        17-AUG-1991
//  MODIFIED 2004 for linux
//  MODIFIED 2021 for C++ and to write textmidi format.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <byteswap.h>

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <filesystem>
#include <ranges>
#include <memory>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp> // to_upper

#include "Midi.h"
#include "MidiMaps.h"
#include "TextmidiUtils.h"
#include "rational_support.h"
#include "SmusTrackEvent.h"
#include "Options.h"
#include "DynamicsOptions.h"

using namespace std;
using namespace boost;
using namespace textmidi;
using namespace midi;
using namespace textmidi::rational;
using namespace smus;

namespace
{
    constexpr uint32_t IdLen           {4};
    constexpr uint32_t TicksPerQuarter {240};
#pragma pack(1)
    struct StartThing
    {
        unsigned short tempo;
        uint8_t gain, trak_qty;
    };
#pragma pack()

#pragma pack(1)
    struct SoundItem
    {
        uint8_t sound_place, type, thing_a, thing_b;
        char name[60];
    };
#pragma pack()

    constexpr MidiStreamArray4 FormId{'F', 'O', 'R', 'M'};
    constexpr MidiStreamArray4 SmusId{'S', 'M', 'U', 'S'};
    constexpr MidiStreamArray4 ShdrId{'S', 'H', 'D', 'R'}; // SMUS header
    constexpr MidiStreamArray4 NameId{'N', 'A', 'M', 'E'};
    constexpr MidiStreamArray4 Ins1Id{'I', 'N', 'S', '1'};
    constexpr MidiStreamArray4 CopyId{'(', 'c', ')', ' '}; // optional
    constexpr MidiStreamArray4 AuthId{'A', 'U', 'T', 'H'}; // optional
    constexpr MidiStreamArray4 TrakId{'T', 'R', 'A', 'K'};
#if defined(SUPPORT_ANNOD)
    constexpr MidiStreamArray4 AnnoId{'A', 'N', 'N', 'O'}; // optional
#endif

    const string SMUSOpt{"smus"};
    constexpr char SMUSTxt[]{"input binary SMUS file"};
}

int main(int argc, char *argv[])
{
    program_options::options_description desc("Allowed options");
    desc.add_options()
        ((help_option.registered_name().c_str()),                                                     help_option.text().c_str())
        ((verbose_option.registered_name().c_str()),                                                  verbose_option.text().c_str())
        ((version_option.registered_name().c_str()),                                                  version_option.text().c_str())
        ((smus_option.registered_name().c_str()),                   program_options::value<string>(), SMUSTxt)
        ((answer_option.registered_name().c_str()),                                                   answer_option.text().c_str())
        ((textmidi_out_option.registered_name().c_str()),           program_options::value<string>(), textmidi_out_option.text().c_str())
        ((dynamics_configuration_option.registered_name().c_str()), program_options::value<string>(), dynamics_configuration_option.text().c_str())
        ((dotted_rhythms_option.registered_name().c_str()),         program_options::value<string>(), dotted_rhythms_option.text().c_str())
        ((rhythm_expression_option.registered_name().c_str()),      program_options::value<string>(), rhythm_expression_option.text().c_str())
    ;
    program_options::positional_options_description pos_opts_desc;
    program_options::variables_map var_map;
    try
    {
        pos_opts_desc.add(SMUSOpt.c_str(), -1);
        program_options::store(
            program_options::command_line_parser(argc, argv)
            .options(desc).positional(pos_opts_desc).run(), var_map);
        program_options::notify(var_map);
    }
    catch (std::logic_error& err)
    {
        cerr << "Program options error: " << err.what() << '\n';
        exit(EXIT_SUCCESS);
    }

    MidiStreamArray4 ID_int{};
    vector<char> smus_score{};

    if (var_map.count(help_option.option()))
    {
        const string logstr{((string{"Usage: smustextmidi [OPTION]... [SMUSFILE]\nsmustextmidi Version 1.0.89\n"}
            += lexical_cast<string>(desc)) += '\n')
            += "Report bugs to: janzentome@gmail.com\nsmustextmidi home page: https://github.com/tomejanzen/textmiditools\n"};
        cout << logstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(version_option.option())) [[unlikely]]
    {
        cout << "smustextmidi\nTextMIDITools 1.0.89\nCopyright © 2024 Thomas E. Janzen\n"
            "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"
            "This is free software: you are free to change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    string smus_filename;
    if (var_map.count(smus_option.option())) [[likely]]
    {
        smus_filename = var_map[smus_option.option()].as<string>();
        if (!filesystem::exists(smus_filename))
        {
            const string errstr{((string(SMUSOpt) += ' ') += smus_filename) += " File does not exist.\n"};
            cerr << errstr;
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        const string errstr{(string{"Usage: smustextmidi [OPTION]... [SMUSFILE]\nYou must provide an SMUS input file\n"}
            += lexical_cast<string>(desc)) += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }

    bool answer{};
    if (var_map.count(answer_option.option())) [[unlikely]]
    {
        answer = true;
    }

    {
        string dynamics_configuration_file{};
        if (var_map.count(dynamics_configuration_option.option())) [[unlikely]]
        {
            dynamics_configuration_file = var_map[dynamics_configuration_option.option()].as<string>();
        }
        midi::dynamics_map = textmidi::read_dynamics_configuration(dynamics_configuration_file);
    }

    bool dotted_rhythms{true};
    if (var_map.count(dotted_rhythms_option.option())) [[unlikely]]
    {
        string dotted_rhythms_string{var_map[dotted_rhythms_option.option()].as<string>()};
        to_upper(dotted_rhythms_string);
        dotted_rhythms = ("TRUE" == dotted_rhythms_string);
    }

    if (var_map.count(rhythm_expression_option.option())) [[unlikely]]
    {
        string rhythm_expression_string{var_map[rhythm_expression_option.option()].as<string>()};
        to_upper(rhythm_expression_string);
        if (midi::rhythm_expression_map.contains(rhythm_expression_string))
        {
            const rational::RhythmExpression rhythm_expression{rhythm_expression_map[rhythm_expression_string]};
            switch (rhythm_expression)
            {
              case rational::RhythmExpression::Rational:
                textmidi::rational::print_rhythm = make_unique<PrintRhythmRational>(dotted_rhythms);
                break;
              case rational::RhythmExpression::SimpleContinuedFraction:
                textmidi::rational::print_rhythm = make_unique<PrintRhythmSimpleContinuedFraction>();
                break;
            }
        }
    }
    else
    {
        if (var_map.count(dotted_rhythms_option.option())) [[unlikely]]
        {
            textmidi::rational::print_rhythm = make_unique<PrintRhythmRational>(dotted_rhythms);
        }
    }

    string textmidi_filename;
    if (var_map.count(textmidi_out_option.option())) [[likely]]
    {
        textmidi_filename = var_map[textmidi_out_option.option()].as<string>();
    }
    else
    {
        const string errstr{(string{"Usage: smustextmidi [OPTION]... [SMUSFILE]\nYou must provide a textmidi output file\n"}
            += lexical_cast<string>(desc)) += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }
    if (answer && filesystem::exists(textmidi_filename)) [[unlikely]]
    {
        cout << "Overwrite " << textmidi_filename << "?" << '\n';
        string answerstr{};
        cin >> answerstr;
        if (!((answerstr[0] == 'y') || (answerstr[0] == 'Y')))
        {
            exit(EXIT_SUCCESS);
        }
    }
    //
    // Open the SMUS file and read its header.
    // Can have NAME, "(c) ", AUTH, ANNO, INS1.
    try
    {
        ifstream smus_file{};
        smus_file.open(smus_filename.c_str(), ios_base::binary | ios_base::in);
        if (!smus_file)
        {
            const string errstr{(string{"can't open "} += smus_filename) += '\n'};
            cerr << errstr;
            exit(EXIT_SUCCESS);
        }
        smus_file.seekg(0);
        smus_file.read(io_bytes(ID_int), sizeof ID_int);
        if (!equal(FormId.begin(), FormId.end(), ID_int.begin()))
        {
            cerr << "Not a FORM file\n";
            exit(EXIT_SUCCESS);
        }
        unsigned int form_len{};
        smus_file.read(io_bytes(form_len), sizeof form_len);
        form_len = htobe32(form_len);

        smus_score.resize(form_len);
        // Copy whole file into smus_score.
        // The copy algorithm does not seem to work on a binary file.
        smus_file.read(&smus_score[0], form_len);
    }
    catch (ios_base::failure &iosfail)
    {
        cerr << iosfail.what() << '\n';
        exit(EXIT_SUCCESS);
    }

    // check for SMUS
    unsigned int smus_index{};
    copy_n(&smus_score[smus_index], ID_int.size(), ID_int.begin());
    if (!equal(SmusId.begin(), SmusId.end(), ID_int.begin()))
    {
        cerr << "Input file was not an SMUS file!?\n";
        exit(EXIT_SUCCESS);
    }
    smus_index += sizeof ID_int;
    // check for SHDR
    copy_n(&smus_score[smus_index], ID_int.size(), ID_int.begin());
    if (!equal(ShdrId.begin(), ShdrId.end(), ID_int.begin()))
    {
        cerr << "No SHDR chunk!\n";
        exit(EXIT_SUCCESS);
    }
    int notes_per_track{};
    unsigned int start_of_tracks_index{};

    // Get the header
    smus_index += 8; // Why are we skipping 8 bytes?
    // We are skipping the SHDR and the length which is = 4 for 8 bytes.
    static StartThing header{};
    memcpy((char *) &header, &smus_score[smus_index], sizeof(StartThing));

    ofstream textmidi_file{textmidi_filename};

    string textmidi_str{};
    ((((((textmidi_str +=  "FILEHEADER ") += lexical_cast<string>(header.trak_qty + 1))
        += ' ') +=  lexical_cast<string>(TicksPerQuarter)) += ' ')
        += lexical_cast<string>(midi::MIDI_Format::MultiTrack)) += '\n';
    textmidi_file << textmidi_str;
    smus_index += sizeof(StartThing);

    bool in_track{};
    uint32_t len{};
    // SMUS puts Meta Events prior to tracks, but MIDI puts them
    // inside tracks, so
    // we save them up for being inside the first track of the MIDI file.
    string meta_events_string;
    do
    {
        memcpy((char *) &ID_int, &smus_score[smus_index], sizeof ID_int);
        smus_index += IdLen; // += 4
        memcpy((char *) &len, &smus_score[smus_index], sizeof len);
        smus_index += sizeof len;
        len = htobe32(len);

        if (equal(NameId.begin(), NameId.end(), ID_int.begin()))
        {
            char score_name[1024];
            copy_n(&smus_score[smus_index], len, score_name);
            score_name[len] = '\0';
            ((meta_events_string += "TEXT NameId ") += score_name) += '\n';
            smus_index += len + (len % 2); // Will be on 2-byte boundaries.
        }

        if (equal(CopyId.begin(), CopyId.end(), ID_int.begin()))
        {
            char copyright[1024];
            copy_n(&smus_score[smus_index], len, copyright);
            copyright[len] = '\0';
            ((meta_events_string += "COPYRIGHT ") += copyright) += '\n';
            smus_index += len + (len % 2); // Will be on 2-byte boundaries.
        }

        if (equal(AuthId.begin(), AuthId.end(), ID_int.begin()))
        {
            char author[1024];
            copy_n(&smus_score[smus_index], len, author);
            author[len] = '\0';
            ((meta_events_string += "TEXT AUTHOR ") += author) += '\n';
            smus_index += len + (len % 2); // Will be on 2-byte boundaries.
        }

        if (equal(Ins1Id.begin(), Ins1Id.end(), ID_int.begin()))
        {
            // copy instrument
            // get length of instrument name
            SoundItem sound_item{};
            copy_n(&smus_score[smus_index], sizeof sound_item, reinterpret_cast<char*>(&sound_item));
            sound_item.name[len - 4] = '\0'; // -4 because instruments are structs
            // skip past name length but land on 2-byte alignment
            smus_index += len + (len % 2);
            ((meta_events_string += "INSTRUMENT ") += sound_item.name) += '\n';
            if (1 == sound_item.type)
            {
                ((((meta_events_string += "PROGRAM ") += lexical_cast<string>(sound_item.thing_a + 1))
                    += ' ') += lexical_cast<string>(sound_item.thing_b + 1)) += '\n';
            }
        }
        // Instrument index is the number of instruments now.
        // I hope all the instruments have been listed by now.
        //
        if (equal(TrakId.begin(), TrakId.end(), ID_int.begin()))
        {
            in_track = true;
            // len is the byte length of the track
        }
    } while (!in_track);

    // DO THE TEMPO TRACK
    start_of_tracks_index = smus_index - 8;

    textmidi_str.clear();
    ((textmidi_str += "\nSTARTTRACK\n") += meta_events_string) += "TRACK Tempo\n";
    textmidi_file << textmidi_str;

    notes_per_track = len / sizeof(SmusTrackEventFilePod);

    // Read the first track and create a rhythm track.
    RhythmRational delay_accum{};
    auto* trackEventPtr{reinterpret_cast<SmusTrackEventFilePod*>(&smus_score[smus_index])};

    SmusTrackEventFactory track_event_factory{};
    vector<unique_ptr<SmusTrackEventBase>> track_events(notes_per_track);
    transform(&trackEventPtr[0], &trackEventPtr[notes_per_track], track_events.begin(), track_event_factory);
    ranges::for_each(track_events, [&](unique_ptr<SmusTrackEventBase>& te){ textmidi_file << te->textmidi_tempo(); });
    if (SmusTrackEventBase::delay_accum_)
    {
        textmidi_file << SmusTrackEventBase::i_am_lazy_string(true) << " R ";
        (*print_rhythm)(textmidi_file, SmusTrackEventBase::delay_accum_) << '\n';
    }
    SmusTrackEventPitch::flush();
    if ((notes_per_track > 0) && !dynamic_cast<SmusTrackEventEnd*>(track_events[notes_per_track - 1].get()))
    {
        textmidi_file << SmusTrackEventBase::i_am_lazy_string(false) << "\nEND_OF_TRACK\n";
    }
    ranges::for_each(track_events, [](unique_ptr<SmusTrackEventBase>& teb) { teb.reset(); });
    track_events.clear();
    //
    // end of tempo track
    //

    // Start Over on tracks for the note events.
    //
    smus_index = start_of_tracks_index;
    for (uint32_t track{}; track < header.trak_qty; ++track)
    {
        textmidi_file << "\nSTARTTRACK\n";
        memcpy((char *) &ID_int, &smus_score[smus_index], sizeof ID_int);
        if (!equal(TrakId.begin(), TrakId.end(), ID_int.begin()))
        {
            cerr << "This was supposed to be a TRACK\n";
            exit(EXIT_SUCCESS);
        }
        smus_index += sizeof ID_int;
        uint32_t bytes_per_track{};
        memcpy((char *) &bytes_per_track, &smus_score[smus_index], sizeof bytes_per_track);
        bytes_per_track = htobe32(bytes_per_track);
        smus_index += sizeof bytes_per_track;

        string track_name{"Track "};
        track_name += lexical_cast<string>(track);
        textmidi_file << "TRACK " << track_name << '\n';

        notes_per_track = bytes_per_track / sizeof(SmusTrackEventFilePod);

        delay_accum = RhythmRational{};
        trackEventPtr = reinterpret_cast<SmusTrackEventFilePod*>(&smus_score[smus_index]);
        smus_index += bytes_per_track;
        transform(&trackEventPtr[0], &trackEventPtr[notes_per_track], back_inserter(track_events), track_event_factory);
        // At the beginning of a track
        // default to medium dynamic of 64 unless a Volume event sets it.
        SmusTrackEventBase::channel(0);
        SmusTrackEventBase::current_dynamic(64);
        ranges::for_each(track_events, [&](unique_ptr<SmusTrackEventBase>& te) { textmidi_file << te->textmidi(); });
        if (SmusTrackEventBase::delay_accum_)
        {
            if (!SmusTrackEventBase::i_am_lazy())
            {
                textmidi_file << "LAZY\n";
                SmusTrackEventBase::i_am_lazy(true);
            }
            textmidi_file << "R ";
            (*print_rhythm)(textmidi_file, SmusTrackEventBase::delay_accum_)  << '\n';
        }
        SmusTrackEventPitch::flush();
        if ((notes_per_track > 0) && !dynamic_cast<SmusTrackEventEnd*>(track_events[notes_per_track - 1].get()))
        {
            textmidi_file << SmusTrackEventBase::i_am_lazy_string(false);
            textmidi_file << "\nEND_OF_TRACK\n";
        }
        ranges::for_each(track_events, [](unique_ptr<SmusTrackEventBase>& teb) { teb.reset(); });
        track_events.clear();
    }
    textmidi_file.close();
    exit(EXIT_SUCCESS);
}

