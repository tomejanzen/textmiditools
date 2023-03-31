//
// TextMIDITools Version 1.0.54
//
// Copyright © 2023 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if !defined(MIDI_H)
#    define  MIDI_H

#include <cstddef>
#include <cstdint>
#include <cstring>

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

    constexpr double UsecPerSecond{1000000.0};
    constexpr double SecondsPerMinute{60.0};
    constexpr int SecondsPerMinuteI{static_cast<int>(SecondsPerMinute)};
    constexpr double MinDynamic{30};
    constexpr double MaxDynamic{127};
    constexpr double MaxPitchBend{127};
    constexpr int MinSignedPan{-64};
    constexpr int CenterSignedPan{0};
    constexpr int MaxSignedPan{63};

    constexpr int MaxSmpteHours{23};
    constexpr int MaxSmpteMinutes{59};
    constexpr int MaxSmpteSeconds{59};
    constexpr int MaxSmpteFrames{29};
    constexpr int MaxSmpteHundredths{99};

    constexpr int MidiPitchQty{128};
    constexpr int MidiProgramQty{128};
    constexpr double MaxKeyboardKey{127};
    constexpr int MidiChannelQty{16};
    constexpr int MidiIdiophoneChannel{10};
    constexpr std::int32_t PanExcess64{64};
    constexpr std::uint32_t variable_len_word_mask{0xfffffff};

    constexpr long int variable_length_quantity_max_len{sizeof(int)};
    constexpr long int variable_length_quantity_min_len{sizeof(int)};
    constexpr MidiStreamAtom event_flag               {0x80};
    constexpr MidiStreamAtom variable_len_flag        {0x80};
    constexpr MidiStreamAtom
        variable_len_byte_mask   {static_cast<MidiStreamAtom>(~variable_len_flag)};
    constexpr MidiStreamAtom variable_len_shift       {7};
    constexpr MidiStreamAtom octet_mask               {0xff};
    constexpr MidiStreamArray1 meta_prefix{0xff};
    constexpr MidiStreamArray2 sequence_number_prefix {0x00, 0x02}; // sequence number
    // Variable-length string meta-events.
    constexpr MidiStreamArray1 text_prefix            {0x01};
    constexpr MidiStreamArray1 copyright_prefix       {0x02};
    constexpr MidiStreamArray1 track_name_prefix      {0x03};
    constexpr MidiStreamArray1 instrument_name_prefix {0x04};
    constexpr MidiStreamArray1 lyric_prefix           {0x05};
    constexpr MidiStreamArray1 marker_prefix          {0x06};
    constexpr MidiStreamArray1 cue_point_prefix       {0x07};
    constexpr MidiStreamArray1 program_name_prefix    {0x08};
    constexpr MidiStreamArray1 device_name_prefix     {0x09};
    constexpr MidiStreamArray1 text_0A_prefix         {0x0A};
    constexpr MidiStreamArray1 text_0B_prefix         {0x0B};
    constexpr MidiStreamArray1 text_0C_prefix         {0x0C};
    constexpr MidiStreamArray1 text_0D_prefix         {0x0D};
    constexpr MidiStreamArray1 text_0E_prefix         {0x0E};
    constexpr MidiStreamArray1 text_0F_prefix         {0x0F};
    constexpr MidiStreamArray1 unknown_prefix         {0x11};
    constexpr MidiStreamArray2 smpte_prefix           {0x54, 5}; // prefix, length
    constexpr MidiStreamArray2 midi_channel_prefix    {0x20, 1}; // prefix, length
    constexpr MidiStreamArray2 end_of_track_prefix    {0x2f, 0}; // prefix, length
    constexpr MidiStreamArray2 tempo_prefix           {0x51, 3}; // prefix, length
    constexpr MidiStreamArray2 time_signature_prefix  {0x58, 4}; // prefix, length
    constexpr MidiStreamArray2 key_signature_prefix   {0x59, 2}; // prefix, length
    constexpr MidiStreamArray2 xmf_patch_type_prefix  {0x60, 1}; // GM1, or GM2, or DLS.
    enum class XmfPatchTypeEnum : MidiStreamAtom
    {
        GM1 = 1,
        GM2 = 2,
        DLS = 3
    };

    constexpr MidiStreamAtom patch_type_gm1{0x01};
    constexpr MidiStreamAtom patch_type_gm2{0x02};
    constexpr MidiStreamAtom patch_type_dls{0x03};
    // Quicktime left out the major/minor mode byte.
    constexpr MidiStreamArray2 key_signature_prefix_mode_missing   {0x59, 1}; // prefix, length;
    constexpr MidiStreamArray2 midi_port_prefix       {0x21, 1};
    constexpr MidiStreamArray1 sequencer_specific_prefix{0x7f};

    // To recognize what is an unknown meta event, first check if it is in this list.
    const std::set<MidiStreamAtom> Initial_Meta{sequence_number_prefix[0], text_prefix[0],
        copyright_prefix[0], track_name_prefix[0], instrument_name_prefix[0],
        lyric_prefix[0], marker_prefix[0], cue_point_prefix[0], program_name_prefix[0],
        device_name_prefix[0], text_0A_prefix[0], text_0B_prefix[0], text_0C_prefix[0],
        text_0D_prefix[0], text_0E_prefix[0], text_0F_prefix[0], midi_channel_prefix[0],
        end_of_track_prefix[0], tempo_prefix[0], smpte_prefix[0], time_signature_prefix[0],
        key_signature_prefix[0], xmf_patch_type_prefix[0], sequencer_specific_prefix[0], midi_port_prefix[0]};

    // fixed-length meta-events
    constexpr MidiStreamAtom smpte_24fps          {0};
    constexpr MidiStreamAtom smpte_25fps          {1};
    constexpr MidiStreamAtom smpte_30fpsdropframe {2};
    constexpr MidiStreamAtom smpte_30fpsnondropframe{3};
    constexpr MidiStreamAtom smpte_fps_shift      {5};
    constexpr MidiStreamAtom smpte_fps_mask       {3};
    constexpr MidiStreamAtom smpte_hours_mask     {0x1f};

    constexpr MidiStreamAtom NoteOn                = 0x90;
    constexpr MidiStreamAtom NoteOff               = 0x80;
    constexpr MidiStreamAtom PolyphonicKeyPressure = 0xa0;
    constexpr MidiStreamAtom Control               = 0xb0;
    constexpr MidiStreamAtom Program               = 0xc0;
    constexpr MidiStreamAtom ChannelPressure       = 0xd0;
    constexpr MidiStreamAtom PitchWheel            = 0xe0;

    constexpr MidiStreamArray1 note_on                {NoteOn};
    constexpr MidiStreamArray1 note_off               {NoteOff};
    constexpr int          full_note_length       {3};
    constexpr int          running_status_note_length{2};
    constexpr MidiStreamArray1 polyphonic_key_pressure{PolyphonicKeyPressure};
    constexpr MidiStreamArray1 control                {Control};
    constexpr MidiStreamArray1 channel_mode           {0xb0};
    constexpr MidiStreamArray1 program                {Program};
    constexpr MidiStreamArray1 channel_pressure       {ChannelPressure};
    constexpr MidiStreamArray1 pitch_wheel            {PitchWheel};
    constexpr MidiStreamAtom channel_mask         {0x0F};
    constexpr MidiStreamAtom byte7_mask           {0x7F};
    constexpr MidiStreamAtom byte7_shift          {7};
    constexpr MidiStreamAtom nybble_mask          {0x0F};

    constexpr MidiStreamArray1 control_bank_select     {0x00};
    constexpr MidiStreamArray1 control_modulation      {0x01};
    constexpr MidiStreamArray1 control_breath_controller{0x02};
    constexpr MidiStreamArray1 control_undefined_03    {0x03};
    constexpr MidiStreamArray1 control_foot_controller {0x04};
    constexpr MidiStreamArray1 control_portamento_time {0x05};
    constexpr MidiStreamArray1 control_data_entry_msb  {0x06};
    constexpr MidiStreamArray1 control_channel_volume  {0x07};
    constexpr MidiStreamArray1 control_balance         {0x08};
    constexpr MidiStreamArray1 control_undefined_09    {0x09};

    constexpr MidiStreamArray1 control_pan             {0x0A};

    constexpr MidiStreamArray1 control_expression      {0x0b};
    constexpr MidiStreamArray1 control_effect_1        {0x0c};
    constexpr MidiStreamArray1 control_effect_2        {0x0d};
    constexpr MidiStreamArray1 control_undefined_14    {0x0e};
    constexpr MidiStreamArray1 control_undefined_15    {0x0f};
    constexpr MidiStreamArray1 control_general_purpose_1{0x10};
    constexpr MidiStreamArray1 control_general_purpose_2{0x11};
    constexpr MidiStreamArray1 control_general_purpose_3{0x12};
    constexpr MidiStreamArray1 control_general_purpose_4{0x13};
    constexpr MidiStreamArray1 control_undefined_20     {0x14};
    constexpr MidiStreamArray1 control_undefined_21     {0x15};
    constexpr MidiStreamArray1 control_undefined_22     {0x16};
    constexpr MidiStreamArray1 control_undefined_23     {0x17};
    constexpr MidiStreamArray1 control_undefined_24     {0x18};
    constexpr MidiStreamArray1 control_undefined_25     {0x19};
    constexpr MidiStreamArray1 control_undefined_26     {0x1a};
    constexpr MidiStreamArray1 control_undefined_27     {0x1b};
    constexpr MidiStreamArray1 control_undefined_28     {0x1c};
    constexpr MidiStreamArray1 control_undefined_29     {0x1d};
    constexpr MidiStreamArray1 control_undefined_30     {0x1e};
    constexpr MidiStreamArray1 control_undefined_31     {0x1f};
    constexpr MidiStreamArray1 control_lsb_00           {0x20};
    constexpr MidiStreamArray1 control_lsb_01           {0x21};
    constexpr MidiStreamArray1 control_lsb_02           {0x22};
    constexpr MidiStreamArray1 control_lsb_03           {0x23};
    constexpr MidiStreamArray1 control_lsb_04           {0x24};
    constexpr MidiStreamArray1 control_lsb_05           {0x25};
    constexpr MidiStreamArray1 control_lsb_06           {0x26};
    constexpr MidiStreamArray1 control_lsb_07           {0x27};
    constexpr MidiStreamArray1 control_lsb_08           {0x28};
    constexpr MidiStreamArray1 control_lsb_09           {0x29};
    constexpr MidiStreamArray1 control_lsb_10           {0x2a};
    constexpr MidiStreamArray1 control_lsb_11           {0x2b};
    constexpr MidiStreamArray1 control_lsb_12           {0x2c};
    constexpr MidiStreamArray1 control_lsb_13           {0x2d};
    constexpr MidiStreamArray1 control_lsb_14           {0x2e};
    constexpr MidiStreamArray1 control_lsb_15           {0x2f};
    constexpr MidiStreamArray1 control_lsb_16           {0x30};
    constexpr MidiStreamArray1 control_lsb_17           {0x31};
    constexpr MidiStreamArray1 control_lsb_18           {0x32};
    constexpr MidiStreamArray1 control_lsb_19           {0x33};
    constexpr MidiStreamArray1 control_lsb_20           {0x34};
    constexpr MidiStreamArray1 control_lsb_21           {0x35};
    constexpr MidiStreamArray1 control_lsb_22           {0x36};
    constexpr MidiStreamArray1 control_lsb_23           {0x37};
    constexpr MidiStreamArray1 control_lsb_24           {0x38};
    constexpr MidiStreamArray1 control_lsb_25           {0x39};
    constexpr MidiStreamArray1 control_lsb_26           {0x3a};
    constexpr MidiStreamArray1 control_lsb_27           {0x3b};
    constexpr MidiStreamArray1 control_lsb_28           {0x3c};
    constexpr MidiStreamArray1 control_lsb_29           {0x3d};
    constexpr MidiStreamArray1 control_lsb_30           {0x3e};
    constexpr MidiStreamArray1 control_lsb_31           {0x3f};
    constexpr MidiStreamArray1 control_damper          {0x40};
    constexpr MidiStreamArray1 control_portamento_on_off{0x41};
    constexpr MidiStreamArray1 control_sostenuto       {0x42};
    constexpr MidiStreamArray1 control_softpedal       {0x43};
    constexpr MidiStreamAtom   control_pedalthreshold  {0x40};
    constexpr MidiStreamArray1 control_legato_foot     {0x44};
    constexpr MidiStreamArray1 control_hold_2           {0x45};
    constexpr MidiStreamArray1 control_sound_variation {0x46};
    constexpr MidiStreamArray1 control_timbre_intensity{0x47};
    constexpr MidiStreamArray1 control_release_time    {0x48};
    constexpr MidiStreamArray1 control_attack_time     {0x49};
    constexpr MidiStreamArray1 control_brightness      {0x4a};
    constexpr MidiStreamArray1 control_decay_time      {0x4b};
    constexpr MidiStreamArray1 control_vibrato_rate    {0x4c};
    constexpr MidiStreamArray1 control_vibrato_depth   {0x4d};
    constexpr MidiStreamArray1 control_sound_controller_9 {0x4e};
    constexpr MidiStreamArray1 control_sound_controller_10 {0x4f};

    constexpr MidiStreamArray1 control_general_purpose_5{0x50};
    constexpr MidiStreamArray1 control_general_purpose_6{0x51};
    constexpr MidiStreamArray1 control_general_purpose_7{0x52};
    constexpr MidiStreamArray1 control_general_purpose_8{0x53};
    constexpr MidiStreamArray1 control_portamento       {0x54};
    constexpr MidiStreamArray1 control_hires_velocity_msb{0x58};

    constexpr MidiStreamArray1 control_reverb_send_level {0x5b};
    constexpr MidiStreamArray1 control_tremolo_depth  {0x5c};
    constexpr MidiStreamArray1 control_chorus_send_level {0x5d};
    constexpr MidiStreamArray1 control_celeste_depth  {0x5e};
    constexpr MidiStreamArray1 control_phaser_depth   {0x5f};
    constexpr MidiStreamArray1 control_data_increment {0x60};
    constexpr MidiStreamArray1 control_data_decrement {0x61};
    constexpr MidiStreamArray1 control_non_registered_parameter_lsb {0x62};
    constexpr MidiStreamArray1 control_non_registered_parameter_msb {0x63};
    constexpr MidiStreamArray1 control_registered_parameter_lsb {0x64};
    constexpr MidiStreamArray1 control_registered_parameter_msb {0x65};

    // Reserved by MIDI Spec:
    constexpr MidiStreamArray1 control_all_sound_off  {0x78};
    constexpr MidiStreamArray1 control_reset_all_controllers {0x79};
    constexpr MidiStreamArray1 control_local_control{0x7a};
    constexpr MidiStreamAtom   local_control_off{0x00};
    constexpr MidiStreamAtom   local_control_on{0x7f};
    constexpr MidiStreamArray1 control_all_notes_off  {0x7b};
    constexpr MidiStreamArray1 control_omni_off       {0x7c};
    constexpr MidiStreamArray1 control_omni_on        {0x7d};
    constexpr MidiStreamArray1 control_mono_on        {0x7e};
    constexpr MidiStreamArray1 control_poly_on        {0x7f};
    constexpr MidiStreamArray1 control_full           {0x7F};
    constexpr MidiStreamArray1 control_off            {0x00};
    constexpr MidiStreamArray1 midi_time_code_quarter_frame{0xf1};
    constexpr MidiStreamArray1 song_position_pointer  {0xf2};
    constexpr MidiStreamArray1 song_select            {0xf3};
    constexpr MidiStreamArray1 tune_request           {0xf6};
    constexpr MidiStreamArray1 start_of_sysex         {0xf0};
    constexpr MidiStreamArray1 end_of_sysex           {0xf7};

    // MIDI 1.0 Detailed Specification M1_v4-2-1_MIDI_1-0_Detailed_Specification_96-1-4.pdf
    // page 35, "Device ID" 0x7f is "All Call" or, on page 57, "Broadcast".
    constexpr MidiStreamArray1 sysex_deviceid_all_call {0x7f};
    constexpr MidiStreamArray1 sysex_subid_non_commercial {0x7d}; //  "NON_COMMERCIAL"
    constexpr MidiStreamArray1 sysex_subid_non_realtime {0x7e}; //  "NON_REALTIME"
    constexpr MidiStreamArray1 sysex_subid_realtime {0x7f}; //  "REAL_TIME"
    constexpr MidiStreamArray1   sysex_subid_nonrt_sample_dump_header {0x01}; //  "NONRT_SAMPLE_DUMP_HEADER"
    constexpr MidiStreamArray1   sysex_subid_nonrt_sample_data_packet {0x02}; //  "NONRT_SAMPLE_DATA_PACKET"
    constexpr MidiStreamArray1   sysex_subid_nonrt_sample_dump_request {0x03}; //  "NONRT_SAMPLE_DUMP_REQUEST"

    constexpr MidiStreamArray1   sysex_subid_nonrt_timecode{0x04}; //  "NONRT_TIMECODE"
    constexpr MidiStreamArray1     sysex_subid_timecode_special {0x00}; //  "SPECIAL"
    constexpr MidiStreamArray1     sysex_subid_timecode_punch_in_points {0x01}; //  "TIMECODE_PUNCH_IN_PTS"
    constexpr MidiStreamArray1     sysex_subid_timecode_punch_out_points {0x02}; //  "TIMECODE_PUNCH_OUT_PTS"
    constexpr MidiStreamArray1     sysex_subid_timecode_delete_punch_in_point {0x03}; //  "TIMECODE_DELETE_PUNCH_IN_POINT"
    constexpr MidiStreamArray1     sysex_subid_timecode_delete_punch_out_point {0x04}; //  "TIMECODE_DELETE_PUNCH_OUT_POINT"
    constexpr MidiStreamArray1     sysex_subid_timecode_event_start_point {0x05}; //  "TIMECODE_EVENT_START_POINT"
    constexpr MidiStreamArray1     sysex_subid_timecode_event_stop_point {0x06}; //  "TIMECODE_EVENT_STOP_POINT"
    constexpr MidiStreamArray1     sysex_subid_timecode_event_start_points_with_additional_info {0x07}; //  "TIMECODE_EVENT_START_PTS_WITH_ADDITIONAL_INFO"
    constexpr MidiStreamArray1     sysex_subid_timecode_event_stop_points_with_additional_info {0x08}; //  "TIMECODE_EVENT_STOP_PTS_WITH_ADDITIONAL_INFO"
    constexpr MidiStreamArray1     sysex_subid_timecode_delete_event_start_point {0x09}; //  "TIMECODE_DELETE_EVENT_START_POINT"
    constexpr MidiStreamArray1     sysex_subid_timecode_delete_event_stop_point {0x0a}; //  "TIMECODE_DELETE_EVENT_STOP_POINT"
    constexpr MidiStreamArray1     sysex_subid_timecode_cue_points {0x0b}; //  "TIMECODE_CUE_PTS"
    constexpr MidiStreamArray1     sysex_subid_timecode_cue_points_with_additional_info {0x0c}; //  "TIMECODE_CUE_PTS_WITH_ADDITIONAL_INFO"
    constexpr MidiStreamArray1     sysex_subid_timecode_delete_cue_point {0x0d}; //  "TIMECODE_DELETE_CUE_POINT"
    constexpr MidiStreamArray1     sysex_subid_timecode_event_name_in_additional_info {0x0e}; //  "TIMECODE_EVENT_NAME_IN_ADDITIONAL_INFO"

    constexpr MidiStreamArray1   sysex_subid_nonrt_sample_dump_extensions{0x05}; //  "NONRT_SAMPLE_DUMP_EXTENSIONS"
    constexpr MidiStreamArray1     sysex_subid_nonrt_sample_dump_extensions_multiple_loop_points{0x01}; //  "NONRT_SAMPLE_DUMP_EXTENSIONS_MULTIPLE_LOOP_PTS"
    constexpr MidiStreamArray1     sysex_subid_nonrt_sample_dump_extensions_loop_points_request{0x02}; //  "NONRT_SAMPLE_DUMP_EXTENSIONS_LOOP_PTS_REQUEST"

    constexpr MidiStreamArray1   sysex_subid_nonrt_general_info{0x06}; //  "NONRT_GENERAL_INFO"
    constexpr MidiStreamArray1     sysex_subid_nonrt_general_info_id_request{0x01}; //  "NONRT_GENERAL_INFO_ID_REQUEST"
    constexpr MidiStreamArray1     sysex_subid_nonrt_general_info_id_reply{0x02}; //  "NONRT_GENERAL_INFO_ID_REPLY"

    constexpr MidiStreamArray1   sysex_subid_nonrt_file_dump{0x07}; //  "NONRT_FILE_DUMP"
    constexpr MidiStreamArray1     sysex_subid_nonrt_file_dump_header{0x01}; //  "NONRT_FILE_DUMP_HEADER"
    constexpr MidiStreamArray1     sysex_subid_nonrt_file_dump_data_packet{0x02}; //  "NONRT_FILE_DUMP_DATA_PACKET"
    constexpr MidiStreamArray1     sysex_subid_nonrt_file_dump_data_request{0x03}; //  "NONRT_FILE_DUMP_DATA_REQUEST"

    constexpr MidiStreamArray1   sysex_subid_nonrt_tuning_std{0x08}; //  "NONRT_TUNING_STD"
    constexpr MidiStreamArray1     sysex_subid_nonrt_tuning_std_bulk_dump_request{0x01}; //  "NONRT_TUNING_STD_BULK_DUMP_REQUEST"
    constexpr MidiStreamArray1     sysex_subid_nonrt_tuning_std_bulk_dump_reply{0x02}; //  "NONRT_TUNING_STD_BULK_DUMP_REPLY"

    constexpr MidiStreamArray1   sysex_subid_nonrt_gm{0x09}; //  "NONRT_GM"
    constexpr MidiStreamArray1     sysex_subid_nonrt_gm_on{0x01}; //  "NONRT_GM_ON"
    constexpr MidiStreamArray1     sysex_subid_nonrt_gm_off{0x02}; //  "NONRT_GM_OFF"

    constexpr MidiStreamArray1   sysex_subid_nonrt_end_of_file{0x7b}; //  "NONRT_END_OF_FILE"
    constexpr MidiStreamArray1   sysex_subid_nonrt_wait{0x7c}; //  "NONRT_WAIT"
    constexpr MidiStreamArray1   sysex_subid_nonrt_cancel{0x7d}; //  "NONRT_CANCEL"
    constexpr MidiStreamArray1   sysex_subid_nonrt_nak{0x7e}; //  "NONRT_NAK"
    constexpr MidiStreamArray1   sysex_subid_nonrt_ack{0x7f}; //  "NONRT_ACK"

    constexpr MidiStreamArray1   sysex_subid_rt_timecode{0x01}; //   "RT_TIMECODE"
    constexpr MidiStreamArray1     sysex_subid_rt_timecode_full_message {0x01}; //   "RT_TIMECODE_FULL_MESSAGE"
    constexpr MidiStreamArray1     sysex_subid_rt_timecode_user_bits{0x02}; //  "RT_TIMECODE_USER_BITS"

    constexpr MidiStreamArray1   sysex_subid_rt_show_control{0x02}; //  "RT_SHOW_CONTROL"
    constexpr MidiStreamArray1     sysex_subid_rt_show_control_extensions {0x00}; //  "RT_SHOW_CONTROL_EXTENSIONS"

    constexpr MidiStreamArray1   sysex_subid_rt_notation_information{0x03}; //  "RT_NOTATION_INFORMATION"
    constexpr MidiStreamArray1     sysex_subid_rt_notation_information_bar_number{0x01}; //  "RT_NOTATION_INFORMATION_BAR_NUMBER"
    constexpr MidiStreamArray1     sysex_subid_rt_notation_information_time_signature_immediate{0x02}; //  "RT_NOTATION_INFORMATION_TIME_SIGNATURE_IMMEDIATE"
    constexpr MidiStreamArray1     sysex_subid_rt_notation_information_time_signature_delayed{0x42}; //  "RT_NOTATION_INFORMATION_TIME_SIGNATURE_DELAYED"

    constexpr MidiStreamArray1   sysex_subid_rt_device_control{0x04}; //  "RT_DEVICE_CONTROL"
    constexpr MidiStreamArray1     sysex_subid_rt_device_control_main_volume{0x01}; //  "RT_DEVICE_CONTROL_MAIN_VOLUME"
    constexpr MidiStreamArray1     sysex_subid_rt_device_control_main_balance{0x02}; //  "RT_DEVICE_CONTROL_MAIN_BALANCE"

    constexpr MidiStreamArray1   sysex_subid_rt_mtc_cueing{0x05}; //  "RT_MTC_CUEING"
    // Use non-RT timecode commands except delete punch, delete event, delete cue point.

    constexpr MidiStreamArray1   sysex_subid_rt_machine_control_commands{0x06}; //  "RT_MACHINE_CONTROL_COMMANDS"
    constexpr MidiStreamArray1   sysex_subid_rt_machine_control_responses{0x07}; //  "RT_MACHINE_CONTROL_RESPONSES"
    constexpr MidiStreamArray1   sysex_subid_rt_tuning_std{0x08}; //  "RT_TUNING_STD"
    constexpr MidiStreamArray1     sysex_subid_rt_tuning_std_note_change{0x02}; //  "RT_TUNING_STD"

    constexpr MidiStreamAtom MiddleC{60};

    constexpr int SMPTE_hours_max{23};

    constexpr std::int64_t QuartersPerWhole(4);
    constexpr size_t bits_per_byte{8};
    constexpr auto ChunkNameLen{4};
    constexpr auto HeaderChunkLen{6};
    constexpr MidiStreamAtom MidiHeaderChunkName[ChunkNameLen]{'M', 'T', 'h', 'd'};

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
        MidiHeader()
          : chunk_name_{},
            chunk_len_{HeaderChunkLen},
            format_{},
            ntrks_{},
            division_{}
            {
                std::copy(&MidiHeaderChunkName[0],
                    &MidiHeaderChunkName[sizeof MidiHeaderChunkName],
                    chunk_name_);
            }

        MidiHeader(std::uint32_t chunk_len, MIDI_Format format, std::uint16_t ntrks,
                   std::uint16_t division)
          : chunk_name_{'M', 'T', 'h', 'd'},
            chunk_len_{chunk_len},
            format_{format},
            ntrks_{ntrks},
            division_{division}
            {}

        MidiHeader(MidiStreamAtom* bytes)
          : chunk_name_{},
            chunk_len_{},
            format_{},
            ntrks_{},
            division_{}
        {
            std::memcpy(this, bytes, sizeof(MidiHeader));
            swapbytes();
        }

        void to_bytes(MidiStreamAtom* bytes);
#pragma pack(push)
#pragma pack(1)
        MidiStreamAtom chunk_name_[ChunkNameLen];
        std::uint32_t chunk_len_; // big-endian
        // 0: single multichannel track;
        // 1: many tracks;
        // 2: multiple independent tracks
        MIDI_Format format_;
        std::uint16_t ntrks_;     // number of tracks
        std::uint16_t division_;  // ticks/quarter (unless SMPTE, check spec)
#pragma pack(pop)
        void swapbytes();
    };

#pragma pack()

    class RunningStatus
    {
      public:
        enum class RunningStatusPolicy : int
        {
            WhenCleared = 1,
            Never
        };
        RunningStatus()
          : running_status_valid_(),
            running_status_value_{},
            policy_{RunningStatusPolicy::WhenCleared}
        {
        }
        explicit RunningStatus(const RunningStatus& ) = default;
        void policy(RunningStatusPolicy policy);
        RunningStatus& operator=(const RunningStatus& ) = default;
        void running_status(midi::MidiStreamAtom running_status_value);
        void clear();
        void operator()(MidiStreamAtom status_byte, MidiStreamVector& track);
        bool running_status_valid() const;
        MidiStreamAtom running_status_value() const;
        // returns a zero-based channel
        MidiStreamAtom channel() const;
        MidiStreamAtom command() const;
      private:
        bool running_status_valid_;
        MidiStreamAtom running_status_value_;
        RunningStatusPolicy policy_;
    };

}
#endif // MIDI_H
