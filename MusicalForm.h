//
// TextMIDITools Version 1.0.19
//
// textmidicgm 1.0
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(TEXTMIDIFORM)
#    define  TEXTMIDIFORM

#include <string>
#include <vector>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

#include "cgmlegacy.h"
#include "Voice.h"
#include "Track.h"
#include "RandomDouble.h"
#include "RandomInt.h"
#include "Scales.h"
#include "GeneralMIDI.h"
#include "MIDIKeyString.h"

namespace cgm
{
    constexpr double MinDynamic{30};
    constexpr double MaxDynamic{127};

    class MusicalFormException
    {
      public:
        explicit MusicalFormException(const std::string& exception_msg)
          : exception_msg_(exception_msg)
        {
        }
        const std::string& what() const
        {
            return exception_msg_;
        }
      private:
        std::string exception_msg_;
    };

    class Sine
    {
        friend class boost::serialization::access;
      public:
        Sine()
          : period_{60.0},
            phase_{0.0},
            amplitude_{0.5},
            offset_{0.5}
        {
        }
        explicit Sine(double period, double phase = 0.0,
                      double amplitude = 0.5, double offset = 0.5)
          : period_{period},
            phase_{phase},
            amplitude_{amplitude},
            offset_{offset}
        {
        }
        Sine(const Sine& ) = default;
        Sine(Sine&& ) = default;
        Sine& operator=(const Sine& ) = default;
        Sine& operator=(Sine&& ) = default;

        double period() const noexcept;
        void period(double period) noexcept;
        double phase() const noexcept;
        void phase(double phase) noexcept;
        double amplitude() const noexcept;
        void amplitude(double amplitude) noexcept;
        double offset() const noexcept;
        void offset(double offset) noexcept;
        double value_now(double the_time) const;
        bool valid() const;
      private:
        double period_;
        double phase_;
        double amplitude_;
        double offset_;

        template<class Archive>
            void serialize(Archive& arc, const unsigned int )
        {
            arc & BOOST_SERIALIZATION_NVP(period_);
            arc & BOOST_SERIALIZATION_NVP(phase_);
            arc & BOOST_SERIALIZATION_NVP(amplitude_);
            arc & BOOST_SERIALIZATION_NVP(offset_);
        }
    };

    class MeanRangeSines
    {
        friend class boost::serialization::access;
      public:
        MeanRangeSines()
          : mean_sine_{},
            range_sine_{}
        {
        }
        MeanRangeSines(const MeanRangeSines& ) = default;
        explicit MeanRangeSines(MeanRangeSines&& ) = default;
        MeanRangeSines& operator=(const MeanRangeSines& ) = default;
        MeanRangeSines& operator=(MeanRangeSines&& ) = default;

        explicit MeanRangeSines(const cgmlegacy::OldFormElement& form_element)
          : mean_sine_ {form_element.mean_period(),
                        form_element.mean_phase()},
            range_sine_{form_element.range_period(),
                        form_element.range_phase()}
        {
        }
        const Sine& mean_sine() const noexcept;
        Sine& mean_sine() noexcept;
        const Sine& range_sine() const noexcept;
        Sine& range_sine() noexcept;
        bool valid() const;
      private:
        Sine mean_sine_;
        Sine range_sine_;
        template<class Archive>
            void serialize(Archive& arc, const unsigned int )
        {
            arc & BOOST_SERIALIZATION_NVP(mean_sine_);
            arc & BOOST_SERIALIZATION_NVP(range_sine_);
        }
    };

    class MelodyProbabilities
    {
      public:
        enum class MelodyDirection
        {
            Rest = 0,
            Down = 1,
            Same = 2,
            Up   = 3
        };

        MelodyProbabilities() :
            down_(0.0),
            same_(0.33),
            up_(0.67)
        {
        }

        MelodyDirection operator()(double random_variable) const;
        template<class Archive>
            void serialize(Archive& arc, const unsigned int )
        {
            arc & BOOST_SERIALIZATION_NVP(down_);
            arc & BOOST_SERIALIZATION_NVP(same_);
            arc & BOOST_SERIALIZATION_NVP(up_);
        }

        void down(double down) noexcept
        {
            down_ = down;
        }

        void same(double same) noexcept
        {
            same_ = same;
        }

        void up(double up) noexcept
        {
            up_ = up;
        }
      private:
        double down_;
        double same_;
        double up_;
    };

    struct MusicalCharacter
    {
        MusicalCharacter()
          : pitch_mean(),
            pitch_range(),
            rhythm_mean(),
            rhythm_range(),
            dynamic_mean(),
            dynamic_range(),
            texture_range()
        {
        }

        MusicalCharacter(const MusicalCharacter& ) = default;
        MusicalCharacter(MusicalCharacter&& ) = default;
        MusicalCharacter& operator=(const MusicalCharacter& ) = default;
        MusicalCharacter& operator=(MusicalCharacter&& ) = default;
        double pitch_mean;
        double pitch_range;
        double rhythm_mean;
        double rhythm_range;
        double dynamic_mean;
        double dynamic_range;
        double texture_range;
        double duration(double rf);
        int pitch_index(double rf);
    };

    class MusicalForm
    {
        friend class boost::serialization::access;
      public:
        MusicalForm()
          : name_{},
            len_{},
            min_note_len_{},
            max_note_len_{},
            scale_{},
            pulse_{},
            melody_probabilities_{},
            pitch_form_{},
            rhythm_form_{},
            dynamic_form_{},
            texture_form_{},
            voices_{}
        {
        }
        MusicalForm(const MusicalForm& )  = default;
        MusicalForm(MusicalForm&& ) = default;
        MusicalForm& operator=(const MusicalForm& ) = default;
        MusicalForm& operator=(MusicalForm&& ) = default;

        // A ctor for converting old files
        MusicalForm(const std::string& name, const cgmlegacy::TextForm& form)
          : name_{name},
            len_{form.len},
            min_note_len_{form.min_note_len},
            max_note_len_{form.max_note_len},
            scale_{},
            pulse_{form.pulse},
            melody_probabilities_{},
            pitch_form_{form.pitch_form},
            rhythm_form_{form.rhythm_form},
            dynamic_form_{form.dynamic_form},
            texture_form_{Sine{form.texture_form.range_period(),
                               form.texture_form.range_phase()}},
            voices_{}
        {
            scale_.resize(form.scale.size());
            transform(form.scale.begin(), form.scale.end(),
                scale_.begin(), [](const std::uint32_t notenum)
                { return textmidi::num_to_note(notenum); });
            voices_.clear();
            voices_.insert(voices_.begin(), form.voices.begin(),
                           form.voices.begin() + form.voice_qty);
        }

        std::string name() const noexcept;
        void name(const std::string_view name) noexcept;
        double len() const noexcept;
        void len(double len) noexcept;
        double min_note_len() const noexcept;
        void min_note_len(double min_note_len) noexcept;
        double max_note_len() const noexcept;
        void max_note_len(double max_note_len) noexcept;
        const std::vector<std::string>& scale() const noexcept;
        std::vector<std::string>& scale() noexcept;
        double pulse() const noexcept;
        void pulse(double pulse) noexcept;
        const MelodyProbabilities& melody_probabilities() const noexcept;
        void melody_probabilities(const MelodyProbabilities& melody_probabilities) noexcept;
        MeanRangeSines pitch_form() const noexcept;
        MeanRangeSines& pitch_form() noexcept;
        MeanRangeSines rhythm_form() const noexcept;
        MeanRangeSines& rhythm_form() noexcept;
        MeanRangeSines dynamic_form() const noexcept;
        MeanRangeSines& dynamic_form() noexcept;
        Sine texture_form() const noexcept;
        Sine& texture_form() noexcept;
        void texture_form(const Sine& s) noexcept;
        std::vector<VoiceXml> voices() const noexcept;
        std::vector<VoiceXml>& voices() noexcept;
        void string_scale_to_int_scale(std::vector<int>& key_scale) const;
        void character_now(TicksDuration theTime,
                MusicalCharacter& musical_character) const;
        void random(std::string formname = "random", std::int32_t instrument_flags = static_cast<std::int32_t>(GeneralMIDIGroup::Melodic));
        bool valid() const;
        void clamp_scale_to_instrument_ranges();
      private:
        std::string name_;
        double len_;
        double min_note_len_;
        double max_note_len_;
        std::vector<std::string> scale_;
        double pulse_;
        MelodyProbabilities melody_probabilities_;
        MeanRangeSines pitch_form_;
        MeanRangeSines rhythm_form_;
        MeanRangeSines dynamic_form_;
        Sine                 texture_form_;
        std::vector<VoiceXml>    voices_;
        template<class Archive>
            void serialize(Archive& arc, const unsigned int )
        {
            arc & BOOST_SERIALIZATION_NVP(name_);
            arc & BOOST_SERIALIZATION_NVP(len_);
            arc & BOOST_SERIALIZATION_NVP(min_note_len_);
            arc & BOOST_SERIALIZATION_NVP(max_note_len_);
            arc & BOOST_SERIALIZATION_NVP(scale_);
            arc & BOOST_SERIALIZATION_NVP(pulse_);
            arc & BOOST_SERIALIZATION_NVP(melody_probabilities_);
            arc & BOOST_SERIALIZATION_NVP(pitch_form_);
            arc & BOOST_SERIALIZATION_NVP(rhythm_form_);
            arc & BOOST_SERIALIZATION_NVP(dynamic_form_);
            arc & BOOST_SERIALIZATION_NVP(texture_form_);
            arc & BOOST_SERIALIZATION_NVP(voices_);
            this->valid();
        }
    };

    extern RandomDouble rd;
    extern RandomInt ri;

}

#endif
