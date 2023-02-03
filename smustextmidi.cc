//
// TextMIDITools Version 1.0.32
//
// smustextmidi 1.0.6
// Copyright © 2023 Thomas E. Janzen
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

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

#include "Midi.h"
#include "TextmidiUtils.h"
#include "rational_support.h"
#include "SmusTrackEvent.h"
#include "Options.h"

using namespace std;
using namespace boost;
using namespace textmidi;
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

    typedef std::array<unsigned char, 4> IdType;
    constexpr IdType FormId{'F', 'O', 'R', 'M'};
    constexpr IdType SmusId{'S', 'M', 'U', 'S'};
    constexpr IdType ShdrId{'S', 'H', 'D', 'R'}; // SMUS header
    constexpr IdType NameId{'N', 'A', 'M', 'E'};
    constexpr IdType Ins1Id{'I', 'N', 'S', '1'};
    constexpr IdType CopyId{'(', 'c', ')', ' '}; // optional
    constexpr IdType AuthId{'A', 'U', 'T', 'H'}; // optional
    constexpr IdType TrakId{'T', 'R', 'A', 'K'};
    constexpr IdType AnnoId{'A', 'N', 'N', 'O'}; // optional

    bool compare_id(IdType id, uint8_t rhs[4])
    {
        auto rtn{true};
        for (auto i{0}; i < 4; ++i)
        {
            if (id[i] != rhs[i])
            {
                rtn = false;
            }
        }
        return rtn;
    }

    const string SMUSOpt{"smus"};
    constexpr char SMUSTxt[]{"input binary SMUS file"};
}

int main(int argc, char *argv[])
{
    program_options::options_description desc("Allowed options");
    desc.add_options()
        ((HelpOpt + ",h").c_str(),                                       HelpTxt)
        ((VerboseOpt + ",v").c_str(),                                    VerboseTxt)
        ((VersionOpt + ",V").c_str(),                                    VersionTxt)
        ((SMUSOpt + ",i").c_str(),     program_options::value<string>(), SMUSTxt)
        ((AnswerOpt + ",a").c_str(),                                     AnswerTxt)
        ((TextmidiOpt + ",o").c_str(), program_options::value<string>(), TextmidiTxt)
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

    uint8_t ID_int[4];
    vector<char> smus_score{};

    if (var_map.count(HelpOpt))
    {
        cout << "Usage: smustextmidi [OPTION]... [SMUSFILE]\n";
        cout << "smustextmidi 1.0.6\n";
        cout << desc << '\n';
        cout << "Report bugs to: janzentome@gmail.com\n";
        cout << "smustextmidi home page: <https://www\n";
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(VersionOpt)) [[unlikely]]
    {
        cout << "smustextmidi\n";
        cout << "TextMIDITools 1.0.32\n";
        cout << "Copyright © 2023 Thomas E. Janzen\n";
        cout << "License GPLv3+: GNU GPL version 3 or later "
             << "<https://gnu.org/licenses/gpl.html>\n";
        cout << "This is free software: "
             << "you are free to change and redistribute it.\n";
        cout << "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    string smus_filename;
    if (var_map.count(SMUSOpt)) [[likely]]
    {
        smus_filename = var_map[SMUSOpt].as<string>();
        if (!std::filesystem::exists(smus_filename))
        {
            cerr << SMUSOpt << ' ' << smus_filename << " File does not exist.\n";
            exit(EXIT_SUCCESS);
        }

    }
    else
    {
        cout << "Usage: smustextmidi [OPTION]... [SMUSFILE]\n";
        cerr << "You must provide an SMUS input file\n";
        cerr << desc << '\n';
        exit(EXIT_SUCCESS);
    }

    bool answer{};
    if (var_map.count(AnswerOpt)) [[unlikely]]
    {
        answer = true;
    }

    string textmidi_filename;
    if (var_map.count(TextmidiOpt)) [[likely]]
    {
        textmidi_filename = var_map[TextmidiOpt].as<string>();
    }
    else
    {
        cout << "Usage: smustextmidi [OPTION]... [SMUSFILE]\n";
        cerr << "You must provide a textmidi output file\n";
        cerr << desc << '\n';
        exit(EXIT_SUCCESS);
    }
    if (answer && std::filesystem::exists(textmidi_filename)) [[unlikely]]
    {
        cout << "Overwrite " << textmidi_filename << "?" << '\n';
        string answer{};
        cin >> answer;
        if (!((answer[0] == 'y') || (answer[0] == 'Y')))
        {
            exit(0);
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
            cerr << "can't open " << smus_filename << '\n';
            exit(EXIT_SUCCESS);
        }
        smus_file.seekg(0);
        smus_file.read(io_bytes(ID_int), sizeof ID_int);
        if (!compare_id(FormId, ID_int))
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
    catch (std::ios_base::failure &iosfail)
    {
        cerr << iosfail.what() << '\n';
        exit(EXIT_SUCCESS);
    }

    // check for SMUS
    unsigned int smus_index{};
    memcpy((char *)ID_int, &smus_score[smus_index], sizeof ID_int);
    if (!compare_id(SmusId, ID_int))
    {
        cerr << "Input file was not an SMUS file!?\n";
        exit(EXIT_SUCCESS);
    }
    smus_index += sizeof ID_int;
    // check for SHDR
    memcpy((char *)ID_int, &smus_score[smus_index], sizeof ID_int);
    if (!compare_id(ShdrId, ID_int))
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

    textmidi_file << "FILEHEADER " << (header.trak_qty + 1) << ' '
                  << TicksPerQuarter << ' ' << MIDI_Format::MultiTrack << '\n';
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

        if (compare_id(NameId, ID_int))
        {
            char score_name[1024];
            memcpy((char *) score_name, &smus_score[smus_index], len);
            score_name[len] = '\0';
            ((meta_events_string += "TEXT NameId ") += score_name) += '\n';
            smus_index += len + (len % 2); // Will be on 2-byte boundaries.
        }

        if (compare_id(CopyId, ID_int))
        {
            char copyright[1024];
            memcpy((char *) copyright, &smus_score[smus_index], len);
            copyright[len] = '\0';
            ((meta_events_string += "COPYRIGHT ") += copyright) += '\n';
            smus_index += len + (len % 2); // Will be on 2-byte boundaries.
        }

        if (compare_id(AuthId, ID_int))
        {
            char author[1024];
            memcpy((char *) author, &smus_score[smus_index], len);
            author[len] = '\0';
            ((meta_events_string += "TEXT AUTHOR ") += author) += '\n';
            smus_index += len + (len % 2); // Will be on 2-byte boundaries.
        }

        if (compare_id(Ins1Id, ID_int))
        {
            // copy instrument
            // get length of instrument name
            SoundItem sound_item{};
            memcpy(&sound_item, &smus_score[smus_index], len);
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
        if (compare_id(TrakId, ID_int))
        {
            in_track = true;
            // len is the byte length of the track
        }
    } while (!in_track);

    // DO THE TEMPO TRACK
    start_of_tracks_index = smus_index - 8;

    textmidi_file << "\nSTARTTRACK\n";
    textmidi_file << meta_events_string;
    textmidi_file << "TRACK Tempo\n";

    notes_per_track = len / sizeof(SmusTrackEventFilePod);

    // Read the first track and create a rhythm track.
    RhythmRational delay_accum{0};
    auto* trackEventPtr{reinterpret_cast<SmusTrackEventFilePod*>(&smus_score[smus_index])};

    SmusTrackEventFactory track_event_factory{};
    vector<unique_ptr<SmusTrackEventBase>> track_events(notes_per_track);
    transform(&trackEventPtr[0], &trackEventPtr[notes_per_track], track_events.begin(), track_event_factory);
    for (const auto& te : track_events)
    {
        textmidi_file << te->textmidi_tempo();
    }
    if (SmusTrackEventBase::delay_accum_)
    {
        textmidi_file << SmusTrackEventBase::i_am_lazy_string(true);
        textmidi_file << "R ";
        print_rhythm(textmidi_file, SmusTrackEventBase::delay_accum_) << '\n';
    }
    SmusTrackEventPitch::flush();
    if ((notes_per_track > 0) && !dynamic_cast<SmusTrackEventEnd*>(track_events[notes_per_track - 1].get()))
    {
        textmidi_file << SmusTrackEventBase::i_am_lazy_string(false);
        textmidi_file << "\nEND_OF_TRACK\n";
    }
    for_each(track_events.begin(), track_events.end(), [](unique_ptr<SmusTrackEventBase>& teb) { teb.reset(); });
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
        if (!compare_id(TrakId, ID_int))
        {
            cerr << "This was supposed to be a TRACK\n";
            exit(0);
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

        delay_accum = RhythmRational{0};
        trackEventPtr = reinterpret_cast<SmusTrackEventFilePod*>(&smus_score[smus_index]);
        smus_index += bytes_per_track;
        track_events.resize(notes_per_track);
        transform(&trackEventPtr[0], &trackEventPtr[notes_per_track], track_events.begin(), track_event_factory);
        // At the beginning of a track
        // default to medium dynamic of 64 unless a Volume event sets it.
        SmusTrackEventBase::channel(0);
        SmusTrackEventBase::current_dynamic(64);
        for (const auto& te : track_events)
        {
            textmidi_file << te->textmidi();
        }
        if (SmusTrackEventBase::delay_accum_)
        {
            if (!SmusTrackEventBase::i_am_lazy())
            {
                textmidi_file << "LAZY\n";
                SmusTrackEventBase::i_am_lazy(true);
            }
            textmidi_file << "R ";
            print_rhythm(textmidi_file, SmusTrackEventBase::delay_accum_)  << '\n';
        }
        SmusTrackEventPitch::flush();
        if ((notes_per_track > 0) && !dynamic_cast<SmusTrackEventEnd*>(track_events[notes_per_track - 1].get()))
        {
            textmidi_file << SmusTrackEventBase::i_am_lazy_string(false);
            textmidi_file << "\nEND_OF_TRACK\n";
        }
        for_each(track_events.begin(), track_events.end(), [](unique_ptr<SmusTrackEventBase>& teb) { teb.reset(); });
        track_events.clear();
    }
    textmidi_file.close();
    exit(EXIT_SUCCESS);
}

