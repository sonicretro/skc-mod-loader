// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cwctype>
#include <tchar.h>

#include <deque>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
using std::deque;
using std::ios_base;
using std::ifstream;
using std::string;
using std::wstring;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

#include "IniFile.hpp"
#include "TextConv.hpp"
#include "CodeParser.hpp"
#include "FileSystem.h"
#include "MidiInterface.h"
#include "FramerateFix.h"
#include "PaletteFix.h"

// Code Parser.
static CodeParser codeParser;

static vector<ModFrameFunc> modFrameFuncs;

unordered_map<void*, const void*> ObjArtPtrs;
unordered_map<void*, const void*> SpriteArtPtrs;
static void __cdecl ProcessCodes_i()
{
	codeParser.processCodeList();
	for (unsigned int i = 0; i < modFrameFuncs.size(); i++)
		modFrameFuncs[i]();
	ObjArtPtrs.clear();
	SpriteArtPtrs.clear();
}

ThiscallFunctionPointer(void, sub_403101, (void*), 0x403101);
__declspec(naked) void ProcessCodes()
{
	__asm
	{
		push ecx
		call ProcessCodes_i
		pop ecx
		jmp sub_403101
	}
}

PCSpriteTableEntry* ConvertSpriteTable_Main(MDSpriteTableEntry* sprite_table_addr, PCSpriteTableEntry* sprite_table_dest)
{
	while (sprite_table_addr->link_field
		&& (sprite_table_addr->y_pos || sprite_table_addr->render_info || sprite_table_addr->x_pos))
	{
		sprite_table_dest->x_pos = (unsigned __int16)sprite_table_addr->x_pos - 128;
		sprite_table_dest->y_pos = (unsigned __int16)sprite_table_addr->y_pos - 128;
		sprite_table_dest->h_cells = ((unsigned __int8)(sprite_table_addr->sprite_size & 0xC) >> 2) + 1;
		sprite_table_dest->v_pixels = 8 * (sprite_table_addr->sprite_size & 3) + 8;
		sprite_table_dest->render_flags = ((unsigned __int16)(sprite_table_addr->render_info & 0x8000) >> 8) | ((unsigned __int16)(sprite_table_addr->render_info & 0x1800) >> 3) | (((unsigned __int16)(sprite_table_addr->render_info & 0x6000) >> 9) + 16);
		auto iter = SpriteArtPtrs.find(sprite_table_addr);
		if (iter != SpriteArtPtrs.cend())
			sprite_table_dest->art_addr = (char*)iter->second;
		else
			sprite_table_dest->art_addr = start_of_VRAM;
		sprite_table_dest->art_addr += 32 * (sprite_table_addr->render_info & 0x7FF);
		++SpritesProcessed;
		++sprite_table_dest;
		++sprite_table_addr;
	}
	return sprite_table_dest;
}

PCSpriteTableEntry* __cdecl ConvertSpriteTable_r(MDSpriteTableEntry* sprite_table_addr, PCSpriteTableEntry* sprite_table_dest)
{
	SpritesProcessed = 0;
	P2SpriteTableProcessed = 0;
	return ConvertSpriteTable_Main(sprite_table_addr, sprite_table_dest);
}

PCSpriteTableEntry* __cdecl ConvertSpriteTable_P2_r(MDSpriteTableEntry* sprite_table_addr, PCSpriteTableEntry* sprite_table_dest)
{
	P2SpriteTableProcessed = 1;
	sprite_table_dest->x_pos = 0;
	sprite_table_dest->y_pos = 0;
	sprite_table_dest->h_cells = 1;
	sprite_table_dest->v_pixels = 8;
	sprite_table_dest->render_flags = 0x10000;
	sprite_table_dest->art_addr = start_of_VRAM;
	++SpritesProcessed;
	return ConvertSpriteTable_Main(sprite_table_addr, sprite_table_dest + 1);
}

void SpriteFunc()
{
	auto iter = ObjArtPtrs.find(reg_a0.Unknown);
	if (iter != ObjArtPtrs.cend())
	{
		auto ptr = iter->second;
		MDSpriteTableEntry* ste = (MDSpriteTableEntry*)reg_a6.Unknown;
		int cnt = reg_d4.Word + 1;
		for (int i = 0; i < cnt; ++i)
			SpriteArtPtrs.insert({ &ste[i], ptr });
	}
}

const void* loc_41FACD = (const void*)0x41FACD;
__declspec(naked) void SpriteFunc1()
{
	__asm
	{
		call SpriteFunc
		mov eax, 0x8549BC
		mov al, byte ptr [eax]
		jmp [loc_41FACD]
	}
}

const void* loc_4201B8 = (const void*)0x4201B8;
__declspec(naked) void SpriteFunc2()
{
	__asm
	{
		call SpriteFunc
		mov eax, 0x8549BC
		mov al, byte ptr[eax]
		jmp [loc_4201B8]
	}
}

static bool dbgConsole;
// File for logging debugging output.
static FILE *dbgFile = nullptr;

/**
 * Debug Output function.
 * @param Format Format string.
 * @param args Arguments.
 * @return Return value from vsnprintf().
 */
int __cdecl PrintDebug(const char *Format, ...)
{
	va_list ap;
	va_start(ap, Format);
	int result = vsnprintf(NULL, 0, Format, ap) + 1;
	va_end(ap);
	char *buf = new char[result];
	va_start(ap, Format);
	result = vsnprintf(buf, result, Format, ap);
	va_end(ap);

	// Console output.
	if (dbgConsole)
	{
		// TODO: Convert from Shift-JIS to CP_ACP?
		fputs(buf, stdout);
		fflush(stdout);
	}

	// File output.
	if (dbgFile)
	{
		fputs(buf, dbgFile);
		fflush(dbgFile);
	}

	delete[] buf;
	return result;
}

static vector<vector<PLC>> PLCList;
static void CheckPLCList()
{
	if (PLCList.size() == 0)
	{
		uint16_t *plcoffs = (uint16_t *)0x41DF98;
		for (int i = 0; i < 124; i++)
		{
			vector<PLC> plcs;
			void *addr = (uint8_t *)plcoffs + plcoffs[i];
			uint16_t cnt = *(uint16_t *)addr;
			PLC *plc = (PLC *)((uint16_t *)addr + 1);
			for (uint16_t j = 0; j <= cnt; j++)
				plcs.push_back(*plc++);
			PLCList.push_back(plcs);
		}
	}
}

static int RegisterPLCList(const PLC *plcs, int length)
{
	CheckPLCList();
	if (plcs && length > 0)
	{
		vector<PLC> plcv;
		for (int i = 0; i < length; i++)
			plcv.push_back(*plcs++);
		PLCList.push_back(plcv);
	}
	return PLCList.size() - 1;
}

static const PLC *GetPLCList(int index, int &length)
{
	CheckPLCList();
	if ((unsigned)index < PLCList.size())
	{
		length = PLCList[index].size();
		return PLCList[index].data();
	}
	else
	{
		length = 0;
		return nullptr;
	}
}

static BOOL SetPLCList(int index, const PLC *plcs, int length)
{
	CheckPLCList();
	if (!plcs || length <= 0)
		return false;
	if ((unsigned)index < PLCList.size())
	{
		vector<PLC> plcv;
		for (int i = 0; i < length; i++)
			plcv.push_back(*plcs++);
		PLCList[index] = plcv;
		return true;
	}
	else
		return false;
}

void RegisterObjArtPtr(const void* art)
{
	ObjArtPtrs[reg_a0.Object] = art;
}

static const HelperFunctions helperFunctions =
{
	ModLoaderVer,
	RegisterPLCList,
	GetPLCList,
	SetPLCList,
	PrintDebug,
	RegisterObjArtPtr
};

static vector<string> &split(const string &s, char delim, vector<string> &elems) {
	std::stringstream ss(s);
	string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


static vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

static string trim(const string &s)
{
	auto st = s.find_first_not_of(' ');
	if (st == string::npos)
		st = 0;
	auto ed = s.find_last_not_of(' ');
	if (ed == string::npos)
		ed = s.size() - 1;
	return s.substr(st, (ed + 1) - st);
}

template<typename T>
static inline T *arrcpy(T *dst, const T *src, size_t cnt)
{
	return (T *)memcpy(dst, src, cnt * sizeof(T));
}

template<typename T>
static inline void clrmem(T *mem)
{
	ZeroMemory(mem, sizeof(T));
}

static const unordered_map<string, uint8_t> levelidsnamemap = {
	{ "angel_island_zone", angel_island_zone },
	{ "hydrocity_zone", hydrocity_zone },
	{ "marble_garden_zone", marble_garden_zone },
	{ "carnival_night_zone", carnival_night_zone },
	{ "flying_battery_zone", flying_battery_zone },
	{ "icecap_zone", icecap_zone },
	{ "launch_base_zone", launch_base_zone },
	{ "mushroom_hill_zone", mushroom_hill_zone },
	{ "sandopolis_zone", sandopolis_zone },
	{ "lava_reef_zone", lava_reef_zone },
	{ "sky_sanctuary_zone", sky_sanctuary_zone },
	{ "death_egg_zone", death_egg_zone },
	{ "doomsday_zone", doomsday_zone },
	{ "ending_zone", ending_zone },
	{ "azure_lake_zone", azure_lake_zone },
	{ "balloon_park_zone", balloon_park_zone },
	{ "desert_palace_zone", desert_palace_zone },
	{ "chrome_gadget_zone", chrome_gadget_zone },
	{ "endless_mine_zone", endless_mine_zone },
	{ "gumball_bonus_zone", gumball_bonus_zone },
	{ "pachinko_bonus_zone", pachinko_bonus_zone },
	{ "slots_bonus_zone", slots_bonus_zone },
	{ "lava_reef_zone_2", lava_reef_zone_2 },
	{ "death_egg_zone_2", death_egg_zone_2 }
};

static uint8_t ParseLevelID(const string &str)
{
	string str2 = trim(str);
	transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
	auto lv = levelidsnamemap.find(str2);
	if (lv != levelidsnamemap.end())
		return lv->second;
	else
		return (uint8_t)strtol(str.c_str(), nullptr, 16);
}

static const unordered_map<string, uint16_t> levelandactidsnamemap = {
	{ "angel_island_zone_act_1", angel_island_zone_act_1 },
	{ "angel_island_zone_act_2", angel_island_zone_act_2 },
	{ "hydrocity_zone_act_1", hydrocity_zone_act_1 },
	{ "hydrocity_zone_act_2", hydrocity_zone_act_2 },
	{ "marble_garden_zone_act_1", marble_garden_zone_act_1 },
	{ "marble_garden_zone_act_2", marble_garden_zone_act_2 },
	{ "carnival_night_zone_act_1", carnival_night_zone_act_1 },
	{ "carnival_night_zone_act_2", carnival_night_zone_act_2 },
	{ "flying_battery_zone_act_1", flying_battery_zone_act_1 },
	{ "flying_battery_zone_act_2", flying_battery_zone_act_2 },
	{ "icecap_zone_act_1", icecap_zone_act_1 },
	{ "icecap_zone_act_2", icecap_zone_act_2 },
	{ "launch_base_zone_act_1", launch_base_zone_act_1 },
	{ "launch_base_zone_act_2", launch_base_zone_act_2 },
	{ "mushroom_hill_zone_act_1", mushroom_hill_zone_act_1 },
	{ "mushroom_hill_zone_act_2", mushroom_hill_zone_act_2 },
	{ "sandopolis_zone_act_1", sandopolis_zone_act_1 },
	{ "sandopolis_zone_act_2", sandopolis_zone_act_2 },
	{ "lava_reef_zone_act_1", lava_reef_zone_act_1 },
	{ "lava_reef_zone_act_2", lava_reef_zone_act_2 },
	{ "sky_sanctuary_zone_act_1", sky_sanctuary_zone_act_1 },
	{ "sky_sanctuary_zone_act_2", sky_sanctuary_zone_act_2 },
	{ "death_egg_zone_act_1", death_egg_zone_act_1 },
	{ "death_egg_zone_act_2", death_egg_zone_act_2 },
	{ "doomsday_zone_act_1", doomsday_zone_act_1 },
	{ "ending_zone_act_2", ending_zone_act_2 },
	{ "azure_lake_zone_act_1", azure_lake_zone_act_1 },
	{ "balloon_park_zone_act_1", balloon_park_zone_act_1 },
	{ "desert_palace_zone_act_1", desert_palace_zone_act_1 },
	{ "chrome_gadget_zone_act_1", chrome_gadget_zone_act_1 },
	{ "endless_mine_zone_act_1", endless_mine_zone_act_1 },
	{ "gumball_bonus_zone_act_1", gumball_bonus_zone_act_1 },
	{ "pachinko_bonus_zone_act_1", pachinko_bonus_zone_act_1 },
	{ "slots_bonus_zone_act_1", slots_bonus_zone_act_1 },
	{ "lava_reef_zone_boss", lava_reef_zone_boss },
	{ "hidden_palace_zone", hidden_palace_zone },
	{ "death_egg_zone_boss", death_egg_zone_boss },
	{ "hidden_palace_shrine", hidden_palace_shrine }
};

static uint16_t ParseLevelAndActID(const string &str)
{
	string str2 = trim(str);
	transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
	auto lv = levelandactidsnamemap.find(str2);
	if (lv != levelandactidsnamemap.end())
		return lv->second;
	else
		return (uint16_t)strtol(str.c_str(), nullptr, 16);
}

template<typename T>
static void ProcessPointerList(const string &list, T *item)
{
	vector<string> ptrs = split(list, ',');
	for (unsigned int i = 0; i < ptrs.size(); i++)
		WriteData((T **)(strtol(ptrs[i].c_str(), nullptr, 16) + 0x400000), item);
}

static void *ReadAllBytes(const wstring &file, int *length = nullptr)
{
	FILE *fp;
	_wfopen_s(&fp, file.c_str(), L"rb");
	if (!fp) return nullptr;
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	char *buf = new char[size];
	fseek(fp, 0, SEEK_SET);
	fread_s(buf, size, 1, size, fp);
	fclose(fp);
	if (length)
		*length = size;
	return buf;
}

static void ProcessLevelINI(const IniGroup *group, const wstring &mod_dir)
{
	uint16_t level = ParseLevelAndActID(group->getString("level", "0"));
	int li = ((level & 0xFF00) >> 7) | (level & 1);
	LoadBlock *lb = &LevelLoadBlock[li];
	if (group->hasKeyNonEmpty("plc1"))
	{
		lb->PLC1 = (uint16_t)group->getIntRadix("plc1", 16);
		lb->PLC2 = (uint16_t)group->getIntRadix("plc2", 16, lb->PLC1);
	}
	if (group->hasKeyNonEmpty("tiles1"))
	{
		void *tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("tiles1"));
		if (tmp)
			lb->TilesPrimary = tmp;
		if (group->hasKeyNonEmpty("tiles2") && (tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("tiles2"))))
			lb->TilesSecondary = tmp;
		else
			lb->TilesSecondary = lb->TilesPrimary;
	}
	lb->Pal2 = lb->Pal1 = (uint8_t)group->getInt("palette", lb->Pal1);
	if (group->hasKeyNonEmpty("blocks1"))
	{
		void *tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("blocks1"));
		if (tmp)
			lb->BlocksPrimary = tmp;
		if (group->hasKeyNonEmpty("blocks2") && (tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("blocks2"))))
			lb->BlocksSecondary = tmp;
		else
			lb->BlocksSecondary = lb->BlocksPrimary;
	}
	if (group->hasKeyNonEmpty("chunks1"))
	{
		void *tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("chunks1"));
		if (tmp)
			lb->ChunksPrimary = tmp;
		if (group->hasKeyNonEmpty("chunks2") && (tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("chunks2"))))
			lb->ChunksSecondary = tmp;
		else
			lb->ChunksSecondary = lb->ChunksPrimary;
	}
	if (group->hasKeyNonEmpty("layout"))
	{
		void *tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("layout"));
		if (tmp)
			LevelPtrs[li] = tmp;
	}
	if (group->hasKeyNonEmpty("objects"))
	{
		void *tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("objects"));
		if (tmp)
			SpriteLocPtrs[li] = tmp;
		if (group->hasKeyNonEmpty("objects3") && (tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("objects3"))))
			SpriteLocPtrs3[li] = tmp;
		else
			SpriteLocPtrs3[li] = SpriteLocPtrs[li];
	}
	if (group->hasKeyNonEmpty("rings"))
	{
		void *tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("rings"));
		if (tmp)
			RingLocPtrs[li] = tmp;
		if (group->hasKeyNonEmpty("rings3") && (tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("rings3"))))
			RingLocPtrs3[li] = tmp;
		else
			RingLocPtrs3[li] = RingLocPtrs[li];
	}
	if (group->hasKeyNonEmpty("sonicstart"))
	{
		StartPos *tmp = (StartPos *)ReadAllBytes(mod_dir + L'\\' + group->getWString("sonicstart"));
		if (tmp)
			Sonic_Start_Locations[li] = *tmp;
	}
	if (group->hasKeyNonEmpty("knuxstart"))
	{
		StartPos *tmp = (StartPos *)ReadAllBytes(mod_dir + L'\\' + group->getWString("knuxstart"));
		if (tmp)
			Knux_Start_Locations[li] = *tmp;
	}
	if (group->hasKeyNonEmpty("colinds"))
	{
		void *tmp = ReadAllBytes(mod_dir + L'\\' + group->getWString("colinds"));
		if (tmp)
			SolidIndexes[li] = tmp;
	}
}

static void ProcessPaletteINI(const IniGroup *group, const wstring &mod_dir)
{
	int index = group->getInt("index");
	PalPtr *ptr = &PalPoint[index];
	int length;
	uint16_t *colors = (uint16_t *)ReadAllBytes(mod_dir + L'\\' + group->getWString("filename"), &length);
	if (!colors) return;
	ptr->Colors = colors;
	ptr->Location = (uint16_t)group->getIntRadix("location", 16, 0xFC00);
	ptr->Size = (uint16_t)((length / 4) - 1);
}

static const unordered_map<string, void(__cdecl *)(const IniGroup *, const wstring &)> exedatafuncmap = {
	{ "level", ProcessLevelINI },
	{ "palette", ProcessPaletteINI }
};

string savepath;
void __cdecl ReadSaveFile()
{
	size_t v0; // eax@1
	CHAR FileName[256]; // [sp+Ch] [bp-108h]@1
	DWORD NumberOfBytesRead; // [sp+10Ch] [bp-8h]@3
	HANDLE hObject; // [sp+110h] [bp-4h]@1

	memset((void*)0x8549E8, 0, 0x400u);
	GetModuleFileNameA(0, FileName, 0x100u);
	v0 = strlen(FileName);
	strcpy(&FileName[v0 - 3], "BIN");
	string path = savepath + '\\' + FileName;
	hObject = CreateFileA(path.c_str(), 0x80000000, 1u, 0, 3u, 0x80u, 0);
	if (hObject != INVALID_HANDLE_VALUE)
	{
		ReadFile(hObject, (void*)0x8549E8, 0x400u, &NumberOfBytesRead, 0);
		CloseHandle(hObject);
	}
}

void __cdecl WriteSaveFile()
{
	size_t v0; // eax@1
	char FileName[256]; // [sp+Ch] [bp-108h]@1
	HANDLE hObject; // [sp+10Ch] [bp-8h]@1
	DWORD NumberOfBytesWritten; // [sp+110h] [bp-4h]@3

	if (!IsDirectory(savepath))
		CreateDirectoryA(savepath.c_str(), nullptr);
	GetModuleFileNameA(0, FileName, 0x100u);
	v0 = strlen(FileName);
	strcpy(&FileName[v0 - 3], "BIN");
	string path = savepath + '\\' + FileName;
	hObject = CreateFileA(path.c_str(), 0x40000000u, 0, 0, 2u, 0x80u, 0);
	if (hObject == INVALID_HANDLE_VALUE)
		MessageBoxA(0, "Cannot Open Save Data File", "Error", 0);
	else
	{
		WriteFile(hObject, (void*)0x8549E8, 0x400u, &NumberOfBytesWritten, 0);
		if (NumberOfBytesWritten != 1024)
			MessageBoxA(0, "Cannot Write Save Data File", "Error", 0);
		CloseHandle(hObject);
	}
}

MidiInterface *musicobj = nullptr;
void FixSoundTestStopButton()
{
	reg_d0.UByte = 0xE2;
	musicobj->stopSong();
}

static void __cdecl InitMods(void)
{
	FILE *f_ini = _wfopen(L"mods\\SKCModLoader.ini", L"r");
	if (!f_ini)
	{
		MessageBox(NULL, L"mods\\SKCModLoader.ini could not be read!", L"S&KC Mod Loader", MB_ICONWARNING);
		return;
	}
	unique_ptr<IniFile> ini(new IniFile(f_ini));
	fclose(f_ini);

	// Get SONIC3K.EXE's path and filename.
	wchar_t pathbuf[MAX_PATH];
	GetModuleFileName(NULL, pathbuf, MAX_PATH);
	wstring exepath(pathbuf);
	wstring exefilename;
	string::size_type slash_pos = exepath.find_last_of(L"/\\");
	if (slash_pos != string::npos)
	{
		exefilename = exepath.substr(slash_pos + 1);
		if (slash_pos > 0)
			exepath = exepath.substr(0, slash_pos);
	}
	
	// Convert the EXE filename to lowercase.
	transform(exefilename.begin(), exefilename.end(), exefilename.begin(), ::towlower);

	// Process the main Mod Loader settings.
	const IniGroup *settings = ini->getGroup("");

	if (settings->getBool("DebugConsole"))
	{
		// Enable the debug console.
		// TODO: setvbuf()?
		AllocConsole();
		SetConsoleTitle(L"S&KC Mod Loader output");
		freopen("CONOUT$", "wb", stdout);
		dbgConsole = true;
	}

	if (settings->getBool("DebugFile"))
	{
		// Enable debug logging to a file.
		// dbgFile will be nullptr if the file couldn't be opened.
		dbgFile = _wfopen(L"mods\\SKCModLoader.log", L"a+");
	}

	// Is any debug method enabled?
	if (dbgConsole || dbgFile)
		PrintDebug("S&KC Mod Loader v%d, built %s\n",
			ModLoaderVer, __TIMESTAMP__);

	// Unprotect the .text section.
	// TODO: Get .text address and length dynamically.
	DWORD oldprot;
	VirtualProtect((void *)0x401000, 0x427C00, PAGE_EXECUTE_WRITECOPY, &oldprot);

	InitFramerateFix();

	InitPaletteFix(settings);

	WriteCall((void*)0x40B33D, FixSoundTestStopButton);

	// Sprite art extension
	WriteJump((void*)0x41FAC8, SpriteFunc1);
	WriteJump((void*)0x4201B3, SpriteFunc2);
	WriteJump((void*)0x4049EB, ConvertSpriteTable_r);
	WriteJump((void*)0x404B1A, ConvertSpriteTable_P2_r);

	// Fix Special Stage floor scrolling
	void **ssmapptr = (void**)0x69EF3A;
	for (int i = 0; i < 8; i++)
	{
		*ssmapptr = (void*)0x8FF5DC0;
		ssmapptr += 4;
	}

	// Fix splash art in HCZ
	unsigned short *splashart = (unsigned short*)0x542F24;
	for (size_t i = 0; i < 0x9C0; i++)
		splashart[i] = _byteswap_ushort(splashart[i]);

	vector<std::pair<ModInitFunc, wstring>> initfuncs;

	// It's mod loading time!
	PrintDebug("Loading mods...\n");
	for (int i = 1; i < 999; i++)
	{
		char key[8];
		snprintf(key, sizeof(key), "Mod%d", i);
		if (!settings->hasKey(key))
			break;

		const string mod_dirA = "mods\\" + settings->getString(key);
		const wstring mod_dir = L"mods\\" + settings->getWString(key);
		const wstring mod_inifile = mod_dir + L"\\mod.ini";
		FILE *f_mod_ini = _wfopen(mod_inifile.c_str(), L"r");
		if (!f_mod_ini)
		{
			PrintDebug("Could not open file mod.ini in \"mods\\%s\".\n", mod_dirA.c_str());
			continue;
		}
		unique_ptr<IniFile> ini_mod(new IniFile(f_mod_ini));
		fclose(f_mod_ini);

		const IniGroup *modinfo = ini_mod->getGroup("");
		const string mod_nameA = modinfo->getString("Name");
		const wstring mod_name = modinfo->getWString("Name");
		PrintDebug("%d. %s\n", i, mod_nameA.c_str());

		string musicfol = mod_dirA + "\\Music";
		if (IsDirectory(musicfol))
			musicobj->AddMusicFolder(musicfol + '\\');

		// Check if a custom EXE is required.
		if (modinfo->hasKeyNonEmpty("EXEFile"))
		{
			wstring modexe = modinfo->getWString("EXEFile");
			transform(modexe.begin(), modexe.end(), modexe.begin(), ::towlower);

			// Are we using the correct EXE?
			if (modexe.compare(exefilename) != 0)
			{
				wchar_t msg[4096];
				swprintf(msg, LengthOfArray(msg),
					L"Mod \"%s\" should be run from \"%s\", but you are running \"%s\".\n\n"
					L"Continue anyway?", mod_name.c_str(), modexe.c_str(), exefilename.c_str());
				if (MessageBox(NULL, msg, L"S&KC Mod Loader", MB_ICONWARNING | MB_YESNO) == IDNO)
					ExitProcess(1);
			}
		}

		// Check if the mod has a DLL file.
		if (modinfo->hasKeyNonEmpty("DLLFile"))
		{
			// Prepend the mod directory.
			// TODO: SetDllDirectory().
			wstring dll_filename = mod_dir + L'\\' + modinfo->getWString("DLLFile");
			HMODULE module = LoadLibrary(dll_filename.c_str());
			if (module)
			{
				const ModInfo *info = (const ModInfo *)GetProcAddress(module, "SKCModInfo");
				if (info)
				{
					const ModInitFunc init = (const ModInitFunc)GetProcAddress(module, "Init");
					if (init)
						initfuncs.push_back({ init, mod_dir });
					const PatchList *patches = (const PatchList *)GetProcAddress(module, "Patches");
					if (patches)
						for (int i = 0; i < patches->Count; i++)
							WriteData(patches->Patches[i].address, patches->Patches[i].data, patches->Patches[i].datasize);
					const PointerList *jumps = (const PointerList *)GetProcAddress(module, "Jumps");
					if (jumps)
						for (int i = 0; i < jumps->Count; i++)
							WriteJump(jumps->Pointers[i].address, jumps->Pointers[i].data);
					const PointerList *calls = (const PointerList *)GetProcAddress(module, "Calls");
					if (calls)
						for (int i = 0; i < calls->Count; i++)
							WriteCall(calls->Pointers[i].address, calls->Pointers[i].data);
					const PointerList *pointers = (const PointerList *)GetProcAddress(module, "Pointers");
					if (pointers)
						for (int i = 0; i < pointers->Count; i++)
							WriteData((void **)pointers->Pointers[i].address, pointers->Pointers[i].data);
					const ModFrameFunc frame = (const ModFrameFunc)GetProcAddress(module, "OnFrame");
					if (frame)
						modFrameFuncs.push_back(frame);
				}
				else
				{
					const string dll_filenameA = UTF16toMBS(dll_filename, CP_ACP);
					PrintDebug("File \"%s\" is not a valid mod file.\n", dll_filenameA.c_str());
				}
			}
			else
			{
				const string dll_filenameA = UTF16toMBS(dll_filename, CP_ACP);
				PrintDebug("Failed loading file \"%s\".\n", dll_filenameA.c_str());
			}
		}

		// Check if the mod has EXE data replacements.
		if (modinfo->hasKeyNonEmpty("EXEData"))
		{
			IniFile *exedata = new IniFile(mod_dir + L'\\' + modinfo->getWString("EXEData"));
			for (auto iter = exedata->cbegin(); iter != exedata->cend(); iter++)
			{
				IniGroup *group = iter->second;
				auto type = exedatafuncmap.find(group->getString("type"));
				if (type != exedatafuncmap.end())
					type->second(group, mod_dir);
			}
			delete exedata;
		}

		if (modinfo->getBool("RedirectSaveFile"))
			savepath = mod_dirA + "\\savedata";
	}

	if (!savepath.empty())
	{
		WriteJump((void*)0x404E0C, ReadSaveFile);
		WriteJump((void*)0x404ECE, WriteSaveFile);
	}

	for (auto &a : initfuncs)
		a.first(a.second.c_str(), helperFunctions);

	if (PLCList.size() > 0)
	{
		int memsz = PLCList.size() * 4;
		for (size_t i = 0; i < PLCList.size(); i++)
			memsz += PLCList[i].size() * 6;
		void *buf = new char[memsz];
		uint16_t off = (uint16_t)PLCList.size() * 2;
		uint16_t *offptr = (uint16_t *)buf;
		for (size_t i = 0; i < PLCList.size(); i++)
		{
			*offptr++ = off;
			off += ((uint16_t)PLCList[i].size() * 6) + 2;
		}
		uint16_t *cntptr = offptr;
		PLC *plcptr = nullptr;
		for (size_t i = 0; i < PLCList.size(); i++)
		{
			*cntptr++ = (uint16_t)PLCList[i].size() - 1;
			plcptr = (PLC *)cntptr;
			for (size_t j = 0; j < PLCList[i].size(); j++)
				*plcptr++ = PLCList[i][j];
			cntptr = (uint16_t *)plcptr;
		}
		WriteData((void **)0x41145C, buf);
	}

	PrintDebug("Finished loading mods\n");

	// Check for patches.
	ifstream patches_str("mods\\Patches.dat", ifstream::binary);
	if (patches_str.is_open())
	{
		CodeParser patchParser;
		static const char codemagic[6] = { 'c', 'o', 'd', 'e', 'v', '5' };
		char buf[sizeof(codemagic)];
		patches_str.read(buf, sizeof(buf));
		if (!memcmp(buf, codemagic, sizeof(codemagic)))
		{
			int codecount_header;
			patches_str.read((char*)&codecount_header, sizeof(codecount_header));
			PrintDebug("Loading %d patches...\n", codecount_header);
			patches_str.seekg(0);
			int codecount = patchParser.readCodes(patches_str);
			if (codecount >= 0)
			{
				PrintDebug("Loaded %d patches.\n", codecount);
				patchParser.processCodeList();
			}
			else
			{
				PrintDebug("ERROR loading patches: ");
				switch (codecount)
				{
				case -EINVAL:
					PrintDebug("Patch file is not in the correct format.\n");
					break;
				default:
					PrintDebug("%s\n", strerror(-codecount));
					break;
				}
			}
		}
		else
		{
			PrintDebug("Patch file is not in the correct format.\n");
		}
		patches_str.close();
	}

	// Check for codes.
	ifstream codes_str("mods\\Codes.dat", ifstream::binary);
	if (codes_str.is_open())
	{
		static const char codemagic[6] = {'c', 'o', 'd', 'e', 'v', '5'};
		char buf[sizeof(codemagic)];
		codes_str.read(buf, sizeof(buf));
		if (!memcmp(buf, codemagic, sizeof(codemagic)))
		{
			int codecount_header;
			codes_str.read((char*)&codecount_header, sizeof(codecount_header));
			PrintDebug("Loading %d codes...\n", codecount_header);
			codes_str.seekg(0);
			int codecount = codeParser.readCodes(codes_str);
			if (codecount >= 0)
			{
				PrintDebug("Loaded %d codes.\n", codecount);
			}
			else
			{
				PrintDebug("ERROR loading codes: ");
				switch (codecount)
				{
					case -EINVAL:
						PrintDebug("Code file is not in the correct format.\n");
						break;
					default:
						PrintDebug("%s\n", strerror(-codecount));
						break;
				}
			}
		}
		else
		{
			PrintDebug("Code file is not in the correct format.\n");
		}
		codes_str.close();
	}
	WriteCall((void *)0x4051EB, ProcessCodes);
}

HMODULE moduleHandle;
extern "C" __declspec(dllexport) MidiInterface *GetMidiInterface()
{
	musicobj = new MidiInterface(moduleHandle);
	InitMods();
	return musicobj;
}

/**
 * DLL entry point.
 * @param hinstDll DLL instance.
 * @param fdwReason Reason for calling DllMain.
 * @param lpvReserved Reserved.
 */
BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			moduleHandle = hinstDll;
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			// Make sure the log file is closed.
			if (dbgFile)
			{
				fclose(dbgFile);
				dbgFile = nullptr;
			}
			break;
	}

	return TRUE;
}
