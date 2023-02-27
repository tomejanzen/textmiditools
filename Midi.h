//
// TextMIDITools Version 1.0.42
//
// textmidi 1.0.6
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

namespace textmidi
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
    const MidiStreamArray1 meta_prefix{0xff};
    const MidiStreamArray2 sequence_number_prefix {0x00, 0x02}; // sequence number
    // Variable-length string meta-events.
    const MidiStreamArray1 text_prefix            {0x01};
    const MidiStreamArray1 copyright_prefix       {0x02};
    const MidiStreamArray1 track_name_prefix      {0x03};
    const MidiStreamArray1 instrument_name_prefix {0x04};
    const MidiStreamArray1 lyric_prefix           {0x05};
    const MidiStreamArray1 marker_prefix          {0x06};
    const MidiStreamArray1 cue_point_prefix       {0x07};
    const MidiStreamArray1 text_08_prefix         {0x08};
    const MidiStreamArray1 text_09_prefix         {0x09};
    const MidiStreamArray1 text_0A_prefix         {0x0A};
    const MidiStreamArray1 text_0B_prefix         {0x0B};
    const MidiStreamArray1 text_0C_prefix         {0x0C};
    const MidiStreamArray1 text_0D_prefix         {0x0D};
    const MidiStreamArray1 text_0E_prefix         {0x0E};
    const MidiStreamArray1 text_0F_prefix         {0x0F};
    const MidiStreamArray1 unknown_prefix         {0x11};
    const MidiStreamArray2 smpte_prefix           {0x54, 5}; // prefix, length
    const MidiStreamArray2 midi_channel_prefix    {0x20, 1}; // prefix, length
    const MidiStreamArray2 end_of_track_prefix    {0x2f, 0}; // prefix, length
    const MidiStreamArray2 tempo_prefix           {0x51, 3}; // prefix, length
    const MidiStreamArray2 time_signature_prefix  {0x58, 4}; // prefix, length
    const MidiStreamArray2 key_signature_prefix   {0x59, 2}; // prefix, length
    // Quicktime left out the major/minor mode byte.
    const MidiStreamArray2 key_signature_prefix_mode_missing   {0x59, 1}; // prefix, length;
    const MidiStreamArray2 midi_port_prefix       {0x21, 1};
    const MidiStreamArray1 sequencer_specific_prefix{0x7f};

    // To recognize what is an unknown meta event, first check if it is in this list.
    const std::set<MidiStreamAtom> Initial_Meta{sequence_number_prefix[0], text_prefix[0],
        copyright_prefix[0], track_name_prefix[0], instrument_name_prefix[0],
        lyric_prefix[0], marker_prefix[0], cue_point_prefix[0], text_08_prefix[0],
        text_09_prefix[0], text_0A_prefix[0], text_0B_prefix[0], text_0C_prefix[0],
        text_0D_prefix[0], text_0E_prefix[0], text_0F_prefix[0], midi_channel_prefix[0],
        end_of_track_prefix[0], tempo_prefix[0], smpte_prefix[0], time_signature_prefix[0],
        key_signature_prefix[0], sequencer_specific_prefix[0], midi_port_prefix[0]};

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

    const MidiStreamArray1 note_on                {NoteOn};
    const MidiStreamArray1 note_off               {NoteOff};
    constexpr int          full_note_length       {3};
    constexpr int          running_status_note_length{2};
    const MidiStreamArray1 polyphonic_key_pressure{PolyphonicKeyPressure};
    const MidiStreamArray1 control                {Control};
    const MidiStreamArray1 channel_mode           {0xb0};
    const MidiStreamArray1 program                {Program};
    const MidiStreamArray1 channel_pressure       {ChannelPressure};
    const MidiStreamArray1 pitch_wheel            {PitchWheel};
    constexpr MidiStreamAtom channel_mask         {0x0F};
    constexpr MidiStreamAtom byte7_mask           {0x7F};
    constexpr MidiStreamAtom byte7_shift          {7};
    constexpr MidiStreamAtom nybble_mask          {0x0F};
    const MidiStreamArray1 control_pan            {0x0A};
    const MidiStreamArray1 control_damper         {0x40};
    const MidiStreamArray1 control_portamento     {0x41};
    const MidiStreamArray1 control_sostenuto      {0x42};
    const MidiStreamArray1 control_softpedal      {0x43};
    const MidiStreamArray1 control_hold2          {0x45};
    const MidiStreamArray1 control_generalpurpose5{0x50};
    const MidiStreamArray1 control_generalpurpose6{0x51};
    const MidiStreamArray1 control_generalpurpose7{0x52};
    const MidiStreamArray1 control_generalpurpose8{0x53};
    const MidiStreamArray1 control_extern_fx_depth{0x5b};
    const MidiStreamArray1 control_tremolo_depth  {0x5c};
    const MidiStreamArray1 control_chorus_depth   {0x5d};
    const MidiStreamArray1 control_celeste_depth  {0x5e};
    const MidiStreamArray1 control_phaser_depth   {0x5f};
    const MidiStreamArray1 control_data_incr      {0x60};
    const MidiStreamArray1 control_data_decr      {0x61};
    const MidiStreamArray1 control_nonregparmlsb  {0x62};
    const MidiStreamArray1 control_nonregparmmsb  {0x63};
    const MidiStreamArray1 control_regparmlsb     {0x64};
    const MidiStreamArray1 control_regparmmsb     {0x65};
    const MidiStreamArray1 control_all_sound_off  {0x78};
    const MidiStreamArray1 control_reset_all_ctrl {0x79};
    constexpr MidiStreamAtom control_local_ctrl_on_off{0x7a};
    const MidiStreamArray1 control_all_notes_off  {0x7b};
    const MidiStreamArray1 control_omni_off       {0x7c};
    const MidiStreamArray1 control_onmi_on        {0x7d};
    const MidiStreamArray1 control_mono_on        {0x7e};
    const MidiStreamArray1 control_poly_on        {0x7f};
    const MidiStreamArray1 control_full           {0x7F};
    const MidiStreamArray1 control_off            {0x00};
    const MidiStreamArray1 control_breath         {2};
    const MidiStreamArray1 midi_time_code_quarter_frame{0xf1};
    const MidiStreamArray1 song_position_pointer  {0xf2};
    const MidiStreamArray1 song_select            {0xf3};
    const MidiStreamArray1 tune_request           {0xf6};
    const MidiStreamArray1 start_of_sysex         {0xf0};
    const MidiStreamArray1 end_of_sysex           {0xf7};

    constexpr MidiStreamAtom MiddleC{60};

    const int SMPTE_hours_max{23};

    using SmpteFpsMap = std::map<int, std::string>;
    const SmpteFpsMap smpte_fps_map
    {
        {0, "smpte_24fps"},
        {1, "smpte_25fps"},
        {2, "smpte_30fpsdropframe"},
        {3, "smpte_30fpsnondropframe"}
    };

    using SmpteFpsReverseMap = std::map<std::string, int>;
    const SmpteFpsReverseMap smpte_fps_reverse_map
    {
        {"smpte_24fps", 0},
        {"smpte_25fps", 1},
        {"smpte_30fpsdropframe", 2},
        {"smpte_30fpsnondropframe", 3}
    };

    using DynamicsReverseMap = std::map<int, std::string>;
    const DynamicsReverseMap dynamics_reverse_map
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
    using DynamicsMap = std::map<std::string, int>;
    const DynamicsMap dynamics_map
    {
        {"pppp",  10},
        {"ppp",   25},
        {"pp",    40},
        {"p",     50},
        {"mp",    62},
        {"mf",    75},
        {"forte", 90},
        {"ff",    110},
        {"fff",   120},
        {"ffff",  127}
    };
    constexpr std::int64_t QuartersPerWhole(4);

    constexpr size_t bits_per_byte{8};

    constexpr auto ChunkNameLen{4};
    constexpr auto HeaderChunkLen{6};
    constexpr MidiStreamAtom MidiHeaderChunkName[ChunkNameLen]{'M', 'T', 'h', 'd'};

    enum class MIDI_Format : std::uint16_t
    {
        MonoTrack,
        MultiTrack,
        MultiSequence
    };

    using FormatMap = std::map<std::string, MIDI_Format>;
    const FormatMap format_map
    {
        {"MONOTRACK", MIDI_Format::MonoTrack},
        {"MULTITRACK", MIDI_Format::MultiTrack},
        {"MULTISEQUENCE", MIDI_Format::MultiSequence}
    };

    std::ostream& operator<<(std::ostream& os, MIDI_Format mf);

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
            swap();
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
        void swap();
    };

#pragma pack()
    std::ostream& operator<<(std::ostream& os, const MidiHeader& mh);
}
#endif // MIDI_H
