//
// TextMIDITools Version 1.0.19
//
// textmidi 1.0.6
// Copyright © 2022 Thomas E. Janzen
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
#include <iostream>

namespace textmidi
{
    typedef std::vector<uint8_t> MidiStreamVector;
    constexpr int MidiPitchQty{128};
    constexpr int MidiChannelQty{16};
    constexpr int MidiIdiophoneChannel{10};
    constexpr std::int32_t PanExcess64{64};
    constexpr std::uint32_t variable_len_word_mask{0xfffffff};

    constexpr std::uint8_t event_flag               {0x80};
    constexpr std::uint8_t variable_len_flag        {0x80};
    constexpr std::uint8_t
        variable_len_byte_mask   {static_cast<std::uint8_t>(~variable_len_flag)};
    constexpr std::uint8_t variable_len_shift       {7};
    constexpr std::uint8_t meta_prefix              {0xff};
    constexpr std::uint8_t sequence_prefix          {}; // sequence name
    // Variable-length string meta-events.
    constexpr std::uint8_t text_prefix              {1};
    constexpr std::uint8_t copyright_prefix         {2};
    constexpr std::uint8_t track_name_prefix        {3};
    constexpr std::uint8_t instrument_name_prefix   {4};
    constexpr std::uint8_t lyric_prefix             {5};
    constexpr std::uint8_t marker_prefix            {6};
    constexpr std::uint8_t cue_point_prefix         {7};
    constexpr std::uint8_t unknown1_prefix[]        {0x11};
    // fixed-length meta-events
    constexpr std::uint8_t midi_channel_prefix[]    {0x20, 1}; // prefix, length
    constexpr std::uint8_t end_of_track_prefix[]    {0x2f, 0}; // prefix, length
    constexpr std::uint8_t tempo_prefix[]           {0x51, 3}; // prefix, length
    constexpr std::uint8_t smpte_prefix[]           {0x54, 5}; // prefix, length
    constexpr std::uint8_t time_signature_prefix[]  {0x58, 4}; // prefix, length
    constexpr std::uint8_t key_signature_prefix[]   {0x59, 2}; // prefix, length
    constexpr std::uint8_t sequence_specific_prefix[]{0x7f};
    constexpr std::uint8_t note_on                  {0x90};
    constexpr std::uint8_t note_off                 {0x80};
    constexpr std::uint8_t poly_key_pressure        {0xa0};
    constexpr std::uint8_t control                  {0xb0};
    constexpr std::uint8_t channel_mode             {0xb0};
    constexpr std::uint8_t program                  {0xc0};
    constexpr std::uint8_t channel_pressure         {0xd0};
    constexpr std::uint8_t pitch_wheel              {0xe0};
    constexpr std::uint8_t channel_mask             {0x0F};
    constexpr std::uint8_t byte7_mask               {0x7F};
    constexpr std::uint8_t nybble_mask              {0x0F};
    constexpr std::uint8_t control_pan              {0x0A};
    constexpr std::uint8_t control_damper           {0x40};
    constexpr std::uint8_t control_portamento       {0x41};
    constexpr std::uint8_t control_sostenuto        {0x42};
    constexpr std::uint8_t control_softpedal        {0x43};
    constexpr std::uint8_t control_hold2            {0x45};
    constexpr std::uint8_t control_generalpurpose5  {0x50};
    constexpr std::uint8_t control_generalpurpose6  {0x51};
    constexpr std::uint8_t control_generalpurpose7  {0x52};
    constexpr std::uint8_t control_generalpurpose8  {0x53};
    constexpr std::uint8_t control_extern_fx_depth  {0x5b};
    constexpr std::uint8_t control_tremolo_depth    {0x5c};
    constexpr std::uint8_t control_chorus_depth     {0x5d};
    constexpr std::uint8_t control_celeste_depth    {0x5e};
    constexpr std::uint8_t control_phaser_depth     {0x5f};
    constexpr std::uint8_t control_data_incr        {0x60};
    constexpr std::uint8_t control_data_decr        {0x61};
    constexpr std::uint8_t control_nonregparmlsb    {0x62};
    constexpr std::uint8_t control_nonregparmmsb    {0x63};
    constexpr std::uint8_t control_regparmlsb       {0x64};
    constexpr std::uint8_t control_regparmmsb       {0x65};
    constexpr std::uint8_t control_all_sound_off    {0x78};
    constexpr std::uint8_t control_reset_all_ctrl   {0x79};
    constexpr std::uint8_t control_local_ctrl_on_off{0x7a};
    constexpr std::uint8_t control_all_notes_off    {0x7b};
    constexpr std::uint8_t control_omni_off         {0x7c};
    constexpr std::uint8_t control_onmi_on          {0x7d};
    constexpr std::uint8_t control_mono_on          {0x7e};
    constexpr std::uint8_t control_poly_on          {0x7f};
    constexpr std::uint8_t control_full             {0x7F};
    constexpr std::uint8_t control_off              {};
    constexpr std::uint8_t midi_port_prefix[]       {0x21, 1};
    constexpr std::uint8_t control_breath           {2};
    constexpr std::uint8_t midi_time_code_quarter_frame {0xf1};
    constexpr std::uint8_t song_position_pointer    {0xf2};
    constexpr std::uint8_t song_select              {0xf3};
    constexpr std::uint8_t tune_request             {0xf6};
    constexpr std::uint8_t start_of_sysex           {0xf0};
    constexpr std::uint8_t end_of_sysex             {0xf7};
    constexpr std::uint8_t yamaha                   {0x43};

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
        std::uint8_t chunk_name_[ChunkNameLen];
        std::uint32_t chunk_len_; // big-endian
        // 0: single multichannel track;
        // 1: many tracks;
        // 2: multiple independent tracks
        MIDI_Format format_;
        std::uint16_t ntrks_;     // number of tracks
        std::uint16_t division_;  // ticks/quarter (unless SMPTE, check spec)
        void swap();
    };

#pragma pack()
    // Stroustrup p 388 Programming
    template<class T>
    char* io_bytes(T& i)
    {
        void* addr = &i;
        return static_cast<char*>(addr);
    }

    std::ostream& operator<<(std::ostream& os, const MidiHeader& mh);

}
#endif // MIDI_H
