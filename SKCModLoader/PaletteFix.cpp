#include "stdafx.h"
#include <algorithm>
#include "SKCModLoader.h"
#include "IniFile.hpp"
#include "PaletteFix.h"

using std::string;
using std::wstring;
using std::unordered_map;
using std::transform;

struct Color
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char X;
};

#pragma warning(push)
#pragma warning(disable : 4838 4309)
const char MDColorTable_Accurate[]{ 0, 52, 87, 116, 144, 172, 206, 255 };
const char MDColorTable_Linear[] = { 0, 0x24, 0x49, 0x6D, 0x92, 0xB6, 0xDB, 0xFF };
const char MDColorTable_SKC[] = { 0, 0x22, 0x44, 0x66, 0x88, 0xAA, 0xCC, 0xEE };
const char MDColorTable_Old[] = { 0, 0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0 };
#pragma warning(pop)

const char *MDColorTable = MDColorTable_Accurate;

inline void ConvertColor(short md, Color &pc)
{
	pc.R = MDColorTable[(md & 0xF) >> 1];
	pc.G = MDColorTable[((md >> 4) & 0xF) >> 1];
	pc.B = MDColorTable[((md >> 8) & 0xF) >> 1];
}

DataPointer(char, b_water_full_screen_flag, 0x8FFF64E);
DataArray(short, water_palette, 0x8FFF080, 64);
DataPointer(int, TransparentColorIndex, 0x8549D8);
FunctionPointer(int, sub_404350, (), 0x404350);
DataPointer(int, dword_854DE8, 0x854DE8);
ThiscallFunctionPointer(void, CopyPalette2, (int _this, const void *Src), 0x4069D0);
ThiscallFunctionPointer(void, CopyPalette, (int _this, const void *Src), 0x40699E);
DataPointer(char, b_hyper_sonic_flash_timer, 0x8FFF666);
DataPointer(short, w_H_int_counter_command, 0x8FFF624);

void __cdecl ConvertPalette_r()
{
	Color rgbpal[64]; // [sp+Ch] [bp-110h]@1
	int i; // [sp+10Ch] [bp-10h]@4
	__int16 *mdpal; // [sp+110h] [bp-Ch]@2
	Color bgcol; // [sp+114h] [bp-8h]@7
	int palette_dest; // [sp+118h] [bp-4h]@7

	memset(rgbpal, 0, 0x100u);
	if (b_water_full_screen_flag)
		mdpal = water_palette;
	else
		mdpal = Normal_palette;
	for (i = 0; i < 64; ++i)
		ConvertColor(mdpal[i], rgbpal[i]);
	bgcol = rgbpal[TransparentColorIndex];
	rgbpal[0] = bgcol;
	rgbpal[16] = bgcol;
	rgbpal[32] = bgcol;
	rgbpal[48] = bgcol;
	palette_dest = sub_404350();
	if (dword_854DE8 == 5)
	{
		CopyPalette2(palette_dest, rgbpal);
		for (i = 0; i < 64; ++i)
		{
			rgbpal[i].R >>= 1;
			rgbpal[i].G >>= 1;
			rgbpal[i].B >>= 1;
		}
		CopyPalette(palette_dest, rgbpal);
	}
	else
	{
		CopyPalette(palette_dest, rgbpal);
		CopyPalette2(palette_dest, rgbpal);
	}
}

void __cdecl ConvertPalette2_r()
{
	signed int flash_flag; // [sp+Ch] [bp-114h]@2
	Color rgbpal[64]; // [sp+10h] [bp-110h]@4
	int i; // [sp+110h] [bp-10h]@7
	__int16 *mdpal; // [sp+114h] [bp-Ch]@6
	Color bgcol; // [sp+118h] [bp-8h]@17
	int palette_dest; // [sp+11Ch] [bp-4h]@17

	if ((signed int)(unsigned __int8)b_hyper_sonic_flash_timer <= 0)
	{
		flash_flag = 0;
	}
	else
	{
		--b_hyper_sonic_flash_timer;
		flash_flag = 1;
	}
	memset(rgbpal, 0, 0x100u);
	if (b_water_full_screen_flag)
		*(char*)&w_H_int_counter_command = 0;
	mdpal = Normal_palette;
	if (flash_flag)
	{
		for (i = 0; i < 32; ++i)
			ConvertColor(0xEEE, rgbpal[i]);
		for (i = 33; i < 64; ++i)
			ConvertColor(0xEEE, rgbpal[i]);
	}
	else
	{
		for (i = 0; i < 64; ++i)
			ConvertColor(mdpal[i], rgbpal[i]);
	}
	bgcol = rgbpal[TransparentColorIndex];
	rgbpal[0] = bgcol;
	rgbpal[16] = bgcol;
	rgbpal[32] = bgcol;
	rgbpal[48] = bgcol;
	palette_dest = sub_404350();
	CopyPalette(palette_dest, rgbpal);
	mdpal = water_palette;
	if (flash_flag)
	{
		for (i = 0; i < 32; ++i)
			ConvertColor(0xEEE, rgbpal[i]);
		for (i = 33; i < 64; ++i)
			ConvertColor(0xEEE, rgbpal[i]);
	}
	else
	{
		for (i = 0; i < 64; ++i)
			ConvertColor(mdpal[i], rgbpal[i]);
	}
	bgcol = rgbpal[TransparentColorIndex];
	rgbpal[0] = bgcol;
	rgbpal[16] = bgcol;
	rgbpal[32] = bgcol;
	rgbpal[48] = bgcol;
	CopyPalette2(palette_dest, rgbpal);
}

const unordered_map<string, const char*> palmodes = {
	{ "linear", MDColorTable_Linear },
	{ "skc", MDColorTable_SKC },
	{ "old", MDColorTable_Old }
};

void InitPaletteFix(const IniGroup *settings)
{
	string palmode = settings->getString("Palette");
	transform(palmode.begin(), palmode.end(), palmode.begin(), tolower);
	auto iter = palmodes.find(palmode);
	if (iter != palmodes.cend())
		MDColorTable = iter->second;
	WriteJump((void*)0x405536, ConvertPalette_r);
	WriteJump((void*)0x4056F7, ConvertPalette2_r);
}