//
// TextMIDITools Version 1.0.98
//
// Copyright © 2025 Thomas E. Janzen
// License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
// This is free software: you are free to change and redistribute it.
// There is NO WARRANTY, to the extent permitted by law.
//
#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstdint>

#include <filesystem>
#include <memory>
#include <utility>

#include <boost/program_options.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/version.hpp>

#include "DynamicsOptions.h"

using std::int32_t, std::string;

midi::NumStringMap<int32_t> textmidi::read_dynamics_configuration(
    const string& dynamics_configuration_file)
{
    using boost::program_options::options_description,
    boost::program_options::notify, boost::program_options::store,
    boost::program_options::positional_options_description,
    boost::program_options::value, boost::program_options::variables_map;

    midi::NumStringMap<int32_t> option_map
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
        options_description
            desc("Allowed dynamics configuration options");

        desc.add_options()
         (ppppp_option.registered_name().c_str(),
             value<int32_t>(), ppppp_option.text().c_str())
         (pppp_option.registered_name().c_str(),
              value<int32_t>(), pppp_option.text().c_str())
         (ppp_option.registered_name().c_str(),
               value<int32_t>(), ppp_option.text().c_str())
         (pp_option.registered_name().c_str(),
                value<int32_t>(), pp_option.text().c_str())
         (p_option.registered_name().c_str(),
                 value<int32_t>(), p_option.text().c_str())
         (mp_option.registered_name().c_str(),
                value<int32_t>(), mp_option.text().c_str())
         (mf_option.registered_name().c_str(),
                value<int32_t>(), mf_option.text().c_str())
         (forte_option.registered_name().c_str(),
             value<int32_t>(), forte_option.text().c_str())
         (ff_option.registered_name().c_str(),
                value<int32_t>(), ff_option.text().c_str())
         (fff_option.registered_name().c_str(),
               value<int32_t>(), fff_option.text().c_str())
         (ffff_option.registered_name().c_str(),
              value<int32_t>(), ffff_option.text().c_str())
         (fffff_option.registered_name().c_str(),
             value<int32_t>(), fffff_option.text().c_str())
          ;
        positional_options_description pos_opts_desc;
        variables_map var_map;
        store(
            parse_config_file(
            dynamics_configuration_file.c_str(), desc), var_map);
        notify(var_map);

        if (var_map.count(ppppp_option.option()))
        {
            option_map.emplace(
                ppppp_option.option(),
                var_map[ppppp_option.option()].as<int32_t>());
        }
        if (var_map.count(pppp_option.option()))
        {
            option_map.emplace(
                pppp_option.option(),
                var_map[pppp_option.option()].as<int32_t>());
        }
        if (var_map.count(ppp_option.option()))
        {
            option_map.emplace(
                ppp_option.option(),
                var_map[ppp_option.option()].as<int32_t>());
        }
        if (var_map.count(pp_option.option()))
        {
            option_map.emplace(
                pp_option.option(), var_map[pp_option.option()].as<int32_t>());
        }
        if (var_map.count(p_option.option()))
        {
            option_map.emplace(p_option.option(),
                var_map[p_option.option()].as<int32_t>());
        }
        if (var_map.count(mp_option.option()))
        {
            option_map.emplace(mp_option.option(),
                var_map[mp_option.option()].as<int32_t>());
        }
        if (var_map.count(mf_option.option()))
        {
            option_map.emplace(mf_option.option(),
                var_map[mf_option.option()].as<int32_t>());
        }
        if (var_map.count(forte_option.option()))
        {
            option_map.emplace(forte_option.option(),
                var_map[forte_option.option()].as<int32_t>());
        }
        if (var_map.count(ff_option.option()))
        {
            option_map.emplace(ff_option.option(),
                var_map[ff_option.option()].as<int32_t>());
        }
        if (var_map.count(fff_option.option()))
        {
            option_map.emplace(fff_option.option(),
                var_map[fff_option.option()].as<int32_t>());
        }
        if (var_map.count(ffff_option.option()))
        {
            option_map.emplace(ffff_option.option(),
                var_map[ffff_option.option()].as<int32_t>());
        }
        if (var_map.count(fffff_option.option()))
        {
            option_map.emplace(fffff_option.option(),
                var_map[fffff_option.option()].as<int32_t>());
        }
    }
    return option_map;
}

