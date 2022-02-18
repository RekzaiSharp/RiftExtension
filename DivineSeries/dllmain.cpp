#include "Champions/Irelia/Irelia.h"
#include "../plugin_sdk/plugin_sdk.hpp"

PLUGIN_NAME("DivineSeries");
SUPPORTED_CHAMPIONS(champion_id::Irelia);

PLUGIN_API bool on_sdk_load(plugin_sdk_core* plugin_sdk_good)
{
    DECLARE_GLOBALS(plugin_sdk_good);

	switch (myhero->get_champion())
	{
	case champion_id::Irelia:
		Irelia::OnLoad(); 
		break;
	default:
		return false;
	}

    return true;
}

PLUGIN_API void on_sdk_unload()
{
	switch (myhero->get_champion())
	{
	case champion_id::Irelia:
		Irelia::OnUnload();
		break;
	default:
		break;
	}

}
