//
// TextMIDITools Version 1.0.35
//
// miditext 1.0
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Write as mididisasm in 2003 as a companion to textmidi.
//
// This is the main driver for miditext, which converts a binary MIDI standard file
// into a textmidi text file.
//
// 2020-03-16
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */
#include <endian.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <filesystem>

#include <boost/program_options.hpp>

#include "rational_support.h"
#include "TextmidiUtils.h"
#include "MidiEvents.h"
#include "Options.h"

using namespace std;
using namespace boost;

using namespace textmidi;
using namespace textmidi::rational;

namespace
{
    typedef pair<MidiStreamIterator, int> StreamLengthPair;
    const string QuantizeOpt{"quantize"};
    constexpr char QuantizeTxt[]{"quantization ratio in quotes: \"1/32\""};
    const string LazyOpt{"lazy"};
    constexpr char LazyTxt[]{"Try to write in textmidi's lazy mode"};


    void find_tracks(MidiStreamIterator midiiter, MidiStreamIterator midiend,
            vector<StreamLengthPair>& track_iters, size_t expected_track_qty)
    {
        track_iters.clear();
        while (midiiter != midiend)
        {
            if (! ((*(midiiter++) == 'M')
                && (*(midiiter++) == 'T')
                && (*(midiiter++) == 'r')
                && (*(midiiter++) == 'k')))
            {
                const auto track_num{track_iters.size() + 1};
                if (track_num > expected_track_qty)
                {
                    cerr << "There is extra data after the tracks which will be ignored.\n";
                }
                else
                {
                    cerr << "No Track header MTrk in track: " << track_num << '\n';
                }
                return;
            }
            int32_t num{};
            copy(midiiter, midiiter + sizeof(num), io_bytes(num));
            midiiter += sizeof(num);
            num = htobe32(num);
            if (std::distance(&(*midiiter), &(*midiend)) < num)
            {
                cerr << "File too short for track length in track: "
                     << track_iters.size() << '\n';
                return;
            }
            track_iters.push_back(StreamLengthPair(midiiter, num));
            midiiter += num;
        }
    }

    class ConvertTrack
    {
      public:
        ConvertTrack(StreamLengthPair stream_length_pair,
            MidiDelayEventPairs& message_pairs,
            uint32_t ticks_per_whole, RhythmRational quantum, bool lazy)
          : stream_length_pair_{stream_length_pair},
            message_pairs_{message_pairs},
            ticks_per_whole_{ticks_per_whole},
            quantum_{quantum},
            lazy_{lazy}
        {
        }
        void operator()()
        {
            auto midiiter{stream_length_pair_.first};
            const auto midiend{stream_length_pair_.first + stream_length_pair_.second};
            MidiEventFactory midi_event_factory{midiend};
            MidiEventFactory::ticks_per_whole_ = ticks_per_whole_;

            MidiDelayEventPair midi_delay_msg_pair;
            do
            {
                midi_delay_msg_pair = midi_event_factory(midiiter);
                message_pairs_.push_back(midi_delay_msg_pair);
            }
            while ((midiiter < midiend)
                && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>
                                (midi_delay_msg_pair.second.get()));
        }
     private:
        const StreamLengthPair stream_length_pair_;
        MidiDelayEventPairs& message_pairs_;
        const uint32_t ticks_per_whole_;
        const RhythmRational& quantum_;
        const bool lazy_;
    };
 }

int main(int argc, char *argv[])
{
    program_options::options_description desc("Allowed options");
    desc.add_options()
        ((HelpOpt     + ",h").c_str(),                                       HelpTxt)
        ((VerboseOpt  + ",v").c_str(),                                    VerboseTxt)
        ((VersionOpt  + ",V").c_str(),                                    VersionTxt)
        ((MidiOpt     + ",i").c_str(), program_options::value<string>(),     MidiTxt)
        ((AnswerOpt   + ",a").c_str(),                                     AnswerTxt)
        ((TextmidiOpt + ",o").c_str(), program_options::value<string>(), TextmidiTxt)
        ((QuantizeOpt + ",q").c_str(), program_options::value<string>(), QuantizeTxt)
        ((LazyOpt     + ",l").c_str(),                                       LazyTxt)
    ;
    program_options::positional_options_description pos_opts_desc;
    program_options::variables_map var_map;
    try
    {
        pos_opts_desc.add(MidiOpt.c_str(), -1);
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

    string output_filename,
           midi_filename;

    if (var_map.count(HelpOpt))
    {
        cout << "Usage: miditext [OPTION]... [MIDIFILE]\n";
        cout << "miditext 1.0\n";
        cout << desc << '\n';
        cout << "Report bugs to: janzentome@gmail.com\n";
        cout << "miditext home page: <https://www\n";
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(VersionOpt)) [[unlikely]]
    {
        cout << "miditext\n";
        cout << "TextMIDITools 1.0.35\n";
        cout << "Copyright © 2023 Thomas E. Janzen\n";
        cout << "License GPLv3+: GNU GPL version 3 or later "
             << "<https://gnu.org/licenses/gpl.html>\n";
        cout << "This is free software: "
             << "you are free to change and redistribute it.\n";
        cout << "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(MidiOpt))
    {
        midi_filename = var_map[MidiOpt].as<string>();
        if (!std::filesystem::exists(midi_filename))
        {
            cerr << MidiOpt << ' ' << midi_filename << " File does not exist.\n";
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        cerr << "You must provide a MIDI input file\n";
        cerr << desc << '\n';
        exit(EXIT_SUCCESS);
    }
    RhythmRational quantum{0};
    if (var_map.count(QuantizeOpt)) [[unlikely]]
    {
        string quantum_string{var_map[QuantizeOpt].as<string>()};
        istringstream iss{quantum_string};
        iss >> quantum;
    }
    bool lazy{};
    if (var_map.count(LazyOpt)) [[unlikely]]
    {
        lazy = true;
    }

    bool verbose{};
    if (var_map.count(VerboseOpt)) [[unlikely]]
    {
        verbose = true;
    }
    bool answer{};
    if (var_map.count(AnswerOpt)) [[unlikely]]
    {
        answer = true;
    }

    if (var_map.count(TextmidiOpt))
    {
        output_filename = var_map[TextmidiOpt].as<string>();
    }
    else
    {
        output_filename = midi_filename + ".txt";
        if (verbose)
        {
            cout << "Will write to " << output_filename << '\n';
        }
    }
    if (answer && std::filesystem::exists(midi_filename))
    {
        cout << "Overwrite " << midi_filename << "?" << '\n';
        string answer{};
        cin >> answer;
        if (!((answer[0] == 'y') || (answer[0] == 'Y')))
        {
            exit(EXIT_SUCCESS);
        }
    }
    if (midi_filename == output_filename)
    {
        std::cerr
          << "You would have overwritten the source name!; Must exit!" << '\n';
        exit(EXIT_SUCCESS);
    }

    MidiStreamVector midivector{};
    try
    {
        ifstream midifilestr;
        midifilestr.open(midi_filename, ios_base::binary);
        if (!midifilestr)
        {
            std::cerr << "Open failed for " << midi_filename << '\n';
            return EXIT_FAILURE;
        }
        MidiStreamAtom ui{};
        while (midifilestr.read(reinterpret_cast<char*>(&ui), sizeof ui))
        {
            midivector.push_back(ui);
        }
        midifilestr.close();
    }
    catch (std::ios_base::failure &iosfail)
    {
        cerr << iosfail.what() << '\n';
        exit(EXIT_SUCCESS);
    }

    ofstream text_filestr(output_filename.c_str());

    if (!text_filestr)
    {
        cout << "can't open " << output_filename << '\n';
    }

    MidiStreamVector::iterator midiiter{midivector.begin()};

    if (midivector.size() < sizeof(MidiHeader))
    {
        cerr << "ERROR File too short for a MIDI Header\n";
        exit(0);
    }
    MidiHeader midi_header{midivector.data()};
    if (verbose)
    {
        cout << midi_header << '\n';
    }
    text_filestr << "FILEHEADER ";
    midiiter += sizeof(MidiHeader);
    const uint32_t ticksperquarter{midi_header.division_};
    uint32_t ticks_per_whole{ticksperquarter * 4};

    text_filestr << midi_header.ntrks_ << ' ' << midi_header.division_ << ' ' << midi_header.format_ << '\n';
    if (verbose)
    {
        cout << "FORMAT: " << midi_header.format_ << '\n';
    }
    size_t track_qty{midi_header.ntrks_};

    vector<StreamLengthPair> stream_length_pairs{};
    find_tracks(midiiter, midivector.end(), stream_length_pairs, track_qty);
    vector<MidiDelayEventPairs> midi_delay_event_pairs(stream_length_pairs.size());
    vector<thread> track_threads(stream_length_pairs.size());

    for (auto& ti : stream_length_pairs)
    {
        const auto i{std::distance(&(*stream_length_pairs.begin()), &ti)};
        ConvertTrack convert_track{ti, midi_delay_event_pairs[i], ticks_per_whole, quantum, lazy};
        thread track_thread{convert_track};
        track_threads[i] = move(track_thread);
    }
    for (auto& thr : track_threads)
    {
        thr.join();
    }
    for (auto& mdep : midi_delay_event_pairs)
    {
        const auto i{std::distance(&(*midi_delay_event_pairs.begin()), &mdep)};
        text_filestr << "\nSTARTTRACK" << " ; bytes in track: "
                    << stream_length_pairs[i].second << '\n';
        if (lazy)
        {
            PrintLazyTrack print_lazy_track{mdep, quantum, ticksperquarter};
            text_filestr << print_lazy_track << '\n';
        }
        else
        {
            copy(mdep.begin(), mdep.end(),
                ostream_iterator<MidiDelayEventPair>(text_filestr, "\n"));
        }
    }
    return EXIT_SUCCESS;
}
