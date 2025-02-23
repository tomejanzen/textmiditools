//
// TextMIDITools Version 1.0.90
//
// textmidicgm 1.0
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(MIDIMAPS_H)
#    define  MIDIMAPS_H

#include <ostream>
#include <utility>
#include <ranges>
#include <optional>

#include "Midi.h"
#include "RhythmRational.h"

namespace midi
{
    std::ostream& operator<<(std::ostream& os, const midi::MidiHeader& mh);

    template<typename NumType> class NumStringMap
    {
      public:
        NumStringMap(std::initializer_list<const std::pair<NumType, std::string_view> > in_initlist) noexcept
          : num_string_map_{in_initlist.begin(), in_initlist.end()}
        {
            std::for_each(in_initlist.begin(), in_initlist.end(),
                [this](const std::pair<const NumType, const std::string_view>& p)
                {this->string_num_map_.emplace(p.second, p.first); });
        }

        NumStringMap(std::initializer_list<std::pair<const std::string_view, NumType> > in_initlist) noexcept
          : string_num_map_(in_initlist)
        {
            std::for_each(in_initlist.begin(), in_initlist.end(),
                [this](const std::pair<const std::string_view, const NumType>& p)
                {this->num_string_map_.emplace(p.second, p.first); });
        }

        std::string_view at(NumType num) const noexcept
        {
            return num_string_map_.at(num);
        }

        std::string_view operator[](NumType num) const noexcept
        {
            return num_string_map_.at(num);
        }

        NumType at(const std::string_view& sv) const noexcept
        {
            return string_num_map_.at(sv);
        }

        NumType operator[](const std::string_view& sv) const noexcept
        {
            return string_num_map_.at(sv);
        }

        bool contains(NumType num) const noexcept
        {
            return num_string_map_.contains(num);
        }

        bool contains(std::string_view sv) const noexcept
        {
            return string_num_map_.contains(sv);
        }

        std::optional<NumType> operator()(std::string_view str) const noexcept
        {
            if (this->contains(str))
            {
                return this->at(str);
            }
            else
            {
                return {};
            }
        }

        std::optional<std::string_view> operator()(NumType num) const noexcept
        {
            if (this->contains(num))
            {
                return this->at(num);
            }
            else
            {
                return {};
            }
        }

        const std::map<std::string_view, NumType> string_num_map() const noexcept
        {
            return string_num_map_;
        }

        void emplace(std::string_view sv, NumType num) noexcept
        {
            string_num_map_.emplace(sv, num);
            num_string_map_.emplace(num, sv);
        }

        void insert(std::map<std::string_view, NumType>::value_type insert_value) noexcept
        {
            string_num_map_.insert(insert_value);
            num_string_map_.emplace(insert_value.second, insert_value.first);
        }

      private:
        std::map<NumType, std::string_view> num_string_map_{};
        std::map<std::string_view, NumType> string_num_map_{};
    };

    extern const NumStringMap<int> smpte_fps_map;
    extern NumStringMap<int> dynamics_map;
    extern const NumStringMap<int> pan_map;
    extern const NumStringMap<midi::MIDI_Format> format_map;
    extern const NumStringMap<midi::MidiStreamAtom> text_meta_map;
    extern const NumStringMap<midi::MidiStreamAtom> control_function_map;
    extern const NumStringMap<MidiStreamAtom> control_on_off_map;
    extern const NumStringMap<midi::XmfPatchTypeEnum> xmf_patch_type_map;

    extern const NumStringMap<midi::MidiStreamAtom> sysex_subid_map;
    extern const NumStringMap<midi::MidiStreamAtom> sysex_nonrt_id1_map;
    extern const NumStringMap<midi::MidiStreamAtom> sysex_rt_id1_map;
    std::ostream& operator<<(std::ostream& os, midi::MIDI_Format mf);

    extern const NumStringMap<midi::MidiStreamAtom> sysex_device_id_map;
    extern const NumStringMap<midi::Registered00ParameterLsbs> parm_00_map;
    extern const NumStringMap<midi::Registered3dParameterLsbs> parm_3d_map;

    extern const NumStringMap<midi::RunningStatusPolicy> running_status_policy_map;

    extern const NumStringMap<textmidi::rational::RhythmExpression> rhythm_expression_map;

}

#endif

