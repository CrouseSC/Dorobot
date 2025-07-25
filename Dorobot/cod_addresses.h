#pragma once
#ifndef CODADDRESSES_H
#define CODADDRESSES_H

constexpr int addr_inair = 0x007944E4;  //1023 - in air, !1023 not
constexpr int addr_velocity = 0x0079449C;
constexpr int addr_position = 0x00794490;
constexpr int addr_ingame = 0xC5F900;
constexpr int addr_timescale = 0x0CBABC88;
constexpr int addr_view = 0x79B698;  //vec3f
constexpr int addr_maxfps_wtmod = 0x00795150;
constexpr int addr_maxfps_3xp = 0x00771F84;
constexpr int addr_delta_angles = 0x0074E3D0;
constexpr int addr_usercmd = 0x00CC4FF8;
constexpr int addr_sprint = 0x00794560; // >= 20 is sprint
constexpr int addr_lean = 0x007944D0;
constexpr int addr_maxfps = 0x0CBAB8AC;  //float
constexpr int addr_engine_draw = 0x658860;
constexpr int addr_view_writable = 0x00C84FD8;
constexpr int addr_deltaAngles = 0x0074E3D0;
constexpr int addr_pmove = 0x00414D10;
constexpr int addr_pmoveSingle = 0x004143A0;
constexpr int addr_pm_airmove = 0x0040f680;
constexpr int addr_fun_0040e2b0 = 0x0040e2b0;
constexpr int addr_pm_accelerate = 0x0040EBB0;
constexpr int addr_createMove = 0x00463E00;
constexpr int addr_writePacket = 0x00463f50;
constexpr int addr_pMove_current = 0x008C9C90;
constexpr int addr_input_s = 0x00CC4FF8;
constexpr int addr_renderScene = 0x005FAF00;
constexpr int addr_calcView = 0x00451990;
constexpr int addr_drawxmodelskinneduncached = 0x00646ED4;
constexpr int addr_drawxmodelskinnedCached = 0x00646870;
constexpr int addr_BGFindWeaponIndexForName = 0x00416610;
constexpr int addr_cg_predictPlayerState = 0x00447260;
constexpr int addr_clientThink_real = 0x004A8500;
constexpr int addr_isDevmap = 0x01288600;
constexpr int addr_cg_readNextSnapshot = 0x0044e2b0;
constexpr int addr_playerState = 0x00794474;

#endif
