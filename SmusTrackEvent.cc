//
// TextMIDITools Version 1.0.26
//
// smustextmidi 1.0.6
// Copyright © 2022 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
// SmusTrackEventFilePod implements Simple Music Score events.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <boost/lexical_cast.hpp>

#include "SmusTrackEvent.h"
#include "Midi.h"
#include "rational_support.h"
#include "MIDIKeyString.h"

using namespace std;
using namespace boost;

using namespace smus;
using namespace textmidi;
using namespace textmidi::rational;

auto SmusTrackEventBase::decision() const
{
    return decision_;
}

auto SmusTrackEventBase::data() const
{
    return data_;
}

void SmusTrackEventBase::add_to_delay_accum(const RhythmRational& delay)
{
    delay_accum_ += delay;
}

RhythmRational SmusTrackEventBase::delay_accum() const
{
    return delay_accum_;
}

void SmusTrackEventBase::delay_accum(const RhythmRational& delay)
{
    delay_accum_ = delay;
}

int SmusTrackEventBase::current_dynamic()
{
    return current_dynamic_;
}

void SmusTrackEventBase::current_dynamic(int current_dynamic)
{
    current_dynamic_ = current_dynamic;
}

bool SmusTrackEventBase::i_am_lazy()
{
    return i_am_lazy_;
}

void SmusTrackEventBase::i_am_lazy(bool i_am_lazy)
{
    i_am_lazy_ = i_am_lazy;
}

string SmusTrackEventBase::i_am_lazy_string(bool i_am_lazy)
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

int SmusTrackEventBase::channel()
{
    return channel_;
}

void SmusTrackEventBase::channel(int channel)
{
    channel_ = channel;
}

string SmusTrackEventBase::pre_rest()
{
    ostringstream oss{};
    if (delay_accum() > rational::RhythmRational{0L})
    {
        oss << i_am_lazy_string(true);
        oss << "R ";
        print_rhythm(oss, delay_accum());
        delay_accum(RhythmRational{0});
        oss << '\n';
    }
    return oss.str();
}

RhythmRational SmusTrackEventBase::duration() const
{
    // 7    6    5    4    3    2    1    0
    // chrd tie  tuplet... dot  division
    //      out  1=trip,2=      0-whole
    //           quintet        1=half,2-qtr,3=8th,4=16th,5=32nd,6=64th,7=128th
    //           3=septuplet
    const int value{data() & 0x7};
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

void SmusTrackEventBase::flush()
{
    delay_accum_ = RhythmRational{0L};
}

bool SmusTrackEventBase::is_dotted() const
{
    return ((data() & (1 << 3)) != 0);
}

RhythmRational SmusTrackEventBase::dotted_multiplier() const
{
    return (is_dotted() ? RhythmRational(3L, 2L) : RhythmRational(1L));
}

RhythmRational SmusTrackEventBase::tuplet_multiplier() const
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

RhythmRational SmusTrackEventBase::delay_accum_{};

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
    if (decision() < Rest)
    {
        oss << i_am_lazy_string(true);
        oss << pre_rest();
        if (SmusTrackEventBase::channel() == 0)
        {
            SmusTrackEventBase::channel(1);
            oss << "chan " << SmusTrackEventBase::channel() << ' ';
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
        oss << num_to_note(decision());
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
            print_rhythm(oss, delay_accum()) << '\n';
            delay_accum(RhythmRational{0L});
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

bool SmusTrackEventPitch::is_tied_back(int tp) const
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

std::vector<int> SmusTrackEventPitch::tied_vec_;
int SmusTrackEventBase::channel_{0};

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
    return string{"; SMUS Instrument Number set to "}
        + lexical_cast<string>(data()) + '\n';
}

pair<unsigned, unsigned> SmusTrackEventTimeSignature::time_signature() const
{
    return make_pair(((data() >> 3) & 0x1F) + 1,
        static_cast<unsigned>(pow(2.0, static_cast<double>
                    (data() & 7))));
}

string SmusTrackEventTimeSignature::textmidi_tempo()
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
    str += i_am_lazy_string(false);
    (((str += "KEY_SIGNATURE ") += key()) += '\n');
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
    string str{};
    str += i_am_lazy_string(true);
    str += ' ';
    str += pre_rest();
    if (current_dynamic() != data())
    {
       if (dynamics_reverse_map.contains(data()))
       {
           (str += dynamics_reverse_map.at(data())) += '\n';
       }
       else
       {
           ((str += "vel ") += lexical_cast<string>(data())) += ' ';
       }
       current_dynamic(data());
    }
    return str;
}

string SmusTrackEventChannel::textmidi_tempo()
{
    return string{};
}

string SmusTrackEventChannel::textmidi()
{
    string str{};
    str += i_am_lazy_string(true);
    str += ' ';
    str += pre_rest();
    channel(data());
    (((str += " chan ") += lexical_cast<string>(channel())) += '\n');
    return str;
}

string SmusTrackEventPreset::textmidi_tempo()
{
    return string{};
}

string SmusTrackEventPreset::textmidi()
{
    string str{pre_rest()};
    str += i_am_lazy_string(false);
    (((((str += "PROGRAM ") += lexical_cast<string>(channel()))
             += ' ') += lexical_cast<string>(data())) += '\n');
    return str;
}

// MIDI has no clef.
string SmusTrackEventClef::textmidi_tempo()
{
    return string{};
}

string SmusTrackEventClef::textmidi()
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

map<int, string> SmusTrackEventClef
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
    str += i_am_lazy_string(false);
    (((str += "TEMPO ")
        += lexical_cast<string>(data())) += '\n');
    return str;
}

string SmusTrackEventEnd::textmidi_tempo()
{
    auto str{pre_rest()};
    str += i_am_lazy_string(false);
    str += '\n';
    str += "END_OF_TRACK\n";
    return str;
}

string SmusTrackEventEnd::textmidi()
{
    return string("\n") + textmidi_tempo();
}

std::unique_ptr<SmusTrackEventBase> SmusTrackEventFactory::operator()(const SmusTrackEventFilePod& te)
{
    unique_ptr<SmusTrackEventBase> teb{};
    switch (te.decision)
    {
      case Rest:
        teb.reset(new SmusTrackEventRest{te});
        break;
      case InstrumentNumber:
        teb.reset(new SmusTrackEventInstrument{te});
        break;
      case TimeSignature:
        teb.reset(new SmusTrackEventTimeSignature{te});
        break;
      case KeySignature: // trackEventIt->data is key
        teb.reset(new SmusTrackEventKeySignature{te});
        break;
      case Volume: // Set Volume
        teb.reset(new SmusTrackEventVolume{te});
        break;
      case Channel:
        teb.reset(new SmusTrackEventChannel{te});
        break;
      case Preset:
        teb.reset(new SmusTrackEventPreset{te});
        break;
      [[unlikely]] case Clef:
        teb.reset(new SmusTrackEventClef{te});
        break;
      case Tempo:
        teb.reset(new SmusTrackEventTempo{te});
        break;
      case EndOfTrack:
        teb.reset(new SmusTrackEventEnd{te});
        break;
      default: // these are pitches
        teb.reset(new SmusTrackEventPitch{te});
        break;
    }
    return teb;
}

int  smus::SmusTrackEventBase::current_dynamic_{64};
bool smus::SmusTrackEventBase::i_am_lazy_{false};

