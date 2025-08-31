#include "patterns.h"

namespace Patterns
{
	namespace Hardware
	{
		DEFINE_PATTERN(flNextCmdTime, "D9 ? ? ? ? ? 75 ? A1");

		DEFINE_PATTERN(Netchan_CanPacket, "D9 ? ? ? ? ? D9 ? DA ? DF E0 F6 C4 ? 8B 44 24 04");
		DEFINE_PATTERN(Netchan_Transmit, "B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 3C 01 01 00");

		DEFINE_PATTERN(V_RenderView, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 F0 01 00 00");
		DEFINE_PATTERN(R_SetupFrame, "83 EC ? A1 ? ? ? ? 33 C4 89 44 24 20 D9 ? ? ? ? ? D9 ? ? ? ? ? DA");
		DEFINE_PATTERN(R_LoadSkyboxInt, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 20 01 00 00 53 8B 9C 24 2C 01 00 00");
		
		DEFINE_PATTERN(CRC_MapFile, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 84 04 00 00");
		DEFINE_PATTERN(UserInfo_Offset, "8D ? ? ? ? ? 50 50 68 ? ? ? ? 8D ? ? ? ? ? C6 81 ? ? ? ? ? 68 04 01 00 00 50 E8 ? ? ? ? 69");
	}

	namespace Client
	{
		DEFINE_PATTERN(IN_Move, "83 3D ? ? ? ? ? 75 ? 83 3D ? ? ? ? ? 74");

		DEFINE_PATTERN(CVotePopup__MsgFunc_VoteMenu, "53 56 57 FF 74 24 14 8B D9 FF 74 24 1C E8 ? ? ? ? E8");
		DEFINE_PATTERN(READ_BYTE, "8B 0D ? ? ? ? 8D 51 01 3B 15 ? ? ? ? 7E ? C7 05 ? ? ? ? ? ? ? ? 83 C8 ? C3 A1 ? ? ? ? 89 15 ? ? ? ? 0F B6 04 08");
		DEFINE_PATTERN(READ_STRING, "8B 15 ? ? ? ? 33 C0");
		
		DEFINE_PATTERN(CHudBaseTextBlock__Print, "55 8B EC 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 53 56 57 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B D9 8B 0D");

		DEFINE_PATTERN(CVoiceBanMgr__SaveState, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 08 01 00 00 8B 84 24 10 01 00 00");
		DEFINE_PATTERN(CVoiceBanMgr__SetPlayerBan, "56 FF 74 24 08 8B F1 E8 ? ? ? ? 80 7C 24 0C");
		DEFINE_PATTERN(CVoiceBanMgr__InternalFindPlayerSquelch, "53 55 8B 6C 24 0C 56");

		DEFINE_PATTERN(CVoiceStatus__IsPlayerBlocked, "83 EC ? A1 ? ? ? ? 33 C4 89 44 24 10 A1 ? ? ? ? 56");
		DEFINE_PATTERN(CVoiceStatus__SetPlayerBlockedState, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 14 01 00 00");
		DEFINE_PATTERN(CVoiceStatus__UpdateServerState, "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 18 0B 00 00");

		DEFINE_PATTERN(HACK_GetPlayerUniqueID, "FF 74 24 08 A1");

		DEFINE_PATTERN(GetClientColor, "8B 4C 24 04 85 C9 7E");

		DEFINE_PATTERN(ScaleColors, "66 0F 6E 4C 24 10");
		DEFINE_PATTERN(ScaleColors_RGBA, "8B 4C 24 04 0F B6 41 03");

		DEFINE_PATTERN(CHud__Think, "83 EC ? 53 56 57 8B F9 E8");

		DEFINE_PATTERN(V_PunchAxis, "8B 44 24 04 F3 0F 10 44 24 08");

		DEFINE_PATTERN(WeaponsResource__GetFirstPos, "6B 54 24 04");
	}

	namespace GameOverlay
	{
		DEFINE_PATTERN(SetCursorPos_Hook, "55 8B EC 80 3D ? ? ? ? 00 74 19 8B 45 08 A3 ? ? ? ? 8B 45 0C A3 ? ? ? ? B8 01");
		DEFINE_PATTERN(ValveUnhookFunc, "55 8B EC C7");
		//DEFINE_PATTERN(ValveUnhookFunc, "55 8B ? 64 ? ? ? ? ? 6A ? 68 ? ? ? ? 50 64 ? ? ? ? ? ? 81 ? ? ? ? ? 56 8B ? ? 85");
	}
}