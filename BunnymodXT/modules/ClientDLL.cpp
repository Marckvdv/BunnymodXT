#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "ClientDLL.hpp"
#include "ServerDLL.hpp"
#include "HwDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"
#include "../triangle_drawing.hpp"
#include <GL/gl.h>

// Linux hooks.
#ifndef _WIN32
extern "C" void __cdecl HUD_Init()
{
	return ClientDLL::HOOKED_HUD_Init();
}

extern "C" void __cdecl HUD_VidInit()
{
	return ClientDLL::HOOKED_HUD_VidInit();
}

extern "C" void __cdecl HUD_Reset()
{
	return ClientDLL::HOOKED_HUD_Reset();
}

extern "C" void __cdecl HUD_Redraw(float time, int intermission)
{
	return ClientDLL::HOOKED_HUD_Redraw(time, intermission);
}

extern "C" void __cdecl HUD_PostRunCmd(local_state_s* from, local_state_s* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	return ClientDLL::HOOKED_HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
}

extern "C" void __cdecl HUD_Frame(double time)
{
	return ClientDLL::HOOKED_HUD_Frame(time);
}

extern "C" void __cdecl V_CalcRefdef(ref_params_t* pparams)
{
	return ClientDLL::HOOKED_V_CalcRefdef(pparams);
}

extern "C" void __cdecl HUD_DrawTransparentTriangles()
{
	return ClientDLL::HOOKED_HUD_DrawTransparentTriangles();
}

extern "C" int __cdecl HUD_Key_Event(int down, int keynum, const char* pszCurrentBinding)
{
	return ClientDLL::HOOKED_HUD_Key_Event(down, keynum, pszCurrentBinding);
}

extern "C" int __cdecl HUD_UpdateClientData(client_data_t* pcldata, float flTime)
{
	return ClientDLL::HOOKED_HUD_UpdateClientData(pcldata, flTime);
}
#endif

void ClientDLL::Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept)
{
	Clear(); // Just in case.

	m_Handle = moduleHandle;
	m_Base = moduleBase;
	m_Length = moduleLength;
	m_Name = moduleName;
	m_Intercepted = needToIntercept;

	FindStuff();
	RegisterCVarsAndCommands();

	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_VidInit), reinterpret_cast<void*>(HOOKED_HUD_VidInit));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Reset), reinterpret_cast<void*>(HOOKED_HUD_Reset));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Redraw), reinterpret_cast<void*>(HOOKED_HUD_Redraw));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_PostRunCmd), reinterpret_cast<void*>(HOOKED_HUD_PostRunCmd));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Frame), reinterpret_cast<void*>(HOOKED_HUD_Frame));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_DrawTransparentTriangles), reinterpret_cast<void*>(HOOKED_HUD_DrawTransparentTriangles));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Key_Event), reinterpret_cast<void*>(HOOKED_HUD_Key_Event));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_UpdateClientData), reinterpret_cast<void*>(HOOKED_HUD_UpdateClientData));

	if (needToIntercept)
	{
		MemUtils::Intercept(moduleName,
			ORIG_PM_Jump, HOOKED_PM_Jump,
			ORIG_PM_PreventMegaBunnyJumping, HOOKED_PM_PreventMegaBunnyJumping,
			ORIG_V_CalcRefdef, HOOKED_V_CalcRefdef,
			ORIG_HUD_Init, HOOKED_HUD_Init,
			ORIG_HUD_VidInit, HOOKED_HUD_VidInit,
			ORIG_HUD_Reset, HOOKED_HUD_Reset,
			ORIG_HUD_Redraw, HOOKED_HUD_Redraw,
			ORIG_HUD_PostRunCmd, HOOKED_HUD_PostRunCmd,
			ORIG_HUD_Frame, HOOKED_HUD_Frame,
			ORIG_HUD_DrawTransparentTriangles, HOOKED_HUD_DrawTransparentTriangles,
			ORIG_HUD_Key_Event, HOOKED_HUD_Key_Event,
			ORIG_HUD_UpdateClientData, HOOKED_HUD_UpdateClientData);
	}

	// HACK: on Windows we don't get a LoadLibrary for SDL2, so when starting using the injector
	// we never see it loaded. Try loading it here.
	Hooks::HookModule(L"SDL2.dll");
}

void ClientDLL::Unhook()
{
	if (m_Intercepted)
	{
		MemUtils::RemoveInterception(m_Name,
			ORIG_PM_Jump,
			ORIG_PM_PreventMegaBunnyJumping,
			ORIG_V_CalcRefdef,
			ORIG_HUD_Init,
			ORIG_HUD_VidInit,
			ORIG_HUD_Reset,
			ORIG_HUD_Redraw,
			ORIG_HUD_PostRunCmd,
			ORIG_HUD_Frame,
			ORIG_HUD_DrawTransparentTriangles,
			ORIG_HUD_Key_Event,
			ORIG_HUD_UpdateClientData);
	}

	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Init));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_VidInit));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Reset));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Redraw));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_PostRunCmd));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Frame));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_DrawTransparentTriangles));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Key_Event));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_UpdateClientData));

	Clear();
}

void ClientDLL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PlayerMove = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_PM_ClipVelocity = nullptr;
	ORIG_PM_WaterMove = nullptr;
	ORIG_PM_Move = nullptr;
	ORIG_V_CalcRefdef = nullptr;
	ORIG_HUD_Init = nullptr;
	ORIG_HUD_VidInit = nullptr;
	ORIG_HUD_Reset = nullptr;
	ORIG_HUD_Redraw = nullptr;
	ORIG_HUD_PostRunCmd = nullptr;
	ORIG_HUD_Frame = nullptr;
	ORIG_HUD_DrawTransparentTriangles = nullptr;
	ORIG_HUD_Key_Event = nullptr;
	ORIG_HUD_UpdateClientData = nullptr;
	ORIG_IN_ActivateMouse = nullptr;
	ORIG_IN_DeactivateMouse = nullptr;
	ppmove = nullptr;
	offOldbuttons = 0;
	offOnground = 0;
	offBhopcap = 0;
	memset(originalBhopcapInsn, 0, sizeof(originalBhopcapInsn));
	pEngfuncs = nullptr;
	cantJumpNextTime = false;
	SeedsQueued = 0;
	m_Intercepted = false;
	last_vieworg = Vector();
	last_viewangles = Vector();
	last_buttons = 0;
}

void ClientDLL::FindStuff()
{
	auto fPM_PreventMegaBunnyJumping = FindFunctionAsync(
		ORIG_PM_PreventMegaBunnyJumping,
		"PM_PreventMegaBunnyJumping",
		patterns::shared::PM_PreventMegaBunnyJumping);

	auto fPM_Jump = FindFunctionAsync(
		ORIG_PM_Jump,
		"PM_Jump",
		patterns::shared::PM_Jump,
		[&](auto pattern) {
			offOldbuttons = 200;
			offOnground = 224;
			if (pattern == patterns::shared::PM_Jump.cend()) // Linux.
			{
				void *bhopcapAddr;
				auto n = MemUtils::find_unique_sequence(
					m_Base,
					m_Length,
					patterns::shared::Bhopcap.cbegin(),
					patterns::shared::Bhopcap.cend(),
					bhopcapAddr);
				if (n != patterns::shared::Bhopcap.cend())
				{
					offBhopcap = reinterpret_cast<ptrdiff_t>(bhopcapAddr) - reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump) + 27;
					memcpy(originalBhopcapInsn, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bhopcapAddr) + 27), sizeof(originalBhopcapInsn));
				}
			}
			else
			{
				switch (pattern - patterns::shared::PM_Jump.cbegin())
				{
				case 0:
				case 1:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 2);
					break;

				case 2: // AG-Server, shouldn't happen here but who knows.
				case 3:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 3);
					break;

				case 4:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 21);
					break;
				}
			}
		});

	ORIG_PM_PlayerMove = reinterpret_cast<_PM_PlayerMove>(MemUtils::GetSymbolAddress(m_Handle, "PM_PlayerMove")); // For Linux.
	ORIG_PM_ClipVelocity = reinterpret_cast<_PM_ClipVelocity>(MemUtils::GetSymbolAddress(m_Handle, "PM_ClipVelocity")); // For Linux.
	ORIG_PM_WaterMove = reinterpret_cast<_PM_WaterMove>(MemUtils::GetSymbolAddress(m_Handle, "PM_WaterMove")); // For Linux.
	ORIG_PM_Move = reinterpret_cast<_PM_Move>(MemUtils::GetSymbolAddress(m_Handle, "PM_Move")); // For Linux.

	pEngfuncs = reinterpret_cast<cl_enginefunc_t*>(MemUtils::GetSymbolAddress(m_Handle, "gEngfuncs"));
	if (pEngfuncs) {
		EngineDevMsg("[client dll] pEngfuncs is %p.\n", pEngfuncs);
	} else {
		// In AG, this thing is the main function, so check that first.
		auto pInitialize = MemUtils::GetSymbolAddress(m_Handle, "?Initialize_Body@@YAHPAUcl_enginefuncs_s@@H@Z");
		if (!pInitialize)
			pInitialize = MemUtils::GetSymbolAddress(m_Handle, "Initialize");
		if (pInitialize)
		{
			EngineDevMsg("Found Initialize at %p.\n", pInitialize);

			// In some cases Initialize contains just a jump to the real function (Residual Life).
			if (*reinterpret_cast<byte*>(pInitialize) == 0xE9) {
				pInitialize = reinterpret_cast<void*>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(pInitialize) + 1)
					+ reinterpret_cast<uintptr_t>(pInitialize) + 5);
				EngineDevMsg("Jump detected, found the real Initialize at %p.\n", pInitialize);
			}

			// Find "mov edi, offset dword; rep movsd" inside Initialize. The pointer to gEngfuncs is that dword.
			static constexpr auto p = PATTERN("BF ?? ?? ?? ?? F3 A5");
			auto addr = MemUtils::find_pattern(pInitialize, 40, p);
			if (!addr) {
				static constexpr auto p = PATTERN("B9 ?? ?? ?? ?? 8B 54 24 10");
				addr = MemUtils::find_pattern(pInitialize, 40, p);
			}

			if (addr)
			{
				pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(addr+1);
				EngineDevMsg("[client dll] pEngfuncs is %p.\n", pEngfuncs);
			}
			else
			{
				EngineDevWarning("[client dll] Couldn't find the pattern in Initialize.\n");
				EngineWarning("Custom HUD is not available.\n");
				EngineWarning("Clientside logging is not available.\n");
			}
		}
		else
		{
			EngineDevWarning("[client dll] Couldn't get the address of Initialize.\n");
			EngineWarning("Custom HUD is not available.\n");
			EngineWarning("Clientside logging is not available.\n");
		}
	}

	// We can draw stuff only if we know that we have already received / will receive engfuncs.
	if (pEngfuncs) {
		if (!FindHUDFunctions()) {
			ORIG_HUD_Init = nullptr;
			ORIG_HUD_VidInit = nullptr;
			ORIG_HUD_Reset = nullptr;
			ORIG_HUD_Redraw = nullptr;
			EngineWarning("Custom HUD is not available.\n");
		}
	}

	ORIG_V_CalcRefdef = reinterpret_cast<_V_CalcRefdef>(MemUtils::GetSymbolAddress(m_Handle, "V_CalcRefdef"));
	if (ORIG_V_CalcRefdef) {
		EngineDevMsg("[client dll] Found V_CalcRefdef at %p.\n", ORIG_V_CalcRefdef);
	} else {
		EngineDevWarning("[client dll] Could not find V_CalcRefdef.\n");
		EngineWarning("Velocity display during demo playback is not available.\n");
	}

	ORIG_HUD_PostRunCmd = reinterpret_cast<_HUD_PostRunCmd>(MemUtils::GetSymbolAddress(m_Handle, "HUD_PostRunCmd"));
	if (ORIG_HUD_PostRunCmd)
		EngineDevMsg("[client dll] Found HUD_PostRunCmd at %p.\n", ORIG_HUD_PostRunCmd);
	else
		EngineDevWarning("[client dll] Could not find HUD_PostRunCmd.\n");

	ORIG_HUD_Frame = reinterpret_cast<_HUD_Frame>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Frame"));
	if (ORIG_HUD_Frame) {
		EngineDevMsg("[client dll] Found HUD_Frame at %p.\n", ORIG_HUD_Frame);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_Frame.\n");
		EngineWarning("In-game timer is not available.\n");
	}

	ORIG_HUD_DrawTransparentTriangles = reinterpret_cast<_HUD_DrawTransparentTriangles>(MemUtils::GetSymbolAddress(m_Handle, "HUD_DrawTransparentTriangles"));
	if (ORIG_HUD_DrawTransparentTriangles) {
		EngineDevMsg("[client dll] Found HUD_DrawTransparentTriangles at %p.\n", ORIG_HUD_DrawTransparentTriangles);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_DrawTransparentTriangles.\n");
		EngineWarning("Features utilizing TriAPI are unavailable.\n");
	}

	ORIG_HUD_Key_Event = reinterpret_cast<_HUD_Key_Event>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Key_Event"));
	if (ORIG_HUD_Key_Event) {
		EngineDevMsg("[client dll] Found HUD_Key_Event at %p.\n", ORIG_HUD_Key_Event);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_Key_Event.\n");
	}

	ORIG_HUD_UpdateClientData = reinterpret_cast<_HUD_UpdateClientData>(MemUtils::GetSymbolAddress(m_Handle, "HUD_UpdateClientData"));
	if (ORIG_HUD_UpdateClientData) {
		EngineDevMsg("[client dll] Found HUD_UpdateClientData at %p.\n", ORIG_HUD_UpdateClientData);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_UpdateClientData.\n");
	}

	ORIG_IN_ActivateMouse = reinterpret_cast<_IN_ActivateMouse>(MemUtils::GetSymbolAddress(m_Handle, "IN_ActivateMouse"));
	if (ORIG_IN_ActivateMouse) {
		EngineDevMsg("[client dll] Found IN_ActivateMouse at %p.\n", ORIG_IN_ActivateMouse);
	} else {
		EngineDevWarning("[client dll] Could not find IN_ActivateMouse.\n");
	}

	ORIG_IN_DeactivateMouse = reinterpret_cast<_IN_DeactivateMouse>(MemUtils::GetSymbolAddress(m_Handle, "IN_DeactivateMouse"));
	if (ORIG_IN_DeactivateMouse) {
		EngineDevMsg("[client dll] Found IN_DeactivateMouse at %p.\n", ORIG_IN_DeactivateMouse);
	} else {
		EngineDevWarning("[client dll] Could not find IN_DeactivateMouse.\n");
	}

	bool noBhopcap = false;
	{
		auto pattern = fPM_PreventMegaBunnyJumping.get();
		if (ORIG_PM_PreventMegaBunnyJumping) {
			if (pattern == patterns::shared::PM_PreventMegaBunnyJumping.cend())
				EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p.\n", ORIG_PM_PreventMegaBunnyJumping);
			else
				EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p (using the %s pattern).\n", ORIG_PM_PreventMegaBunnyJumping, pattern->name());
		} else {
			EngineDevWarning("[client dll] Could not find PM_PreventMegaBunnyJumping.\n");
			EngineWarning("Bhopcap prediction disabling is not available.\n");
			noBhopcap = true;
		}
	}

	{
		auto pattern = fPM_Jump.get();
		if (ORIG_PM_Jump) {
			if (pattern == patterns::shared::PM_Jump.cend())
				EngineDevMsg("[client dll] Found PM_Jump at %p.\n", ORIG_PM_Jump);
			else
				EngineDevMsg("[client dll] Found PM_Jump at %p (using the %s pattern).\n", ORIG_PM_Jump, pattern->name());
			if (offBhopcap)
				EngineDevMsg("[client dll] Found the bhopcap pattern at %p.\n", reinterpret_cast<void*>(offBhopcap + reinterpret_cast<uintptr_t>(ORIG_PM_Jump)-27));
		} else {
			EngineDevWarning("[client dll] Could not find PM_Jump.\n");
			EngineWarning("Autojump prediction is not available.\n");
			if (!noBhopcap)
				EngineWarning("Bhopcap prediction disabling is not available.\n");
		}

		if (!ppmove)
			ppmove = reinterpret_cast<void**>(MemUtils::GetSymbolAddress(m_Handle, "pmove"));
	}
}

bool ClientDLL::FindHUDFunctions()
{
	if ((ORIG_HUD_Init = reinterpret_cast<_HUD_Init>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Init")))) {
		EngineDevMsg("[client dll] Found HUD_Init at %p.\n", ORIG_HUD_Init);
	} else {
		EngineDevWarning("[client dll] Could not HUD_Init.\n");
		return false;
	}

	if ((ORIG_HUD_VidInit = reinterpret_cast<_HUD_VidInit>(MemUtils::GetSymbolAddress(m_Handle, "HUD_VidInit")))) {
		EngineDevMsg("[client dll] Found HUD_VidInit at %p.\n", ORIG_HUD_VidInit);
	} else {
		EngineDevWarning("[client dll] Could not HUD_VidInit.\n");
		return false;
	}

	if ((ORIG_HUD_Reset = reinterpret_cast<_HUD_Reset>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Reset")))) {
		EngineDevMsg("[client dll] Found HUD_Reset at %p.\n", ORIG_HUD_Reset);
	} else {
		EngineDevWarning("[client dll] Could not HUD_Reset.\n");
		return false;
	}

	if ((ORIG_HUD_Redraw = reinterpret_cast<_HUD_Redraw>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Redraw")))) {
		EngineDevMsg("[client dll] Found HUD_Redraw at %p.\n", ORIG_HUD_Redraw);
	} else {
		EngineDevWarning("[client dll] Could not HUD_Redraw.\n");
		return false;
	}

	return true;
}

void ClientDLL::RegisterCVarsAndCommands()
{
	EngineDevMsg("[client dll] Registering CVars.\n");

	#define REG(cvar) HwDLL::GetInstance().RegisterCVar(CVars::cvar)
	if (ORIG_PM_Jump)
		REG(bxt_autojump_prediction);

	if (ORIG_PM_PreventMegaBunnyJumping)
		REG(bxt_bhopcap_prediction);

	if (ORIG_HUD_DrawTransparentTriangles) {
		REG(bxt_show_triggers);
		REG(bxt_show_custom_triggers);
		REG(bxt_show_nodes);
		REG(bxt_hud_useables);
		REG(bxt_hud_useables_radius);
	}

	if (ORIG_V_CalcRefdef) {
		REG(bxt_unlock_camera_during_pause);
	}

	if (ORIG_HUD_Init)
	{
		CVars::con_color.Assign(HwDLL::GetInstance().FindCVar("con_color"));
		REG(bxt_hud);
		REG(bxt_hud_color);
		REG(bxt_hud_precision);
		REG(bxt_hud_velocity);
		REG(bxt_hud_velocity_offset);
		REG(bxt_hud_velocity_anchor);
		REG(bxt_hud_origin);
		REG(bxt_hud_origin_offset);
		REG(bxt_hud_origin_anchor);
		REG(bxt_hud_viewangles);
		REG(bxt_hud_viewangles_offset);
		REG(bxt_hud_viewangles_anchor);
		REG(bxt_hud_distance);
		REG(bxt_hud_distance_offset);
		REG(bxt_hud_distance_anchor);
		REG(bxt_hud_entity_info);
		REG(bxt_hud_entity_info_offset);
		REG(bxt_hud_entity_info_anchor);
		REG(bxt_hud_selfgauss);
		REG(bxt_hud_selfgauss_offset);
		REG(bxt_hud_selfgauss_anchor);
		REG(bxt_hud_speedometer);
		REG(bxt_hud_speedometer_offset);
		REG(bxt_hud_speedometer_anchor);
		REG(bxt_hud_jumpspeed);
		REG(bxt_hud_jumpspeed_offset);
		REG(bxt_hud_jumpspeed_anchor);
		REG(bxt_hud_health);
		REG(bxt_hud_health_offset);
		REG(bxt_hud_health_anchor);
		REG(bxt_hud_nihilanth);
		REG(bxt_hud_nihilanth_offset);
		REG(bxt_hud_nihilanth_anchor);
		REG(bxt_hud_timer);
		REG(bxt_hud_timer_offset);
		REG(bxt_hud_timer_anchor);
		REG(bxt_hud_visible_landmarks);
		REG(bxt_hud_visible_landmarks_offset);
		REG(bxt_hud_visible_landmarks_anchor);
		REG(bxt_hud_incorrect_fps_indicator);
		REG(bxt_hud_tas_editor_status);
		REG(bxt_hud_tas_editor_status_offset);
		REG(bxt_hud_tas_editor_status_anchor);
		REG(bxt_hud_entities);
		REG(bxt_hud_entities_offset);
		REG(bxt_hud_entities_anchor);
	}
	#undef REG
}

void ClientDLL::SetMouseState(bool active)
{
	if (ORIG_IN_ActivateMouse && ORIG_IN_DeactivateMouse) {
		if (active)
			ORIG_IN_ActivateMouse();
		else
			ORIG_IN_DeactivateMouse();
	}
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_Jump)
{
	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
	int *onground = reinterpret_cast<int*>(pmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = reinterpret_cast<int*>(pmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (CVars::bxt_autojump_prediction.GetBool())
	{
		if ((orig_onground != -1) && !cantJumpNextTime)
			*oldbuttons &= ~IN_JUMP;
	}

	cantJumpNextTime = false;

	if (offBhopcap)
	{
		auto pPMJump = reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump);
		if (CVars::bxt_bhopcap_prediction.GetBool())
		{
			if (*reinterpret_cast<byte*>(pPMJump + offBhopcap) == 0x90
				&& *reinterpret_cast<byte*>(pPMJump + offBhopcap + 1) == 0x90)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pPMJump + offBhopcap), 6, originalBhopcapInsn);
		}
		else if (*reinterpret_cast<byte*>(pPMJump + offBhopcap) == 0x0F
				&& *reinterpret_cast<byte*>(pPMJump + offBhopcap + 1) == 0x82)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pPMJump + offBhopcap), 6, reinterpret_cast<const byte*>("\x90\x90\x90\x90\x90\x90"));
	}

	ORIG_PM_Jump();

	if ((orig_onground != -1) && (*onground == -1))
		cantJumpNextTime = true;

	if (CVars::bxt_autojump_prediction.GetBool())
		*oldbuttons = orig_oldbuttons;
}

HOOK_DEF_1(ClientDLL, void, __cdecl, PM_PlayerMove, qboolean, server)
{
	ORIG_PM_PlayerMove(server);
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_PreventMegaBunnyJumping)
{
	if (CVars::bxt_bhopcap_prediction.GetBool())
		ORIG_PM_PreventMegaBunnyJumping();
}

HOOK_DEF_4(ClientDLL, int, __cdecl, PM_ClipVelocity, float*, in, float*, normal, float*, out, float, overbounce)
{
	return ORIG_PM_ClipVelocity(in, normal, out, overbounce);
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_WaterMove)
{
	return ORIG_PM_WaterMove();
}

HOOK_DEF_2(ClientDLL, void, __cdecl, PM_Move, struct playermove_s*, ppmove, int, server)
{
	return ORIG_PM_Move(ppmove, server);
}

HOOK_DEF_1(ClientDLL, void, __cdecl, V_CalcRefdef, ref_params_t*, pparams)
{
	CustomHud::UpdatePlayerInfoInaccurate(pparams->simvel, pparams->simorg);

	const HwDLL &hwDLL = HwDLL::GetInstance();

	static bool free_cam_was_active = false;

	auto paused = pparams->paused;
	auto unlock_camera = CVars::bxt_unlock_camera_during_pause.GetBool() || hwDLL.free_cam_active
		// We want to unlock the camera 1 frame after free cam has been disabled so that it can
		// snap back to its original position.
		|| (!hwDLL.free_cam_active && free_cam_was_active);
	free_cam_was_active = hwDLL.free_cam_active;

	if (unlock_camera)
		pparams->paused = false;

	ORIG_V_CalcRefdef(pparams);

	if (hwDLL.GetIsOverridingCamera()) {
		// We want to keep looking as is in freecam.
		if (!hwDLL.free_cam_active)
			hwDLL.GetCameraOverrideAngles(pparams->viewangles);

		hwDLL.GetCameraOverrideOrigin(pparams->vieworg);
	} else if (hwDLL.GetIsOffsettingCamera()) {
		static float old_camera_offset_angles[3] = { 0.0f };
		static float old_camera_offset_origin[3] = { 0.0f };

		float vector[3];
		hwDLL.GetCameraOffsetAngles(vector);

		if (pparams->paused) {
			for (int i = 0; i < 3; ++i)
				pparams->viewangles[i] += vector[i] - old_camera_offset_angles[i];
		} else {
			for (int i = 0; i < 3; ++i)
				pparams->viewangles[i] += vector[i];
		}

		for (int i = 0; i < 3; ++i)
			old_camera_offset_angles[i] = vector[i];

		hwDLL.GetCameraOffsetOrigin(vector);

		if (pparams->paused) {
			for (int i = 0; i < 3; ++i)
				pparams->vieworg[i] += vector[i] - old_camera_offset_origin[i];
		} else {
			for (int i = 0; i < 3; ++i)
				pparams->vieworg[i] += vector[i];
		}

		for (int i = 0; i < 3; ++i)
			old_camera_offset_origin[i] = vector[i];
	}

	if (unlock_camera)
		pparams->paused = paused;

	last_vieworg = pparams->vieworg;
	last_viewangles = pparams->viewangles;
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_Init)
{
	ORIG_HUD_Init();

	CustomHud::Init();
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_VidInit)
{
	ORIG_HUD_VidInit();

	CustomHud::InitIfNecessary();
	CustomHud::VidInit();

	TriangleDrawing::VidInit();
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_Reset)
{
	ORIG_HUD_Reset();

	CustomHud::InitIfNecessary();
	CustomHud::VidInit();
}

HOOK_DEF_2(ClientDLL, void, __cdecl, HUD_Redraw, float, time, int, intermission)
{
	ORIG_HUD_Redraw(time, intermission);

	CustomHud::Draw(time);
}

HOOK_DEF_6(ClientDLL, void, __cdecl, HUD_PostRunCmd, local_state_s*, from, local_state_s*, to, usercmd_s*, cmd, int, runfuncs, double, time, unsigned int, random_seed)
{
	HwDLL::GetInstance().SetLastRandomSeed(random_seed);
	auto seed = random_seed;
	bool changedSeed = false;
	if (HwDLL::GetInstance().IsCountingSharedRNGSeed()) {
		auto lastSeed = HwDLL::GetInstance().GetSharedRNGSeedCounter();
		seed = lastSeed - HwDLL::GetInstance().QueuedSharedRNGSeeds + 1 + SeedsQueued;
		SeedsQueued++;
		changedSeed = true;
	}

	last_buttons = cmd->buttons;

	// Not sure if this is the best spot.
	HwDLL::GetInstance().FreeCamTick();

	if (CVars::_bxt_taslog.GetBool())
		if (pEngfuncs)
		{
			#define PRINTF(format, ...) pEngfuncs->Con_Printf(const_cast<char*>(format), ##__VA_ARGS__)
			PRINTF("-- HUD_PostRunCmd Start --\n");
			PRINTF("Msec %hhu (%Lf)\n", cmd->msec, static_cast<long double>(cmd->msec) * 0.001);
			PRINTF("Viewangles: %.8f %.8f %.8f; forwardmove: %f; sidemove: %f; upmove: %f\n", cmd->viewangles[0], cmd->viewangles[1], cmd->viewangles[2], cmd->forwardmove, cmd->sidemove, cmd->upmove);
			PRINTF("Buttons: %hu\n", cmd->buttons);
			PRINTF("Random seed: %u", random_seed);
			if (changedSeed)
				PRINTF(" (overriding with %u)", seed);
			PRINTF("\n");
			PRINTF("-- HUD_PostRunCmd End --\n");
			#undef P
		}

	return ORIG_HUD_PostRunCmd(from, to, cmd, runfuncs, time, seed);
}

HOOK_DEF_1(ClientDLL, void, __cdecl, HUD_Frame, double, time)
{
	ORIG_HUD_Frame(time);

	if (CVars::_bxt_taslog.GetBool() && pEngfuncs)
		pEngfuncs->Con_Printf(const_cast<char*>("HUD_Frame time: %f\n"), time);

	SeedsQueued = 0;
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_DrawTransparentTriangles)
{
	ORIG_HUD_DrawTransparentTriangles();

	glDisable(GL_TEXTURE_2D);

	TriangleDrawing::Draw();

	glEnable(GL_TEXTURE_2D);

	// This is required for the WON DLLs.
	pEngfuncs->pTriAPI->RenderMode(kRenderNormal);
}

HOOK_DEF_3(ClientDLL, int, __cdecl, HUD_Key_Event, int, down, int, keynum, const char*, pszCurrentBinding)
{
	insideKeyEvent = true;

	auto rv = ORIG_HUD_Key_Event(down, keynum, pszCurrentBinding);

	insideKeyEvent = false;

	return rv;
}

HOOK_DEF_2(ClientDLL, int, __cdecl, HUD_UpdateClientData, client_data_t*, pcldata, float, flTime)
{
	const auto norefresh = CVars::_bxt_norefresh.GetBool();
	int (*ORIG_GetScreenInfo)(SCREENINFO *pscrinfo) = nullptr;

	if (norefresh && pEngfuncs) {
		ORIG_GetScreenInfo = pEngfuncs->pfnGetScreenInfo;
		pEngfuncs->pfnGetScreenInfo = [](SCREENINFO *pscrinfo) { return 0; };
	}

	const auto rv = ORIG_HUD_UpdateClientData(pcldata, flTime);

	if (norefresh && pEngfuncs) {
		pEngfuncs->pfnGetScreenInfo = ORIG_GetScreenInfo;
	}

	return rv;
}
