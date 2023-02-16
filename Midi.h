//
// TextMIDITools Version 1.0.37
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
    //
    // Holds binary standard MIDI file format data.
    typedef std::uint8_t MidiStreamAtom;
    typedef std::vector<MidiStreamAtom> MidiStreamVector;
    typedef MidiStreamVector::iterator MidiStreamIterator;
    typedef MidiStreamVector::const_iterator MidiStreamConstIterator;

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
    const MidiStreamVector meta_prefix{0xff};
    const MidiStreamVector sequence_number_prefix {0x00, 0x02}; // sequence number
    // Variable-length string meta-events.
    const MidiStreamVector text_prefix            {0x01};
    const MidiStreamVector copyright_prefix       {0x02};
    const MidiStreamVector track_name_prefix      {0x03};
    const MidiStreamVector instrument_name_prefix {0x04};
    const MidiStreamVector lyric_prefix           {0x05};
    const MidiStreamVector marker_prefix          {0x06};
    const MidiStreamVector cue_point_prefix       {0x07};
    const MidiStreamVector text_08_prefix         {0x08};
    const MidiStreamVector text_09_prefix         {0x09};
    const MidiStreamVector text_0A_prefix         {0x0A};
    const MidiStreamVector text_0B_prefix         {0x0B};
    const MidiStreamVector text_0C_prefix         {0x0C};
    const MidiStreamVector text_0D_prefix         {0x0D};
    const MidiStreamVector text_0E_prefix         {0x0E};
    const MidiStreamVector text_0F_prefix         {0x0F};
    const MidiStreamVector unknown_prefix         {0x11};
    const MidiStreamVector smpte_prefix           {0x54, 5}; // prefix, length
    const MidiStreamVector midi_channel_prefix    {0x20, 1}; // prefix, length
    const MidiStreamVector end_of_track_prefix    {0x2f, 0}; // prefix, length
    const MidiStreamVector tempo_prefix           {0x51, 3}; // prefix, length
    const MidiStreamVector time_signature_prefix  {0x58, 4}; // prefix, length
    const MidiStreamVector key_signature_prefix   {0x59, 2}; // prefix, length
    const MidiStreamVector midi_port_prefix       {0x21, 1};
    const MidiStreamVector sequencer_specific_prefix{0x7f};

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

    const MidiStreamVector note_on                {NoteOn};
    const MidiStreamVector note_off               {NoteOff};
    constexpr int          full_note_length       {3};
    constexpr int          running_status_note_length{2};
    const MidiStreamVector polyphonic_key_pressure{PolyphonicKeyPressure};
    const MidiStreamVector control                {Control};
    const MidiStreamVector channel_mode           {0xb0};
    const MidiStreamVector program                {Program};
    const MidiStreamVector channel_pressure       {ChannelPressure};
    const MidiStreamVector pitch_wheel            {PitchWheel};
    constexpr MidiStreamAtom channel_mask         {0x0F};
    constexpr MidiStreamAtom byte7_mask           {0x7F};
    constexpr MidiStreamAtom byte7_shift          {7};
    constexpr MidiStreamAtom nybble_mask          {0x0F};
    const MidiStreamVector control_pan            {0x0A};
    const MidiStreamVector control_damper         {0x40};
    const MidiStreamVector control_portamento     {0x41};
    const MidiStreamVector control_sostenuto      {0x42};
    const MidiStreamVector control_softpedal      {0x43};
    const MidiStreamVector control_hold2          {0x45};
    const MidiStreamVector control_generalpurpose5{0x50};
    const MidiStreamVector control_generalpurpose6{0x51};
    const MidiStreamVector control_generalpurpose7{0x52};
    const MidiStreamVector control_generalpurpose8{0x53};
    const MidiStreamVector control_extern_fx_depth{0x5b};
    const MidiStreamVector control_tremolo_depth  {0x5c};
    const MidiStreamVector control_chorus_depth   {0x5d};
    const MidiStreamVector control_celeste_depth  {0x5e};
    const MidiStreamVector control_phaser_depth   {0x5f};
    const MidiStreamVector control_data_incr      {0x60};
    const MidiStreamVector control_data_decr      {0x61};
    const MidiStreamVector control_nonregparmlsb  {0x62};
    const MidiStreamVector control_nonregparmmsb  {0x63};
    const MidiStreamVector control_regparmlsb     {0x64};
    const MidiStreamVector control_regparmmsb     {0x65};
    const MidiStreamVector control_all_sound_off  {0x78};
    const MidiStreamVector control_reset_all_ctrl {0x79};
    constexpr MidiStreamAtom control_local_ctrl_on_off{0x7a};
    const MidiStreamVector control_all_notes_off  {0x7b};
    const MidiStreamVector control_omni_off       {0x7c};
    const MidiStreamVector control_onmi_on        {0x7d};
    const MidiStreamVector control_mono_on        {0x7e};
    const MidiStreamVector control_poly_on        {0x7f};
    const MidiStreamVector control_full           {0x7F};
    const MidiStreamVector control_off            {0x00};
    const MidiStreamVector control_breath         {2};
    const MidiStreamVector midi_time_code_quarter_frame{0xf1};
    const MidiStreamVector song_position_pointer  {0xf2};
    const MidiStreamVector song_select            {0xf3};
    const MidiStreamVector tune_request           {0xf6};
    const MidiStreamVector start_of_sysex         {0xf0};
    const MidiStreamVector end_of_sysex           {0xf7};

    constexpr MidiStreamAtom MiddleC{60};

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
    constexpr MidiStreamAtom MidiHeaderChunkName[ChunkNameLen]{'M', 'T', 'h', 'd'};

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
