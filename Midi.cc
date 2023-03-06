//
// TextMIDITools Version 1.0.45
//
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <endian.h>

#include <algorithm>
#include <ranges>

#include "Midi.h"

using namespace std;
using namespace textmidi;

void textmidi::MidiHeader::to_bytes(MidiStreamAtom* bytes)
{
    MidiHeader* header_bytes{reinterpret_cast<MidiHeader*>(bytes)};
    *header_bytes = *this;
    header_bytes->swap();
}

void textmidi::MidiHeader::swap()
{
    chunk_len_ = htobe32(chunk_len_);
    format_    = static_cast<MIDI_Format>(htobe16(static_cast<uint16_t>
                (format_)));
    ntrks_     = htobe16(ntrks_);
    division_  = htobe16(division_);
}

ostream& textmidi::operator<<(ostream& os, textmidi::MIDI_Format mf)
{
    auto flags{os.flags()};
    if (format_map.contains(mf))
    {
        os << format_map.at(mf);
    }
    else [[unlikely]]
    {
        os << "UNKNOWN FORMAT";
    }
    static_cast<void>(os.flags(flags));
    return os;
}

ostream& textmidi::operator<<(ostream& os, const MidiHeader& mh)
{
    auto flags{os.flags()};
    os << mh.chunk_name_[0] << mh.chunk_name_[1]    << mh.chunk_name_[2]
       << mh.chunk_name_[3] << ' ' << mh.chunk_len_ << ' ' << mh.format_
       << ' ' << mh.ntrks_  << ' ' << mh.division_;
    static_cast<void>(os.flags(flags));
    return os;
}

const textmidi::NumStringMap<int> textmidi::smpte_fps_map
{
    {0, std::string_view("SMPTE_24FPS")},
    {1, std::string_view("SMPTE_25FPS")},
    {2, std::string_view("SMPTE_30FPSDROPFRAME")},
    {3, std::string_view("SMPTE_30FPSNONDROPFRAME")}
};

const textmidi::NumStringMap<int> textmidi::dynamics_map
{
    { 10, "pppp"},
    { 25, "ppp"},
    { 40, "pp"},
    { 50, "p"},
    { 62, "mp"},
    { 75, "mf"},
    { 90, "forte"},
    {110, "ff"},
    {120, "fff"},
    {127, "ffff"}
};

const textmidi::NumStringMap<textmidi::MIDI_Format> textmidi::format_map
{
        {"MONOTRACK", MIDI_Format::MonoTrack},
        {"MULTITRACK", MIDI_Format::MultiTrack},
        {"MULTISEQUENCE", MIDI_Format::MultiSequence}
};

// MIDI Pan is actually excess 64,
// but textmidi treats it as -64..0..63, where 0 is center.
// In addition we allow the words "left", "center", and "right".
const textmidi::NumStringMap<int> textmidi::pan_map
{
    {"LEFT",   MinSignedPan},
    {"CENTER", CenterSignedPan},
    {"RIGHT",  MaxSignedPan}
};

const textmidi::NumStringMap<textmidi::MidiStreamAtom> textmidi::text_meta_map 
{
    {"TEXT",            text_prefix[0]},
    {"COPYRIGHT",       copyright_prefix[0]},
    {"TRACK",           track_name_prefix[0]},
    {"INSTRUMENT",      instrument_name_prefix[0]},
    {"LYRIC",           lyric_prefix[0]},
    {"MARKER",          marker_prefix[0]},
    {"CUE_POINT",       cue_point_prefix[0]},
    {"PROGRAM_NAME",    program_name_prefix[0]},
    {"DEVICE_NAME",     device_name_prefix[0]},
    {"TEXT_0A",         text_0A_prefix[0]},
    {"TEXT_0B",         text_0B_prefix[0]},
    {"TEXT_0C",         text_0C_prefix[0]},
    {"TEXT_0D",         text_0D_prefix[0]},
    {"TEXT_0E",         text_0E_prefix[0]},
    {"TEXT_0F",         text_0F_prefix[0]},
    {"SEQUENCER_SPECIFIC", sequencer_specific_prefix[0]}
};

const textmidi:: NumStringMap<MidiStreamAtom> textmidi::control_function_map
{
    {"RESETALL",     control_reset_all_ctrl[0]},
    {"ALLNOTES_OFF", control_all_notes_off[0]},
    {"ALL_SOUND_OFF", control_all_sound_off[0]},
    {"BREATH",       control_breath[0]},
    {"SOFTPEDAL",    control_softpedal[0]},
    {"PORTAMENTO",   control_portamento[0]},
    {"BANK_SELECT", control_bank_select[0]},
    {"MODULATION", control_modulation[0]},
    {"FOOT_CONTROLLER", control_foot_controller[0]},
    {"PORTAMENTO_TIME", control_portamento_time[0]},
    {"DATA_ENTRY_MSB", control_data_entry_msb[0]},
    {"CHANNEL_VOLUME", control_channel_volume[0]},
    {"BALANCE", control_balance[0]},
    {"PAN", control_pan[0]},
    {"EXPRESSION", control_expression[0]},
    {"EFFECT_1", control_effect_1[0]},
    {"EFFECT_2", control_effect_2[0]},
    {"GENERAL_PURPOSE_1", control_general_purpose_1[0]},
    {"GENERAL_PURPOSE_2", control_general_purpose_2[0]},
    {"GENERAL_PURPOSE_3", control_general_purpose_3[0]},
    {"GENERAL_PURPOSE_4", control_general_purpose_4[0]},
    {"DAMPER", control_damper[0]},
    {"PORTAMENTO_ON_OFF", control_portamento_on_off[0]},
    {"SOSTENUTO", control_sostenuto[0]},
    {"LEGATO_FOOT", control_legato_foot[0]},
    {"HOLD_2", control_hold_2[0]},
    {"SOUND_VARIATION", control_sound_variation[0]},
    {"TIMBRE_INTENSITY", control_timbre_intensity[0]},
    {"RELEASE_TIME", control_release_time[0]},
    {"ATTACK_TIME", control_attack_time[0]},
    {"BRIGHTNESS", control_brightness[0]},
    {"DECAY_TIME", control_decay_time[0]},
    {"VIBRATO_RATE", control_vibrato_rate[0]},
    {"VIBRATO_DEPTH", control_vibrato_depth[0]},
    {"SOUND_CONTROLLER_9", control_sound_controller_9[0]},
    {"SOUND_CONTROLLER_10", control_sound_controller_10[0]},
    {"GENERAL_PURPOSE_5", control_general_purpose_5[0]},
    {"GENERAL_PURPOSE_6", control_general_purpose_6[0]},
    {"GENERAL_PURPOSE_7", control_general_purpose_7[0]},
    {"GENERAL_PURPOSE_8", control_general_purpose_8[0]},
    {"HIRES_VELOCITY_MSB", control_hires_velocity_msb[0]},
    {"REVERB_SEND_LEVEL", control_reverb_send_level[0]},
    {"TREMOLO_DEPTH", control_tremolo_depth[0]},
    {"CHORUS_SEND_LEVEL", control_chorus_send_level[0]},
    {"CELESTE_DEPTH", control_celeste_depth[0]},
    {"PHASER_DEPTH", control_phaser_depth[0]},
    {"DATA_INCREMENT", control_data_increment[0]},
    {"DATA_DECREMENT", control_data_decrement[0]},
    {"NON_REGISTERED_PARAMETER_LSB", control_non_registered_parameter_lsb[0]},
    {"NON_REGISTERED_PARAMETER_MSB", control_non_registered_parameter_msb[0]},
    {"REGISTERED_PARAMETER_LSB", control_registered_parameter_lsb[0]},
    {"REGISTERED_PARAMETER_MSB", control_registered_parameter_msb[0]}
};

const NumStringMap<XmfPatchTypeEnum> textmidi::xmf_patch_type_map
{
    {"GM1", XmfPatchTypeEnum::GM1}, 
    {"GM2", XmfPatchTypeEnum::GM2}, 
    {"DLS", XmfPatchTypeEnum::DLS}
};

