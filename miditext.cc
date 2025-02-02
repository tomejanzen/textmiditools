//
// TextMIDITools Version 1.0.86
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// Written as mididisasm in 2003 as a companion to textmidi.
//
// miditext is the main driver for miditext, which converts a binary MIDI standard file
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

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp> // to_upper

#include "rational_support.h"
#include "TextmidiUtils.h"
#include "MidiEvents.h"
#include "MidiMaps.h"
#include "Options.h"
#include "DynamicsOptions.h"

using namespace std;
using namespace boost;

using namespace midi;
using namespace textmidi;
using namespace textmidi::rational;

namespace
{
    using StreamLengthPair = pair<MidiStreamIterator, int>;
    const string QuantizeOpt{"quantize"};
    constexpr char QuantizeTxt[]{"quantization ratio in quotes: \"1/32\""};
    const string LazyOpt{"lazy"};
    constexpr char LazyTxt[]{"Try to write in textmidi's lazy mode"};

    void find_tracks(MidiStreamIterator midiiter, MidiStreamIterator midiend,
            vector<StreamLengthPair>& track_iters, size_t expected_track_qty)
    {
        track_iters.clear();
        track_iters.reserve(140); // one file had 140 tracks
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
                    const string errstr{(string{"No Track header MTrk in track: "}
                        += track_num) += '\n'};
                    cerr << errstr;
                }
                return;
            }
            int32_t num{};
            copy(midiiter, midiiter + sizeof(num), io_bytes(num));
            midiiter += sizeof(num);
            num = htobe32(num);
            if (std::distance(&(*midiiter), &(*midiend)) < num)
            {
                const string errstr{(string{"File too short for track length in track:"}
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
            const auto midiend{stream_length_pair_.first + stream_length_pair_.second};

            MidiEventFactory midi_event_factory{midiend, ticks_per_whole_};
            DelayEvent midi_delay_msg_pair;
            int64_t ticks_accumulated{};

            do
            {
                midi_delay_msg_pair = midi_event_factory(midiiter, ticks_accumulated);
                message_pairs_.push_back(midi_delay_msg_pair);
            }
            while ((midiiter < midiend)
                && !dynamic_cast<MidiFileMetaEndOfTrackEvent*>(midi_delay_msg_pair.second.get()));
        }
     private:
        const StreamLengthPair stream_length_pair_;
        DelayEvents& message_pairs_;
        const uint32_t ticks_per_whole_;
        const RhythmRational quantum_;
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
        ((DynamicsConfigurationOpt + ",y").c_str(), program_options::value<string>(),   DynamicsConfigurationTxt)
        ((DottedRhythmsOpt + ",w").c_str(),    program_options::value<string>(), DottedRhythmsTxt)
        ((RhythmExpressionOpt + ",e").c_str(), program_options::value<string>(), RhythmExpressionTxt)
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
        const string errstr{(string{"Program options error: "} += err.what()) += '\n'};
        cerr << errstr;
        exit(EXIT_SUCCESS);
    }


    if (var_map.count(HelpOpt))
    {
        const string logstr{((string{"Usage: miditext [OPTION]... [MIDIFILE]\nmiditext Version 1.0.86\n"}
            += lexical_cast<string>(desc)) += '\n')
            += "Report bugs to: janzentome@gmail.com\nmiditext home page: <https://www\n"};
        cout << logstr;
        exit(EXIT_SUCCESS);
    }

    if (var_map.count(VersionOpt)) [[unlikely]]
    {
        cout << "miditext\nTextMIDITools 1.0.86\nCopyright © 2024 Thomas E. Janzen\n"
            "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"
            "This is free software: you are free to change and redistribute it.\n"
            "There is NO WARRANTY, to the extent permitted by law.\n";
        exit(EXIT_SUCCESS);
    }

    string midi_filename;
    if (var_map.count(MidiOpt))
    {
        midi_filename = var_map[MidiOpt].as<string>();
        if (!filesystem::exists(midi_filename))
        {
            const string errstr{((string{MidiOpt} += ' ') += midi_filename) += " File does not exist.\n"};
            cerr  << errstr;
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        const string errstr{(string{"You must provide a MIDI input file\n"} += lexical_cast<string>(desc)) += '\n'};
        cerr << errstr;
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
    if (var_map.count(LazyOpt))
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

    string text_filename;
    if (var_map.count(TextmidiOpt))
    {
        text_filename = var_map[TextmidiOpt].as<string>();
    }
    else
    {
        text_filename = midi_filename + ".txt";
        if (verbose)
        {
            const string logstr{(string{"Will write to "} += text_filename) += '\n'};
            cout << logstr;
        }
    }

    {
        string dynamics_configuration_file{};
        if (var_map.count(DynamicsConfigurationOpt)) [[unlikely]]
        {
            dynamics_configuration_file = var_map[DynamicsConfigurationOpt].as<string>();
        }
        midi::dynamics_map = textmidi::read_dynamics_configuration(dynamics_configuration_file);
    }

    bool dotted_rhythms{true};
    if (var_map.count(DottedRhythmsOpt)) [[unlikely]]
    {
        string dotted_rhythms_string{var_map[DottedRhythmsOpt].as<string>()};
        to_upper(dotted_rhythms_string);
        dotted_rhythms = ("TRUE" == dotted_rhythms_string);
    }

    if (var_map.count(RhythmExpressionOpt)) [[unlikely]]
    {
        string rhythm_expression_string{var_map[RhythmExpressionOpt].as<string>()};
        to_upper(rhythm_expression_string);
        if (midi::rhythm_expression_map.contains(rhythm_expression_string))
        {
            const rational::RhythmExpression rhythm_expression{rhythm_expression_map[rhythm_expression_string]};
            switch (rhythm_expression)
            {
              case textmidi::rational::RhythmExpression::Rational:
                textmidi::rational::print_rhythm = make_unique<PrintRhythmRational>(dotted_rhythms);
                break;
              case textmidi::rational::RhythmExpression::SimpleContinuedFraction:
                textmidi::rational::print_rhythm = make_unique<PrintRhythmSimpleContinuedFraction>();
                break;
            }
        }
    }
    else
    {
        if (var_map.count(DottedRhythmsOpt)) [[unlikely]]
        {
            textmidi::rational::print_rhythm = make_unique<PrintRhythmRational>(dotted_rhythms);
        }
    }


    if (answer && filesystem::exists(midi_filename))
    {
        const string answer_prompt{(string{"Overwrite "} += midi_filename) += "?\n"};
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
        cerr << "The text and MIDI filenames are the same!  You would have overwritten the MIDI file!; Must exit!\n";
        exit(EXIT_SUCCESS);
    }

    MidiStreamVector midivector{};
    try
    {
        ifstream midifilestr;
        midifilestr.open(midi_filename, ios_base::binary);
        if (!midifilestr)
        {
            const string errstr{(string{"Open failed for "} += midi_filename) += '\n'};
            cerr << errstr;
            return EXIT_FAILURE;
        }
        const auto len{filesystem::file_size(midi_filename)};
        midivector.resize(len);
        midifilestr.read(reinterpret_cast<char*>(midivector.data()), len);
        midifilestr.close();
    }
    catch (ios_base::failure &iosfail)
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

    MidiStreamIterator midiiter{midivector.begin()};

    if (midivector.size() < sizeof(MidiHeader))
    {
        cerr << "ERROR File too short for a MIDI Header\n";
        exit(0);
    }
    MidiHeader midi_header{midivector.data()};
    if (verbose)
    {
        const string logstr{string{lexical_cast<string>(midi_header)} += '\n'};
        cout << logstr;
    }
    text_filestr << "FILEHEADER ";
    midiiter += sizeof(MidiHeader);
    const uint32_t ticksperquarter{midi_header.division_};
    uint32_t ticks_per_whole{static_cast<uint32_t>(ticksperquarter * QuartersPerWhole)};

    string textmidi_str{};
    textmidi_str.reserve(32);
    (((((textmidi_str += lexical_cast<string>(midi_header.ntrks_)) += ' ')
       += lexical_cast<string>(midi_header.division_)) += ' ')
       += lexical_cast<string>(midi_header.format_)) += '\n';
    text_filestr << textmidi_str;
    if (verbose)
    {
        const string logstr{(string{"FORMAT: "} += lexical_cast<string>(midi_header.format_)) += '\n'};
        cout << logstr;
    }
    size_t track_qty{midi_header.ntrks_};

    vector<StreamLengthPair> stream_length_pairs{};
    find_tracks(midiiter, midivector.end(), stream_length_pairs, track_qty);
    vector<DelayEvents> midi_delay_event_tracks(stream_length_pairs.size());

#define DEBUG_THREADLESS
#if defined(DEBUG_THREADLESS)
    for (int i{}; auto& ti : stream_length_pairs)
    {
        ConvertTrack convert_track{ti, midi_delay_event_tracks[i], ticks_per_whole,
            quantum};
        convert_track();
        ++i;
    }
#else
    {
        vector<jthread> track_threads(stream_length_pairs.size());

        for (int i{}; auto& ti : stream_length_pairs)
        {
            ConvertTrack convert_track{ti, midi_delay_event_tracks[i], ticks_per_whole,
                quantum};
            jthread track_thread{convert_track};
            track_threads[i] = std::move(track_thread);
            ++i;
        }
    }
#endif
    if (verbose)
    {
        const string logstr{(string{"Will write to: "} += text_filename) += '\n'};
        cout << logstr;
    }

    if (verbose)
    {
        cout << "Events per track:\n";
        for (int i{}; auto& mdep : midi_delay_event_tracks)
        {
            cout << "  Track: " << setw(3) << (i + 1)
                 << ": " << setw(8) << mdep.size() << '\n';
            ++i;
        }
    }
    int64_t rigid_rhythms_count{};
    int64_t non_rigid_rhythms_count{};
    int64_t zero_rhythms_count{};
    for (int i{}; auto& mdet : midi_delay_event_tracks)
    {
        text_filestr << "\nSTARTTRACK\n";
        if (verbose)
        {
            // If a score was entered in a scoring program rather than played in,
            // it should largely contain delays that are simply related to
            // whole notes and their musical ratios will have low divisors.
            const set<int64_t> musical_divisors{1, 2, 4, 6, 8, 12, 16};
            const auto ticks_per_whole_local{QuartersPerWhole * ticksperquarter};
            zero_rhythms_count += ranges::count_if(mdet, [](DelayEvent mde) { return !mde.first; } );
            rigid_rhythms_count += ranges::count_if(mdet,
                [ticks_per_whole_local, musical_divisors](DelayEvent mde) {
                const RhythmRational rr{mde.first, static_cast<int64_t>(ticks_per_whole_local)};
                return musical_divisors.contains(rr.denominator()); } );
            non_rigid_rhythms_count += ranges::count_if(mdet,
                [ticks_per_whole_local, musical_divisors](DelayEvent mde) {
                const RhythmRational rr{mde.first, static_cast<int64_t>(ticks_per_whole_local)};
                return !musical_divisors.contains(rr.denominator()); } );
        }
        if (lazy)
        {
            PrintLazyTrack print_lazy_track{mdet, quantum, ticksperquarter};
            text_filestr << print_lazy_track << '\n';
        }
        else
        {
            ranges::copy(mdet, ostream_iterator<DelayEvent>(text_filestr, "\n"));
        }
        ++i;
    }
    if (verbose)
    {
        rigid_rhythms_count -= zero_rhythms_count;
        const double rigid_rhythms_fraction{static_cast<double>(rigid_rhythms_count) / static_cast<double>(non_rigid_rhythms_count + rigid_rhythms_count)};
        cout << "Rigid rhythms: " << (rigid_rhythms_fraction * 100.0) << "%\n";
    }
    return EXIT_SUCCESS;
}
