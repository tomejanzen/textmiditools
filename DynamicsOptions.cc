//
// TextMIDITools Version 1.0.73
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
    {
        {  5, "ppppp"},
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
        {127, "fffff"}
    };

    if (!dynamics_configuration_file.empty())
    {
        program_options::options_description desc("Allowed dynamics configuration options");
    
        desc.add_options()
         (pppppOpt.c_str(), program_options::value<int>(), pppppTxt)
         ( ppppOpt.c_str(), program_options::value<int>(), ppppTxt)
         (  pppOpt.c_str(), program_options::value<int>(), pppTxt)
         (   ppOpt.c_str(), program_options::value<int>(), ppTxt)
         (    pOpt.c_str(), program_options::value<int>(), pTxt)
         (   mpOpt.c_str(), program_options::value<int>(), mpTxt)
         (   mfOpt.c_str(), program_options::value<int>(), mfTxt)
         (forteOpt.c_str(), program_options::value<int>(), forteTxt)
         (   ffOpt.c_str(), program_options::value<int>(), ffTxt)
         (  fffOpt.c_str(), program_options::value<int>(), fffTxt)
         ( ffffOpt.c_str(), program_options::value<int>(), ffffTxt)
         (fffffOpt.c_str(), program_options::value<int>(), fffffTxt)
          ;
        program_options::positional_options_description pos_opts_desc;
        program_options::variables_map var_map;
        program_options::store(
            program_options::parse_config_file(dynamics_configuration_file.c_str(), desc), var_map);
        program_options::notify(var_map);
    
        string input_filename;
    
        if (var_map.count(pppppOpt))
        {
            option_map.insert(pair<string_view, int>{pppppOpt, var_map[pppppOpt].as<int>()}); 
        }
        if (var_map.count(ppppOpt))
        {
            option_map.insert(pair<string_view, int>{ppppOpt, var_map[ppppOpt].as<int>()}); 
        }
        if (var_map.count(pppOpt))
        {
            option_map.insert(pair<string_view, int>{pppOpt, var_map[pppOpt].as<int>()}); 
        }
        if (var_map.count(ppOpt))
        {
            option_map.insert(pair<string_view, int>{ppOpt, var_map[ppOpt].as<int>()}); 
        }
        if (var_map.count(pOpt))
        {
            option_map.insert(pair<string_view, int>{pOpt, var_map[pOpt].as<int>()}); 
        }
        if (var_map.count(mpOpt))
        {
            option_map.insert(pair<string_view, int>{mpOpt, var_map[mpOpt].as<int>()}); 
        }
        if (var_map.count(mfOpt))
        {
            option_map.insert(pair<string_view, int>{mfOpt, var_map[mfOpt].as<int>()}); 
        }
        if (var_map.count(forteOpt))
        {
            option_map.insert(pair<string_view, int>{forteOpt, var_map[forteOpt].as<int>()}); 
        }
        if (var_map.count(ffOpt))
        {
            option_map.insert(pair<string_view, int>{ffOpt, var_map[ffOpt].as<int>()}); 
        }
        if (var_map.count(fffOpt))
        {
            option_map.insert(pair<string_view, int>{fffOpt, var_map[fffOpt].as<int>()}); 
        }
        if (var_map.count(ffffOpt))
        {
            option_map.insert(pair<string_view, int>{ffffOpt, var_map[ffffOpt].as<int>()}); 
        }
        if (var_map.count(fffffOpt))
        {
            option_map.insert(pair<string_view, int>{fffffOpt, var_map[fffffOpt].as<int>()}); 
        }
    }
    return option_map;
}

