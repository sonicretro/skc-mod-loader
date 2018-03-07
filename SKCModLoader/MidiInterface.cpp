#include "stdafx.h"
#include <ctime>
#include <tchar.h>
#include <MMSystem.h>
#include "bass.h"
#include "bass_fx.h"
#include "bassmidi.h"
#include "bass_vgmstream.h"
#include "SMPSPlay.h"
#include "resource.h"
#include "MidiInterface.h"
#include "SKCModLoader.h"
#include "FileSystem.h"

using std::string;

const char *const INISections[] = { "S3K", "S&K", "S3" };

MidiInterface *instance;

MidiInterface::MidiInterface(HMODULE moduleHandle)
{
	instance = this;
	this->moduleHandle = moduleHandle;
	SMPS_InitializeDriver();
	bassinit = BASS_Init(-1, 44100, 0, nullptr, nullptr) ? true : false;
	if (bassinit)
	{
		BASS_SetConfigPtr(BASS_CONFIG_MIDI_DEFFONT, "Music\\CT8MGM.SF2");
		BASS_PluginLoad("bassflac.dll", 0);
	}
	musicFolders.push_back("Music\\");
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(tc));
	timeBeginPeriod(tc.wPeriodMin);
	timePeriod = tc.wPeriodMin;
}

MidiInterface::~MidiInterface()
{
	SMPS_DeInitializeDriver();
	BASS_Free();
	timeEndPeriod(timePeriod);
}

const char *const TrackOptions[] = {
	"TitleScreenTrack",
	"AngelIsland1Track",
	"AngelIsland2Track",
	"Hydrocity1Track",
	"Hydrocity2Track",
	"MarbleGarden1Track",
	"MarbleGarden2Track",
	"CarnivalNight1Track",
	"CarnivalNight2Track",
	"FlyingBattery1Track",
	"FlyingBattery2Track",
	"IceCap1Track",
	"IceCap2Track",
	"LaunchBase1Track",
	"LaunchBase2Track",
	"MushroomHill1Track",
	"MushroomHill2Track",
	"Sandopolis1Track",
	"Sandopolis2Track",
	"LavaReef1Track",
	"LavaReef2Track",
	"SkySanctuaryTrack",
	"DeathEgg1Track",
	"DeathEgg2Track",
	"MidbossTrack",
	"BossTrack",
	"DoomsdayTrack",
	"MagneticOrbsTrack",
	"SpecialStageTrack",
	"SlotMachineTrack",
	"GumballMachineTrack",
	"KnucklesTrack",
	"AzureLakeTrack",
	"BalloonParkTrack",
	"DesertPalaceTrack",
	"ChromeGadgetTrack",
	"EndlessMineTrack",
	"GameOverTrack",
	"ContinueTrack",
	"ActClearTrack",
	"OneUpTrack",
	"ChaosEmeraldTrack",
	"InvincibilityTrack",
	"CompetitionMenuTrack",
	"UnusedTrack",
	"LevelSelectTrack",
	"FinalBossTrack",
	"DrowningTrack",
	"AllClearTrack",
	"CreditsTrack",
	"KnucklesTrack",
	"TitleScreenTrack",
	"OneUpTrack",
	"InvincibilityTrack",
	"AllClearTrack",
	"CreditsTrack",
	"HiddenPalaceTrack",
	"SuperSonicTrack",
	"EndingTrack",
	"DataSelectTrack",
	"SpecialStageResultTrack",
	"BlueSphereResultTrack",
	"BlueSphereTitleTrack",
	"BlueSphereDifficultyTrack",
	"TimeAttackRecordsTrack",
	"KnucklesBossTrack"
};

void MidiInterface::AddMusicFolder(const string &folder)
{
	musicFolders.push_back(folder);
}

void MidiInterface::readSettings(const string &folder, const IniGroup *settings, short *trackSettings, string *trackFilenames)
{
	for (int i = 0; i < TrackCount; i++)
	{
		if (!settings->hasKeyNonEmpty(TrackOptions[i])) continue;
		string value = settings->getString(TrackOptions[i]);
		bool found = false;
		if (value == "ByCharacter")
		{
			if (!GameSelection)
			{
				trackSettings[i] = MusicID_ByCharacter;
				trackFilenames[i].clear();
			}
			continue;
		}
		else if (value == "ByZone")
		{
			if (!GameSelection)
			{
				trackSettings[i] = MusicID_ByZone;
				trackFilenames[i].clear();
			}
			continue;
		}
		else if (bassinit && value == "MIDI")
		{
			trackSettings[i] = MusicID_MIDI;
			trackFilenames[i].clear();
			continue;
		}
		else if (value == "Default")
			continue;
		for (unsigned int j = 0; j < smpscount; j++)
			if (value == smpsnames[j])
			{
				trackSettings[i] = j;
				trackFilenames[i].clear();
				found = true;
				break;
			}
		if (found) continue;
		if (!bassinit) continue;
		trackSettings[i] = GetExtension(value, true) == ".mid" ? MusicID_MIDI : MusicID_VGMStream;
		trackFilenames[i] = folder + value;
	}
}

void SMPS_onTrackEnd()
{
	instance->songStopped();
}

BOOL MidiInterface::init(HWND hwnd)
{
	for (const string &fol : musicFolders)
			SMPS_AddCustomSongs((fol + "SMPS.ini").c_str());
	smpsnames = SMPS_GetSongNames(smpscount);

	SMPS_RegisterSongStoppedCallback(SMPS_onTrackEnd);

	memset(trackSettings, MusicID_Default, SizeOfArray(trackSettings));
	if (!GameSelection)
	{
		memset(s3TrackSettings, MusicID_Default, SizeOfArray(s3TrackSettings));
		memset(skTrackSettings, MusicID_Default, SizeOfArray(skTrackSettings));
	}

	for (const string &fol : musicFolders)
	{
		IniFile settings(fol + "Music.ini");
		if (bassinit && settings.hasKeyNonEmpty("", "MIDISoundFont"))
			BASS_SetConfigPtr(BASS_CONFIG_MIDI_DEFFONT, (fol + settings.getString("", "MIDISoundFont")).c_str());

		short tmpSet[TrackCount];
		string tmpFn[TrackCount];

		memset(tmpSet, MusicID_Default, SizeOfArray(tmpSet));

		const IniGroup *group = settings.getGroup("All");
		if (group != nullptr)
			readSettings(fol, group, tmpSet, tmpFn);

		for (int i = 0; i < TrackCount; i++)
			if (tmpSet[i] != MusicID_Default)
			{
				trackSettings[i] = tmpSet[i];
				trackFilenames[i] = tmpFn[i];
				if (!GameSelection)
				{
					s3TrackSettings[i] = tmpSet[i];
					s3TrackFilenames[i] = tmpFn[i];
					skTrackSettings[i] = tmpSet[i];
					skTrackFilenames[i] = tmpFn[i];
				}
			}

		group = settings.getGroup(INISections[GameSelection]);
		if (group != nullptr)
			readSettings(fol, group, trackSettings, trackFilenames);

		if (!GameSelection)
		{
			group = settings.getGroup("S3");
			if (group != nullptr)
				readSettings(fol, group, s3TrackSettings, s3TrackFilenames);

			group = settings.getGroup("S&K");
			if (group != nullptr)
				readSettings(fol, group, skTrackSettings, skTrackFilenames);
		}
	}

	switch (GameSelection)
	{
	case 0:
		if (s3TrackSettings[MusicID_Midboss] == MusicID_Default)
			s3TrackSettings[MusicID_Midboss] = MusicID_S3Midboss;
		if (s3TrackSettings[MusicID_Continue] == MusicID_Default)
			s3TrackSettings[MusicID_Continue] = MusicID_S3Continue;
		break;
	case 2:
		if (trackSettings[MusicID_Midboss] == MusicID_Default)
			trackSettings[MusicID_Midboss] = MusicID_S3Midboss;
		if (trackSettings[MusicID_Continue] == MusicID_Default)
			trackSettings[MusicID_Continue] = MusicID_S3Continue;
		break;
	}
	return TRUE;
}

/**
* BASS callback: Current track has ended.
* @param handle
* @param channel
* @param data
* @param user
*/
static void __stdcall BASS_onTrackEnd(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	BASS_ChannelStop(channel);
	BASS_StreamFree(channel);
	((MidiInterface*)user)->songStopped();
}

void CALLBACK MIDI_DoLoop(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	BASS_ChannelSetPosition(channel, *(int*)user, BASS_POS_MIDI_TICK);
}

BOOL MidiInterface::loadSong(short id, unsigned int bgmmode)
{
	--id;
	bool is1up = id == MusicID_S31Up || id == MusicID_SK1Up;
	if (is1up)
	{
		lastTrackType = trackType;
		switch (trackType)
		{
		case TrackType_MIDI:
			lastmidichan = midichan;
			lastmidiloopstart = midiloopstart;
			BASS_ChannelPause(midichan);
			break;
		case TrackType_VGMStream:
			lastbasschan = basschan;
			BASS_ChannelPause(basschan);
			break;
		}
	}
	else
	{
		lastTrackType = TrackType_None;
		switch (trackType)
		{
		case TrackType_MIDI:
			if (midichan != 0)
			{
				BASS_ChannelStop(midichan);
				BASS_StreamFree(midichan);
			}
			break;
		case TrackType_SMPS:
			SMPS_StopSong();
			break;
		case TrackType_VGMStream:
			if (basschan != 0)
			{
				BASS_ChannelStop(basschan);
				BASS_StreamFree(basschan);
			}
			break;
		}
	}
	int newid = id;
	switch (newid)
	{
	case MusicID_LavaReef2:
		if (Current_zone_and_act == hidden_palace_zone
			|| Current_zone_and_act == hidden_palace_shrine)
			newid = MusicID_HiddenPalace;
		break;
	case MusicID_SkySanctuary:
		if (Current_zone_and_act == ending_zone_act_2 || Ending_running_flag)
			newid = MusicID_Ending;
		break;
	case MusicID_Continue:
		if (Game_mode == GameModeID_BlueSpheresTitle)
			newid = MusicID_BlueSphereTitle;
		else if (Game_mode == GameModeID_TimeAttackRecords)
			newid = MusicID_TimeAttackRecords;
		break;
	case MusicID_ActClear:
		if (Game_mode == GameModeID_SpecialStageResults)
			newid = MusicID_SpecialStageResult;
		else if (Game_mode == GameModeID_BlueSpheresResults)
			newid = MusicID_BlueSphereResult;
		break;
	case MusicID_S3Invincibility:
	case MusicID_SKInvincibility:
		if (Game_mode == GameModeID_BlueSpheresDifficulty)
			newid = MusicID_BlueSphereDifficulty;
		else if (Super_Sonic_Knux_flag || Super_Tails_flag)
			newid = MusicID_SuperSonic;
		break;
	case MusicID_LevelSelect:
		if (Game_mode == GameModeID_SaveScreen)
			newid = MusicID_DataSelect;
		break;
	case MusicID_S3Knuckles:
	case MusicID_SKKnuckles:
		if (Current_zone_and_act == hidden_palace_zone)
			newid = MusicID_KnucklesBoss;
		break;
	}
	short *settings = trackSettings;
	string *filenames = trackFilenames;
checkbycharbyzone:
	if (!GameSelection)
	{
		if (settings[newid] == MusicID_ByCharacter)
			if (Player_mode == knuckles_alone)
			{
				settings = skTrackSettings;
				filenames = skTrackFilenames;
			}
			else
			{
				settings = s3TrackSettings;
				filenames = s3TrackFilenames;
			}
		else if (settings[newid] == MusicID_ByZone)
		{
			unsigned char level = Current_zone_and_act >> 8;
			switch (level)
			{
			case gumball_bonus_zone:
			case pachinko_bonus_zone:
			case slots_bonus_zone:
				level = Saved_zone_and_act >> 8;
				break;
			}
			if (level == flying_battery_zone || level >= mushroom_hill_zone)
			{
				settings = skTrackSettings;
				filenames = skTrackFilenames;
			}
			else
			{
				settings = s3TrackSettings;
				filenames = s3TrackFilenames;
			}
		}
	}
	if (newid >= MusicID_HiddenPalace && settings[newid] == MusicID_Default)
	{
		newid = id;
		goto checkbycharbyzone;
	}
	short set = settings[newid];
	if (set == MusicID_MIDI)
	{
		trackType = TrackType_MIDI;
		if (filenames[newid].empty()) // use default MIDI
		{
			HRSRC hres = FindResource(moduleHandle, MAKEINTRESOURCE((!bgmmode ? IDR_MIDI_FM1 : IDR_MIDI_GM1) + id), !bgmmode ? _T("MIDI_FM") : _T("MIDI_GM"));
			midichan = BASS_MIDI_StreamCreateFile(TRUE, LockResource(LoadResource(moduleHandle, hres)), 0, SizeofResource(moduleHandle, hres), BASS_MIDI_DECAYEND | BASS_MIDI_DECAYSEEK | BASS_MIDI_NOCROP, 0);
		}
		else
			midichan = BASS_MIDI_StreamCreateFile(false, filenames[newid].c_str(), 0, 0, BASS_MIDI_DECAYEND | BASS_MIDI_DECAYSEEK | BASS_MIDI_NOCROP, 0);
		if (midichan != 0)
		{
			BASS_ChannelSetSync(midichan, BASS_SYNC_END, 0, BASS_onTrackEnd, this);
			bool found = false;
			BASS_MIDI_MARK mark;
			for (int i = 0; BASS_MIDI_StreamGetMark(midichan, BASS_MIDI_MARK_MARKER | BASS_MIDI_MARK_TICK, i, &mark); i++)
			{
				if (!_stricmp(mark.text, "loopStart"))
					midiloopstart = mark.pos;
				else if (!_stricmp(mark.text, "loopEnd"))
				{
					BASS_ChannelSetSync(midichan, BASS_SYNC_MIDI_TICK, mark.pos, MIDI_DoLoop, &midiloopstart);
					found = true;
				}
			}
			if (!found)
			{
				BASS_MIDI_EVENT events[2];
				if (BASS_MIDI_StreamGetEventsEx(midichan, -1, 111, events, 0, 2) == 2)
				{
					midiloopstart = events[0].tick;
					BASS_ChannelSetSync(midichan, BASS_SYNC_MIDI_TICK, events[1].tick, MIDI_DoLoop, &midiloopstart);
				}
			}
			if (is1up && lastTrackType == TrackType_SMPS)
				SMPS_PauseSong();
			return TRUE;
		}
		newid = id;
	}
	else if (set == MusicID_VGMStream)
	{
		trackType = TrackType_VGMStream;
		basschan = BASS_VGMSTREAM_StreamCreate(filenames[newid].c_str(), BASS_STREAM_DECODE);
		if (basschan == 0)
			basschan = BASS_StreamCreateFile(false, filenames[newid].c_str(), 0, 0, BASS_STREAM_DECODE);
		if (basschan != 0)
		{
			int tempochan = BASS_FX_TempoCreate(basschan, BASS_FX_FREESOURCE);
			if (tempochan != 0)
			{
				basschan = tempochan;
				BASS_ChannelSetSync(basschan, BASS_SYNC_END, 0, BASS_onTrackEnd, this);
				if (is1up && lastTrackType == TrackType_SMPS)
					SMPS_PauseSong();
				return TRUE;
			}
			else
				BASS_StreamFree(basschan);
		}
		newid = id;
	}
	else if (set != MusicID_Default)
		newid = set;
	trackType = TrackType_SMPS;
	if (is1up)
		SMPS_SetDefault1UpHandling(lastTrackType == TrackType_SMPS);
	SMPS_LoadSong(newid);
	return TRUE;
}

BOOL MidiInterface::playSong()
{
	switch (trackType)
	{
	case TrackType_MIDI:
		return BASS_ChannelPlay(midichan, false);
	case TrackType_SMPS:
		return SMPS_PlaySong();
	case TrackType_VGMStream:
		return BASS_ChannelPlay(basschan, false);
	}
	return TRUE;
}

BOOL MidiInterface::stopSong()
{
	if (reg_d0.UByte == 0xFF)
		return PlaySound(MAKEINTRESOURCE(IDR_WAVE_SEGA), moduleHandle, SND_RESOURCE | SND_ASYNC);
	else if (reg_d0.UByte == 0xFE)
		return PlaySound(NULL, NULL, SND_ASYNC);
	if (reg_d0.UByte < 0xE0)
		return TRUE;	// don't stop if we're going to play a new song anyway
	if (reg_d0.UByte == 0xE1)
	{
		switch (trackType)
		{
		case TrackType_MIDI:
			return BASS_ChannelSlideAttribute(midichan, BASS_ATTRIB_VOL | BASS_SLIDE_LOG, -1, 4000);
		case TrackType_SMPS:
			SMPS_FadeOutSong();
		case TrackType_VGMStream:
			return BASS_ChannelSlideAttribute(basschan, BASS_ATTRIB_VOL | BASS_SLIDE_LOG, -1, 4000);
		}
		return TRUE;
	}
	switch (trackType)
	{
	case TrackType_MIDI:
		BASS_ChannelStop(midichan);
		return BASS_StreamFree(midichan);
	case TrackType_SMPS:
		return SMPS_StopSong();
	case TrackType_VGMStream:
		BASS_ChannelStop(basschan);
		return BASS_StreamFree(basschan);
	}
	return TRUE;
}

BOOL MidiInterface::pauseSong()
{
	switch (trackType)
	{
	case TrackType_MIDI:
		return BASS_ChannelPause(midichan);
	case TrackType_SMPS:
		return SMPS_PauseSong();
	case TrackType_VGMStream:
		return BASS_ChannelPause(basschan);
	}
	return TRUE;
}

BOOL MidiInterface::resumeSong()
{
	switch (trackType)
	{
	case TrackType_MIDI:
		return BASS_ChannelPlay(midichan, false);
	case TrackType_SMPS:
		return SMPS_ResumeSong();
	case TrackType_VGMStream:
		return BASS_ChannelPlay(basschan, false);
	}
	return TRUE;
}

BOOL MidiInterface::setTempo(unsigned int delay)
{
	double multi = 100.0 / delay;
	switch (trackType)
	{
	case TrackType_MIDI:
		return BASS_MIDI_StreamEvent(midichan, 0, MIDI_EVENT_SPEED, (int)(multi * 10000));
	case TrackType_SMPS:
		return SMPS_SetSongTempo(multi);
	case TrackType_VGMStream:
		return BASS_ChannelSetAttribute(basschan, BASS_ATTRIB_TEMPO, (float)(multi * 100 - 100));
	}
	return TRUE;
}

void MidiInterface::songStopped()
{
	switch (lastTrackType)
	{
	case TrackType_None:
		return;
	case TrackType_MIDI:
		midichan = lastmidichan;
		midiloopstart = lastmidiloopstart;
		BASS_ChannelSetAttribute(midichan, BASS_ATTRIB_VOL, 0);
		BASS_ChannelSlideAttribute(midichan, BASS_ATTRIB_VOL | BASS_SLIDE_LOG, 1, 4000);
		BASS_ChannelPlay(midichan, FALSE);
		break;
	case TrackType_SMPS:
		if (trackType != TrackType_SMPS)
		{
			SMPS_FadeInSong();
			SMPS_ResumeSong();
		}
		break;
	case TrackType_VGMStream:
		basschan = lastbasschan;
		BASS_ChannelSetAttribute(basschan, BASS_ATTRIB_VOL, 0);
		BASS_ChannelSlideAttribute(basschan, BASS_ATTRIB_VOL | BASS_SLIDE_LOG, 1, 4000);
		BASS_ChannelPlay(basschan, FALSE);
		break;
	}
	trackType = lastTrackType;
	lastTrackType = TrackType_None;
}