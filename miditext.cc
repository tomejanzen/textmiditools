//
// TextMIDITools Version 1.0.92
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Written as mididisasm in 2003 as a companion to textmidi.
//
// miditext is the main driver for miditext,
// which converts a binary MIDI standard file
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
#include <set>
#include <map>
#include <filesystem>
#include <ranges>
#include <memory>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp> // to_upper

#include "rational_support.h"
#include "TextmidiUtils.h"
#include "MidiEvents.h"
#include "MidiMaps.h"
#include "Options.h"
#include "DynamicsOptions.h"

using std::size_t, std::pair, std::vector, std::cin, std::cout, std::cerr,
      std::ifstream, std::ofstream, std::string, std::setw,
      std::istringstream, std::filesystem::exists, std::filesystem::file_size,
      std::make_unique, std::set, std::ranges::count_if, std::ranges::copy;

using textmidi::rational::RhythmRational,
      textmidi::rational::PrintRhythmRational,
      textmidi::DelayEvents, textmidi::DelayEvent,
      textmidi::rational::RhythmExpression,
      textmidi::PrintLazyTrack;
using boost::lexical_cast, boost::to_upper;

namespace
{
    using StreamLengthPair = pair<midi::MidiStreamIterator, int>;

    void find_tracks(midi::MidiStreamIterator midiiter,
            midi::MidiStreamIterator midiend,
            vector<StreamLengthPair>& track_iters, size_t expected_track_qty)
    {
        track_iters.clear();
        track_iters.reserve(140); // one file had 140 tracks
        while (midiiter != midiend)
        {
            if (!std::equal(midiiter,
                midiiter + midi::MidiTrackChunkName.size(),
                midi::MidiTrackChunkName.begin()))
            {
                const auto track_num{track_iters.size() + 1};
                if (track_num > expected_track_qty)
                {
                    cerr << "There is extra data after "
                        "the tracks which will be ignored.\n";
                }
                else
                {
                    const string errstr{
                        (string{"No Track header MTrk in track: "}
                        += track_num) += '\n'};
                    cerr << errstr;
                }
                return;
            }
            midiiter += midi::MidiTrackChunkName.size();
            int32_t num{};
            copy(midiiter, midiiter + sizeof(num), textmidi::io_bytes(num));
            midiiter += sizeof(num);
            num = htobe32(num);
            if (std::distance(&(*midiiter), &(*midiend)) < num)
            {
                const string errstr{(string{
                    "File too short for track length in track:"}
                    += lexical_cast<string>(track_iters.size())) += '\n'};
                cerr << errstr;
                return;
            }
            track_iters.emplace_back(midiiter, num);
            midiiter += num;
        }
    }

    class ConvertTrack
    {
      public:
        ConvertTrack(StreamLengthPair stream_length_pair,
            DelayEvents& message_pairs,
            uint32_t ticks_per_whole, RhythmRational quantum)
          : stream_length_pair_{stream_length_pair},
            message_pairs_{message_pairs},
            ticks_per_whole_{ticks_per_whole},
            quantum_{quantum}
        {
        }
        void operator()()
        {
            auto midiiter{stream_length_pair_.first};
            const auto midiend{
                stream_length_pair_.first + stream_length_pair_.second};

            textmidi::MidiEventFactory midi_event_factory{midiend, ticks_per_whole_};
            DelayEvent midi_delay_msg_pair;
            int64_t ticks_accumulated{};

            do
            {
                midi_delay_msg_pair
                    = midi_event_factory(midiiter, ticks_accumulated);
                message_pairs_.push_back(midi_delay_msg_pair);
            }
            while ((midiiter < midiend)
                && !dynamic_cast<textmidi::MidiFileMetaEndOfTrackEvent*>
                (midi_delay_msg_pair.second.get()));
        }
     private:
        const StreamLengthPair stream_length_pair_;
        DelayEvents& message_pairs_;
        const uint32_t ticks_per_whole_;
        const RhythmRational quantum_;
    };
} // namespace

int main(int argc, char *argv[])
{
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ((help_option.registered_name().c_str()), help_option.text().c_str())
        ((verbose_option.registered_name().c_str()),
            verbose_option.text().c_str())
        ((version_option.registered_name().c_str()),
            version_option.text().c_str())
        ((midi_in_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            midi_in_option.text().c_str())
        ((answer_option.registered_name().c_str()),
            answer_option.text().c_str())
        ((textmidi_out_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            textmidi_out_option.text().c_str())
        ((quantize_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            quantize_option.text().c_str())
        ((lazy_option.registered_name().c_str()),
            lazy_option.text().c_str())
        ((dynamics_configuration_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            dynamics_configuration_option.text().c_str())
        ((dotted_rhythms_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            dotted_rhythms_option.text().c_str())
        ((rhythm_expression_option.registered_name().c_str()),
            boost::program_options::value<string>(),
            rhythm_expression_option.text().c_str())
    ;
    boost::program_options::positional_options_description pos_opts_desc;
    boost::program_options::variables_map var_map;
    try
    {
        pos_opts_desc.add(midi_in_option.option().c_str(), -1);
        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv)
            .options(desc).positional(pos_opts_desc).run(), var_map);
        boost::program_options::notify(var_map);
    }
    catch (std::logic_error& err)
    {
        const string errstr{(string{"Program options error: "}
            += err.what()) += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }


    if (var_map.count(help_option.option()))
    {
        const string logstr{((string{"Usage: miditext [OPTION]... "
            "[MIDIFILE]\nmiditext Version 1.0.92\n"}
            += lexical_cast<string>(desc)) += '\n')
            += "Report bugs to: janzentome@gmail.com\nmiditext home page: "
               "https://github.com/tomejanzen/textmiditools\n"};
        cout << logstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(version_option.option())) [[unlikely]]
    {
        cout << "miditext\nTextMIDITools 1.0.92\n"
            "Copyright © 2025 Thomas E. Janzen\n"
            "License GPLv3+: GNU GPL version 3 "
            "or later <https://gnu.org/licenses/gpl.html>\n"
            "This is free software: you are free to "
            "change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    string midi_filename;
    if (var_map.count(midi_in_option.option()))
    {
        midi_filename = var_map[midi_in_option.option()].as<string>();
        if (!exists(midi_filename))
        {
            const string errstr{((midi_in_option.option() + ' ')
                += midi_filename) += " File does not exist.\n"};
            cerr  << errstr;
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        const string errstr{(string{"You must provide a MIDI input file\n"}
            += lexical_cast<string>(desc)) += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }
    RhythmRational quantum{0};
    if (var_map.count(quantize_option.option())) [[unlikely]]
    {
        string quantum_string{var_map[quantize_option.option()].as<string>()};
        istringstream iss{quantum_string};
        iss >> quantum;
    }
    bool lazy{};
    if (var_map.count(lazy_option.option()))
    {
        lazy = true;
    }

    bool verbose{};
    if (var_map.count(verbose_option.option())) [[unlikely]]
    {
        verbose = true;
    }
    bool answer{};
    if (var_map.count(answer_option.option())) [[unlikely]]
    {
        answer = true;
    }

    string text_filename;
    if (var_map.count(textmidi_out_option.option()))
    {
        text_filename = var_map[textmidi_out_option.option()].as<string>();
    }
    else
    {
        text_filename = midi_filename + ".txt";
        if (verbose)
        {
            const string logstr{(string{"Will write to "}
                += text_filename) += '\n'};
            cout << logstr;
        }
    }

    {
        string dynamics_configuration_file{};
        if (var_map.count(dynamics_configuration_option.option())) [[unlikely]]
        {
            dynamics_configuration_file
                = var_map[dynamics_configuration_option.option()].as<string>();
        }
        midi::dynamics_map = textmidi::read_dynamics_configuration
            (dynamics_configuration_file);
    }

    bool dotted_rhythms{true};
    if (var_map.count(dotted_rhythms_option.option())) [[unlikely]]
    {
        string dotted_rhythms_string{
            var_map[dotted_rhythms_option.option()].as<string>()};
        to_upper(dotted_rhythms_string);
        dotted_rhythms = ("TRUE" == dotted_rhythms_string);
    }

    if (var_map.count(rhythm_expression_option.option())) [[unlikely]]
    {
        string rhythm_expression_string{
            var_map[rhythm_expression_option.option()].as<string>()};
        to_upper(rhythm_expression_string);
        if (midi::rhythm_expression_map.contains(rhythm_expression_string))
        {
            const RhythmExpression rhythm_expression{midi::rhythm_expression_map
                    [rhythm_expression_string]};
            switch (rhythm_expression)
            {
              case RhythmExpression::Rational:
                textmidi::rational::print_rhythm
                    = make_unique<PrintRhythmRational>(dotted_rhythms);
                break;
              case RhythmExpression::SimpleContinuedFraction:
                textmidi::rational::print_rhythm
                    = make_unique<textmidi::rational
                       ::PrintRhythmSimpleContinuedFraction>();
                break;
            }
        }
    }
    else
    {
        if (var_map.count(dotted_rhythms_option.option())) [[unlikely]]
        {
            textmidi::rational::print_rhythm
                = make_unique<PrintRhythmRational>(dotted_rhythms);
        }
    }


    if (answer && exists(midi_filename))
    {
        const string answer_prompt{(string{"Overwrite "}
                += midi_filename) += "?\n"};
        cout << answer_prompt;
        string answerstr{};
        cin >> answerstr;
        if (!((answerstr[0] == 'y') || (answerstr[0] == 'Y')))
        {
            exit(EXIT_SUCCESS);
        }
    }
    if (midi_filename == text_filename)
    {
        cerr << "The text and MIDI filenames are the same!  "
            "You would have overwritten the MIDI file!; Must exit!\n";
        exit(EXIT_SUCCESS);
    }

    midi::MidiStreamVector midivector{};
    try
    {
        ifstream midifilestr;
        midifilestr.open(midi_filename, std::ios_base::binary);
        if (!midifilestr)
        {
            const string errstr{(string{"Open failed for "} += midi_filename)
                += '\n'};
            cerr << errstr;
            return EXIT_FAILURE;
        }
        const auto len{file_size(midi_filename)};
        midivector.resize(len);
        midifilestr.read(reinterpret_cast<char*>(midivector.data()), len);
        midifilestr.close();
    }
    catch (std::ios_base::failure &iosfail)
    {
        const string errstr{string{iosfail.what()} += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }

    ofstream text_filestr(text_filename.c_str());

    if (!text_filestr)
    {
        const string logstr{(string{"can't open "} += text_filename) += '\n'};
        cout << logstr;
    }

    midi::MidiStreamIterator midiiter{midivector.begin()};

    if (midivector.size() < sizeof(midi::MidiHeader))
    {
        cerr << "ERROR File too short for a MIDI Header\n";
        exit(0);
    }
    midi::MidiHeader midi_header{midivector.data()};
    if (verbose)
    {
        const string logstr{string{lexical_cast<string>(midi_header)} += '\n'};
        cout << logstr;
    }
    text_filestr << "FILEHEADER ";
    midiiter += sizeof(midi::MidiHeader);
    const uint32_t ticksperquarter{midi_header.division_};
    uint32_t ticks_per_whole{
        static_cast<uint32_t>(ticksperquarter * midi::QuartersPerWhole)};

    string textmidi_str{};
    textmidi_str.reserve(32);
    (((((textmidi_str += lexical_cast<string>(midi_header.ntrks_)) += ' ')
       += lexical_cast<string>(midi_header.division_)) += ' ')
       += lexical_cast<string>(midi_header.format_)) += '\n';
    text_filestr << textmidi_str;
    if (verbose)
    {
        const string logstr{(string{"FORMAT: "}
            += lexical_cast<string>(midi_header.format_)) += '\n'};
        cout << logstr;
    }
    size_t track_qty{midi_header.ntrks_};

    vector<StreamLengthPair> stream_length_pairs{};
    find_tracks(midiiter, midivector.end(), stream_length_pairs, track_qty);
    vector<DelayEvents> midi_delay_event_tracks(stream_length_pairs.size());

#undef DEBUG_THREADLESS
#if defined(DEBUG_THREADLESS)
    for (int32_t i{}; auto& ti : stream_length_pairs)
    {
        ConvertTrack convert_track{
            ti, midi_delay_event_tracks[i], ticks_per_whole,
            quantum};
        convert_track();
        ++i;
    }
#else
    {
        vector<std::jthread> track_threads(stream_length_pairs.size());

        for (int32_t i{}; auto& ti : stream_length_pairs)
        {
            ConvertTrack convert_track{ti, midi_delay_event_tracks[i],
                ticks_per_whole, quantum};
            std::jthread track_thread{convert_track};
            track_threads[i] = std::move(track_thread);
            ++i;
        }
    }
#endif
    if (verbose)
    {
        const string logstr{(string{"Will write to: "} += text_filename)
            += '\n'};
        cout << logstr;

        cout << "Events per track:\n";
        for (int32_t i{}; auto& mdep : midi_delay_event_tracks)
        {
            cout << "  Track: " << setw(3) << (i + 1)
                 << ": " << setw(8) << mdep.size() << '\n';
            ++i;
        }
    }
    int64_t rigid_rhythms_count{};
    int64_t non_rigid_rhythms_count{};
    int64_t zero_rhythms_count{};
    for (auto& mdet : midi_delay_event_tracks)
    {
        text_filestr << "\nSTARTTRACK\n";
        if (verbose)
        {
            // If a score was entered in a scoring program rather
            // than played in,
            // it should largely contain delays that are simply related to
            // whole notes and their musical ratios will have low divisors.
            const set<int64_t> musical_divisors{1, 2, 4, 6, 8, 12, 16};
            const auto ticks_per_whole_local{
                midi::QuartersPerWhole * ticksperquarter};
            zero_rhythms_count += count_if(mdet, [](DelayEvent mde)
                { return !mde.first; } );
            rigid_rhythms_count += count_if(mdet,
                [ticks_per_whole_local, musical_divisors](DelayEvent mde) {
                const RhythmRational
                    rr{mde.first, static_cast<int64_t>(ticks_per_whole_local)};
                return musical_divisors.contains(rr.denominator()); } );
            non_rigid_rhythms_count += count_if(mdet,
                [ticks_per_whole_local, musical_divisors](DelayEvent mde) {
                const RhythmRational rr{mde.first,
                    static_cast<int64_t>(ticks_per_whole_local)};
                return !musical_divisors.contains(rr.denominator()); } );
        }
        if (lazy)
        {
            PrintLazyTrack print_lazy_track{mdet, quantum, ticksperquarter};
            text_filestr << print_lazy_track << '\n';
        }
        else
        {
            copy(mdet, std::ostream_iterator<DelayEvent>(text_filestr, "\n"));
        }
    }
    if (verbose)
    {
        rigid_rhythms_count -= zero_rhythms_count;
        const double rigid_rhythms_fraction{
            static_cast<double>(rigid_rhythms_count)
            / static_cast<double>(non_rigid_rhythms_count
            + rigid_rhythms_count)};
        cout << "Rigid rhythms: " << (rigid_rhythms_fraction * 100.0) << "%\n";
    }
    return EXIT_SUCCESS;
}
