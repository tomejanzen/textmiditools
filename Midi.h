//
// TextMIDITools Version 1.0.33
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

#include <vector>
#include <map>
#include <set>
#include <iostream>

namespace textmidi
{
    constexpr double MinDynamic{30};
    constexpr double MaxDynamic{127};

    constexpr int MidiPitchQty{128};
    constexpr int MidiChannelQty{16};
    constexpr int MidiIdiophoneChannel{10};
    constexpr std::int32_t PanExcess64{64};
    constexpr std::uint32_t variable_len_word_mask{0xfffffff};

    constexpr long int variable_length_quantity_max_len{sizeof(int)};
    constexpr long int variable_length_quantity_min_len{sizeof(int)};
    constexpr std::uint8_t event_flag               {0x80};
    constexpr std::uint8_t variable_len_flag        {0x80};
    constexpr std::uint8_t
        variable_len_byte_mask   {static_cast<std::uint8_t>(~variable_len_flag)};
    constexpr std::uint8_t variable_len_shift       {7};
    constexpr std::uint8_t octet_mask               {0xff};
    constexpr std::uint8_t meta_prefix[]            {0xff};
    constexpr std::uint8_t sequence_number_prefix[] {0x00, 0x02}; // sequence number
    // Variable-length string meta-events.
    constexpr std::uint8_t text_prefix[]            {0x01};
    constexpr std::uint8_t copyright_prefix[]       {0x02};
    constexpr std::uint8_t track_name_prefix[]      {0x03};
    constexpr std::uint8_t instrument_name_prefix[] {0x04};
    constexpr std::uint8_t lyric_prefix[]           {0x05};
    constexpr std::uint8_t marker_prefix[]          {0x06};
    constexpr std::uint8_t cue_point_prefix[]       {0x07};
    constexpr std::uint8_t text_08_prefix[]         {0x08};
    constexpr std::uint8_t text_09_prefix[]         {0x09};
    constexpr std::uint8_t text_0A_prefix[]         {0x0A};
    constexpr std::uint8_t text_0B_prefix[]         {0x0B};
    constexpr std::uint8_t text_0C_prefix[]         {0x0C};
    constexpr std::uint8_t text_0D_prefix[]         {0x0D};
    constexpr std::uint8_t text_0E_prefix[]         {0x0E};
    constexpr std::uint8_t text_0F_prefix[]         {0x0F};
    constexpr std::uint8_t unknown1_prefix[]        {0x11};

    const std::set<int> Initial_Meta{sequence_number_prefix[0], text_prefix[0], 
        copyright_prefix[0], track_name_prefix[0], instrument_name_prefix[0], 
        lyric_prefix[0], marker_prefix[0], cue_point_prefix[0], text_08_prefix[0],
        text_09_prefix[0], text_0A_prefix[0], text_0B_prefix[0], text_0C_prefix[0],
        text_0D_prefix[0], text_0E_prefix[0], text_0F_prefix[0]};

    // fixed-length meta-events
    constexpr std::uint8_t midi_channel_prefix[]    {0x20, 1}; // prefix, length
    constexpr std::uint8_t end_of_track_prefix[]    {0x2f, 0}; // prefix, length
    constexpr std::uint8_t tempo_prefix[]           {0x51, 3}; // prefix, length
    constexpr std::uint8_t smpte_prefix[]           {0x54, 5}; // prefix, length
    constexpr std::uint8_t smpte_24fps{0};
    constexpr std::uint8_t smpte_25fps{1};
    constexpr std::uint8_t smpte_30fpsdropframe{2};
    constexpr std::uint8_t smpte_30fpsnondropframe{3};
    constexpr std::uint8_t smpte_fps_shift{5};
    constexpr std::uint8_t smpte_fps_mask{3};
    constexpr std::uint8_t smpte_hours_mask{0x1f};
    constexpr std::uint8_t time_signature_prefix[]  {0x58, 4}; // prefix, length
    constexpr std::uint8_t key_signature_prefix[]   {0x59, 2}; // prefix, length
    constexpr std::uint8_t sequencer_specific_prefix[]{0x7f};
    constexpr std::uint8_t note_on[]                {0x90};
    constexpr std::uint8_t note_off[]               {0x80};
    constexpr int          full_note_length         {3};
    constexpr int          running_status_note_length{2};
    constexpr std::uint8_t polyphonic_key_pressure[]{0xa0};
    constexpr std::uint8_t control[]                {0xb0};
    constexpr std::uint8_t channel_mode[]           {0xb0};
    constexpr std::uint8_t program[]                {0xc0};
    constexpr std::uint8_t channel_pressure[]       {0xd0};
    constexpr std::uint8_t pitch_wheel[]            {0xe0};
    constexpr std::uint8_t channel_mask             {0x0F};
    constexpr std::uint8_t byte7_mask               {0x7F};
    constexpr std::uint8_t byte7_shift              {7};
    constexpr std::uint8_t nybble_mask              {0x0F};
    constexpr std::uint8_t control_pan[]            {0x0A};
    constexpr std::uint8_t control_damper[]         {0x40};
    constexpr std::uint8_t control_portamento[]     {0x41};
    constexpr std::uint8_t control_sostenuto[]      {0x42};
    constexpr std::uint8_t control_softpedal[]      {0x43};
    constexpr std::uint8_t control_hold2[]          {0x45};
    constexpr std::uint8_t control_generalpurpose5[]{0x50};
    constexpr std::uint8_t control_generalpurpose6[]{0x51};
    constexpr std::uint8_t control_generalpurpose7[]{0x52};
    constexpr std::uint8_t control_generalpurpose8[]{0x53};
    constexpr std::uint8_t control_extern_fx_depth[]{0x5b};
    constexpr std::uint8_t control_tremolo_depth[]  {0x5c};
    constexpr std::uint8_t control_chorus_depth[]   {0x5d};
    constexpr std::uint8_t control_celeste_depth[]  {0x5e};
    constexpr std::uint8_t control_phaser_depth[]   {0x5f};
    constexpr std::uint8_t control_data_incr[]      {0x60};
    constexpr std::uint8_t control_data_decr[]      {0x61};
    constexpr std::uint8_t control_nonregparmlsb[]  {0x62};
    constexpr std::uint8_t control_nonregparmmsb[]  {0x63};
    constexpr std::uint8_t control_regparmlsb[]     {0x64};
    constexpr std::uint8_t control_regparmmsb[]     {0x65};
    constexpr std::uint8_t control_all_sound_off[]  {0x78};
    constexpr std::uint8_t control_reset_all_ctrl[] {0x79};
    constexpr std::uint8_t control_local_ctrl_on_off{0x7a};
    constexpr std::uint8_t control_all_notes_off[]  {0x7b};
    constexpr std::uint8_t control_omni_off[]       {0x7c};
    constexpr std::uint8_t control_onmi_on[]        {0x7d};
    constexpr std::uint8_t control_mono_on[]        {0x7e};
    constexpr std::uint8_t control_poly_on[]        {0x7f};
    constexpr std::uint8_t control_full[]           {0x7F};
    constexpr std::uint8_t control_off[]            {};
    constexpr std::uint8_t midi_port_prefix[]       {0x21, 1};
    constexpr std::uint8_t control_breath[]         {2};
    constexpr std::uint8_t midi_time_code_quarter_frame[]{0xf1};
    constexpr std::uint8_t song_position_pointer[]  {0xf2};
    constexpr std::uint8_t song_select[]            {0xf3};
    constexpr std::uint8_t tune_request[]           {0xf6};
    constexpr std::uint8_t start_of_sysex[]         {0xf0};
    constexpr std::uint8_t end_of_sysex[]           {0xf7};
    constexpr std::uint8_t yamaha                   {0x43};

    const int SMPTE_hours_max{23};

    typedef std::map<int, std::string> SmpteFpsMap;
    const SmpteFpsMap smpte_fps_map
    {
        {0, "smpte_24fps"},
        {1, "smpte_25fps"},
        {2, "smpte_30fpsdropframe"},
        {3, "smpte_30fpsnondropframe"}
    };

    typedef std::map<std::string, int> SmpteFpsReverseMap;
    const SmpteFpsReverseMap smpte_fps_reverse_map
    {
        {"smpte_24fps", 0},
        {"smpte_25fps", 1},
        {"smpte_30fpsdropframe", 2},
        {"smpte_30fpsnondropframe", 3}
    };

    typedef std::map<int, std::string> DynamicsReverseMap;
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
    typedef std::map<std::string, int> DynamicsMap;
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
    constexpr std::uint8_t MidiHeaderChunkName[ChunkNameLen]{'M', 'T', 'h', 'd'};

    enum class MIDI_Format : std::uint16_t
    {
        MonoTrack,
        MultiTrack,
        MultiSequence
    };

    typedef std::map<std::string, MIDI_Format> FormatMap;
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

        MidiHeader(std::uint8_t* bytes)
          : chunk_name_{},
            chunk_len_{},
            format_{},
            ntrks_{},
            division_{}
        {
            std::memcpy(this, bytes, sizeof(MidiHeader));
            swap();
        }

        void to_bytes(std::uint8_t* bytes);
#pragma pack(push)
#pragma pack(1)
        std::uint8_t chunk_name_[ChunkNameLen];
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
