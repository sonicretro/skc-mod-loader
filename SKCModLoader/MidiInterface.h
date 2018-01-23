#pragma once
#include "IniFile.hpp"
#include "musicid.gen.h"

enum MusicID2 {
	MusicID_ByCharacter = -5,
	MusicID_ByZone,
	MusicID_MIDI,
	MusicID_VGMStream,
	MusicID_Default,
	MusicID_Midboss = MusicID_SKMidboss,
	MusicID_Unused = MusicID_S3Midboss,
	MusicID_HiddenPalace = MusicID_SKCredits + 1,
	MusicID_SuperSonic,
	MusicID_Ending,
	MusicID_DataSelect,
	MusicID_SpecialStageResult,
	MusicID_BlueSphereResult,
	MusicID_BlueSphereTitle,
	MusicID_BlueSphereDifficulty,
	MusicID_TimeAttackRecords,
	TrackCount
};

enum TrackType
{
	TrackType_None = -1,
	TrackType_MIDI,
	TrackType_SMPS,
	TrackType_VGMStream
};

class MidiInterface
{
public:
	MidiInterface(HMODULE moduleHandle);
	~MidiInterface();
	virtual BOOL init(HWND hwnd); // hwnd = game window
	virtual BOOL loadSong(short id, unsigned int bgmmode); // id = song to be played + 1 (well, +1 compared to the sound test id, it's the ID of the song in the MIDIOUT.DLL's resources); bgmmode = 0 for FM synth, 1 for General MIDI
	virtual BOOL playSong();
	virtual BOOL stopSong();
	virtual BOOL pauseSong();
	virtual BOOL resumeSong();
	virtual BOOL setTempo(unsigned int pct); // pct = percentage of delay between beats the song should be set to. lower = faster tempo
	void songStopped();
	void AddMusicFolder(const std::string &folder);
private:
	void readSettings(const std::string &folder, const IniGroup *settings, short *trackSettings, std::string *trackFilenames);
	std::vector<std::string> musicFolders;
	UINT timePeriod;
	short trackSettings[TrackCount], s3TrackSettings[TrackCount], skTrackSettings[TrackCount];
	std::string trackFilenames[TrackCount], s3TrackFilenames[TrackCount], skTrackFilenames[TrackCount];
	TrackType trackType = TrackType_None;
	const char **smpsnames;
	unsigned int smpscount;
	HMODULE moduleHandle;
	bool bassinit = false;
	DWORD basschan = 0;
	DWORD midichan = 0;
	int midiloopstart;
	TrackType lastTrackType = TrackType_None;
	DWORD lastbasschan = 0;
	DWORD lastmidichan = 0;
	int lastmidiloopstart;
};

