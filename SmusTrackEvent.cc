//
// TextMIDITools Version 1.0.92
//
// smustextmidi 1.0.6
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// SmusTrackEventFilePod implements Simple Music Score events.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */
#include <cstdint>

#include <ranges>
#include <memory>

#include <boost/lexical_cast.hpp>

#include "SmusTrackEvent.h"
#include "rational_support.h"
#include "MIDIKeyString.h"
#include "MidiMaps.h"

using std::int32_t, std::string, std::ostringstream, std::vector,
    std::pair, std::unique_ptr, std::make_pair, std::make_unique;
using std::ranges::find_if;
using boost::lexical_cast;

using smus::SmusTrackEventFilePod, smus::SmusTrackEventFactory,
      smus::SmusTrackEventBase, smus::SmusTrackEventPitch,
      smus::SmusTrackEventEnd;

using textmidi::rational::RhythmRational;
using textmidi::rational::print_rhythm;
using textmidi::num_to_note;

auto smus::SmusTrackEventBase::decision() const noexcept
{
    return decision_;
}

auto smus::SmusTrackEventBase::data() const noexcept
{
    return data_;
}

RhythmRational smus::SmusTrackEventBase::delay_accum() const noexcept
{
    return delay_accum_;
}

void smus::SmusTrackEventBase::delay_accum(const RhythmRational& delay) noexcept
{
    delay_accum_ = delay;
}

int32_t smus::SmusTrackEventBase::current_dynamic() noexcept
{
    return current_dynamic_;
}

void smus::SmusTrackEventBase::current_dynamic(int32_t current_dynamic) noexcept
{
    current_dynamic_ = current_dynamic;
}

bool smus::SmusTrackEventBase::i_am_lazy() noexcept
{
    return i_am_lazy_;
}

void smus::SmusTrackEventBase::i_am_lazy(bool i_am_lazy) noexcept
{
    i_am_lazy_ = i_am_lazy;
}

string smus::SmusTrackEventBase::i_am_lazy_string(bool i_am_lazy) noexcept
{
    string str{};
    if (i_am_lazy)
    {
        if (!i_am_lazy_)
        {
            i_am_lazy_ = i_am_lazy;
            return string{"LAZY "};
        }
    }
    else
    {
        if (i_am_lazy_)
        {
            i_am_lazy_ = i_am_lazy;
            return string{"END_LAZY "};
        }
    }
    return str;
}

int32_t smus::SmusTrackEventBase::channel() noexcept
{
    return channel_;
}

void smus::SmusTrackEventBase::channel(int32_t channel) noexcept
{
    channel_ = channel;
}

string smus::SmusTrackEventBase::pre_rest() noexcept
{
    ostringstream oss{};
    if (delay_accum() > RhythmRational{0L})
    {
        oss << i_am_lazy_string(true);
        oss << "R ";
        (*print_rhythm)(oss, delay_accum());
        delay_accum(RhythmRational{});
        oss << '\n';
    }
    return oss.str();
}

RhythmRational smus::SmusTrackEventBase::duration() const noexcept
{
    // 7    6    5    4    3    2    1    0
    // chrd tie  tuplet... dot  division
    //      out  1=trip,2=      0-whole
    //           quintet        1=half,2-qtr,3=8th,4=16th,5=32nd,6=64th,7=128th
    //           3=septuplet
    const int32_t value{data() & 0x7};
    auto denominator = 0L;
    if (0 == value)
    {
        denominator = 1L; // whole note
    }
    else
    {
        denominator = 1L << value; // other than whole notes
    }
    RhythmRational len{1L, denominator};
    len *= dotted_multiplier();
    len *= tuplet_multiplier();
    return len;
}

void smus::SmusTrackEventBase::flush() noexcept
{
    delay_accum_ = RhythmRational{0L};
}

bool smus::SmusTrackEventBase::is_dotted() const noexcept
{
    return ((data() & (1 << 3)) != 0);
}

RhythmRational smus::SmusTrackEventBase::dotted_multiplier() const noexcept
{
    return (is_dotted() ? RhythmRational(3L, 2L) : RhythmRational(1L));
}

RhythmRational smus::SmusTrackEventBase::tuplet_multiplier() const noexcept
{
    switch ((data() >> 4) & 3)
    {
      case 1:                        // triplet
        return RhythmRational{2L, 3L};
        break;
      case 2:                        // quintuplet
        return RhythmRational{4L, 5L};
        break;
      case 3:                        // septuplet
        return RhythmRational{6L, 7L};
        break;
      default:
        return RhythmRational(1L);
        break;
    }
}

RhythmRational smus::SmusTrackEventBase::delay_accum_{};

string smus::SmusTrackEventPitch::textmidi_tempo() noexcept
{
    if (!is_chorded())
    {
        delay_accum(delay_accum() + duration());
    }
    return string{};
}

string smus::SmusTrackEventPitch::textmidi() noexcept
{
    ostringstream oss{};
    std::shared_ptr<bool> prefer_sharp{std::make_shared<bool>()};
    if (decision() < Rest)
    {
        oss << i_am_lazy_string(true);
        oss << pre_rest();
        if (SmusTrackEventBase::channel() == 0)
        {
            smus::SmusTrackEventBase::channel(1);
            oss << "chan " << smus::SmusTrackEventBase::channel() << ' ';
        }

        const auto tied_back{is_tied_back(decision())};
        if (tied_back)
        {
            oss << '-';
            if (!is_tiedout())
            {
                remove_from_tied();
            }
        }
        oss << num_to_note(decision(), prefer_sharp);
        if (is_tiedout())
        {
            if (!tied_back)
            {
                add_to_tied();
            }
            oss << '-';
        }
        oss << ' ';
        if (!is_chorded())
        {
            delay_accum(delay_accum() + duration());
            (*print_rhythm)(oss, delay_accum()) << '\n';
            delay_accum(RhythmRational{0L});
        }
    }
    else
    {
        oss << "; Unknown SMUS decision code\n";
    }
    return oss.str();
}

bool smus::SmusTrackEventPitch::is_tiedout() const noexcept
{
    return ((data() & (1 << 6)) != 0);
}

bool smus::SmusTrackEventPitch::is_chorded() const noexcept
{
    return ((data() & (1 << 7)) != 0);
}

bool smus::SmusTrackEventPitch::is_tied_back(int32_t tp) const noexcept
{
    bool rtn{};
    const auto tplocal{tp};
    auto it{find_if(tied_vec_, [tplocal](char p)
        { return tplocal == p; })};
    if (it != tied_vec_.end()) // this is tied back
    {
        rtn = true;
    }
    return rtn;
}

void smus::SmusTrackEventPitch::remove_from_tied() noexcept
{
    const auto tplocal{decision()};
    auto it{find_if(tied_vec_, [tplocal](char p)
        { return tplocal == p; })};
    if (it != tied_vec_.end()) // this is tied back
    {
        tied_vec_.erase(it);
    }
}

void smus::SmusTrackEventPitch::add_to_tied() noexcept
{
    tied_vec_.insert(tied_vec_.end(), decision());
}

void smus::SmusTrackEventPitch::flush() noexcept
{
    smus::SmusTrackEventBase::flush();
    tied_vec_.clear();
}

vector<int32_t> smus::SmusTrackEventPitch::tied_vec_;
int32_t smus::SmusTrackEventBase::channel_{};

string smus::SmusTrackEventRest::textmidi() noexcept
{
    return textmidi_tempo();
}

string smus::SmusTrackEventRest::textmidi_tempo() noexcept
{
    delay_accum(delay_accum() + duration());
    return string{};
}

string smus::SmusTrackEventInstrument::textmidi_tempo() noexcept
{
    return string{};
}

string smus::SmusTrackEventInstrument::textmidi() noexcept
{
    return string{"; SMUS Instrument Number set to "}
        + lexical_cast<string>(data()) + '\n';
}

pair<unsigned, unsigned>
    smus::SmusTrackEventTimeSignature::time_signature() const
    noexcept
{
    return make_pair(((data() >> 3) & 0x1F) + 1,
        static_cast<uint32_t>(pow(2.0, static_cast<double>
                    (data() & 7))));
}

string smus::SmusTrackEventTimeSignature::textmidi_tempo() noexcept
{
    string str{pre_rest()};
    const auto time_sig{time_signature()};
    str += i_am_lazy_string(false);
    ((((((str += "TIME_SIGNATURE ")
        += lexical_cast<string>(time_sig.first))  += ' ')
        += lexical_cast<string>(time_sig.second)) += ' ')
        += "24\n");
    return str;
}

string smus::SmusTrackEventTimeSignature::textmidi() noexcept
{
    return string{};
}

string smus::SmusTrackEventKeySignature::textmidi() noexcept
{
    return string{};
}

string smus::SmusTrackEventKeySignature::textmidi_tempo() noexcept
{
    auto str{pre_rest()};
    str += i_am_lazy_string(false);
    (((str += "KEY_SIGNATURE ") += key()) += '\n');
    return str;
}

string smus::SmusTrackEventKeySignature::key() const noexcept
{
    string keystr{};
    switch (data())
    {
      case 0:
        keystr = "C";
        break;
      case 1:
        keystr = "G";
        break;
      case 2:
        keystr = "D";
        break;
      case 3:
        keystr = "A";
        break;
      case 4:
        keystr = "E";
        break;
      case 5:
        keystr = "B";
        break;
      case 6:
        keystr = "F#";
        break;
      case 7:
        keystr = "C#";
        break;
      case 8:
        keystr = "F";
        break;
      case 9:
        keystr = "Bb";
        break;
      case 10:
        keystr = "Eb";
        break;
      case 11:
        keystr = "Ab";
        break;
      case 12:
        keystr = "Db";
        break;
      case 13:
        keystr = "Gb";
        break;
      case 14:
        keystr = "Cb";
        break;
      default:
        break;
    }
    return keystr;
}

string smus::SmusTrackEventVolume::textmidi_tempo() noexcept
{
    return string{};
}

string smus::SmusTrackEventVolume::textmidi() noexcept
{
    string str{};
    str += i_am_lazy_string(true);
    str += ' ';
    str += pre_rest();
    if (current_dynamic() != data())
    {
        if (midi::dynamics_map.contains(data()))
        {
            const auto dynamic{midi::dynamics_map.at(data())};
            (str += dynamic) += '\n';
        }
        else
        {
            ((str += "vel ") += lexical_cast<string>(data())) += ' ';
        }
        current_dynamic(data());
    }
    return str;
}

string smus::SmusTrackEventChannel::textmidi_tempo() noexcept
{
    return string{};
}

string smus::SmusTrackEventChannel::textmidi() noexcept
{
    string str{};
    str += i_am_lazy_string(true);
    str += ' ';
    str += pre_rest();
    channel(data());
    (((str += " chan ") += lexical_cast<string>(channel())) += '\n');
    return str;
}

string smus::SmusTrackEventPreset::textmidi_tempo() noexcept
{
    return string{};
}

string smus::SmusTrackEventPreset::textmidi() noexcept
{
    string str{pre_rest()};
    str += i_am_lazy_string(false);
    (((((str += "PROGRAM ") += lexical_cast<string>(channel()))
             += ' ') += lexical_cast<string>(data())) += '\n');
    return str;
}

// MIDI has no clef.
string smus::SmusTrackEventClef::textmidi_tempo() noexcept
{
    return string{};
}

string smus::SmusTrackEventClef::textmidi() noexcept
{
    string str{"; SMUS Clef set to "};
    if (clef_map.contains(data()))
    {
        (str += clef_map[data()]) += '\n';
    }
    else
    {
        (str += lexical_cast<string>(data())) += '\n';
    }
    return str;
}

const midi::NumStringMap<int32_t> smus::clef_map
{
    {0, string{"Treble"}},
    {1, string{"Bass"}},
    {2, string{"Alto"}},
    {3, string{"Tenor"}}
};

string smus::SmusTrackEventTempo::textmidi() noexcept
{
    return string{};
}

string smus::SmusTrackEventTempo::textmidi_tempo() noexcept
{
    auto str{pre_rest()};
    str += i_am_lazy_string(false);
    (((str += "TEMPO ")
        += lexical_cast<string>(data())) += '\n');
    return str;
}

string smus::SmusTrackEventEnd::textmidi_tempo() noexcept
{
    auto str{pre_rest()};
    str += i_am_lazy_string(false);
    str += '\n';
    str += "END_OF_TRACK\n";
    return str;
}

string smus::SmusTrackEventEnd::textmidi() noexcept
{
    return string("\n") + textmidi_tempo();
}

std::unique_ptr<SmusTrackEventBase> smus::SmusTrackEventFactory::
    operator()(const smus::SmusTrackEventFilePod& te) noexcept
{
    unique_ptr<SmusTrackEventBase> teb{};
    switch (te.decision)
    {
      case Rest:
        teb = std::make_unique<smus::SmusTrackEventRest>(te);
        break;
      case InstrumentNumber:
        teb = make_unique<smus::SmusTrackEventInstrument>(te);
        break;
      case TimeSignature:
        teb = make_unique<smus::SmusTrackEventTimeSignature>(te);
        break;
      case KeySignature: // trackEventIt->data is key
        teb = make_unique<smus::SmusTrackEventKeySignature>(te);
        break;
      case Volume: // Set Volume
        teb = make_unique<smus::SmusTrackEventVolume>(te);
        break;
      case Channel:
        teb = make_unique<smus::SmusTrackEventChannel>(te);
        break;
      case Preset:
        teb = make_unique<smus::SmusTrackEventPreset>(te);
        break;
      [[unlikely]] case Clef:
        teb = make_unique<smus::SmusTrackEventClef>(te);
        break;
      case Tempo:
        teb = make_unique<smus::SmusTrackEventTempo>(te);
        break;
      case EndOfTrack:
        teb = make_unique<smus::SmusTrackEventEnd>(te);
        break;
      default: // these are pitches
        teb = make_unique<smus::SmusTrackEventPitch>(te);
        break;
    }
    return teb;
}

int32_t smus::SmusTrackEventBase::current_dynamic_{64};
bool smus::SmusTrackEventBase::i_am_lazy_{false};
