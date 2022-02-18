#pragma once
#include "../../../plugin_sdk/plugin_sdk.hpp"

namespace Irelia
{
	void OnLoad();
	void OnDraw();
	void OnUnload();
	void OnUpdate();
	float GetRealQDamage(game_object_script target);
	void DrawDamage(game_object_script target, float damage);


	void Combo();
	void LaneClear();
	void Lasthit();
	void Flee();
	void Bladesurge();
	void FlawlessDuet();
	void VanguardsEdge();
	void GatherStacks(game_object_script target);
	void Gapcloser();
};