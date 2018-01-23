#pragma once
#include <windows.h>

#ifdef SMPSPlay_EXPORTS
#define SMPSPlay_API __declspec(dllexport)
#else
#define SMPSPlay_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __SMPS_H__
	typedef void(*SMPS_CB_SIGNAL)(void);
#endif

	SMPSPlay_API BOOL SMPS_InitializeDriver();
	SMPSPlay_API BOOL SMPS_DeInitializeDriver();
	SMPSPlay_API void SMPS_RegisterSongStoppedCallback(void(*callback)());
	SMPSPlay_API BOOL SMPS_LoadSong(short id);
	SMPSPlay_API BOOL SMPS_PlaySong();
	SMPSPlay_API BOOL SMPS_LoadAndPlaySong(short id);
	SMPSPlay_API BOOL SMPS_StopSong();
	SMPSPlay_API void SMPS_FadeOutSong();
	SMPSPlay_API void SMPS_FadeInSong();
	SMPSPlay_API BOOL SMPS_PauseSong();
	SMPSPlay_API BOOL SMPS_ResumeSong();
	SMPSPlay_API BOOL SMPS_SetSongTempo(double multiplier);
	SMPSPlay_API void SMPS_SetDefault1UpHandling(bool enable);
	SMPSPlay_API void SMPS_AddCustomSongs(const char *filename);
	SMPSPlay_API const char **SMPS_GetSongNames(unsigned int &count);
	SMPSPlay_API void SMPS_SetVolume(double volume);
	SMPSPlay_API void SMPS_SetWaveLogPath(const char *logfile);
	SMPSPlay_API void SMPS_RegisterSongLoopCallback(SMPS_CB_SIGNAL func);


#ifdef __cplusplus
}
#endif