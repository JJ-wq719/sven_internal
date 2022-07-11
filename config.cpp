#include <algorithm>

#include <IConfigManager.h>
#include <ISvenModAPI.h>
#include <dbg.h>

#include "config.h"

#include "features/skybox.h"
#include "utils/menu_styles.h"

extern void WindowStyle();

//-----------------------------------------------------------------------------
// Vars
//-----------------------------------------------------------------------------

CConfig g_Config;

static char s_szConfigsDir[MAX_PATH] = { 0 };

//-----------------------------------------------------------------------------
// Init config stuff
//-----------------------------------------------------------------------------

void CConfig::Init()
{
	std::string sDir = g_pSvenModAPI->GetBaseDirectory();

#ifdef PLATFORM_WINDOWS
	snprintf(s_szConfigsDir, sizeof(s_szConfigsDir), "%s\\sven_internal\\config", g_pSvenModAPI->GetBaseDirectory());
#else
	snprintf(s_szConfigsDir, sizeof(s_szConfigsDir), "%s/sven_internal/config", g_pSvenModAPI->GetBaseDirectory());
#endif

#ifdef PLATFORM_WINDOWS
	DWORD ret = GetFileAttributes( (sDir + "\\sven_internal\\config\\default.ini").c_str() );

	if ( ret == INVALID_FILE_ATTRIBUTES || ret & FILE_ATTRIBUTE_DIRECTORY )
	{
		FILE *file = fopen("sven_internal/config/default.ini", "w");
		if (file) fclose(file);
	}
#else
#error Implement Linux equivalent
#endif

	UpdateConfigs();
	current_config = "default.ini";

	sDir.clear();
}

void CConfig::UpdateConfigs()
{
	configs.clear();

#ifdef PLATFORM_WINDOWS
	HANDLE hFile;
	WIN32_FIND_DATAA FileInformation;

	char szFolderInitialPath[MAX_PATH] = { 0 };

	snprintf(szFolderInitialPath, sizeof(szFolderInitialPath), "%s\\*.*", s_szConfigsDir);

	hFile = ::FindFirstFileA(szFolderInitialPath, &FileInformation);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
			#pragma warning(push)
			#pragma warning(push)
			#pragma warning(disable: 26450)
			#pragma warning(disable: 4307)

				if ( !(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
					const char *pszExtension = NULL;
					const char *buffer = FileInformation.cFileName;

					while (*buffer)
					{
						if (*buffer == '.')
							pszExtension = buffer;

						buffer++;
					}

					if (pszExtension && !stricmp(pszExtension, ".ini"))
					{
						configs.push_back(FileInformation.cFileName);
					}
				}

			#pragma warning(pop)
			#pragma warning(pop)
			}
		} while (::FindNextFileA(hFile, &FileInformation) == TRUE);

		::FindClose(hFile);
	}
#else
#error Implement Linux equivalent
#endif

	std::vector<std::string>::iterator it = std::find(configs.begin(), configs.end(), current_config.c_str());

	if (it == configs.end())
	{
		current_config.clear();
	}

	std::sort(configs.begin(), configs.end());
}

//-----------------------------------------------------------------------------
// Import config vars
//-----------------------------------------------------------------------------

bool CConfig::Load()
{
	if (current_config.empty())
		return false;

	if (ConfigManager()->BeginImport( (std::string("sven_internal/config/") + current_config).c_str() ))
	{
		if (ConfigManager()->BeginSectionImport("SETTINGS"))
		{
			ConfigManager()->SetConversionRadix(16);
			ConfigManager()->ImportParam("ToggleButton", *(uint32 *)&cvars.toggle_button);
			ConfigManager()->ResetRadix();

			ConfigManager()->ImportParam("AutoResize", cvars.menu_auto_resize);
			ConfigManager()->ImportParam("Theme", cvars.menu_theme);
			ConfigManager()->ImportParam("Opacity", cvars.menu_opacity);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("ESP"))
		{
			ConfigManager()->ImportParam("Enable", cvars.esp);
			ConfigManager()->ImportParam("Distance", cvars.esp_distance);
			ConfigManager()->ImportParam("Box", cvars.esp_box);
			ConfigManager()->ImportParam("Outline", cvars.esp_box_outline);
			ConfigManager()->ImportParam("Fill", cvars.esp_box_fill);
			ConfigManager()->ImportParam("ShowIndex", cvars.esp_box_index);
			ConfigManager()->ImportParam("ShowDistance", cvars.esp_box_distance);
			ConfigManager()->ImportParam("ShowPlayerHealth", cvars.esp_box_player_health);
			ConfigManager()->ImportParam("ShowPlayerArmor", cvars.esp_box_player_armor);
			ConfigManager()->ImportParam("ShowEntityName", cvars.esp_box_entity_name);
			ConfigManager()->ImportParam("ShowPlayerName", cvars.esp_box_player_name);
			ConfigManager()->ImportParam("ShowItems", cvars.esp_show_items);
			ConfigManager()->ImportParam("IgnoreUnknownEnts", cvars.esp_ignore_unknown_ents);
			ConfigManager()->ImportParam("Targets", cvars.esp_targets);
			ConfigManager()->ImportParam("ShowSkeleton", cvars.esp_skeleton);
			ConfigManager()->ImportParam("ShowBonesName", cvars.esp_bones_name);
			ConfigManager()->ImportParam("ShowSkeletonType", cvars.esp_skeleton_type);
			ConfigManager()->ImportParam("FriendColor_R", cvars.esp_friend_color[0]);
			ConfigManager()->ImportParam("FriendColor_G", cvars.esp_friend_color[1]);
			ConfigManager()->ImportParam("FriendColor_B", cvars.esp_friend_color[2]);
			ConfigManager()->ImportParam("EnemyColor_R", cvars.esp_enemy_color[0]);
			ConfigManager()->ImportParam("EnemyColor_G", cvars.esp_enemy_color[1]);
			ConfigManager()->ImportParam("EnemyColor_B", cvars.esp_enemy_color[2]);
			ConfigManager()->ImportParam("NeutralColor_R", cvars.esp_neutral_color[0]);
			ConfigManager()->ImportParam("NeutralColor_G", cvars.esp_neutral_color[1]);
			ConfigManager()->ImportParam("NeutralColor_B", cvars.esp_neutral_color[2]);
			ConfigManager()->ImportParam("ItemColor_R", cvars.esp_item_color[0]);
			ConfigManager()->ImportParam("ItemColor_G", cvars.esp_item_color[1]);
			ConfigManager()->ImportParam("ItemColor_B", cvars.esp_item_color[2]);

			ConfigManager()->EndSectionImport();
		}
		
		if (ConfigManager()->BeginSectionImport("WALLHACK"))
		{
			ConfigManager()->ImportParam("Wallhack", cvars.wallhack);
			ConfigManager()->ImportParam("Negative", cvars.wallhack_negative);
			ConfigManager()->ImportParam("WhiteWalls", cvars.wallhack_white_walls);
			ConfigManager()->ImportParam("Wireframe", cvars.wallhack_wireframe);
			ConfigManager()->ImportParam("WireframeModels", cvars.wallhack_wireframe_models);
			ConfigManager()->ImportParam("Wireframe_Width", cvars.wh_wireframe_width);
			ConfigManager()->ImportParam("Wireframe_R", cvars.wh_wireframe_color[0]);
			ConfigManager()->ImportParam("Wireframe_G", cvars.wh_wireframe_color[2]);
			ConfigManager()->ImportParam("Wireframe_B", cvars.wh_wireframe_color[1]);

			ConfigManager()->EndSectionImport();
		}
	
		if (ConfigManager()->BeginSectionImport("CROSSHAIR"))
		{
			ConfigManager()->ImportParam("Enable", cvars.draw_crosshair);
			ConfigManager()->ImportParam("EnableDot", cvars.draw_crosshair_dot);
			ConfigManager()->ImportParam("EnableOutline", cvars.draw_crosshair_outline);
			ConfigManager()->ImportParam("Size", cvars.crosshair_size);
			ConfigManager()->ImportParam("Gap", cvars.crosshair_gap);
			ConfigManager()->ImportParam("Thickness", cvars.crosshair_thickness);
			ConfigManager()->ImportParam("OutlineThickness", cvars.crosshair_outline_thickness);
			ConfigManager()->ImportParam("OutlineColor_R", cvars.crosshair_outline_color[0]);
			ConfigManager()->ImportParam("OutlineColor_G", cvars.crosshair_outline_color[1]);
			ConfigManager()->ImportParam("OutlineColor_B", cvars.crosshair_outline_color[2]);
			ConfigManager()->ImportParam("OutlineColor_A", cvars.crosshair_outline_color[3]);
			ConfigManager()->ImportParam("Color_R", cvars.crosshair_color[0]);
			ConfigManager()->ImportParam("Color_G", cvars.crosshair_color[1]);
			ConfigManager()->ImportParam("Color_B", cvars.crosshair_color[2]);
			ConfigManager()->ImportParam("Color_A", cvars.crosshair_color[3]);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("VISUAL"))
		{
			ConfigManager()->ImportParam("NoShake", cvars.no_shake);
			ConfigManager()->ImportParam("NoFade", cvars.no_fade);
			ConfigManager()->ImportParam("DrawEntities", cvars.draw_entities);

			ConfigManager()->ImportParam("ShowSpeed", cvars.show_speed);
			ConfigManager()->ImportParam("ShowJumpSpeed", cvars.show_jumpspeed);
			ConfigManager()->ImportParam("StoreVerticalSpeed", cvars.show_vertical_speed);
			ConfigManager()->ImportParam("JumpSpeedFadeDuration", cvars.jumpspeed_fade_duration);
			ConfigManager()->ImportParam("SpeedWidthFraction", cvars.speed_width_fraction);
			ConfigManager()->ImportParam("SpeedHeightFraction", cvars.speed_height_fraction);
			ConfigManager()->ImportParam("SpeedColor_R", cvars.speed_color[0]);
			ConfigManager()->ImportParam("SpeedColor_G", cvars.speed_color[1]);
			ConfigManager()->ImportParam("SpeedColor_B", cvars.speed_color[2]);
			
			ConfigManager()->ImportParam("ShowSpeed_Legacy", cvars.show_speed_legacy);
			ConfigManager()->ImportParam("StoreVerticalSpeed_Legacy", cvars.show_vertical_speed_legacy);
			ConfigManager()->ImportParam("SpeedWidthFraction_Legacy", cvars.speed_width_fraction_legacy);
			ConfigManager()->ImportParam("SpeedHeightFraction_Legacy", cvars.speed_height_fraction_legacy);
			ConfigManager()->ImportParam("SpeedColor_Legacy_R", cvars.speed_color_legacy[0]);
			ConfigManager()->ImportParam("SpeedColor_Legacy_G", cvars.speed_color_legacy[1]);
			ConfigManager()->ImportParam("SpeedColor_Legacy_B", cvars.speed_color_legacy[2]);
			ConfigManager()->ImportParam("SpeedColor_Legacy_A", cvars.speed_color_legacy[3]);

			ConfigManager()->ImportParam("LightmapOverride", cvars.lightmap_override);
			ConfigManager()->ImportParam("LightmapOverrideBrightness", cvars.lightmap_brightness);
			ConfigManager()->ImportParam("LightmapOverride_R", cvars.lightmap_color[0]);
			ConfigManager()->ImportParam("LightmapOverride_G", cvars.lightmap_color[1]);
			ConfigManager()->ImportParam("LightmapOverride_B", cvars.lightmap_color[2]);
			ConfigManager()->ImportParam("ShowPlayersPushDirection", cvars.show_players_push_direction);
			ConfigManager()->ImportParam("PushDirectionLength", cvars.push_direction_length);
			ConfigManager()->ImportParam("PushDirectionWidth", cvars.push_direction_width);
			ConfigManager()->ImportParam("PushDirectionColor_R", cvars.push_direction_color[0]);
			ConfigManager()->ImportParam("PushDirectionColor_G", cvars.push_direction_color[1]);
			ConfigManager()->ImportParam("PushDirectionColor_B", cvars.push_direction_color[2]);
			ConfigManager()->ImportParam("RemapHUDColor", cvars.remap_hud_color);
			ConfigManager()->ImportParam("HUDColor_R", cvars.hud_color[0]);
			ConfigManager()->ImportParam("HUDColor_G", cvars.hud_color[1]);
			ConfigManager()->ImportParam("HUDColor_B", cvars.hud_color[2]);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("CHAMS"))
		{
			ConfigManager()->ImportParam("Enable", cvars.chams);
			ConfigManager()->ImportParam("ChamsPlayers", cvars.chams_players);
			ConfigManager()->ImportParam("ChamsEntities", cvars.chams_entities);
			ConfigManager()->ImportParam("ChamsItems", cvars.chams_items);
			ConfigManager()->ImportParam("ChamsPlayersWall", cvars.chams_players_wall);
			ConfigManager()->ImportParam("ChamsEntitiesWall", cvars.chams_entities_wall);
			ConfigManager()->ImportParam("ChamsItemsWall", cvars.chams_items_wall);
			ConfigManager()->ImportParam("ChamsPlayersColor_R", cvars.chams_players_color[0]);
			ConfigManager()->ImportParam("ChamsPlayersColor_G", cvars.chams_players_color[1]);
			ConfigManager()->ImportParam("ChamsPlayersColor_B", cvars.chams_players_color[2]);
			ConfigManager()->ImportParam("ChamsEntitiesColor_R", cvars.chams_entities_color[0]);
			ConfigManager()->ImportParam("ChamsEntitiesColor_G", cvars.chams_entities_color[1]);
			ConfigManager()->ImportParam("ChamsEntitiesColor_B", cvars.chams_entities_color[2]);
			ConfigManager()->ImportParam("ChamsItemsColor_R", cvars.chams_items_color[0]);
			ConfigManager()->ImportParam("ChamsItemsColor_G", cvars.chams_items_color[1]);
			ConfigManager()->ImportParam("ChamsItemsColor_B", cvars.chams_items_color[2]);
			ConfigManager()->ImportParam("ChamsPlayersWallColor_R", cvars.chams_players_wall_color[0]);
			ConfigManager()->ImportParam("ChamsPlayersWallColor_G", cvars.chams_players_wall_color[1]);
			ConfigManager()->ImportParam("ChamsPlayersWallColor_B", cvars.chams_players_wall_color[2]);
			ConfigManager()->ImportParam("ChamsEntitiesWallColor_R", cvars.chams_entities_wall_color[0]);
			ConfigManager()->ImportParam("ChamsEntitiesWallColor_G", cvars.chams_entities_wall_color[1]);
			ConfigManager()->ImportParam("ChamsEntitiesWallColor_B", cvars.chams_entities_wall_color[2]);
			ConfigManager()->ImportParam("ChamsItemsWallColor_R", cvars.chams_items_wall_color[0]);
			ConfigManager()->ImportParam("ChamsItemsWallColor_G", cvars.chams_items_wall_color[1]);
			ConfigManager()->ImportParam("ChamsItemsWallColor_B", cvars.chams_items_wall_color[2]);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("GLOW"))
		{
			ConfigManager()->ImportParam("Enable", cvars.glow);
			ConfigManager()->ImportParam("Optimize", cvars.glow_optimize);
			ConfigManager()->ImportParam("GlowPlayers", cvars.glow_players);
			ConfigManager()->ImportParam("GlowEntities", cvars.glow_entities);
			ConfigManager()->ImportParam("GlowItems", cvars.glow_items);
			ConfigManager()->ImportParam("GlowPlayersWidth", cvars.glow_players_width);
			ConfigManager()->ImportParam("GlowEntitiesWidth", cvars.glow_entities_width);
			ConfigManager()->ImportParam("GlowItemsWidth", cvars.glow_items_width);
			ConfigManager()->ImportParam("GlowPlayersWall", cvars.glow_players_wall);
			ConfigManager()->ImportParam("GlowEntitiesWall", cvars.glow_entities_wall);
			ConfigManager()->ImportParam("GlowItemsWall", cvars.glow_items_wall);
			ConfigManager()->ImportParam("GlowPlayersColor_R", cvars.glow_players_color[0]);
			ConfigManager()->ImportParam("GlowPlayersColor_G", cvars.glow_players_color[1]);
			ConfigManager()->ImportParam("GlowPlayersColor_B", cvars.glow_players_color[2]);
			ConfigManager()->ImportParam("GlowEntitiesColor_R", cvars.glow_entities_color[0]);
			ConfigManager()->ImportParam("GlowEntitiesColor_G", cvars.glow_entities_color[1]);
			ConfigManager()->ImportParam("GlowEntitiesColor_B", cvars.glow_entities_color[2]);
			ConfigManager()->ImportParam("GlowItemsColor_R", cvars.glow_items_color[0]);
			ConfigManager()->ImportParam("GlowItemsColor_G", cvars.glow_items_color[1]);
			ConfigManager()->ImportParam("GlowItemsColor_B", cvars.glow_items_color[2]);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("DYNAMICGLOW"))
		{
			ConfigManager()->ImportParam("GlowAttach", cvars.dyn_glow_attach);
			ConfigManager()->ImportParam("GlowSelf", cvars.dyn_glow_self);
			ConfigManager()->ImportParam("GlowSelfRadius", cvars.dyn_glow_self_radius);
			ConfigManager()->ImportParam("GlowSelfDecay", cvars.dyn_glow_self_decay);
			ConfigManager()->ImportParam("GlowSelfColor_R", cvars.dyn_glow_self_color[0]);
			ConfigManager()->ImportParam("GlowSelfColor_G", cvars.dyn_glow_self_color[1]);
			ConfigManager()->ImportParam("GlowSelfColor_B", cvars.dyn_glow_self_color[2]);
			ConfigManager()->ImportParam("GlowPlayers", cvars.dyn_glow_players);
			ConfigManager()->ImportParam("GlowPlayersRadius", cvars.dyn_glow_players_radius);
			ConfigManager()->ImportParam("GlowPlayersDecay", cvars.dyn_glow_players_decay);
			ConfigManager()->ImportParam("GlowPlayersColor_R", cvars.dyn_glow_players_color[0]);
			ConfigManager()->ImportParam("GlowPlayersColor_G", cvars.dyn_glow_players_color[1]);
			ConfigManager()->ImportParam("GlowPlayersColor_B", cvars.dyn_glow_players_color[2]);
			ConfigManager()->ImportParam("GlowEntities", cvars.dyn_glow_entities);
			ConfigManager()->ImportParam("GlowEntitiesRadius", cvars.dyn_glow_entities_radius);
			ConfigManager()->ImportParam("GlowEntitiesDecay", cvars.dyn_glow_entities_decay);
			ConfigManager()->ImportParam("GlowEntitiesColor_R", cvars.dyn_glow_entities_color[0]);
			ConfigManager()->ImportParam("GlowEntitiesColor_G", cvars.dyn_glow_entities_color[1]);
			ConfigManager()->ImportParam("GlowEntitiesColor_B", cvars.dyn_glow_entities_color[2]);
			ConfigManager()->ImportParam("GlowItems", cvars.dyn_glow_items);
			ConfigManager()->ImportParam("GlowItemsRadius", cvars.dyn_glow_items_radius);
			ConfigManager()->ImportParam("GlowItemsDecay", cvars.dyn_glow_items_decay);
			ConfigManager()->ImportParam("GlowItemsColor_R", cvars.dyn_glow_items_color[0]);
			ConfigManager()->ImportParam("GlowItemsColor_G", cvars.dyn_glow_items_color[1]);
			ConfigManager()->ImportParam("GlowItemsColor_B", cvars.dyn_glow_items_color[2]);

			ConfigManager()->EndSectionImport();
		}
	
		if (ConfigManager()->BeginSectionImport("STRAFE"))
		{
			ConfigManager()->ImportParam("Enable", cvars.strafe);
			ConfigManager()->ImportParam("IgnoreGround", cvars.strafe_ignore_ground);
			ConfigManager()->ImportParam("Direction", cvars.strafe_dir);
			ConfigManager()->ImportParam("Type", cvars.strafe_type);

			ConfigManager()->EndSectionImport();
		}
	
		if (ConfigManager()->BeginSectionImport("FAKELAG"))
		{
			ConfigManager()->ImportParam("Enable", cvars.fakelag);
			ConfigManager()->ImportParam("AdaptiveInterp", cvars.fakelag_adaptive_ex_interp);
			ConfigManager()->ImportParam("Type", cvars.fakelag_type);
			ConfigManager()->ImportParam("Move", cvars.fakelag_move);
			ConfigManager()->ImportParam("Limit", cvars.fakelag_limit);
			ConfigManager()->ImportParam("Variance", cvars.fakelag_variance);

			ConfigManager()->EndSectionImport();
		}
	
		if (ConfigManager()->BeginSectionImport("ANTIAFK"))
		{
			ConfigManager()->ImportParam("Type", cvars.antiafk);
			ConfigManager()->ImportParam("RotateCamera", cvars.antiafk_rotate_camera);
			ConfigManager()->ImportParam("StayWithinRange", cvars.antiafk_stay_within_range);
			ConfigManager()->ImportParam("ResetStayPos", cvars.antiafk_reset_stay_pos);
			ConfigManager()->ImportParam("RotationAngle", cvars.antiafk_rotation_angle);
			ConfigManager()->ImportParam("StayRadius", cvars.antiafk_stay_radius);
			ConfigManager()->ImportParam("StayRadiusOffsetAngle", cvars.antiafk_stay_radius_offset_angle);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("MISC"))
		{
			ConfigManager()->ImportParam("AutoJump", cvars.autojump);
			ConfigManager()->ImportParam("JumpBug", cvars.jumpbug);
			ConfigManager()->ImportParam("DoubleDuck", cvars.doubleduck);
			ConfigManager()->ImportParam("FastRun", cvars.fastrun);
			ConfigManager()->ImportParam("QuakeGuns", cvars.quake_guns);
			ConfigManager()->ImportParam("TertiaryAttackGlitch", cvars.tertiary_attack_glitch);
			ConfigManager()->ImportParam("SaveSoundcache", cvars.save_soundcache);
			ConfigManager()->ImportParam("RotateDeadBody", cvars.rotate_dead_body);
			ConfigManager()->ImportParam("AutoCeilClipping", cvars.auto_ceil_clipping);
			ConfigManager()->ImportParam("RemoveFOVCap", cvars.remove_fov_cap);
			ConfigManager()->ImportParam("NoWeaponAnim", cvars.no_weapon_anim);
			ConfigManager()->ImportParam("LockPitch", cvars.lock_pitch);
			ConfigManager()->ImportParam("LockYaw", cvars.lock_yaw);
			ConfigManager()->ImportParam("LockPitchAngle", cvars.lock_pitch_angle);
			ConfigManager()->ImportParam("LockYawAngle", cvars.lock_yaw_angle);
			ConfigManager()->ImportParam("SpinYaw", cvars.spin_yaw_angle);
			ConfigManager()->ImportParam("SpinPitch", cvars.spin_pitch_angle);
			ConfigManager()->ImportParam("SpinYawAngle", cvars.spin_yaw_rotation_angle);
			ConfigManager()->ImportParam("SpinPitchAngle", cvars.spin_pitch_rotation_angle);
			ConfigManager()->ImportParam("ColorPulsator", cvars.color_pulsator);
			ConfigManager()->ImportParam("ColorPulsatorTop", cvars.color_pulsator_top);
			ConfigManager()->ImportParam("ColorPulsatorBottom", cvars.color_pulsator_bottom);
			ConfigManager()->ImportParam("ColorPulsatorDelay", cvars.color_pulsator_delay);
			ConfigManager()->ImportParam("IgnoreDifferentMapVersions", cvars.ignore_different_map_versions);
			ConfigManager()->ImportParam("OneTickExploit", cvars.one_tick_exploit);
			ConfigManager()->ImportParam("OneTickExploitLagInterval", cvars.one_tick_exploit_lag_interval);
			ConfigManager()->ImportParam("OneTickExploitSpeedhack", cvars.one_tick_exploit_speedhack);
			ConfigManager()->ImportParam("FastCrowbar", cvars.fast_crowbar);
			ConfigManager()->ImportParam("FastCrowbar2", cvars.fast_crowbar2);
			ConfigManager()->ImportParam("FastRevive", cvars.fast_medkit);
			ConfigManager()->ImportParam("ApplicationSpeed", cvars.application_speed);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("KEYSPAM"))
		{
			ConfigManager()->ImportParam("HoldMode", cvars.keyspam_hold_mode);
			ConfigManager()->ImportParam("Spam_E", cvars.keyspam_e);
			ConfigManager()->ImportParam("Spam_W", cvars.keyspam_w);
			ConfigManager()->ImportParam("Spam_S", cvars.keyspam_s);
			ConfigManager()->ImportParam("Spam_Q", cvars.keyspam_q);
			ConfigManager()->ImportParam("Spam_CTRL", cvars.keyspam_ctrl);

			ConfigManager()->EndSectionImport();
		}
	
		if (ConfigManager()->BeginSectionImport("FOG"))
		{
			ConfigManager()->ImportParam("Enable", cvars.fog);
			ConfigManager()->ImportParam("FogSkybox", cvars.fog_skybox);
			ConfigManager()->ImportParam("RemoveInWater", cvars.remove_water_fog);
			ConfigManager()->ImportParam("Start", cvars.fog_start);
			ConfigManager()->ImportParam("End", cvars.fog_end);
			ConfigManager()->ImportParam("Density", cvars.fog_density);
			ConfigManager()->ImportParam("Fog_R", cvars.fog_color[0]);
			ConfigManager()->ImportParam("Fog_G", cvars.fog_color[1]);
			ConfigManager()->ImportParam("Fog_B", cvars.fog_color[2]);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("SKYBOX"))
		{
			ConfigManager()->ImportParam("Type", cvars.skybox);

			ConfigManager()->EndSectionImport();
		}
	
		if (ConfigManager()->BeginSectionImport("CHATCOLORS"))
		{
			ConfigManager()->ImportParam("Enable", cvars.enable_chat_colors);
			ConfigManager()->ImportParam("PlayerName_R", cvars.player_name_color[0]);
			ConfigManager()->ImportParam("PlayerName_G", cvars.player_name_color[1]);
			ConfigManager()->ImportParam("PlayerName_B", cvars.player_name_color[2]);
			ConfigManager()->ImportParam("RainbowUpdateDelay", cvars.chat_rainbow_update_delay);
			ConfigManager()->ImportParam("RainbowHueDelta", cvars.chat_rainbow_hue_delta);
			ConfigManager()->ImportParam("RainbowSaturation", cvars.chat_rainbow_saturation);
			ConfigManager()->ImportParam("RainbowLightness", cvars.chat_rainbow_lightness);
			ConfigManager()->ImportParam("ColorOne_R", cvars.chat_color_one[0]);
			ConfigManager()->ImportParam("ColorOne_G", cvars.chat_color_one[1]);
			ConfigManager()->ImportParam("ColorOne_B", cvars.chat_color_one[2]);
			ConfigManager()->ImportParam("ColorTwo_R", cvars.chat_color_two[0]);
			ConfigManager()->ImportParam("ColorTwo_G", cvars.chat_color_two[1]);
			ConfigManager()->ImportParam("ColorTwo_B", cvars.chat_color_two[2]);
			ConfigManager()->ImportParam("ColorThree_R", cvars.chat_color_three[0]);
			ConfigManager()->ImportParam("ColorThree_G", cvars.chat_color_three[1]);
			ConfigManager()->ImportParam("ColorThree_B", cvars.chat_color_three[2]);
			ConfigManager()->ImportParam("ColorFour_R", cvars.chat_color_four[0]);
			ConfigManager()->ImportParam("ColorFour_G", cvars.chat_color_four[1]);
			ConfigManager()->ImportParam("ColorFour_B", cvars.chat_color_four[2]);
			ConfigManager()->ImportParam("ColorFive_R", cvars.chat_color_five[0]);
			ConfigManager()->ImportParam("ColorFive_G", cvars.chat_color_five[1]);
			ConfigManager()->ImportParam("ColorFive_B", cvars.chat_color_five[2]);
			ConfigManager()->ImportParam("ColorSix_R", cvars.chat_color_six[0]);
			ConfigManager()->ImportParam("ColorSix_G", cvars.chat_color_six[1]);
			ConfigManager()->ImportParam("ColorSix_B", cvars.chat_color_six[2]);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("CAMHACK"))
		{
			ConfigManager()->ImportParam("SpeedFactor", cvars.camhack_speed_factor);
			ConfigManager()->ImportParam("HideHUD", cvars.camhack_hide_hud);
			ConfigManager()->ImportParam("ShowModel", cvars.camhack_show_model);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("FPROAMING"))
		{
			ConfigManager()->ImportParam("Enable", cvars.fp_roaming);
			ConfigManager()->ImportParam("Crosshair", cvars.fp_roaming_draw_crosshair);
			ConfigManager()->ImportParam("Lerp", cvars.fp_roaming_lerp);
			ConfigManager()->ImportParam("LerpValue", cvars.fp_roaming_lerp_value);

			ConfigManager()->EndSectionImport();
		}

		if (ConfigManager()->BeginSectionImport("VOTEPOPUP"))
		{
			ConfigManager()->ImportParam("Enable", cvars.vote_popup);
			ConfigManager()->ImportParam("WidthSize", cvars.vote_popup_width_size);
			ConfigManager()->ImportParam("HeightSize", cvars.vote_popup_height_size);
			ConfigManager()->ImportParam("WidthBorderPixels", cvars.vote_popup_w_border_pix);
			ConfigManager()->ImportParam("HeightBorderPixels", cvars.vote_popup_h_border_pix);
			ConfigManager()->ImportParam("WidthFraction", cvars.vote_popup_width_frac);
			ConfigManager()->ImportParam("HeightFraction", cvars.vote_popup_height_frac);

			ConfigManager()->EndSectionImport();
		}

		//if (ConfigManager()->BeginSectionImport("AUTOVOTE"))
		//{
		//	ConfigManager()->ImportParam("Mode", cvars.autovote_mode);
		//	ConfigManager()->ImportParam("UseOnCustomVotes", cvars.autovote_custom);
		//	ConfigManager()->ImportParam("IgnoreVoteFilter", cvars.autovote_ignore_filter);

		//	ConfigManager()->EndSectionImport();
		//}

		ConfigManager()->EndImport();

		// Callbacks
		g_Skybox.OnConfigLoad();

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Export config vars
//-----------------------------------------------------------------------------

void CConfig::Save()
{
	if (current_config.empty())
		return;

	if (ConfigManager()->BeginExport( (std::string("sven_internal/config/") + current_config).c_str()))
	{
		if (ConfigManager()->BeginSectionExport("SETTINGS"))
		{
			ConfigManager()->SetConversionRadix(16);
			ConfigManager()->ExportParam("ToggleButton", *(uint32 *)&cvars.toggle_button);
			ConfigManager()->ResetRadix();

			ConfigManager()->ExportParam("AutoResize", cvars.menu_auto_resize);
			ConfigManager()->ExportParam("Theme", cvars.menu_theme);
			ConfigManager()->ExportParam("Opacity", cvars.menu_opacity);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("ESP"))
		{
			ConfigManager()->ExportParam("Enable", cvars.esp);
			ConfigManager()->ExportParam("Distance", cvars.esp_distance);
			ConfigManager()->ExportParam("Box", cvars.esp_box);
			ConfigManager()->ExportParam("Outline", cvars.esp_box_outline);
			ConfigManager()->ExportParam("Fill", cvars.esp_box_fill);
			ConfigManager()->ExportParam("ShowIndex", cvars.esp_box_index);
			ConfigManager()->ExportParam("ShowDistance", cvars.esp_box_distance);
			ConfigManager()->ExportParam("ShowPlayerHealth", cvars.esp_box_player_health);
			ConfigManager()->ExportParam("ShowPlayerArmor", cvars.esp_box_player_armor);
			ConfigManager()->ExportParam("ShowEntityName", cvars.esp_box_entity_name);
			ConfigManager()->ExportParam("ShowPlayerName", cvars.esp_box_player_name);
			ConfigManager()->ExportParam("ShowItems", cvars.esp_show_items);
			ConfigManager()->ExportParam("IgnoreUnknownEnts", cvars.esp_ignore_unknown_ents);
			ConfigManager()->ExportParam("Targets", cvars.esp_targets);
			ConfigManager()->ExportParam("ShowSkeleton", cvars.esp_skeleton);
			ConfigManager()->ExportParam("ShowBonesName", cvars.esp_bones_name);
			ConfigManager()->ExportParam("ShowSkeletonType", cvars.esp_skeleton_type);
			ConfigManager()->ExportParam("FriendColor_R", cvars.esp_friend_color[0]);
			ConfigManager()->ExportParam("FriendColor_G", cvars.esp_friend_color[1]);
			ConfigManager()->ExportParam("FriendColor_B", cvars.esp_friend_color[2]);
			ConfigManager()->ExportParam("EnemyColor_R", cvars.esp_enemy_color[0]);
			ConfigManager()->ExportParam("EnemyColor_G", cvars.esp_enemy_color[1]);
			ConfigManager()->ExportParam("EnemyColor_B", cvars.esp_enemy_color[2]);
			ConfigManager()->ExportParam("NeutralColor_R", cvars.esp_neutral_color[0]);
			ConfigManager()->ExportParam("NeutralColor_G", cvars.esp_neutral_color[1]);
			ConfigManager()->ExportParam("NeutralColor_B", cvars.esp_neutral_color[2]);
			ConfigManager()->ExportParam("ItemColor_R", cvars.esp_item_color[0]);
			ConfigManager()->ExportParam("ItemColor_G", cvars.esp_item_color[1]);
			ConfigManager()->ExportParam("ItemColor_B", cvars.esp_item_color[2]);

			ConfigManager()->EndSectionExport();
		}
		
		if (ConfigManager()->BeginSectionExport("WALLHACK"))
		{
			ConfigManager()->ExportParam("Wallhack", cvars.wallhack);
			ConfigManager()->ExportParam("Negative", cvars.wallhack_negative);
			ConfigManager()->ExportParam("WhiteWalls", cvars.wallhack_white_walls);
			ConfigManager()->ExportParam("Wireframe", cvars.wallhack_wireframe);
			ConfigManager()->ExportParam("WireframeModels", cvars.wallhack_wireframe_models);
			ConfigManager()->ExportParam("Wireframe_Width", cvars.wh_wireframe_width);
			ConfigManager()->ExportParam("Wireframe_R", cvars.wh_wireframe_color[0]);
			ConfigManager()->ExportParam("Wireframe_G", cvars.wh_wireframe_color[2]);
			ConfigManager()->ExportParam("Wireframe_B", cvars.wh_wireframe_color[1]);

			ConfigManager()->EndSectionExport();
		}
	
		if (ConfigManager()->BeginSectionExport("CROSSHAIR"))
		{
			ConfigManager()->ExportParam("Enable", cvars.draw_crosshair);
			ConfigManager()->ExportParam("EnableDot", cvars.draw_crosshair_dot);
			ConfigManager()->ExportParam("EnableOutline", cvars.draw_crosshair_outline);
			ConfigManager()->ExportParam("Size", cvars.crosshair_size);
			ConfigManager()->ExportParam("Gap", cvars.crosshair_gap);
			ConfigManager()->ExportParam("Thickness", cvars.crosshair_thickness);
			ConfigManager()->ExportParam("OutlineThickness", cvars.crosshair_outline_thickness);
			ConfigManager()->ExportParam("OutlineColor_R", cvars.crosshair_outline_color[0]);
			ConfigManager()->ExportParam("OutlineColor_G", cvars.crosshair_outline_color[1]);
			ConfigManager()->ExportParam("OutlineColor_B", cvars.crosshair_outline_color[2]);
			ConfigManager()->ExportParam("OutlineColor_A", cvars.crosshair_outline_color[3]);
			ConfigManager()->ExportParam("Color_R", cvars.crosshair_color[0]);
			ConfigManager()->ExportParam("Color_G", cvars.crosshair_color[1]);
			ConfigManager()->ExportParam("Color_B", cvars.crosshair_color[2]);
			ConfigManager()->ExportParam("Color_A", cvars.crosshair_color[3]);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("VISUAL"))
		{
			ConfigManager()->ExportParam("NoShake", cvars.no_shake);
			ConfigManager()->ExportParam("NoFade", cvars.no_fade);
			ConfigManager()->ExportParam("DrawEntities", cvars.draw_entities);

			ConfigManager()->ExportParam("ShowSpeed", cvars.show_speed);
			ConfigManager()->ExportParam("ShowJumpSpeed", cvars.show_jumpspeed);
			ConfigManager()->ExportParam("StoreVerticalSpeed", cvars.show_vertical_speed);
			ConfigManager()->ExportParam("JumpSpeedFadeDuration", cvars.jumpspeed_fade_duration);
			ConfigManager()->ExportParam("SpeedWidthFraction", cvars.speed_width_fraction);
			ConfigManager()->ExportParam("SpeedHeightFraction", cvars.speed_height_fraction);
			ConfigManager()->ExportParam("SpeedColor_R", cvars.speed_color[0]);
			ConfigManager()->ExportParam("SpeedColor_G", cvars.speed_color[1]);
			ConfigManager()->ExportParam("SpeedColor_B", cvars.speed_color[2]);

			ConfigManager()->ExportParam("ShowSpeed_Legacy", cvars.show_speed_legacy);
			ConfigManager()->ExportParam("StoreVerticalSpeed_Legacy", cvars.show_vertical_speed_legacy);
			ConfigManager()->ExportParam("SpeedWidthFraction_Legacy", cvars.speed_width_fraction_legacy);
			ConfigManager()->ExportParam("SpeedHeightFraction_Legacy", cvars.speed_height_fraction_legacy);
			ConfigManager()->ExportParam("SpeedColor_Legacy_R", cvars.speed_color_legacy[0]);
			ConfigManager()->ExportParam("SpeedColor_Legacy_G", cvars.speed_color_legacy[1]);
			ConfigManager()->ExportParam("SpeedColor_Legacy_B", cvars.speed_color_legacy[2]);
			ConfigManager()->ExportParam("SpeedColor_Legacy_A", cvars.speed_color_legacy[3]);

			ConfigManager()->ExportParam("LightmapOverride", cvars.lightmap_override);
			ConfigManager()->ExportParam("LightmapOverrideBrightness", cvars.lightmap_brightness);
			ConfigManager()->ExportParam("LightmapOverride_R", cvars.lightmap_color[0]);
			ConfigManager()->ExportParam("LightmapOverride_G", cvars.lightmap_color[1]);
			ConfigManager()->ExportParam("LightmapOverride_B", cvars.lightmap_color[2]);
			ConfigManager()->ExportParam("ShowPlayersPushDirection", cvars.show_players_push_direction);
			ConfigManager()->ExportParam("PushDirectionLength", cvars.push_direction_length);
			ConfigManager()->ExportParam("PushDirectionWidth", cvars.push_direction_width);
			ConfigManager()->ExportParam("PushDirectionColor_R", cvars.push_direction_color[0]);
			ConfigManager()->ExportParam("PushDirectionColor_G", cvars.push_direction_color[1]);
			ConfigManager()->ExportParam("PushDirectionColor_B", cvars.push_direction_color[2]);
			ConfigManager()->ExportParam("RemapHUDColor", cvars.remap_hud_color);
			ConfigManager()->ExportParam("HUDColor_R", cvars.hud_color[0]);
			ConfigManager()->ExportParam("HUDColor_G", cvars.hud_color[1]);
			ConfigManager()->ExportParam("HUDColor_B", cvars.hud_color[2]);

			ConfigManager()->EndSectionExport();
		}
	
		if (ConfigManager()->BeginSectionExport("CHAMS"))
		{
			ConfigManager()->ExportParam("Enable", cvars.chams);
			ConfigManager()->ExportParam("ChamsPlayers", cvars.chams_players);
			ConfigManager()->ExportParam("ChamsEntities", cvars.chams_entities);
			ConfigManager()->ExportParam("ChamsItems", cvars.chams_items);
			ConfigManager()->ExportParam("ChamsPlayersWall", cvars.chams_players_wall);
			ConfigManager()->ExportParam("ChamsEntitiesWall", cvars.chams_entities_wall);
			ConfigManager()->ExportParam("ChamsItemsWall", cvars.chams_items_wall);
			ConfigManager()->ExportParam("ChamsPlayersColor_R", cvars.chams_players_color[0]);
			ConfigManager()->ExportParam("ChamsPlayersColor_G", cvars.chams_players_color[1]);
			ConfigManager()->ExportParam("ChamsPlayersColor_B", cvars.chams_players_color[2]);
			ConfigManager()->ExportParam("ChamsEntitiesColor_R", cvars.chams_entities_color[0]);
			ConfigManager()->ExportParam("ChamsEntitiesColor_G", cvars.chams_entities_color[1]);
			ConfigManager()->ExportParam("ChamsEntitiesColor_B", cvars.chams_entities_color[2]);
			ConfigManager()->ExportParam("ChamsItemsColor_R", cvars.chams_items_color[0]);
			ConfigManager()->ExportParam("ChamsItemsColor_G", cvars.chams_items_color[1]);
			ConfigManager()->ExportParam("ChamsItemsColor_B", cvars.chams_items_color[2]);
			ConfigManager()->ExportParam("ChamsPlayersWallColor_R", cvars.chams_players_wall_color[0]);
			ConfigManager()->ExportParam("ChamsPlayersWallColor_G", cvars.chams_players_wall_color[1]);
			ConfigManager()->ExportParam("ChamsPlayersWallColor_B", cvars.chams_players_wall_color[2]);
			ConfigManager()->ExportParam("ChamsEntitiesWallColor_R", cvars.chams_entities_wall_color[0]);
			ConfigManager()->ExportParam("ChamsEntitiesWallColor_G", cvars.chams_entities_wall_color[1]);
			ConfigManager()->ExportParam("ChamsEntitiesWallColor_B", cvars.chams_entities_wall_color[2]);
			ConfigManager()->ExportParam("ChamsItemsWallColor_R", cvars.chams_items_wall_color[0]);
			ConfigManager()->ExportParam("ChamsItemsWallColor_G", cvars.chams_items_wall_color[1]);
			ConfigManager()->ExportParam("ChamsItemsWallColor_B", cvars.chams_items_wall_color[2]);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("GLOW"))
		{
			ConfigManager()->ExportParam("Enable", cvars.glow);
			ConfigManager()->ExportParam("Optimize", cvars.glow_optimize);
			ConfigManager()->ExportParam("GlowPlayers", cvars.glow_players);
			ConfigManager()->ExportParam("GlowEntities", cvars.glow_entities);
			ConfigManager()->ExportParam("GlowItems", cvars.glow_items);
			ConfigManager()->ExportParam("GlowPlayersWidth", cvars.glow_players_width);
			ConfigManager()->ExportParam("GlowEntitiesWidth", cvars.glow_entities_width);
			ConfigManager()->ExportParam("GlowItemsWidth", cvars.glow_items_width);
			ConfigManager()->ExportParam("GlowPlayersWall", cvars.glow_players_wall);
			ConfigManager()->ExportParam("GlowEntitiesWall", cvars.glow_entities_wall);
			ConfigManager()->ExportParam("GlowItemsWall", cvars.glow_items_wall);
			ConfigManager()->ExportParam("GlowPlayersColor_R", cvars.glow_players_color[0]);
			ConfigManager()->ExportParam("GlowPlayersColor_G", cvars.glow_players_color[1]);
			ConfigManager()->ExportParam("GlowPlayersColor_B", cvars.glow_players_color[2]);
			ConfigManager()->ExportParam("GlowEntitiesColor_R", cvars.glow_entities_color[0]);
			ConfigManager()->ExportParam("GlowEntitiesColor_G", cvars.glow_entities_color[1]);
			ConfigManager()->ExportParam("GlowEntitiesColor_B", cvars.glow_entities_color[2]);
			ConfigManager()->ExportParam("GlowItemsColor_R", cvars.glow_items_color[0]);
			ConfigManager()->ExportParam("GlowItemsColor_G", cvars.glow_items_color[1]);
			ConfigManager()->ExportParam("GlowItemsColor_B", cvars.glow_items_color[2]);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("DYNAMICGLOW"))
		{
			ConfigManager()->ExportParam("GlowAttach", cvars.dyn_glow_attach);
			ConfigManager()->ExportParam("GlowSelf", cvars.dyn_glow_self);
			ConfigManager()->ExportParam("GlowSelfRadius", cvars.dyn_glow_self_radius);
			ConfigManager()->ExportParam("GlowSelfDecay", cvars.dyn_glow_self_decay);
			ConfigManager()->ExportParam("GlowSelfColor_R", cvars.dyn_glow_self_color[0]);
			ConfigManager()->ExportParam("GlowSelfColor_G", cvars.dyn_glow_self_color[1]);
			ConfigManager()->ExportParam("GlowSelfColor_B", cvars.dyn_glow_self_color[2]);
			ConfigManager()->ExportParam("GlowPlayers", cvars.dyn_glow_players);
			ConfigManager()->ExportParam("GlowPlayersRadius", cvars.dyn_glow_players_radius);
			ConfigManager()->ExportParam("GlowPlayersDecay", cvars.dyn_glow_players_decay);
			ConfigManager()->ExportParam("GlowPlayersColor_R", cvars.dyn_glow_players_color[0]);
			ConfigManager()->ExportParam("GlowPlayersColor_G", cvars.dyn_glow_players_color[1]);
			ConfigManager()->ExportParam("GlowPlayersColor_B", cvars.dyn_glow_players_color[2]);
			ConfigManager()->ExportParam("GlowEntities", cvars.dyn_glow_entities);
			ConfigManager()->ExportParam("GlowEntitiesRadius", cvars.dyn_glow_entities_radius);
			ConfigManager()->ExportParam("GlowEntitiesDecay", cvars.dyn_glow_entities_decay);
			ConfigManager()->ExportParam("GlowEntitiesColor_R", cvars.dyn_glow_entities_color[0]);
			ConfigManager()->ExportParam("GlowEntitiesColor_G", cvars.dyn_glow_entities_color[1]);
			ConfigManager()->ExportParam("GlowEntitiesColor_B", cvars.dyn_glow_entities_color[2]);
			ConfigManager()->ExportParam("GlowItems", cvars.dyn_glow_items);
			ConfigManager()->ExportParam("GlowItemsRadius", cvars.dyn_glow_items_radius);
			ConfigManager()->ExportParam("GlowItemsDecay", cvars.dyn_glow_items_decay);
			ConfigManager()->ExportParam("GlowItemsColor_R", cvars.dyn_glow_items_color[0]);
			ConfigManager()->ExportParam("GlowItemsColor_G", cvars.dyn_glow_items_color[1]);
			ConfigManager()->ExportParam("GlowItemsColor_B", cvars.dyn_glow_items_color[2]);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("STRAFE"))
		{
			ConfigManager()->ExportParam("Enable", cvars.strafe);
			ConfigManager()->ExportParam("IgnoreGround", cvars.strafe_ignore_ground);
			ConfigManager()->ExportParam("Direction", cvars.strafe_dir);
			ConfigManager()->ExportParam("Type", cvars.strafe_type);

			ConfigManager()->EndSectionExport();
		}
	
		if (ConfigManager()->BeginSectionExport("FAKELAG"))
		{
			ConfigManager()->ExportParam("Enable", cvars.fakelag);
			ConfigManager()->ExportParam("AdaptiveInterp", cvars.fakelag_adaptive_ex_interp);
			ConfigManager()->ExportParam("Type", cvars.fakelag_type);
			ConfigManager()->ExportParam("Move", cvars.fakelag_move);
			ConfigManager()->ExportParam("Limit", cvars.fakelag_limit);
			ConfigManager()->ExportParam("Variance", cvars.fakelag_variance);

			ConfigManager()->EndSectionExport();
		}
	
		if (ConfigManager()->BeginSectionExport("ANTIAFK"))
		{
			ConfigManager()->ExportParam("Type", cvars.antiafk);
			ConfigManager()->ExportParam("RotateCamera", cvars.antiafk_rotate_camera);
			ConfigManager()->ExportParam("StayWithinRange", cvars.antiafk_stay_within_range);
			ConfigManager()->ExportParam("ResetStayPos", cvars.antiafk_reset_stay_pos);
			ConfigManager()->ExportParam("RotationAngle", cvars.antiafk_rotation_angle);
			ConfigManager()->ExportParam("StayRadius", cvars.antiafk_stay_radius);
			ConfigManager()->ExportParam("StayRadiusOffsetAngle", cvars.antiafk_stay_radius_offset_angle);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("MISC"))
		{
			ConfigManager()->ExportParam("AutoJump", cvars.autojump);
			ConfigManager()->ExportParam("JumpBug", cvars.jumpbug);
			ConfigManager()->ExportParam("DoubleDuck", cvars.doubleduck);
			ConfigManager()->ExportParam("FastRun", cvars.fastrun);
			ConfigManager()->ExportParam("QuakeGuns", cvars.quake_guns);
			ConfigManager()->ExportParam("TertiaryAttackGlitch", cvars.tertiary_attack_glitch);
			ConfigManager()->ExportParam("SaveSoundcache", cvars.save_soundcache);
			ConfigManager()->ExportParam("RotateDeadBody", cvars.rotate_dead_body);
			ConfigManager()->ExportParam("AutoCeilClipping", cvars.auto_ceil_clipping);
			ConfigManager()->ExportParam("FastCrowbar", cvars.fast_crowbar);
			ConfigManager()->ExportParam("FastRevive", cvars.fast_medkit);
			ConfigManager()->ExportParam("RemoveFOVCap", cvars.remove_fov_cap);
			ConfigManager()->ExportParam("NoWeaponAnim", cvars.no_weapon_anim);
			ConfigManager()->ExportParam("LockPitch", cvars.lock_pitch);
			ConfigManager()->ExportParam("LockYaw", cvars.lock_yaw);
			ConfigManager()->ExportParam("LockPitchAngle", cvars.lock_pitch_angle);
			ConfigManager()->ExportParam("LockYawAngle", cvars.lock_yaw_angle);
			ConfigManager()->ExportParam("SpinYaw", cvars.spin_yaw_angle);
			ConfigManager()->ExportParam("SpinPitch", cvars.spin_pitch_angle);
			ConfigManager()->ExportParam("SpinYawAngle", cvars.spin_yaw_rotation_angle);
			ConfigManager()->ExportParam("SpinPitchAngle", cvars.spin_pitch_rotation_angle);
			ConfigManager()->ExportParam("ColorPulsator", cvars.color_pulsator);
			ConfigManager()->ExportParam("ColorPulsatorTop", cvars.color_pulsator_top);
			ConfigManager()->ExportParam("ColorPulsatorBottom", cvars.color_pulsator_bottom);
			ConfigManager()->ExportParam("ColorPulsatorDelay", cvars.color_pulsator_delay);
			ConfigManager()->ExportParam("IgnoreDifferentMapVersions", cvars.ignore_different_map_versions);
			ConfigManager()->ExportParam("OneTickExploit", cvars.one_tick_exploit);
			ConfigManager()->ExportParam("OneTickExploitLagInterval", cvars.one_tick_exploit_lag_interval);
			ConfigManager()->ExportParam("OneTickExploitSpeedhack", cvars.one_tick_exploit_speedhack);
			ConfigManager()->ExportParam("FastCrowbar", cvars.fast_crowbar);
			ConfigManager()->ExportParam("FastCrowbar2", cvars.fast_crowbar2);
			ConfigManager()->ExportParam("FastRevive", cvars.fast_medkit);
			ConfigManager()->ExportParam("ApplicationSpeed", cvars.application_speed);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("KEYSPAM"))
		{
			ConfigManager()->ExportParam("HoldMode", cvars.keyspam_hold_mode);
			ConfigManager()->ExportParam("Spam_E", cvars.keyspam_e);
			ConfigManager()->ExportParam("Spam_W", cvars.keyspam_w);
			ConfigManager()->ExportParam("Spam_S", cvars.keyspam_s);
			ConfigManager()->ExportParam("Spam_Q", cvars.keyspam_q);
			ConfigManager()->ExportParam("Spam_CTRL", cvars.keyspam_ctrl);

			ConfigManager()->EndSectionExport();
		}
	
		if (ConfigManager()->BeginSectionExport("FOG"))
		{
			ConfigManager()->ExportParam("Enable", cvars.fog);
			ConfigManager()->ExportParam("FogSkybox", cvars.fog_skybox);
			ConfigManager()->ExportParam("RemoveInWater", cvars.remove_water_fog);
			ConfigManager()->ExportParam("Start", cvars.fog_start);
			ConfigManager()->ExportParam("End", cvars.fog_end);
			ConfigManager()->ExportParam("Density", cvars.fog_density);
			ConfigManager()->ExportParam("Fog_R", cvars.fog_color[0]);
			ConfigManager()->ExportParam("Fog_G", cvars.fog_color[1]);
			ConfigManager()->ExportParam("Fog_B", cvars.fog_color[2]);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("SKYBOX"))
		{
			ConfigManager()->ExportParam("Type", cvars.skybox);

			ConfigManager()->EndSectionExport();
		}
	
		if (ConfigManager()->BeginSectionExport("CHATCOLORS"))
		{
			ConfigManager()->ExportParam("Enable", cvars.enable_chat_colors);
			ConfigManager()->ExportParam("PlayerName_R", cvars.player_name_color[0]);
			ConfigManager()->ExportParam("PlayerName_G", cvars.player_name_color[1]);
			ConfigManager()->ExportParam("PlayerName_B", cvars.player_name_color[2]);
			ConfigManager()->ExportParam("RainbowUpdateDelay", cvars.chat_rainbow_update_delay);
			ConfigManager()->ExportParam("RainbowHueDelta", cvars.chat_rainbow_hue_delta);
			ConfigManager()->ExportParam("RainbowSaturation", cvars.chat_rainbow_saturation);
			ConfigManager()->ExportParam("RainbowLightness", cvars.chat_rainbow_lightness);
			ConfigManager()->ExportParam("ColorOne_R", cvars.chat_color_one[0]);
			ConfigManager()->ExportParam("ColorOne_G", cvars.chat_color_one[1]);
			ConfigManager()->ExportParam("ColorOne_B", cvars.chat_color_one[2]);
			ConfigManager()->ExportParam("ColorTwo_R", cvars.chat_color_two[0]);
			ConfigManager()->ExportParam("ColorTwo_G", cvars.chat_color_two[1]);
			ConfigManager()->ExportParam("ColorTwo_B", cvars.chat_color_two[2]);
			ConfigManager()->ExportParam("ColorThree_R", cvars.chat_color_three[0]);
			ConfigManager()->ExportParam("ColorThree_G", cvars.chat_color_three[1]);
			ConfigManager()->ExportParam("ColorThree_B", cvars.chat_color_three[2]);
			ConfigManager()->ExportParam("ColorFour_R", cvars.chat_color_four[0]);
			ConfigManager()->ExportParam("ColorFour_G", cvars.chat_color_four[1]);
			ConfigManager()->ExportParam("ColorFour_B", cvars.chat_color_four[2]);
			ConfigManager()->ExportParam("ColorFive_R", cvars.chat_color_five[0]);
			ConfigManager()->ExportParam("ColorFive_G", cvars.chat_color_five[1]);
			ConfigManager()->ExportParam("ColorFive_B", cvars.chat_color_five[2]);
			ConfigManager()->ExportParam("ColorSix_R", cvars.chat_color_six[0]);
			ConfigManager()->ExportParam("ColorSix_G", cvars.chat_color_six[1]);
			ConfigManager()->ExportParam("ColorSix_B", cvars.chat_color_six[2]);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("CAMHACK"))
		{
			ConfigManager()->ExportParam("SpeedFactor", cvars.camhack_speed_factor);
			ConfigManager()->ExportParam("HideHUD", cvars.camhack_hide_hud);
			ConfigManager()->ExportParam("ShowModel", cvars.camhack_show_model);

			ConfigManager()->EndSectionExport();
		}
	
		if (ConfigManager()->BeginSectionExport("FPROAMING"))
		{
			ConfigManager()->ExportParam("Enable", cvars.fp_roaming);
			ConfigManager()->ExportParam("Crosshair", cvars.fp_roaming_draw_crosshair);
			ConfigManager()->ExportParam("Lerp", cvars.fp_roaming_lerp);
			ConfigManager()->ExportParam("LerpValue", cvars.fp_roaming_lerp_value);

			ConfigManager()->EndSectionExport();
		}

		if (ConfigManager()->BeginSectionExport("VOTEPOPUP"))
		{
			ConfigManager()->ExportParam("Enable", cvars.vote_popup);
			ConfigManager()->ExportParam("WidthSize", cvars.vote_popup_width_size);
			ConfigManager()->ExportParam("HeightSize", cvars.vote_popup_height_size);
			ConfigManager()->ExportParam("WidthBorderPixels", cvars.vote_popup_w_border_pix);
			ConfigManager()->ExportParam("HeightBorderPixels", cvars.vote_popup_h_border_pix);
			ConfigManager()->ExportParam("WidthFraction", cvars.vote_popup_width_frac);
			ConfigManager()->ExportParam("HeightFraction", cvars.vote_popup_height_frac);

			ConfigManager()->EndSectionExport();
		}

		//if (ConfigManager()->BeginSectionExport("AUTOVOTE"))
		//{
		//	ConfigManager()->ExportParam("Mode", cvars.autovote_mode);
		//	ConfigManager()->ExportParam("UseOnCustomVotes", cvars.autovote_custom);
		//	ConfigManager()->ExportParam("IgnoreVoteFilter", cvars.autovote_ignore_filter);

		//	ConfigManager()->EndSectionExport();
		//}

		ConfigManager()->EndExport();
	}
}

void CConfig::New()
{
	config_vars v;
	std::string sSavedConfig = current_config;

	memcpy(&v, &cvars, sizeof(config_vars));
	memcpy(&cvars, &default_cvars, sizeof(config_vars));

	current_config = "new_config.ini";

	Save();

	current_config = sSavedConfig;

	memcpy(&cvars, &v, sizeof(config_vars));
}

void CConfig::Remove()
{
	if (current_config.empty())
		return;

	std::string sDir = g_pSvenModAPI->GetBaseDirectory();

#ifdef PLATFORM_WINDOWS
	sDir += "\\sven_internal\\config\\";
#else
	sDir += "/sven_internal/config/";
#endif
	sDir += current_config;

#ifdef PLATFORM_WINDOWS
	DeleteFile(sDir.c_str());
#else
#error Implement Linux equivalent
#endif
}

//-----------------------------------------------------------------------------
// Console commands
//-----------------------------------------------------------------------------

CON_COMMAND(sc_load_config, "Load a config file from folder \"../sven_internal/config/*.ini\"\nUsage:  sc_load_config <optional: filename>")
{
	if (args.ArgC() > 1)
	{
		const char *pszFileName = args[1];

		const char *pszExtension = NULL;
		const char *buffer = pszFileName;

		while (*buffer)
		{
			if (*buffer == '.')
				pszExtension = buffer;

			buffer++;
		}

		if (pszExtension && stricmp(pszExtension, ".ini"))
		{
			Msg("sc_load_config: expected name of the file with \".ini\" extension\n");
			return;
		}

		std::string sPrevConfig;
		std::string sFileName = args[1];

		if (!pszExtension)
		{
			sFileName += ".ini";
		}

		sPrevConfig = g_Config.current_config;
		g_Config.current_config = sFileName;

		if ( !g_Config.Load() )
		{
			g_Config.current_config = sPrevConfig;
		}
		else
		{
			LoadMenuTheme();
			WindowStyle();
		}
	}
	else
	{
		g_Config.Load();
	}
}

CON_COMMAND(sc_save_config, "Save a config to folder \"../sven_internal/config/\"")
{
	g_Config.Save();
}