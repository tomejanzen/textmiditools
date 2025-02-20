//
// TextMIDITools Version 1.0.89
//
// textmidi
// Copyright © 2024 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <utility>
#include <memory>
#include <filesystem>

#include <boost/program_options.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/version.hpp>

#include "DynamicsOptions.h"

using namespace std;
using namespace boost;
using namespace textmidi;

midi::NumStringMap<int> textmidi::read_dynamics_configuration(const string& dynamics_configuration_file)
{
    midi::NumStringMap<int> option_map
      {{{  5, "ppppp"},
        { 10, "pppp"},
        { 25, "ppp"},
        { 40, "pp"},
        { 50, "p"},
        { 62, "mp"},
        { 75, "mf"},
        { 90, "forte"},
        {110, "ff"},
        {120, "fff"},
        {127, "ffff"},
        {127, "fffff"}}};
    if (!dynamics_configuration_file.empty())
    {
        program_options::options_description desc("Allowed dynamics configuration options");

        desc.add_options()
         (ppppp_option.registered_name().c_str(), program_options::value<int>(), ppppp_option.text().c_str())
         ( pppp_option.registered_name().c_str(), program_options::value<int>(), pppp_option.text().c_str())
         (  ppp_option.registered_name().c_str(), program_options::value<int>(), ppp_option.text().c_str())
         (   pp_option.registered_name().c_str(), program_options::value<int>(), pp_option.text().c_str())
         (    p_option.registered_name().c_str(), program_options::value<int>(), p_option.text().c_str())
         (   mp_option.registered_name().c_str(), program_options::value<int>(), mp_option.text().c_str())
         (   mf_option.registered_name().c_str(), program_options::value<int>(), mf_option.text().c_str())
         (forte_option.registered_name().c_str(), program_options::value<int>(), forte_option.text().c_str())
         (   ff_option.registered_name().c_str(), program_options::value<int>(), ff_option.text().c_str())
         (  fff_option.registered_name().c_str(), program_options::value<int>(), fff_option.text().c_str())
         ( ffff_option.registered_name().c_str(), program_options::value<int>(), ffff_option.text().c_str())
         (fffff_option.registered_name().c_str(), program_options::value<int>(), fffff_option.text().c_str())
          ;
        program_options::positional_options_description pos_opts_desc;
        program_options::variables_map var_map;
        program_options::store(
            program_options::parse_config_file(dynamics_configuration_file.c_str(), desc), var_map);
        program_options::notify(var_map);

        if (var_map.count(ppppp_option.option()))
        {
            option_map.emplace(ppppp_option.option(), var_map[ppppp_option.option()].as<int>());
        }
        if (var_map.count(pppp_option.option()))
        {
            option_map.emplace(pppp_option.option(), var_map[pppp_option.option()].as<int>());
        }
        if (var_map.count(ppp_option.option()))
        {
            option_map.emplace(ppp_option.option(), var_map[ppp_option.option()].as<int>());
        }
        if (var_map.count(pp_option.option()))
        {
            option_map.emplace(pp_option.option(), var_map[pp_option.option()].as<int>());
        }
        if (var_map.count(p_option.option()))
        {
            option_map.emplace(p_option.option(), var_map[p_option.option()].as<int>());
        }
        if (var_map.count(mp_option.option()))
        {
            option_map.emplace(mp_option.option(), var_map[mp_option.option()].as<int>());
        }
        if (var_map.count(mf_option.option()))
        {
            option_map.emplace(mf_option.option(), var_map[mf_option.option()].as<int>());
        }
        if (var_map.count(forte_option.option()))
        {
            option_map.emplace(forte_option.option(), var_map[forte_option.option()].as<int>());
        }
        if (var_map.count(ff_option.option()))
        {
            option_map.emplace(ff_option.option(), var_map[ff_option.option()].as<int>());
        }
        if (var_map.count(fff_option.option()))
        {
            option_map.emplace(fff_option.option(), var_map[fff_option.option()].as<int>());
        }
        if (var_map.count(ffff_option.option()))
        {
            option_map.emplace(ffff_option.option(), var_map[ffff_option.option()].as<int>());
        }
        if (var_map.count(fffff_option.option()))
        {
            option_map.emplace(fffff_option.option(), var_map[fffff_option.option()].as<int>());
        }
    }
    return option_map;
}

