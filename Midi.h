//
// TextMIDITools Version 1.0.92
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(MIDI_H)
#    define  MIDI_H

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <memory>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <iostream>

namespace midi
{
    //
    // Holds binary standard MIDI file format data.
    using MidiStreamAtom = std::uint8_t;
    using MidiStreamVector = std::vector<MidiStreamAtom>;
    using MidiStreamIterator = MidiStreamVector::iterator;
    using MidiStreamConstIterator = MidiStreamVector::const_iterator;
    using MidiStreamArray1 = std::array<MidiStreamAtom, 1>;
    using MidiStreamArray2 = std::array<MidiStreamAtom, 2>;
    using MidiStreamArray4 = std::array<MidiStreamAtom, 4>;

    inline constexpr double UsecPerSecond{1000000.0};
    inline constexpr double SecondsPerMinute{60.0};
    inline constexpr std::int32_t
        SecondsPerMinuteI{static_cast<std::int32_t>(SecondsPerMinute)};
    inline constexpr double MinDynamic{30};
    inline constexpr double MaxDynamic{127};
    inline constexpr double MaxPitchBend{127};
    inline constexpr std::int32_t MinSignedPan{-64};
    inline constexpr std::int32_t CenterSignedPan{0};
    inline constexpr std::int32_t MaxSignedPan{63};

    inline constexpr std::int32_t MaxSmpteHours{23};
    inline constexpr std::int32_t MaxSmpteMinutes{59};
    inline constexpr std::int32_t MaxSmpteSeconds{59};
    inline constexpr std::int32_t MaxSmpteFrames{29};
    inline constexpr std::int32_t MaxSmpteHundredths{99};

    inline constexpr std::int32_t MidiPitchQty{128};
    inline constexpr std::int32_t MidiProgramQty{128};
    inline constexpr double MaxKeyboardKey{127};
    inline constexpr std::int32_t MidiChannelQty{16};
    inline constexpr std::int32_t MidiIdiophoneChannel{10};
    inline constexpr std::int32_t PanExcess64{64};
    inline constexpr std::uint32_t variable_len_word_mask{0xfffffff};

    inline constexpr std::int64_t
        variable_length_quantity_min_len{sizeof(std::int32_t)};
    inline constexpr MidiStreamAtom event_flag               {0x80};
    inline constexpr MidiStreamAtom variable_len_flag        {0x80};
    inline constexpr MidiStreamAtom
        variable_len_byte_mask
        {static_cast<MidiStreamAtom>(~variable_len_flag)};
    inline constexpr MidiStreamAtom variable_len_shift       {7};
    inline constexpr MidiStreamAtom octet_mask               {0xff};
    inline constexpr MidiStreamArray1 meta_prefix{0xff};
    inline constexpr MidiStreamArray2 sequence_number_prefix {0x00, 0x02};
    // Variable-length string meta-events.
    inline constexpr MidiStreamArray1 text_prefix            {0x01};
    inline constexpr MidiStreamArray1 copyright_prefix       {0x02};
    inline constexpr MidiStreamArray1 track_name_prefix      {0x03};
    inline constexpr MidiStreamArray1 instrument_name_prefix {0x04};
    inline constexpr MidiStreamArray1 lyric_prefix           {0x05};
    inline constexpr MidiStreamArray1 marker_prefix          {0x06};
    inline constexpr MidiStreamArray1 cue_point_prefix       {0x07};
    inline constexpr MidiStreamArray1 program_name_prefix    {0x08};
    inline constexpr MidiStreamArray1 device_name_prefix     {0x09};
    inline constexpr MidiStreamArray1 text_0A_prefix         {0x0A};
    inline constexpr MidiStreamArray1 text_0B_prefix         {0x0B};
    inline constexpr MidiStreamArray1 text_0C_prefix         {0x0C};
    inline constexpr MidiStreamArray1 text_0D_prefix         {0x0D};
    inline constexpr MidiStreamArray1 text_0E_prefix         {0x0E};
    inline constexpr MidiStreamArray1 text_0F_prefix         {0x0F};
    inline constexpr MidiStreamArray1 unknown_prefix         {0x11};
    inline constexpr MidiStreamArray2 smpte_prefix{0x54, 5};
    inline constexpr MidiStreamArray2 midi_channel_prefix    {0x20, 1};
    inline constexpr MidiStreamArray2 end_of_track_prefix    {0x2f, 0};
    inline constexpr MidiStreamArray2 tempo_prefix           {0x51, 3};
    inline constexpr MidiStreamArray2 time_signature_prefix  {0x58, 4};
    inline constexpr MidiStreamArray2 key_signature_prefix   {0x59, 2};
    inline constexpr MidiStreamArray2 xmf_patch_type_prefix  {0x60, 1};
    enum class XmfPatchTypeEnum : MidiStreamAtom
    {
        GM1 = 1,
        GM2 = 2,
        DLS = 3
    };

    inline constexpr MidiStreamAtom patch_type_gm1{0x01};
    inline constexpr MidiStreamAtom patch_type_gm2{0x02};
    inline constexpr MidiStreamAtom patch_type_dls{0x03};
    // Quicktime left out the major/minor mode byte.
    inline constexpr MidiStreamArray2
        key_signature_prefix_mode_missing{0x59, 1};
    inline constexpr MidiStreamArray2 midi_port_prefix       {0x21, 1};
    inline constexpr MidiStreamArray1 sequencer_specific_prefix{0x7f};

    // To recognize what is an unknown meta event,
    // first check if it is in this list.
    const std::set<MidiStreamAtom> Initial_Meta{sequence_number_prefix[0],
        text_prefix[0], copyright_prefix[0], track_name_prefix[0],
        instrument_name_prefix[0], lyric_prefix[0], marker_prefix[0],
        cue_point_prefix[0], program_name_prefix[0], device_name_prefix[0],
        text_0A_prefix[0], text_0B_prefix[0], text_0C_prefix[0],
        text_0D_prefix[0], text_0E_prefix[0], text_0F_prefix[0],
        midi_channel_prefix[0], end_of_track_prefix[0], tempo_prefix[0],
        smpte_prefix[0], time_signature_prefix[0], key_signature_prefix[0],
        xmf_patch_type_prefix[0], sequencer_specific_prefix[0],
        midi_port_prefix[0]};

    // fixed-length meta-events
    inline constexpr MidiStreamAtom smpte_24fps          {0};
    inline constexpr MidiStreamAtom smpte_25fps          {1};
    inline constexpr MidiStreamAtom smpte_30fpsdropframe {2};
    inline constexpr MidiStreamAtom smpte_30fpsnondropframe{3};
    inline constexpr MidiStreamAtom smpte_fps_shift      {5};
    inline constexpr MidiStreamAtom smpte_fps_mask       {3};
    inline constexpr MidiStreamAtom smpte_hours_mask     {0x1f};

    inline constexpr MidiStreamAtom NoteOn                = 0x90;
    inline constexpr MidiStreamAtom NoteOff               = 0x80;
    inline constexpr MidiStreamAtom PolyphonicKeyPressure = 0xa0;
    inline constexpr MidiStreamAtom Control               = 0xb0;
    inline constexpr MidiStreamAtom Program               = 0xc0;
    inline constexpr MidiStreamAtom ChannelPressure       = 0xd0;
    inline constexpr MidiStreamAtom PitchWheel            = 0xe0;

    inline constexpr MidiStreamArray1 note_on                {NoteOn};
    inline constexpr MidiStreamArray1 note_off               {NoteOff};
    inline constexpr std::int32_t          full_note_length       {3};
    inline constexpr std::int32_t          running_status_note_length{2};
    inline constexpr MidiStreamArray1
        polyphonic_key_pressure{PolyphonicKeyPressure};
    inline constexpr MidiStreamArray1 control                {Control};
    inline constexpr MidiStreamArray1 channel_mode           {0xb0};
    inline constexpr MidiStreamArray1 program                {Program};
    inline constexpr MidiStreamArray1 channel_pressure       {ChannelPressure};
    inline constexpr MidiStreamArray1 pitch_wheel            {PitchWheel};
    inline constexpr MidiStreamAtom channel_mask         {0x0F};
    inline constexpr MidiStreamAtom byte7_mask           {0x7F};
    inline constexpr MidiStreamAtom byte7_shift          {7};
    inline constexpr MidiStreamAtom nybble_mask          {0x0F};

    inline constexpr MidiStreamArray1 control_bank_select     {0x00};
    inline constexpr MidiStreamArray1 control_modulation      {0x01};
    inline constexpr MidiStreamArray1 control_breath_controller{0x02};
    inline constexpr MidiStreamArray1 control_undefined_03    {0x03};
    inline constexpr MidiStreamArray1 control_foot_controller {0x04};
    inline constexpr MidiStreamArray1 control_portamento_time {0x05};
    inline constexpr MidiStreamArray1 control_data_entry_msb  {0x06};
    inline constexpr MidiStreamArray1 control_channel_volume  {0x07};
    inline constexpr MidiStreamArray1 control_balance         {0x08};
    inline constexpr MidiStreamArray1 control_undefined_09    {0x09};

    inline constexpr MidiStreamArray1 control_pan             {0x0A};

    inline constexpr MidiStreamArray1 control_expression      {0x0b};
    inline constexpr MidiStreamArray1 control_effect_1        {0x0c};
    inline constexpr MidiStreamArray1 control_effect_2        {0x0d};
    inline constexpr MidiStreamArray1 control_undefined_14    {0x0e};
    inline constexpr MidiStreamArray1 control_undefined_15    {0x0f};
    inline constexpr MidiStreamArray1 control_general_purpose_1{0x10};
    inline constexpr MidiStreamArray1 control_general_purpose_2{0x11};
    inline constexpr MidiStreamArray1 control_general_purpose_3{0x12};
    inline constexpr MidiStreamArray1 control_general_purpose_4{0x13};
    inline constexpr MidiStreamArray1 control_undefined_20     {0x14};
    inline constexpr MidiStreamArray1 control_undefined_21     {0x15};
    inline constexpr MidiStreamArray1 control_undefined_22     {0x16};
    inline constexpr MidiStreamArray1 control_undefined_23     {0x17};
    inline constexpr MidiStreamArray1 control_undefined_24     {0x18};
    inline constexpr MidiStreamArray1 control_undefined_25     {0x19};
    inline constexpr MidiStreamArray1 control_undefined_26     {0x1a};
    inline constexpr MidiStreamArray1 control_undefined_27     {0x1b};
    inline constexpr MidiStreamArray1 control_undefined_28     {0x1c};
    inline constexpr MidiStreamArray1 control_undefined_29     {0x1d};
    inline constexpr MidiStreamArray1 control_undefined_30     {0x1e};
    inline constexpr MidiStreamArray1 control_undefined_31     {0x1f};
    inline constexpr MidiStreamArray1 control_lsb_00           {0x20};
    inline constexpr MidiStreamArray1 control_lsb_01           {0x21};
    inline constexpr MidiStreamArray1 control_lsb_02           {0x22};
    inline constexpr MidiStreamArray1 control_lsb_03           {0x23};
    inline constexpr MidiStreamArray1 control_lsb_04           {0x24};
    inline constexpr MidiStreamArray1 control_lsb_05           {0x25};
    inline constexpr MidiStreamArray1 control_lsb_06           {0x26};
    inline constexpr MidiStreamArray1 control_lsb_07           {0x27};
    inline constexpr MidiStreamArray1 control_lsb_08           {0x28};
    inline constexpr MidiStreamArray1 control_lsb_09           {0x29};
    inline constexpr MidiStreamArray1 control_lsb_10           {0x2a};
    inline constexpr MidiStreamArray1 control_lsb_11           {0x2b};
    inline constexpr MidiStreamArray1 control_lsb_12           {0x2c};
    inline constexpr MidiStreamArray1 control_lsb_13           {0x2d};
    inline constexpr MidiStreamArray1 control_lsb_14           {0x2e};
    inline constexpr MidiStreamArray1 control_lsb_15           {0x2f};
    inline constexpr MidiStreamArray1 control_lsb_16           {0x30};
    inline constexpr MidiStreamArray1 control_lsb_17           {0x31};
    inline constexpr MidiStreamArray1 control_lsb_18           {0x32};
    inline constexpr MidiStreamArray1 control_lsb_19           {0x33};
    inline constexpr MidiStreamArray1 control_lsb_20           {0x34};
    inline constexpr MidiStreamArray1 control_lsb_21           {0x35};
    inline constexpr MidiStreamArray1 control_lsb_22           {0x36};
    inline constexpr MidiStreamArray1 control_lsb_23           {0x37};
    inline constexpr MidiStreamArray1 control_lsb_24           {0x38};
    inline constexpr MidiStreamArray1 control_lsb_25           {0x39};
    inline constexpr MidiStreamArray1 control_lsb_26           {0x3a};
    inline constexpr MidiStreamArray1 control_lsb_27           {0x3b};
    inline constexpr MidiStreamArray1 control_lsb_28           {0x3c};
    inline constexpr MidiStreamArray1 control_lsb_29           {0x3d};
    inline constexpr MidiStreamArray1 control_lsb_30           {0x3e};
    inline constexpr MidiStreamArray1 control_lsb_31           {0x3f};
    inline constexpr MidiStreamArray1 control_damper          {0x40};
    inline constexpr MidiStreamArray1 control_portamento_on_off{0x41};
    inline constexpr MidiStreamArray1 control_sostenuto       {0x42};
    inline constexpr MidiStreamArray1 control_softpedal       {0x43};
    inline constexpr MidiStreamAtom   control_pedalthreshold  {0x40};
    inline constexpr MidiStreamArray1 control_legato_foot     {0x44};
    inline constexpr MidiStreamArray1 control_hold_2           {0x45};
    inline constexpr MidiStreamArray1 control_sound_variation {0x46};
    inline constexpr MidiStreamArray1 control_timbre_intensity{0x47};
    inline constexpr MidiStreamArray1 control_release_time    {0x48};
    inline constexpr MidiStreamArray1 control_attack_time     {0x49};
    inline constexpr MidiStreamArray1 control_brightness      {0x4a};
    inline constexpr MidiStreamArray1 control_decay_time      {0x4b};
    inline constexpr MidiStreamArray1 control_vibrato_rate    {0x4c};
    inline constexpr MidiStreamArray1 control_vibrato_depth   {0x4d};
    inline constexpr MidiStreamArray1 control_sound_controller_9 {0x4e};
    inline constexpr MidiStreamArray1 control_sound_controller_10 {0x4f};

    inline constexpr MidiStreamArray1 control_general_purpose_5{0x50};
    inline constexpr MidiStreamArray1 control_general_purpose_6{0x51};
    inline constexpr MidiStreamArray1 control_general_purpose_7{0x52};
    inline constexpr MidiStreamArray1 control_general_purpose_8{0x53};
    inline constexpr MidiStreamArray1 control_portamento       {0x54};
    inline constexpr MidiStreamArray1 control_hires_velocity_msb{0x58};

    inline constexpr MidiStreamArray1 control_reverb_send_level {0x5b};
    inline constexpr MidiStreamArray1 control_tremolo_depth  {0x5c};
    inline constexpr MidiStreamArray1 control_chorus_send_level {0x5d};
    inline constexpr MidiStreamArray1 control_celeste_depth  {0x5e};
    inline constexpr MidiStreamArray1 control_phaser_depth   {0x5f};
    inline constexpr MidiStreamArray1 control_data_increment {0x60};
    inline constexpr MidiStreamArray1 control_data_decrement {0x61};
    inline constexpr MidiStreamArray1
        control_non_registered_parameter_lsb {0x62};
    inline constexpr MidiStreamArray1
        control_non_registered_parameter_msb {0x63};
    inline constexpr MidiStreamArray1 control_registered_parameter_lsb {0x64};
    inline constexpr MidiStreamArray1 control_registered_parameter_msb {0x65};

    // Reserved by MIDI Spec:
    inline constexpr MidiStreamArray1 control_all_sound_off  {0x78};
    inline constexpr MidiStreamArray1 control_reset_all_controllers {0x79};
    inline constexpr MidiStreamArray1 control_local_control{0x7a};
    inline constexpr MidiStreamAtom   local_control_off{0x00};
    inline constexpr MidiStreamAtom   local_control_on{0x7f};
    inline constexpr MidiStreamArray1 control_all_notes_off  {0x7b};
    inline constexpr MidiStreamArray1 control_omni_off       {0x7c};
    inline constexpr MidiStreamArray1 control_omni_on        {0x7d};
    inline constexpr MidiStreamArray1 control_mono_on        {0x7e};
    inline constexpr MidiStreamArray1 control_poly_on        {0x7f};
    inline constexpr MidiStreamArray1 control_full           {0x7F};
    inline constexpr MidiStreamArray1 control_off            {0x00};
    inline constexpr MidiStreamArray1 midi_time_code_quarter_frame{0xf1};
    inline constexpr MidiStreamArray1 song_position_pointer  {0xf2};
    inline constexpr MidiStreamArray1 song_select            {0xf3};
    inline constexpr MidiStreamArray1 tune_request           {0xf6};
    inline constexpr MidiStreamArray1 start_of_sysex         {0xf0};
    inline constexpr MidiStreamArray1 end_of_sysex           {0xf7};

    // MIDI 1.0 Detailed Specification
    // M1_v4-2-1_MIDI_1-0_Detailed_Specification_96-1-4.pdf
    // page 35, "Device ID" 0x7f is "All Call" or, on page 57, "Broadcast".
    inline constexpr MidiStreamArray1 sysex_deviceid_all_call {0x7f};
    inline constexpr MidiStreamArray1 sysex_subid_non_commercial {0x7d};
    inline constexpr MidiStreamArray1 sysex_subid_non_realtime {0x7e};
    inline constexpr MidiStreamArray1 sysex_subid_realtime {0x7f};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_sample_dump_header {0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_sample_data_packet {0x02};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_sample_dump_request {0x03};

    inline constexpr MidiStreamArray1 sysex_subid_nonrt_timecode{0x04};
    inline constexpr MidiStreamArray1 sysex_subid_timecode_special{0x00};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_punch_in_points{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_punch_out_points{0x02};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_delete_punch_in_point{0x03};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_delete_punch_out_point{0x04};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_event_start_point{0x05};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_event_stop_point{0x06};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_event_start_points_with_additional_info{0x07};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_event_stop_points_with_additional_info{0x08};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_delete_event_start_point{0x09};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_delete_event_stop_point{0x0a};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_cue_points{0x0b};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_cue_points_with_additional_info{0x0c};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_delete_cue_point{0x0d};
    inline constexpr MidiStreamArray1
        sysex_subid_timecode_event_name_in_additional_info{0x0e};

    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_sample_dump_extensions{0x05};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_sample_dump_extensions_multiple_loop_points{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_sample_dump_extensions_loop_points_request{0x02};

    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_general_info{0x06};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_general_info_id_request{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_general_info_id_reply{0x02};

    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_file_dump{0x07};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_file_dump_header{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_file_dump_data_packet{0x02};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_file_dump_data_request{0x03};

    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_tuning_std{0x08};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_tuning_std_bulk_dump_request{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_tuning_std_bulk_dump_reply{0x02};

    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_gm{0x09};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_gm_on{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_gm_off{0x02};

    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_end_of_file{0x7b};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_wait{0x7c};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_cancel{0x7d};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_nak{0x7e};
    inline constexpr MidiStreamArray1
        sysex_subid_nonrt_ack{0x7f};

    inline constexpr MidiStreamArray1
        sysex_subid_rt_timecode{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_timecode_full_message {0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_timecode_user_bits{0x02};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_show_control{0x02};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_show_control_extensions {0x00};

    inline constexpr MidiStreamArray1
        sysex_subid_rt_notation_information{0x03};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_notation_information_bar_number{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_notation_information_time_signature_immediate{0x02};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_notation_information_time_signature_delayed{0x42};

    inline constexpr MidiStreamArray1
        sysex_subid_rt_device_control{0x04};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_device_control_main_volume{0x01};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_device_control_main_balance{0x02};

    inline constexpr MidiStreamArray1
        sysex_subid_rt_mtc_cueing{0x05};
    // Use non-RT timecode commands except delete punch,
    // delete event, delete cue point.

    inline constexpr MidiStreamArray1
        sysex_subid_rt_machine_control_commands{0x06};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_machine_control_responses{0x07};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_tuning_std{0x08};
    inline constexpr MidiStreamArray1
        sysex_subid_rt_tuning_std_note_change{0x02};

    inline constexpr MidiStreamAtom MiddleC{60};

    inline constexpr std::int32_t SMPTE_hours_max{23};

    inline constexpr std::int64_t QuartersPerWhole(4);
    inline constexpr size_t bits_per_byte{8};
    inline constexpr auto ChunkNameLen{4};
    inline constexpr auto HeaderChunkLen{6};
    inline constexpr MidiStreamArray4 MidiHeaderChunkName{'M', 'T', 'h', 'd'};
    inline constexpr MidiStreamArray4 MidiTrackChunkName {'M', 'T', 'r', 'k'};

    enum class RegisteredParameterMsbs : MidiStreamAtom
    {
        parameter_00_msb = 0x00,
        parameter_3d_msb = 0x3d,
    };

    enum class Registered00ParameterLsbs : MidiStreamAtom
    {
         pitch_bend_sensitivity = 0x00,
         fine_tuning            = 0x01,
         coarse_tuning          = 0x02,
         tuning_program_select  = 0x03,
         tuning_bank_select     = 0x04,
    };

    enum class Registered3dParameterLsbs : MidiStreamAtom
    {
         azimuth                  = 0x00,
         elevation                = 0x01,
         gain                     = 0x02,
         distance                 = 0x03,
         maximum_distance         = 0x04,
         gain_at_max_distance     = 0x05,
         reference_distance_ratio = 0x06,
         pan_spread_angle         = 0x07,
         roll_angle               = 0x08,
    };

    enum class MIDI_Format : std::uint16_t
    {
        MonoTrack,
        MultiTrack,
        MultiSequence
    };


#pragma pack(1)
    struct MidiHeader
    {
        MidiHeader() = default;
        MidiHeader(std::uint32_t chunk_len, MIDI_Format format,
            std::uint16_t ntrks, std::uint16_t division) noexcept
         :  chunk_len_{chunk_len},
            format_{format},
            ntrks_{ntrks},
            division_{division}
            {}

        explicit MidiHeader(MidiStreamAtom* bytes) noexcept
          : chunk_len_{},
            format_{},
            ntrks_{},
            division_{}
        {
            std::memcpy(this, bytes, sizeof(MidiHeader));
            swapbytes();
        }

#pragma pack(push)
#pragma pack(1)
        MidiStreamArray4 chunk_name_{MidiHeaderChunkName};
        std::uint32_t chunk_len_{HeaderChunkLen}; // big-endian
        // 0: single multichannel track;
        // 1: many tracks;
        // 2: multiple independent tracks
        MIDI_Format format_{};
        std::uint16_t ntrks_{};     // number of tracks
        std::uint16_t division_{};  // ticks/quarter (unless SMPTE, check spec)
#pragma pack(pop)
        void swapbytes() noexcept;
    };

#pragma pack()

    enum class RunningStatusPolicy : std::int32_t
    {
        Standard                   = 1,
        Never                      = 2,
        PersistentAfterMeta        = 3,
        PersistentAfterSysex       = 4,
        PersistentAfterSysexOrMeta = 5,
    };
    class RunningStatusImplBase
    {
      public:
        virtual void running_status(MidiStreamAtom running_status_value) = 0;
        virtual bool running_status_valid() const = 0;
        virtual MidiStreamAtom running_status_value() const = 0;
        // returns a zero-based channel
        virtual void clear() = 0;
        virtual MidiStreamAtom channel() const = 0;
        virtual MidiStreamAtom command() const = 0;
        virtual ~RunningStatusImplBase() = default;
    };

    class RunningStatusImpl final : public RunningStatusImplBase
    {
      public:
        RunningStatusImpl() = default;
        void running_status(MidiStreamAtom running_status_value)
            noexcept final;
        bool running_status_valid() const noexcept override;
        MidiStreamAtom running_status_value() const noexcept override;
        void clear() noexcept override;
        // returns a zero-based channel
        MidiStreamAtom channel() const noexcept override;
        MidiStreamAtom command() const noexcept override;
      private:
        bool running_status_valid_{};
        MidiStreamAtom running_status_value_{};
    };

    class RunningStatusBase
    {
      public:
        RunningStatusBase() = default;
        // Rule of zero: if no custom D-tor, copy, assign, move, move copy,
        // then define none of them.
        virtual void running_status(midi::MidiStreamAtom running_status_value)
            noexcept
        {
            running_status_impl_->running_status(running_status_value);
        }

        void clear() noexcept
        {
            running_status_impl_->clear();
        }

        virtual void operator()(MidiStreamAtom status_byte,
            MidiStreamVector& track) = 0;

        virtual bool running_status_valid() const noexcept
        {
            return running_status_impl_->running_status_valid();
        }

        virtual MidiStreamAtom running_status_value() const noexcept
        {
            return running_status_impl_->running_status_value();
        }

        // returns a zero-based channel
        virtual MidiStreamAtom channel() const noexcept
        {
            return running_status_impl_->channel();
        }

        virtual MidiStreamAtom command() const noexcept
        {
            return running_status_impl_->command();
        }

        virtual ~RunningStatusBase() = default;
      private:
        std::shared_ptr<RunningStatusImplBase>
            running_status_impl_{std::make_shared<RunningStatusImpl>()};
    };

    class RunningStatusStandard final : public RunningStatusBase
    {
      public:
        void operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
            noexcept final;
    };

    class RunningStatusNever final : public RunningStatusBase
    {
      public:
        void operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
            noexcept final;
    };

    class RunningStatusPersistentAfterMeta final : public RunningStatusBase
    {
      public:
        void operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
            noexcept final;
    };

    class RunningStatusPersistentAfterSysex final : public RunningStatusBase
    {
      public:
        void operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
            noexcept final;
    };

    class RunningStatusPersistentAfterSysexOrMeta final
        : public RunningStatusBase
    {
      public:
        void operator()(MidiStreamAtom status_byte, MidiStreamVector& track)
            noexcept final;
    };

    class RunningStatusFactory
    {
      public:
          std::unique_ptr<RunningStatusBase>
              operator()(RunningStatusPolicy policy) noexcept;
    };
} // namespace midi
#endif // MIDI_H
