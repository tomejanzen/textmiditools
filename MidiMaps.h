//
// TextMIDITools Version 1.0.49
//
// textmidicgm 1.0
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(MIDIMAPS_H)
#    define  MIDIMAPS_H

#include <ostream>

#include "Midi.h"

namespace midi
{
    std::ostream& operator<<(std::ostream& os, const midi::MidiHeader& mh);

    template<typename NumType> class NumStringMap
    {
      public:
        NumStringMap(std::initializer_list<std::pair<NumType, std::string_view> > in_initlist)
          : num_string_map_(),
            string_num_map_()
        {
            for (auto mi(in_initlist.begin()); mi != in_initlist.end(); ++mi)
            {
                num_string_map_.emplace(*mi);
                string_num_map_.emplace(std::pair<std::string_view, NumType>(mi->second, mi->first));
            }
        }

        NumStringMap(std::initializer_list<std::pair<std::string_view, NumType> > in_initlist)
          : num_string_map_(),
            string_num_map_()
        {
            for (auto mi(in_initlist.begin()); mi != in_initlist.end(); ++mi)
            {
                num_string_map_.emplace(std::pair<NumType, std::string_view>(mi->second, mi->first));
                string_num_map_.emplace(*mi);
            }
        }

        std::string_view at(NumType num) const
        {
            return num_string_map_.at(num);
        }

        std::string_view operator[](NumType num) const
        {
            return num_string_map_.at(num);
        }

        NumType at(const std::string_view& sv) const
        {
            return string_num_map_.at(sv);
        }

        NumType operator[](const std::string_view& sv) const
        {
            return string_num_map_.at(sv);
        }

        bool contains(NumType num) const
        {
            return num_string_map_.contains(num);
        }

        bool contains(std::string_view sv) const
        {
            return string_num_map_.contains(sv);
        }

        bool contains(std::initializer_list<std::string_view> candidates)
        {
            bool rtn{};
            for (auto candi{candidates.begin()}; candi != candidates.end(); ++candi)
            {
                if (this->contains(*candi))
                {
                    rtn = true;
                }
            }
            return rtn;
        }

        const std::map<std::string_view, NumType> string_num_map() const
        {
            return string_num_map_;
        }
      private:
        std::map<NumType, std::string_view> num_string_map_;
        std::map<std::string_view, NumType> string_num_map_;
    };

    extern const NumStringMap<int> smpte_fps_map;
    extern const NumStringMap<int> dynamics_map;
    extern const NumStringMap<int> pan_map;
    extern const NumStringMap<midi::MIDI_Format> format_map;
    extern const NumStringMap<midi::MidiStreamAtom> text_meta_map;
    extern const NumStringMap<midi::MidiStreamAtom> control_function_map;
    extern const NumStringMap<midi::XmfPatchTypeEnum> xmf_patch_type_map;

    extern const NumStringMap<midi::MidiStreamAtom> sysex_subid_map;
    extern const NumStringMap<midi::MidiStreamAtom> sysex_nonrt_id1_map;
    extern const NumStringMap<midi::MidiStreamAtom> sysex_rt_id1_map;

    std::ostream& operator<<(std::ostream& os, midi::MIDI_Format mf);

}

#endif

