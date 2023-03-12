//
// TextMIDITools Version 1.0.49
//
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//

#include <ostream>

#include "MidiMaps.h"

using namespace std;
using namespace midi;

const midi::NumStringMap<int> midi::smpte_fps_map
{
    {0, "SMPTE_24FPS"},
    {1, "SMPTE_25FPS"},
    {2, "SMPTE_30FPSDROPFRAME"},
    {3, "SMPTE_30FPSNONDROPFRAME"}
};

const midi::NumStringMap<int> midi::dynamics_map
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

const midi::NumStringMap<midi::MIDI_Format> midi::format_map
{
    {"MONOTRACK", midi::MIDI_Format::MonoTrack},
    {"MULTITRACK", midi::MIDI_Format::MultiTrack},
    {"MULTISEQUENCE", midi::MIDI_Format::MultiSequence}
};

// MIDI Pan is actually excess 64,
// but textmidi treats it as -64..0..63, where 0 is center.
// In addition we allow the words "left", "center", and "right".
const midi::NumStringMap<int> midi::pan_map
{
    {"LEFT",   MinSignedPan},
    {"CENTER", CenterSignedPan},
    {"RIGHT",  MaxSignedPan}
};

const midi::NumStringMap<midi::MidiStreamAtom> midi::text_meta_map
{
    {"TEXT",            midi::text_prefix[0]},
    {"COPYRIGHT",       midi::copyright_prefix[0]},
    {"TRACK",           midi::track_name_prefix[0]},
    {"INSTRUMENT",      midi::instrument_name_prefix[0]},
    {"LYRIC",           midi::lyric_prefix[0]},
    {"MARKER",          midi::marker_prefix[0]},
    {"CUE_POINT",       midi::cue_point_prefix[0]},
    {"PROGRAM_NAME",    midi::program_name_prefix[0]},
    {"DEVICE_NAME",     midi::device_name_prefix[0]},
    {"TEXT_0A",         midi::text_0A_prefix[0]},
    {"TEXT_0B",         midi::text_0B_prefix[0]},
    {"TEXT_0C",         midi::text_0C_prefix[0]},
    {"TEXT_0D",         midi::text_0D_prefix[0]},
    {"TEXT_0E",         midi::text_0E_prefix[0]},
    {"TEXT_0F",         midi::text_0F_prefix[0]},
    {"SEQUENCER_SPECIFIC", midi::sequencer_specific_prefix[0]}
};

const midi:: NumStringMap<midi::MidiStreamAtom> midi::control_function_map
{
    {"RESETALL",     midi::control_reset_all_ctrl[0]},
    {"ALLNOTES_OFF", midi::control_all_notes_off[0]},
    {"ALL_SOUND_OFF", midi::control_all_sound_off[0]},
    {"BREATH",       midi::control_breath[0]},
    {"SOFTPEDAL",    midi::control_softpedal[0]},
    {"PORTAMENTO",   midi::control_portamento[0]},
    {"BANK_SELECT", midi::control_bank_select[0]},
    {"MODULATION", midi::control_modulation[0]},
    {"FOOT_CONTROLLER", midi::control_foot_controller[0]},
    {"PORTAMENTO_TIME", midi::control_portamento_time[0]},
    {"DATA_ENTRY_MSB", midi::control_data_entry_msb[0]},
    {"CHANNEL_VOLUME", midi::control_channel_volume[0]},
    {"BALANCE", midi::control_balance[0]},
    {"PAN", midi::control_pan[0]},
    {"EXPRESSION", midi::control_expression[0]},
    {"EFFECT_1", midi::control_effect_1[0]},
    {"EFFECT_2", midi::control_effect_2[0]},
    {"GENERAL_PURPOSE_1", midi::control_general_purpose_1[0]},
    {"GENERAL_PURPOSE_2", midi::control_general_purpose_2[0]},
    {"GENERAL_PURPOSE_3", midi::control_general_purpose_3[0]},
    {"GENERAL_PURPOSE_4", midi::control_general_purpose_4[0]},
    {"DAMPER", midi::control_damper[0]},
    {"PORTAMENTO_ON_OFF", midi::control_portamento_on_off[0]},
    {"SOSTENUTO", midi::control_sostenuto[0]},
    {"LEGATO_FOOT", midi::control_legato_foot[0]},
    {"HOLD_2", midi::control_hold_2[0]},
    {"SOUND_VARIATION", midi::control_sound_variation[0]},
    {"TIMBRE_INTENSITY", midi::control_timbre_intensity[0]},
    {"RELEASE_TIME", midi::control_release_time[0]},
    {"ATTACK_TIME", midi::control_attack_time[0]},
    {"BRIGHTNESS", midi::control_brightness[0]},
    {"DECAY_TIME", midi::control_decay_time[0]},
    {"VIBRATO_RATE", midi::control_vibrato_rate[0]},
    {"VIBRATO_DEPTH", midi::control_vibrato_depth[0]},
    {"SOUND_CONTROLLER_9", midi::control_sound_controller_9[0]},
    {"SOUND_CONTROLLER_10", midi::control_sound_controller_10[0]},
    {"GENERAL_PURPOSE_5", midi::control_general_purpose_5[0]},
    {"GENERAL_PURPOSE_6", midi::control_general_purpose_6[0]},
    {"GENERAL_PURPOSE_7", midi::control_general_purpose_7[0]},
    {"GENERAL_PURPOSE_8", midi::control_general_purpose_8[0]},
    {"HIRES_VELOCITY_MSB", midi::control_hires_velocity_msb[0]},
    {"REVERB_SEND_LEVEL", midi::control_reverb_send_level[0]},
    {"TREMOLO_DEPTH", midi::control_tremolo_depth[0]},
    {"CHORUS_SEND_LEVEL", midi::control_chorus_send_level[0]},
    {"CELESTE_DEPTH", midi::control_celeste_depth[0]},
    {"PHASER_DEPTH", midi::control_phaser_depth[0]},
    {"DATA_INCREMENT", midi::control_data_increment[0]},
    {"DATA_DECREMENT", midi::control_data_decrement[0]},
    {"NON_REGISTERED_PARAMETER_LSB", midi::control_non_registered_parameter_lsb[0]},
    {"NON_REGISTERED_PARAMETER_MSB", midi::control_non_registered_parameter_msb[0]},
    {"REGISTERED_PARAMETER_LSB", midi::control_registered_parameter_lsb[0]},
    {"REGISTERED_PARAMETER_MSB", midi::control_registered_parameter_msb[0]}
};

const NumStringMap<XmfPatchTypeEnum> midi::xmf_patch_type_map
{
    {"GM1", midi::XmfPatchTypeEnum::GM1},
    {"GM2", midi::XmfPatchTypeEnum::GM2},
    {"DLS", midi::XmfPatchTypeEnum::DLS}
};

ostream& midi::operator<<(ostream& os, midi::MIDI_Format mf)
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

ostream& midi::operator<<(ostream& os, const midi::MidiHeader& mh)
{
    auto flags{os.flags()};
    os << mh.chunk_name_[0] << mh.chunk_name_[1]    << mh.chunk_name_[2]
       << mh.chunk_name_[3] << ' ' << mh.chunk_len_ << ' ' << mh.format_
       << ' ' << mh.ntrks_  << ' ' << mh.division_;
    static_cast<void>(os.flags(flags));
    return os;
}

const NumStringMap<MidiStreamAtom> midi::sysex_subid_map
{
    {"NON_COMMERCIAL", sysex_subid_non_commercial[0]},
    {"NON_REALTIME", sysex_subid_non_realtime[0]},
    {"REALTIME", sysex_subid_realtime[0]}
};

const NumStringMap<MidiStreamAtom> midi::sysex_nonrt_id1_map
{
    {"NONRT_SAMPLE_DUMP_HEADER", sysex_subid_nonrt_sample_dump_header[0]},
    {"NONRT_SAMPLE_DATA_PACKET", sysex_subid_nonrt_sample_data_packet[0]},
    {"NONRT_SAMPLE_DUMP_REQUEST", sysex_subid_nonrt_sample_dump_request[0]},
    {"NONRT_TIMECODE", sysex_subid_nonrt_timecode[0]},
    {"NONRT_SAMPLE_DUMP_EXTENSIONS", sysex_subid_nonrt_sample_dump_extensions[0]},
    {"NONRT_GENERAL_INFO", sysex_subid_nonrt_general_info[0]},
    {"NONRT_FILE_DUMP", sysex_subid_nonrt_file_dump[0]},
    {"NONRT_TUNING_STD", sysex_subid_nonrt_tuning_std[0]},
    {"NONRT_GM", sysex_subid_nonrt_gm[0]},
    {"NONRT_END_OF_FILE", sysex_subid_nonrt_end_of_file[0]},
    {"NONRT_WAIT", sysex_subid_nonrt_wait[0]},
    {"NONRT_CANCEL", sysex_subid_nonrt_cancel[0]},
    {"NONRT_NAK", sysex_subid_nonrt_nak[0]},
    {"NONRT_ACK", sysex_subid_nonrt_ack[0]}
};

const NumStringMap<MidiStreamAtom> midi::sysex_rt_id1_map
{
    {"RT_TIMECODE", sysex_subid_rt_timecode[0]},
    {"RT_SHOW_CONTROL", sysex_subid_rt_show_control[0]},
    {"RT_NOTATION_INFORMATION", sysex_subid_rt_notation_information[0]},
    {"RT_DEVICE_CONTROL", sysex_subid_rt_device_control[0]},
    {"RT_MTC_CUEING", sysex_subid_rt_mtc_cueing[0]},
    {"RT_MACHINE_CONTROL_COMMANDS", sysex_subid_rt_machine_control_commands[0]},
    {"RT_MACHINE_CONTROL_RESPONSES", sysex_subid_rt_machine_control_responses[0]},
    {"RT_TUNING_STD", sysex_subid_rt_tuning_std[0]}
};

