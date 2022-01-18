//
// TextMIDITools Version 1.0.10
//
// smustextmidi 1.0.6
// Copyright © 2021 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// SmusTrackEvent implements Simple Music Score events.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <boost/lexical_cast.hpp>

#include "SmusTrackEvent.h"
#include "MidiMessages.h"
#include "rational_support.h"
#include "MIDIKeyString.h"

using namespace std;
using namespace boost;

using namespace smus;
using namespace textmidi;

auto SmusTrackEventBase::decision() const
{
    return decision_;
}

auto SmusTrackEventBase::data() const
{
    return data_;
}

void SmusTrackEventBase::add_to_delay_accum(const Ratio64& delay)
{
    delay_accum_ += delay;
}

Ratio64 SmusTrackEventBase::delay_accum() const
{
    return delay_accum_;
}

void SmusTrackEventBase::delay_accum(const Ratio64& delay)
{
    delay_accum_ = delay;
}

uint8_t SmusTrackEventBase::current_dynamic() const
{
    return current_dynamic_;
}

void SmusTrackEventBase::current_dynamic(uint8_t current_dynamic)
{
    current_dynamic_ = current_dynamic;
}

uint8_t SmusTrackEventBase::channel() const
{
    return channel_;
}

void SmusTrackEventBase::channel(uint8_t channel)
{
    channel_ = channel;
}

string SmusTrackEventBase::pre_rest()
{
    ostringstream oss{};
    if (delay_accum() > 0)
    {
        oss << "R ";
        print_lazy_value(oss, delay_accum());
        delay_accum(Ratio64{0});
        oss << '\n';
    }
    return oss.str();
}

Ratio64 SmusTrackEventBase::duration() const
{
    // 7    6    5    4    3    2    1    0
    // chrd tie  tuplet... dot  division
    //      out  1=trip,2=      0-whole
    //           quintet        1=half,2-qtr,3=8th,4=16th,5=32nd,6=64th,7=128th
    Ratio64 len{1, (1 << (data() & 0x7))};
    len *= dotted_multiplier();
    len *= tuplet_multiplier();
    return len;
}

void SmusTrackEventBase::flush()
{
    delay_accum_ = Ratio64{0};
}

bool SmusTrackEventBase::is_dotted() const
{
    return ((data() & (1 << 3)) != 0);
}

Ratio64 SmusTrackEventBase::dotted_multiplier() const
{
    return (is_dotted() ? Ratio64(3, 2) : Ratio64(1));
}

Ratio64 SmusTrackEventBase::tuplet_multiplier() const
{
    switch ((data() >> 4) & 3)
    {
      case 1:                        // triplet
        return Ratio64{2, 3};
        break;
      case 2:                        // quintuplet
        return Ratio64{4, 5};
        break;
      case 3:                        // septuplet
        return Ratio64{6, 7};
        break;
      default:
        return Ratio64(1);
        break;
    }
}

textmidi::Ratio64 SmusTrackEventBase::delay_accum_{};

string SmusTrackEventPitch::textmidi_tempo()
{
    if (!is_chorded())
    {
        delay_accum(delay_accum() + duration());
    }
    return string{};
}

string SmusTrackEventPitch::textmidi()
{
    ostringstream oss{};
    if (decision() < 128)
    {
        oss << pre_rest();
        if (is_tied_back(decision()))
        {
            oss << '-';
            if (!is_tiedout())
            {
                remove_from_tied();
            }
        }
        oss << num_to_note(decision());
        if (is_tiedout())
        {
            add_to_tied();
            oss << '-';
        }
        oss << ' ';
        if (!is_chorded())
        {
            delay_accum(delay_accum() + duration());
            print_lazy_value(oss, delay_accum()) << '\n';
            delay_accum(Ratio64{0});
        }
    }
    else
    {
        oss << "; Unknown SMUS decision code\n";
    }
    return oss.str();
}

bool SmusTrackEventPitch::is_tiedout() const
{
    return ((data() & (1 << 6)) != 0);
}

bool SmusTrackEventPitch::is_chorded() const
{
    return ((data() & (1 << 7)) != 0);
}

bool SmusTrackEventPitch::is_tied_back(uint8_t tp) const
{
    bool rtn{};
    const auto tplocal{tp};
    auto it{find_if(tied_vec_.begin(), tied_vec_.end(), [tplocal](char p)
        { return tplocal == p; })};
    if (it != tied_vec_.end()) // this is tied back
    {
        rtn = true;
    }
    return rtn;
}

void SmusTrackEventPitch::remove_from_tied()
{
    const auto tplocal{decision()};
    auto it{find_if(tied_vec_.begin(), tied_vec_.end(), [tplocal](char p)
        { return tplocal == p; })};
    if (it != tied_vec_.end()) // this is tied back
    {
        tied_vec_.erase(it);
    }
}

void SmusTrackEventPitch::add_to_tied()
{
    tied_vec_.insert(tied_vec_.end(), decision());
}

void SmusTrackEventPitch::flush()
{
    SmusTrackEventBase::flush();
    tied_vec_.clear();
}

std::vector<std::uint8_t> SmusTrackEventPitch::tied_vec_;

string SmusTrackEventRest::textmidi()
{
    return textmidi_tempo();
}

string SmusTrackEventRest::textmidi_tempo()
{
    delay_accum(delay_accum() + duration());
    return string{};
}

string SmusTrackEventInstrument::textmidi_tempo()
{
    return string{};
}

string SmusTrackEventInstrument::textmidi()
{
    return string{"; SMUS Instrument Number set to "} + lexical_cast<string>(data()) + '\n';
}

pair<unsigned, unsigned> SmusTrackEventTimeSignature::time_signature() const
{
    return make_pair(((data() >> 3) & 0x1F) + 1,
        static_cast<unsigned>(pow(2.0, static_cast<double>
                    (data() & 7))));
}

string SmusTrackEventTimeSignature::textmidi_tempo()
{
    string rtn{pre_rest()};
    const auto time_sig{time_signature()};
    ((((((rtn += "END_LAZY TIME_SIGNATURE ")
        += lexical_cast<string>(time_sig.first))  += ' ')
        += lexical_cast<string>(time_sig.second)) += ' ')
        += "24 LAZY\n");
    return rtn;
}

string SmusTrackEventTimeSignature::textmidi()
{
    return string{};
}

string SmusTrackEventKeySignature::textmidi()
{
    return string{};
}

string SmusTrackEventKeySignature::textmidi_tempo()
{
    auto str{pre_rest()};
    (((str += "END_LAZY KEY_SIGNATURE ") += key())
        += " LAZY\n");
    return str;
}

string SmusTrackEventKeySignature::key() const
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

string SmusTrackEventVolume::textmidi_tempo()
{
    return string{};
}

string SmusTrackEventVolume::textmidi()
{
    string str{pre_rest()};
    if (current_dynamic() != data())
    {
        current_dynamic(data());
        (((str += "vel ") += lexical_cast<string>(static_cast<int>(data()))) += '\n');
    }
    return str;
}

string SmusTrackEventChannel::textmidi_tempo()
{
    return string{};
}

string SmusTrackEventChannel::textmidi()
{
    string str{pre_rest()};
    channel(data());
    (((str += " chan ") += lexical_cast<string>(static_cast<int>(channel()))) += '\n');
    return str;
}

string SmusTrackEventPreset::textmidi_tempo()
{
    return string{};
}

string SmusTrackEventPreset::textmidi()
{
    string str{pre_rest()};
    (((((str += "END_LAZY PROGRAM ") += lexical_cast<string>(static_cast<int>(channel())))
             += ' ') += lexical_cast<string>(static_cast<int>(data()))) += " LAZY\n");
    return str;
}

string SmusTrackEventClef::textmidi_tempo()
{
    return string{};
}

string SmusTrackEventClef::textmidi()
{
    string str{"; SMUS Clef set to "};
    (str += clef_map[data()]) += '\n';
    return str;
}

map<uint8_t, string> SmusTrackEventClef
    ::clef_map{{0, string{"Treble"}},
               {1, string{"Bass"}},
               {2, string{"Alto"}},
               {3, string{"Tenor"}}};

string SmusTrackEventTempo::textmidi()
{
    return string{};
}

string SmusTrackEventTempo::textmidi_tempo()
{
    auto str{pre_rest()};
    (((str += "END_LAZY TEMPO ")
        += lexical_cast<string>(static_cast<int>(data())))
        += " LAZY\n");
    return str;
}

string SmusTrackEventEnd::textmidi_tempo()
{
    return pre_rest();
}

string SmusTrackEventEnd::textmidi()
{
    return textmidi_tempo();
}

std::unique_ptr<SmusTrackEventBase> SmusTrackEventFactory::operator()(const SmusTrackEvent& te)
{
    unique_ptr<SmusTrackEventBase> teb{};
    switch (te.decision)
    {
      case 128: // Rest
        teb.reset(new SmusTrackEventRest{reinterpret_cast<const uint8_t*>(&te)});
        break;
      case 129: // Instrument Number
        teb.reset(new SmusTrackEventInstrument{reinterpret_cast<const uint8_t*>(&te)});
        break;
      case 130: // Time Signature
        teb.reset(new SmusTrackEventTimeSignature{reinterpret_cast<const uint8_t*>(&te)});
        break;
      case 131: // trackEventIt->data is key
        teb.reset(new SmusTrackEventKeySignature{reinterpret_cast<const uint8_t*>(&te)});
        break;
      case 132: // Set Volume
        teb.reset(new SmusTrackEventVolume{reinterpret_cast<const uint8_t*>(&te)});
        break;
      case 133: // Set MIDI Channel
        teb.reset(new SmusTrackEventChannel{reinterpret_cast<const uint8_t*>(&te)});
        break;
      case 134: // MIDI preset
        teb.reset(new SmusTrackEventPreset{reinterpret_cast<const uint8_t*>(&te)});
        break;
      [[unlikely]] case 135: // clef
        teb.reset(new SmusTrackEventClef{reinterpret_cast<const uint8_t*>(&te)});
        break;
      case 136: // Tempo
        teb.reset(new SmusTrackEventTempo{reinterpret_cast<const uint8_t*>(&te)});
        break;
      case 255: // End of Track
        teb.reset(new SmusTrackEventEnd{reinterpret_cast<const uint8_t*>(&te)});
        break;
      default: // these are pitches
        teb.reset(new SmusTrackEventPitch{reinterpret_cast<const uint8_t*>(&te)});
        break;
    }
    return teb;
}

