#include "Irelia.h"


namespace Irelia
{
	#define Q_DRAW_COLOR (MAKE_COLOR ( 62, 129, 237, 255 ))
	#define W_DRAW_COLOR (MAKE_COLOR ( 227, 203, 20, 255 ))
	#define E_DRAW_COLOR (MAKE_COLOR ( 235, 12, 223, 255 ))
	#define R_DRAW_COLOR (MAKE_COLOR ( 224, 77, 13, 255 ))

	script_spell* q = nullptr;
	script_spell* q2 = nullptr;
	script_spell* w = nullptr;
	script_spell* e = nullptr;
	script_spell* r = nullptr;

	vector blade_pos;
	bool isCharging = false;
	float chargeTime = 0.f;

	TreeTab* main_tab = nullptr;
	vector fleePos1[] =
	{
		vector(2129.51, 8760.85, 51.77),
		vector(3968.23, 8078.92, 50.50)
	};
	
	vector fleePos2[] =
	{
		vector(2298.95, 8293.25, 51.77),
		vector(3583.48, 7670.07, 51.54)
	};


	namespace menuSettings
	{
		// Q Settings
		TreeEntry* combo_q = nullptr;
		TreeEntry* clear_q = nullptr;
		TreeEntry* combo_qMode = nullptr;
		TreeEntry* combo_qKs = nullptr;
		TreeEntry* combo_qStack = nullptr;
		TreeEntry* combo_qSpam = nullptr;
		TreeEntry* qManaPerc = nullptr;
		TreeEntry* combo_qhp = nullptr;
		TreeEntry* combo_qhpp = nullptr;
		TreeEntry* combo_hpmaxdist = nullptr;
		TreeEntry* farm_qHp = nullptr;
		TreeEntry* farm_qEnemyRange = nullptr;
		TreeEntry* dontqturrentkey = nullptr;

		// W Settings
		TreeEntry* combo_w = nullptr;
		TreeEntry* farm_w = nullptr;
		TreeEntry* farm_w_mana = nullptr;
		TreeEntry* w_damagePercent = nullptr;
		TreeEntry* w_chargeDuration = nullptr;

		// E Settings
		TreeEntry* combo_e = nullptr;
		TreeEntry* combo_eMode = nullptr;
		TreeEntry* combo_e_mask = nullptr;
		TreeEntry* combo_e_extendrange = nullptr;

		// R Settings
		TreeEntry* combo_r = nullptr;
		TreeEntry* combo_r_count = nullptr;


		//Flee
		TreeEntry* flee_key = nullptr;
		TreeEntry* flee_q = nullptr;
		TreeEntry* flee_e = nullptr;
		TreeEntry* flee_jungle = nullptr;
		TreeEntry* flee_r = nullptr;
		TreeEntry* flee_r_hp = nullptr;
		TreeEntry* flee_r_enemy = nullptr;
		TreeEntry* flee_q_distance = nullptr;

		// Hitchance
		TreeEntry* w_hitchance = nullptr;
		TreeEntry* e_hitchance = nullptr;
		TreeEntry* r_hitchance = nullptr;

		// Drawings
		TreeEntry* draw_enable = nullptr;
		TreeEntry* draw_damage = nullptr;	
		TreeEntry* draw_text = nullptr;
		TreeEntry* draw_range_q = nullptr;
		TreeEntry* draw_range_extended_q = nullptr;
		TreeEntry* draw_range_w = nullptr;
		TreeEntry* draw_range_e = nullptr;
		TreeEntry* draw_range_r = nullptr;
		TreeEntry* draw_damage_q = nullptr;
		TreeEntry* draw_damage_w = nullptr;
		TreeEntry* draw_damage_e = nullptr;
		TreeEntry* draw_damage_r = nullptr;


		TreeEntry* debugKey = nullptr;
	}

	void OnLoad()
	{
		q = plugin_sdk->register_spell(spellslot::q, 600);
		q2 = plugin_sdk->register_spell(spellslot::q, 1200);
		w = plugin_sdk->register_spell(spellslot::w, 775);
		w->set_charged(775, 775, 1.5f);
		e = plugin_sdk->register_spell(spellslot::e, 775);
		e->set_skillshot(0.45f, 80.f, FLT_MAX, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);
		r = plugin_sdk->register_spell(spellslot::r, 1000);
		r->set_skillshot(0.25f, 200.f, FLT_MAX, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);

		main_tab = menu->create_tab(myhero->get_model() + "dsIrelia", "DS Irelia - Beta");
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());
		{
			auto qMenu = main_tab->add_tab(myhero->get_model() + ".Q", "[Q] - Bladesurge");
			{
				qMenu->set_assigned_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
				qMenu->add_separator("irelia.seperatorq2", "Combo");
				menuSettings::combo_q = qMenu->add_checkbox(myhero->get_model() + ".comboUseQ", "Use Q", true);
				std::vector < std::pair<std::string, void*>> qModes = { { "Smart", nullptr }, { "Marked", nullptr } };
				menuSettings::combo_qMode = qMenu->add_combobox(myhero->get_model() + ".comboQMode", "Q Mode", qModes, 0, true);
				menuSettings::combo_qKs = qMenu->add_checkbox(myhero->get_model() + ".comboUseKs", "Q Killsteal", true);
				menuSettings::combo_qStack = qMenu->add_checkbox(myhero->get_model() + ".comboQStack", "Stack before Engage", true);
				menuSettings::dontqturrentkey = qMenu->add_hotkey("debug.key", "Dont Q Under Turret", TreeHotkeyMode::Toggle, 0x54, true);
				menuSettings::combo_qSpam = qMenu->add_checkbox(myhero->get_model() + ".comboQTurret", "Q Every Mark", true);
				qMenu->add_separator("irelia.seperatorq1", "Farming");
				menuSettings::clear_q = qMenu->add_checkbox(myhero->get_model() + ".comboUseQclear", "Use in Clear", true);
				menuSettings::qManaPerc = qMenu->add_slider(myhero->get_model() + ".manaPerc", "Mana % Clear", 20, 0, 100);
				qMenu->add_separator("irelia.seperatorq3", "Misc - Smart Fight");
				menuSettings::combo_qhp = qMenu->add_checkbox(myhero->get_model() + ".comboUseQhp", "Q Around Target", true);
				menuSettings::combo_qhp->set_tooltip("Tries to dance around target by using q on minions to gain health back.");
				menuSettings::combo_qhpp = qMenu->add_slider(myhero->get_model() + ".hpperce", "Health %", 35, 0, 100);
				menuSettings::combo_hpmaxdist = qMenu->add_slider(myhero->get_model() + ".maxdist", "Max. Dist", 150, 50, 450);
				menuSettings::combo_hpmaxdist->set_tooltip("Maximum range you will move away from your target.");
				auto seperator = qMenu->add_separator("irelia.seperatorq4", "Misc - Clear Safety");
				seperator->set_tooltip("Safety checks to not q into minions when you are low life and enemies are near.");
				menuSettings::farm_qHp = qMenu->add_slider(myhero->get_model() + ".farmqhpperc", "Health %", 35, 0, 100);
				menuSettings::farm_qEnemyRange = qMenu->add_slider(myhero->get_model() + ".maxdisteee", "Min. Dist", 150, 50, 450);
				menuSettings::farm_qEnemyRange->set_tooltip("Minimum distance between enemy and target minion.");
			}

			auto wMenu = main_tab->add_tab(myhero->get_model() + ".W", "[W] - Defiant Dance"); 
			{
				wMenu->set_assigned_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
				menuSettings::combo_w = wMenu->add_checkbox(myhero->get_model() + ".comboUseW", "Use W", true);
				menuSettings::w_damagePercent = wMenu->add_slider(myhero->get_model() + ".wdamageperc", "Damage %", 15, 1, 100);
				menuSettings::w_chargeDuration = wMenu->add_slider(myhero->get_model() + ".wchargeduration", "W Charge Duration (ms)", 500, 250, 1000);
				wMenu->add_separator("irelia.seperatorw1", "Lasthit");
				menuSettings::farm_w = wMenu->add_checkbox(myhero->get_model() + ".farmUseW", "Use W", true);
				menuSettings::farm_w_mana = wMenu->add_slider(myhero->get_model() + ".wmanaperc", "Mana %", 15, 1, 100);
			}


			auto eMenu = main_tab->add_tab(myhero->get_model() + ".E", "[E] - Flawless Duet");
			{
				eMenu->set_assigned_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
				menuSettings::combo_e = eMenu->add_checkbox(myhero->get_model() + ".comboUseE", "Use E", true);
				std::vector < std::pair<std::string, void*>> eModes = { { "Behind Player", nullptr }, { "Cursor", nullptr } };
				menuSettings::combo_eMode = eMenu->add_combobox(myhero->get_model() + ".comboEMode", "First Blade Pos", eModes, 0, true);
				menuSettings::combo_e_extendrange = eMenu->add_slider(myhero->get_model() + ".eextendrange", "Extend Range", 150, 0, e->range());
			}

			auto rMenu = main_tab->add_tab(myhero->get_model() + ".R", "[R] - Vanguard's Edge");
			{
				rMenu->set_assigned_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
				menuSettings::combo_r = rMenu->add_checkbox(myhero->get_model() + ".comboR", "Use R", true);
				menuSettings::combo_r_count = rMenu->add_slider(myhero->get_model() + ".rCount", "Min. R", 1, 1, 5);
				
			}

			auto fleeMenu = main_tab->add_tab(myhero->get_model() + ".flee", "[Misc] - Flee");
			{
				fleeMenu->add_separator("irelia.seperatorflee1", "Comes with next Update");
				/*menuSettings::flee_q = fleeMenu->add_checkbox(myhero->get_model() + ".fleeQ", "Use Q", true);
				menuSettings::flee_q_distance = rMenu->add_slider(myhero->get_model() + ".fleeQDist", "Q Min. Distance", 250, 150, 550);
				menuSettings::flee_e = fleeMenu->add_checkbox(myhero->get_model() + ".fleeE", "Use E", true);
				fleeMenu->add_separator("irelia.seperatorflee1", "Advanced Settings");
				menuSettings::flee_jungle = fleeMenu->add_checkbox(myhero->get_model() + ".fleeJungle", "Use Jungle", true);
				menuSettings::flee_r = fleeMenu->add_checkbox(myhero->get_model() + ".fleeR", "Use R", true);
				menuSettings::flee_r_enemy = rMenu->add_slider(myhero->get_model() + ".fleeRCount", "R Min. Enemies", 4, 1, 5);
				menuSettings::flee_r_hp = rMenu->add_slider(myhero->get_model() + ".fleeRHP", "R Min. Health", 20, 10, 100);*/
			}

			auto hitchance = main_tab->add_tab("dsirelia.hitchance", "[Misc] - Hitchance");
			{
				menuSettings::w_hitchance = hitchance->add_combobox("dsirelia.hitchance.w", "Hitchance W", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
				menuSettings::e_hitchance = hitchance->add_combobox("dsirelia.hitchance.e", "Hitchance E", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
				menuSettings::r_hitchance = hitchance->add_combobox("dsirelia.hitchance.r", "Hitchance R", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
			}

			auto drawingMenu = main_tab->add_tab(myhero->get_model() + ".draw", "[Misc] - Drawings");
			{
				drawingMenu->add_separator("irelia.seperatordraw1", "Global Settings");
				menuSettings::draw_enable = drawingMenu->add_checkbox(myhero->get_model() + ".draw", "Draw Spells", true);
				menuSettings::draw_damage = drawingMenu->add_checkbox(myhero->get_model() + ".drawDamage", "Draw Damage", true);
				menuSettings::draw_text = drawingMenu->add_checkbox(myhero->get_model() + ".drawText", "Draw Text", true);
				drawingMenu->add_separator("irelia.seperatordraw2", "Spell Settings");
				menuSettings::draw_range_q = drawingMenu->add_checkbox(myhero->get_model() + ".drawQ", "Draw Q Range", true);
				menuSettings::draw_range_extended_q = drawingMenu->add_checkbox(myhero->get_model() + ".drawQE", "Draw Extended Q Range", true);
				menuSettings::draw_range_w = drawingMenu->add_checkbox(myhero->get_model() + ".drawW", "Draw W Range", false);
				menuSettings::draw_range_e = drawingMenu->add_checkbox(myhero->get_model() + ".drawE", "Draw E Range", true);
				menuSettings::draw_range_r = drawingMenu->add_checkbox(myhero->get_model() + ".drawR", "Draw R Range", true);
				drawingMenu->add_separator("irelia.seperatordraw3", "Damage Settings");
				menuSettings::draw_damage_q = drawingMenu->add_checkbox(myhero->get_model() + ".drawQD", "Draw Q Damage", true);
				menuSettings::draw_damage_w = drawingMenu->add_checkbox(myhero->get_model() + ".drawQW", "Draw W Damage", true);
				menuSettings::draw_damage_e = drawingMenu->add_checkbox(myhero->get_model() + ".drawED", "Draw E Damage", true);
				menuSettings::draw_damage_r = drawingMenu->add_checkbox(myhero->get_model() + ".drawRD", "Draw R Damage", true);
			}

			//menuSettings::debugKey = main_tab->add_hotkey("debug.key", "Debug Key", TreeHotkeyMode::Hold, 0x56, false);
		}
		
		event_handler<events::on_update>::add_callback(OnUpdate);
		event_handler<events::on_draw>::add_callback(OnDraw);
	}

	void OnDraw()
	{
		if (menuSettings::draw_enable->get_bool())
		{
			if (menuSettings::draw_range_q->get_bool() && q->is_ready()) draw_manager->add_circle(myhero->get_position(), q->range(), Q_DRAW_COLOR, 3.f);
			if (menuSettings::draw_range_extended_q->get_bool() && q->is_ready()) draw_manager->add_circle(myhero->get_position(), q->range() * 2, Q_DRAW_COLOR, 3.f);
			if (menuSettings::draw_range_w->get_bool() && w->is_ready()) draw_manager->add_circle(myhero->get_position(), w->range(), W_DRAW_COLOR, 3.f);
			if (menuSettings::draw_range_e->get_bool() && e->is_ready()) draw_manager->add_circle(myhero->get_position(), e->range(), E_DRAW_COLOR, 3.f);
			if (menuSettings::draw_range_r->get_bool() && r->is_ready()) draw_manager->add_circle(myhero->get_position(), r->range(), R_DRAW_COLOR, 3.f);
		}

		if (menuSettings::draw_damage->get_bool())
		{
			for (auto& enemy : entitylist->get_enemy_heroes())
			{
				if (enemy->get_distance(myhero) > 1200) continue;
				auto damage = 0.f;
				if (menuSettings::draw_damage_q->get_bool() && q->is_ready()) damage += GetRealQDamage(enemy);
				if (menuSettings::draw_damage_w->get_bool() && w->is_ready()) damage += w->get_damage(enemy);
				if (menuSettings::draw_damage_e->get_bool() && e->is_ready()) damage += e->get_damage(enemy) + GetRealQDamage(enemy);
				if (menuSettings::draw_damage_r->get_bool() && r->is_ready()) damage += r->get_damage(enemy) + GetRealQDamage(enemy);
				DrawDamage(enemy, damage);
			}
		}

		if (menuSettings::draw_text->get_bool())
		{
			auto const qTurretColor = menuSettings::dontqturrentkey->get_bool() ? MAKE_COLOR(245, 66, 66, 255) : MAKE_COLOR(87, 245, 66, 255);
			draw_manager->add_text(myhero->get_position() + vector(0, -50, 0), qTurretColor, 16, "Q Under Turret");
		}
		
		

	}

	hit_chance get_hitchance_by_config(TreeEntry* hit)
	{
		switch (hit->get_int())
		{
		case 0:
			return hit_chance::low;
			break;
		case 1:
			return hit_chance::medium;
			break;
		case 2:
			return hit_chance::high;
			break;
		case 3:
			return hit_chance::very_high;
			break;
		}
		return hit_chance::medium;
	}

	void OnUpdate()
	{
		if (orbwalker->combo_mode())
			Combo();

		if (orbwalker->lane_clear_mode())
			LaneClear();

		if (orbwalker->last_hit_mode())
			Lasthit();

		/*if (menuSettings::debugKey->get_bool())
		{
			auto hpvalue = std::to_string((myhero->get_max_health() / 100) * menuSettings::w_damagePercent->get_int()).c_str();
			console->print(hpvalue);
		}*/
	}

	float GetRealQDamage(game_object_script target)
	{
		auto physicalDamage = 0.f;
		auto magicalDamage = 0.f;
		auto totalDamage = 0.f;
		if (myhero->has_item(ItemId::Noonquiver) != spellslot::invalid && (target->is_monster() || target->is_minion())) physicalDamage += 20.f;
		if (myhero->has_item(ItemId::Blade_of_the_Ruined_King) != spellslot::invalid)
		{
			if (target->is_monster() || target->is_minion()) physicalDamage += (target->get_health() * 0.12 > 60 ? 60 : target->get_health() * 0.12);
			if (target->is_ai_hero()) physicalDamage += target->get_health() * 0.12;
		}
		if (myhero->has_item(ItemId::Wits_End) != spellslot::invalid)
		{
			if (myhero->get_level() < 9) magicalDamage += 15.f;
			else if (myhero->get_level() >= 9 && myhero->get_level() < 15) magicalDamage += 25 + ((myhero->get_level() - 9) * 10.f);
			else if (myhero->get_level() >= 15) magicalDamage += 76.f + ((myhero->get_level() - 15) * 1.25f);
		}

		if (myhero->has_item(ItemId::Titanic_Hydra) != spellslot::invalid) physicalDamage += 4.f + myhero->get_max_health() * 0.015f;
		if (myhero->has_item(ItemId::Ravenous_Hydra) != spellslot::invalid) physicalDamage += 4.f + myhero->get_max_health() * 0.015f;
		
		totalDamage = damagelib->calculate_damage_on_unit(myhero, target, damage_type::physical, physicalDamage);
		totalDamage = damagelib->calculate_damage_on_unit(myhero, target, damage_type::magical, magicalDamage);
		totalDamage += q->get_damage(target);

		if (target->is_minion() && target->has_buff(buff_hash("barondebuff"))) totalDamage * 0.6;
		
		return totalDamage;
	}

	void DrawDamage(game_object_script target, float damage)
	{

		if (target->is_dead()) return;

		vector startPos;
		vector endPos = target->get_hpbar_pos();

		endPos.x += target->get_health() / target->get_max_health() * 105;
		endPos.y += -6;

		startPos = endPos;

		

		startPos.x += (-damage / target->get_max_health() * 105);

		if (startPos.x < target->get_hpbar_pos().x - 46) startPos.x = target->get_hpbar_pos().x - 46;

		draw_manager->add_line_on_screen(startPos, endPos, MAKE_COLOR(255, 255, 255, 120), 12.f);
	}

	void GatherStacks(game_object_script target)
	{
		if (!target->has_buff(buff_hash("ireliamark")))
		{
			if ((myhero->get_buff_count(buff_hash("ireliapassivestacks")) < 4 || myhero->get_health_percent() <= menuSettings::farm_qHp->get_int()) && q->is_ready())
			{
				auto minions = entitylist->get_enemy_minions();
				for (auto& minion : minions)
				{
					if (minion->get_position().distance(myhero) >= 600 || minion->get_distance(target) >= menuSettings::combo_hpmaxdist->get_int())
						continue;

					if (minion->get_health() <= GetRealQDamage(minion) && myhero->get_distance(target->get_position()) >= minion->get_distance(target->get_position()))
					{
						if (minion->get_position().is_under_enemy_turret() && menuSettings::dontqturrentkey->get_bool())
						{
							auto allied_minions = entitylist->get_ally_minions();
							for (auto& aMinion : allied_minions)
							{
								if (aMinion->get_position().distance(myhero) >= 1000)
									continue;

								if (aMinion->get_position().is_under_enemy_turret())
									q->cast(minion);
							}
						}
						else
						{
							q->cast(minion);
						}
					}
				}
			}
		}
	}

	float wDamage[] = { 10.f, 25.f, 40.f, 55.f, 70.f };
	void Lasthit()
	{
		if (w->is_ready())
		{
			if (menuSettings::farm_w->get_bool() && menuSettings::farm_w_mana->get_int() <= myhero->get_mana_percent())
			{
				for (auto& minion : entitylist->get_enemy_minions())
				{
					if (minion->get_distance(myhero) <= 700 && minion->get_health() <= damagelib->calculate_damage_on_unit(myhero, minion, damage_type::physical, wDamage[w->level()] + myhero->get_total_attack_damage() * 0.4f))
						w->cast(minion->get_position());
				}
			}
		}
	}

	void Flee()
	{

	}

	void Combo()
	{
		auto target = target_selector->get_target(600.f, damage_type::physical);

		if (target)
		{
			if (menuSettings::combo_qStack->get_bool() && myhero->get_mana_percent() >= menuSettings::qManaPerc->get_int())
				GatherStacks(target);

			//Scuffed w implementation
			if (w->is_ready() && menuSettings::combo_w->get_bool() && !isCharging)
			{

				if (health_prediction->get_incoming_damage(myhero, 1.5f, true) >= (myhero->get_max_health() / 100) * menuSettings::w_damagePercent->get_int())
				{
					if (!myhero->is_under_enemy_turret())
					{
						w->cast();
						chargeTime = gametime->get_time();
						isCharging = true;
						console->print("w casted, chargetime set and ischarging.");
					}
				}
			}

			if (w->is_ready() && isCharging)
			{
				if (gametime->get_time() - chargeTime >= (menuSettings::w_chargeDuration->get_int() / 1000.f))
				{
					if (target)
					{
						myhero->update_charged_spell(w->slot, target->get_position(), true);
						isCharging = false;
					}
					
				}
			}

			if (q->is_ready()) Bladesurge();
			if (e->is_ready()) FlawlessDuet();
			if (r->is_ready()) VanguardsEdge();
		}
		else
		{
			Gapcloser();
		}
	}

	void LaneClear()
	{
		if (menuSettings::clear_q->get_bool() && myhero->get_mana_percent() >= menuSettings::qManaPerc->get_int() && q->is_ready())
		{
			for (auto& minion : entitylist->get_enemy_minions())
			{
				if (myhero->get_health_percent() <= menuSettings::farm_qHp->get_int() && minion->count_allies_in_range(menuSettings::farm_qEnemyRange->get_int()) >= 1)
					continue;

				if (minion->get_health() <= GetRealQDamage(minion) && minion->get_distance(myhero) <= 600)
				{
					if (menuSettings::dontqturrentkey->get_bool() && minion->get_position().is_under_enemy_turret())
					{
						auto allyminions = entitylist->get_ally_minions();
						for (auto& aMinion : allyminions)
						{
							if (aMinion->get_distance(myhero) <= 1000 && aMinion->get_position().is_under_enemy_turret())
							{
								q->cast(minion);
							}
						}
					}
					else
					{
						q->cast(minion);
					}
				}
			}
		}
	}

	void Gapcloser()
	{
		auto minions = entitylist->get_enemy_minions();
		for (auto& minion : minions)
		{
			if (minion->get_health() <= GetRealQDamage(minion) && q->is_ready())
			{
				auto target = target_selector->get_target(1200.f, damage_type::physical);
				if (target && myhero->get_distance(target) > minion->get_distance(target) && minion->get_distance(myhero) <= 600)
				{
					q->cast(minion);
				}
			}
		}
	}

	void Bladesurge()
	{
		if (menuSettings::combo_q->get_bool() && q->is_ready())
		{
			auto target = target_selector->get_target(600.f, damage_type::physical);

			if (target)
			{
				if (menuSettings::combo_qKs->get_bool())
				{
					auto enemies = entitylist->get_enemy_heroes();
					for (auto& enemy : enemies)
					{
						if (enemy->get_distance(target) <= e->range() && enemy->get_health() + enemy->get_physical_shield() + 10 <=
							GetRealQDamage(enemy))
							q->cast(enemy);
					}
					
				}
				

				if (menuSettings::combo_qSpam->get_bool())
				{
					auto enemies = entitylist->get_enemy_heroes();
					for (auto& enemy : enemies)
					{
						if (enemy->get_network_id() == target->get_network_id())
							continue;
						
						if (enemy->has_buff(buff_hash("ireliamark")) && enemy->get_distance(target) <= 600.f)
							q->cast(enemy);						
					}
				}

				if (menuSettings::combo_qMode->get_int() == 0)
				{
					if (myhero->get_distance(target) >= myhero->get_attack_range() + 50.f && myhero->get_distance(target) <= 600 && target->get_health() +
						target->get_physical_shield() + 10 <= GetRealQDamage(target) + myhero->get_auto_attack_damage(target, true) * 2)
						q->cast(target);
				}

				if (target->has_buff(buff_hash("ireliamark")))
					q->cast(target);
			}
		}
	}

	void FlawlessDuet()
	{
		auto target = target_selector->get_target(e->range() - 50.f, damage_type::physical);

		if (!target) return;
	
		if (strcmp(e->name().c_str(), "IreliaE") == 0 && e->is_ready())
		{
			if (menuSettings::combo_eMode->get_int() == 0)
			{
				auto extendedPos = target->get_position().extend(myhero->get_position(), myhero->get_distance(target) + e->range());
				if (extendedPos.is_valid())
				{
					e->cast(extendedPos);
					blade_pos = extendedPos;
				}
			}
			else
			{
				auto castpos = hud->get_hud_input_logic()->get_game_cursor_position();
				e->cast(castpos);
				blade_pos = castpos;
			}
				
		}
		else if (strcmp(e->name().c_str(), "IreliaE2") == 0 && e->is_ready())
		{
			auto pred = e->get_prediction(target);
			if (pred.hitchance >= get_hitchance_by_config(menuSettings::e_hitchance))
			{
				auto extendby = blade_pos.distance(target) + menuSettings::combo_e_extendrange->get_int();
				e->cast(blade_pos.extend(pred.get_cast_position(), extendby));
			}
						
		}
		
	}

	void VanguardsEdge()
	{
		const auto target = target_selector->get_target(r->range() - 50.f, damage_type::physical);
		if (!target) return;

		if (menuSettings::combo_r->get_bool())
		{
			if (menuSettings::combo_r_count->get_int() >= 1)
			{
				auto pred = r->get_prediction(target, true);
				if (pred.get_cast_position().is_valid() && pred.aoe_targets_hit_count() >= menuSettings::combo_r_count->get_int())
				{
					r->cast(pred.get_cast_position());
				}
			}

			const auto damage = GetRealQDamage(target) * 2 + r->get_damage(target) + myhero->get_auto_attack_damage(target) * 2;
			if (target->get_health() + target->get_physical_shield() <= damage && target->get_health() > GetRealQDamage(target) + myhero->get_auto_attack_damage(target) * 2)
			{
				auto pred = prediction->get_prediction(target, 0.25f, 200.f, 2000.f);
				if (pred.get_cast_position().is_valid())
					r->cast(pred.get_cast_position());
			}
		}
	}


	void OnUnload()
	{
		plugin_sdk->remove_spell(q);
		plugin_sdk->remove_spell(w);
		plugin_sdk->remove_spell(e);
		plugin_sdk->remove_spell(r);

		event_handler<events::on_update>::remove_handler(OnUpdate);
		event_handler<events::on_draw>::remove_handler(OnDraw);
	}
}