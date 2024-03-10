//
// TextMIDITools Version 1.0.72
//
// textmidi 1.0.6
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(VOICE_H)
#    define  VOICE_H

#include <cstdint>
#include <cstdio>
#include <limits>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include "cgmlegacy.h"
#include "MIDIKeyString.h"
#include "RhythmRational.h"

namespace textmidi
{
    namespace cgm
    {
    // Crashes without pack pragma; VoiceXml was inexplicably a different size
    // in textmidicgm.cc than in Composer.cc (seen in gdb debugger).
    // Moving the bool to the end did not help.
    #pragma pack(4)
        class VoiceXml
        {
            friend class boost::serialization::access;
          public:
            struct Follower
            {
              enum class IntervalType : std::int32_t
              {
                  Neither,
                  Scalar,
                  Chromatic
              };

              Follower()
               : follow_{false},
                 leader_{std::numeric_limits<int>().max()},
                 interval_type_{IntervalType::Neither},
                 interval_{},
                 delay_{rational::RhythmRational{0L}},
                 inversion_{},
                 retrograde_{}
              {
              }
              template<class Archive>
                  void serialize(Archive& arc, const unsigned int version)
              {
                  arc & BOOST_SERIALIZATION_NVP(follow_);
                  arc & BOOST_SERIALIZATION_NVP(leader_);
                  arc & BOOST_SERIALIZATION_NVP(interval_type_);
                  arc & BOOST_SERIALIZATION_NVP(interval_);
                  if (version > 1)
                  {
                      arc & BOOST_SERIALIZATION_NVP(delay_);
                      arc & BOOST_SERIALIZATION_NVP(inversion_);
                      arc & BOOST_SERIALIZATION_NVP(retrograde_);
                  }
              }
              public:
                bool follow_;
                int leader_;
                IntervalType interval_type_;
                int interval_;
                rational::RhythmRational delay_;
                bool inversion_;
                bool retrograde_;
            };
            // There is no serialize for string_view, so these have to be strings.
            explicit VoiceXml(
                    std::string low_pitch = "",
                    std::string high_pitch = "",
                    std::uint32_t channel = 0U,
                    double walking = 0.0,
                    std::string program = std::string("1"),
                    std::int32_t pan = 0,
                    Follower follower = Follower{})
              : low_pitch_{low_pitch},
                high_pitch_{high_pitch},
                channel_{channel},
                walking_{walking},
                program_{program},
                pan_{pan},
                follower_{follower}
            {
            }

            std::shared_ptr<bool> prefer_sharp{new bool};
            VoiceXml(const cgmlegacy::VoiceOld& v)
              : low_pitch_{textmidi::num_to_note(v.low_pitch_, prefer_sharp)},
                high_pitch_{textmidi::num_to_note(v.high_pitch_, prefer_sharp)},
                channel_{v.channel_ + 1},
                walking_{v.walking_ ? 1.0 : 0.0},
                program_{"1"},
                pan_{},
                follower_()
            {}
            std::string low_pitch() const noexcept;
            void low_pitch(std::string_view low_pitch) noexcept;
            std::string high_pitch() const noexcept;
            void high_pitch(std::string_view high_pitch) noexcept;
            std::uint32_t channel() const noexcept;
            void channel(std::uint32_t channel) noexcept;
            double walking() const noexcept;
            void walking(double walking) noexcept;

            std::string program() const noexcept;
            void program(std::string program) noexcept;
            std::int32_t pan() const noexcept;
            void pan(std::int32_t pan) noexcept;
            const Follower& follower() const noexcept;
            void follower(const Follower& follower) noexcept;
          private:
            std::string low_pitch_;
            std::string high_pitch_;
            std::uint32_t channel_; // one-based in VoiceXml
            double walking_;
            std::string program_;
            std::int32_t pan_; // 0 = center, negative is left, positive is right.
            Follower follower_;
            template<class Archive>
                void serialize(Archive& arc, const unsigned int )
            {
                arc & BOOST_SERIALIZATION_NVP(low_pitch_);
                arc & BOOST_SERIALIZATION_NVP(high_pitch_);
                arc & BOOST_SERIALIZATION_NVP(channel_);
                arc & BOOST_SERIALIZATION_NVP(walking_);
                arc & BOOST_SERIALIZATION_NVP(program_);
                arc & BOOST_SERIALIZATION_NVP(pan_);
                arc & BOOST_SERIALIZATION_NVP(follower_);
            }
        };
#pragma pack()
    }
}
BOOST_CLASS_VERSION(textmidi::cgm::VoiceXml::Follower, 2)

#endif
