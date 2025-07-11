//
// TextMIDITools Version 1.1.0
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(COMPOSER_H)
#    define  COMPOSER_H

#include <fstream>
#include <list>
#include <memory>
#include <random>
#include <vector>

#include "Arrangements.h"
#include "Midi.h"
#include "MidiMaps.h"
#include "MusicalForm.h"
#include "RhythmRational.h"

namespace textmidi
{
    namespace cgm
    {
        class TimeConverter
        {
          public:
            explicit TimeConverter(const MusicTime& music_time)
              : music_time_(music_time)
            {
                rational::RhythmRational beat_tempo{};
                beat_tempo.from_double(music_time_.beat_tempo_, music_time_.ticks_per_quarter_ * midi::QuartersPerWhole);
                wholes_per_second_ = beat_tempo * music_time_.beat_ / rational::RhythmRational{midi::SecondsPerMinuteI};

                const rational::RhythmRational ticks_per_second_rat(QuartersPerWholeRat
                    * wholes_per_second_
                    * rational::RhythmRational{music_time_.ticks_per_quarter_});
                const double ticks_per_second_dbl{
                    static_cast<double>(ticks_per_second_rat.numerator())
                    / static_cast<double>(ticks_per_second_rat.denominator())};
                ticks_per_second_ = static_cast<int64_t>(round(ticks_per_second_dbl));
            }
            rational::RhythmRational wholes_per_second() const;
            rational::RhythmRational duration_to_rhythm(double duration) const;
            rational::RhythmRational snap_to_pulse(rational::RhythmRational rhythm, double pulse) const;
            std::int64_t ticks_per_whole() const;
            int64_t ticks_per_second() const
            {
                return ticks_per_second_;
            }
          private:
            MusicTime music_time_;
            rational::RhythmRational wholes_per_second_{1L, 4L};
            int64_t ticks_per_second_{720L};
        };

        class Composer
        {
          public:
            struct TrackScramble
            {
                TrackScramble() = default;
                explicit constexpr TrackScramble(
                    arrangements::PermutationEnum scramble,
                    rational::RhythmRational period) noexcept
                  : scramble_(scramble),
                    period_(period),
                    arrangements_{}
                {}

                arrangements::PermutationEnum scramble_
                    {arrangements::PermutationEnum::Identity};
                rational::RhythmRational     period_{120UL};
                std::shared_ptr<arrangements::Arrangements> arrangements_{};
            };

            Composer(bool gnuplot, bool answer,
                arrangements::PermutationEnum track_scramble,
                rational::RhythmRational track_scramble_period, const MusicTime& music_time,
                size_t max_events_per_track = 100000) noexcept
              : gnuplot_(gnuplot),
                answer_(answer),
                track_scramble_(track_scramble, track_scramble_period),
                max_events_per_track_{max_events_per_track},
                random_dev_(),
                generator_{random_dev_()},
                music_time_{music_time},
                time_converter_{music_time}
            {
            }

            // Rule of 5: If you delete or declare any of assign, copy, move,
            // move copy, d-tor, then delete or declare all of them.
            Composer(const Composer& ) = delete;
            Composer(Composer&& ) = delete;
            Composer& operator=(const Composer& ) = delete;
            Composer& operator=(Composer&& ) = delete;

            void operator()(std::ofstream& textmidi_file,
                const MusicalForm& xml_form, bool write_header = true);

          private:
            void build_track_scramble_sequences(
                std::vector<std::vector<std::int32_t>>&
                track_scramble_sequences,
                rational::RhythmRational total_duration) noexcept;
            void build_composition_priority_graph(const MusicalForm& xml_form,
                std::vector<std::list<std::int32_t>>&
                leaders_topo_sort) noexcept;
            bool gnuplot_;
            bool answer_;
            TrackScramble track_scramble_;
            size_t max_events_per_track_;
            std::random_device random_dev_;
            std::mt19937 generator_;
            MusicTime music_time_;
            TimeConverter time_converter_;
        };

        extern const midi::NumStringMap<arrangements::PermutationEnum>
            arrangement_map;
    } // namespace cgm
} // namespace textmidi

#endif

