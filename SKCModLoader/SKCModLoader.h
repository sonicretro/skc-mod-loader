#ifndef SKCMODLOADER_H
#define SKCMODLOADER_H

#include <stdint.h>

#ifdef _MSC_VER
// MSVC doesn't have snprintf(), but it does have _snprintf().
#define snprintf(str, size, format, ...) _snprintf(str, size, format, __VA_ARGS__)
#endif

// Utility Functions
#ifdef __cplusplus
// C++ version.

/**
* Get the number of elements in an array.
* @return Number of elements in the array.
*/
template <typename T, size_t N>
static inline size_t LengthOfArray(const T(&)[N])
{
	return N;
}

/**
* Get the size of an array.
* @return Size of the array, in bytes.
*/
template <typename T, size_t N>
static inline size_t SizeOfArray(const T(&)[N])
{
	return N * sizeof(T);
}
#else

// C version.

/**
* Number of elements in an array.
*
* Includes a static check for pointers to make sure
* a dynamically-allocated array wasn't specified.
* Reference: http://stackoverflow.com/questions/8018843/macro-definition-array-size
*/
#define LengthOfArray(x) \
	((int)(((sizeof(x) / sizeof(x[0]))) / \
		(size_t)(!(sizeof(x) % sizeof(x[0])))))

#define SizeOfArray(x) sizeof(x)

#endif

// Macros for functions that need both an array
// and the array length or size.
#define arrayptrandlength(data) data, LengthOfArray(data)
#define arraylengthandptr(data) LengthOfArray(data), data
#define arrayptrandsize(data) data, SizeOfArray(data)
#define arraysizeandptr(data) SizeOfArray(data), data

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

static inline BOOL WriteData(void *writeaddress, const void *data, SIZE_T datasize, SIZE_T *byteswritten)
{
	return WriteProcessMemory(GetCurrentProcess(), writeaddress, data, datasize, byteswritten);
}

static inline BOOL WriteData(void *writeaddress, const void *data, SIZE_T datasize)
{
	return WriteData(writeaddress, data, datasize, nullptr);
}

template<typename T>
static inline BOOL WriteData(T const *writeaddress, const T data, SIZE_T *byteswritten)
{
	return WriteData((void*)writeaddress, (void*)&data, (SIZE_T)sizeof(data), byteswritten);
}

template<typename T>
static inline BOOL WriteData(T const *writeaddress, const T data)
{
	return WriteData(writeaddress, data, nullptr);
}

template<typename T>
static inline BOOL WriteData(T *writeaddress, const T &data, SIZE_T *byteswritten)
{
	return WriteData(writeaddress, &data, sizeof(data), byteswritten);
}

template<typename T>
static inline BOOL WriteData(T *writeaddress, const T &data)
{
	return WriteData(writeaddress, data, nullptr);
}

template <typename T, size_t N>
static inline BOOL WriteData(void *writeaddress, const T(&data)[N], SIZE_T *byteswritten)
{
	return WriteData(writeaddress, data, SizeOfArray(data), byteswritten);
}

template <typename T, size_t N>
static inline BOOL WriteData(void *writeaddress, const T(&data)[N])
{
	return WriteData(writeaddress, data, nullptr);
}

/**
* Write a repeated byte to an arbitrary address.
* @param address	[in] Address.
* @param data		[in] Byte to write.
* @param count		[in] Number of repetitions.
* @param byteswritten	[out, opt] Number of bytes written.
* @return Nonzero on success; 0 on error (check GetLastError()).
*/
static inline BOOL WriteData(void *address, const char data, int count, SIZE_T *byteswritten)
{
	char *buf = new char[count];
	memset(buf, data, count);
	int result = WriteData(address, buf, count, byteswritten);
	delete[] buf;
	return result;
}

/**
* Write a repeated byte to an arbitrary address.
* @param address	[in] Address.
* @param data		[in] Byte to write.
* @param count		[in] Number of repetitions.
* @return Nonzero on success; 0 on error (check GetLastError()).
*/
static inline BOOL WriteData(void *address, char data, int count)
{
	return WriteData(address, data, count, nullptr);
}

#if (defined(__i386__) || defined(_M_IX86)) && \
    !(defined(__x86_64__) || defined(_M_X64))
/**
* Write a JMP instruction to an arbitrary address.
* @param writeaddress Address to insert the JMP instruction.
* @param funcaddress Address to JMP to.
* @return Nonzero on success; 0 on error (check GetLastError()).
*/
static inline BOOL WriteJump(void *writeaddress, void *funcaddress)
{
	uint8_t data[5];
	data[0] = 0xE9; // JMP DWORD (relative)
	*(int32_t*)(data + 1) = (uint32_t)funcaddress - ((uint32_t)writeaddress + 5);
	return WriteData(writeaddress, data);
}

/**
* Write a CALL instruction to an arbitrary address.
* @param writeaddress Address to insert the CALL instruction.
* @param funcaddress Address to CALL.
* @return Nonzero on success; 0 on error (check GetLastError()).
*/
static inline BOOL WriteCall(void *writeaddress, void *funcaddress)
{
	uint8_t data[5];
	data[0] = 0xE8; // CALL DWORD (relative)
	*(int32_t*)(data + 1) = (uint32_t)funcaddress - ((uint32_t)writeaddress + 5);
	return WriteData(writeaddress, data);
}

#endif

// Constants
enum LevelIDs
{
	angel_island_zone,
	hydrocity_zone,
	marble_garden_zone,
	carnival_night_zone,
	flying_battery_zone,
	icecap_zone,
	launch_base_zone,
	mushroom_hill_zone,
	sandopolis_zone,
	lava_reef_zone,
	sky_sanctuary_zone,
	death_egg_zone,
	doomsday_zone,
	ending_zone,
	azure_lake_zone,
	balloon_park_zone,
	desert_palace_zone,
	chrome_gadget_zone,
	endless_mine_zone,
	gumball_bonus_zone,
	pachinko_bonus_zone,
	slots_bonus_zone,
	lava_reef_zone_2,
	death_egg_zone_2
};

enum LevelAndActIDs
{
	angel_island_zone_act_1 = angel_island_zone << 8,
	angel_island_zone_act_2,
	hydrocity_zone_act_1 = hydrocity_zone << 8,
	hydrocity_zone_act_2,
	marble_garden_zone_act_1 = marble_garden_zone << 8,
	marble_garden_zone_act_2,
	carnival_night_zone_act_1 = carnival_night_zone << 8,
	carnival_night_zone_act_2,
	flying_battery_zone_act_1 = flying_battery_zone << 8,
	flying_battery_zone_act_2,
	icecap_zone_act_1 = icecap_zone << 8,
	icecap_zone_act_2,
	launch_base_zone_act_1 = launch_base_zone << 8,
	launch_base_zone_act_2,
	mushroom_hill_zone_act_1 = mushroom_hill_zone << 8,
	mushroom_hill_zone_act_2,
	sandopolis_zone_act_1 = sandopolis_zone << 8,
	sandopolis_zone_act_2,
	lava_reef_zone_act_1 = lava_reef_zone << 8,
	lava_reef_zone_act_2,
	sky_sanctuary_zone_act_1 = sky_sanctuary_zone << 8,
	sky_sanctuary_zone_act_2,
	death_egg_zone_act_1 = death_egg_zone << 8,
	death_egg_zone_act_2,
	doomsday_zone_act_1 = doomsday_zone << 8,
	ending_zone_act_2 = (ending_zone << 8) | 1,
	azure_lake_zone_act_1 = azure_lake_zone << 8,
	balloon_park_zone_act_1 = balloon_park_zone << 8,
	desert_palace_zone_act_1 = desert_palace_zone << 8,
	chrome_gadget_zone_act_1 = chrome_gadget_zone << 8,
	endless_mine_zone_act_1 = endless_mine_zone << 8,
	gumball_bonus_zone_act_1 = gumball_bonus_zone << 8,
	pachinko_bonus_zone_act_1 = pachinko_bonus_zone << 8,
	slots_bonus_zone_act_1 = slots_bonus_zone << 8,
	lava_reef_zone_boss = lava_reef_zone_2 << 8,
	hidden_palace_zone,
	death_egg_zone_boss = death_egg_zone_2 << 8,
	hidden_palace_shrine
};

// Structures
struct PLC
{
	const void *ArtLoc;
	uint16_t VRAMAddr;
};

struct LoadBlock
{
	uint16_t PLC1, PLC2;
	const void *TilesPrimary;
	const void *TilesSecondary;
	uint16_t Pal1, Pal2;
	const void *BlocksPrimary;
	const void *BlocksSecondary;
	const void *ChunksPrimary;
	const void *ChunksSecondary;
};

struct StartPos
{
	uint16_t X, Y;
};

struct PalPtr
{
	uint16_t *Colors;
	uint16_t Location;
	uint16_t Size;
};

union MDReg
{
	char Byte;
	short Word;
	int Long;
};

struct Controller
{
	char Held;
	char Press;
};

// Data pointer and array declarations.
#define DataPointer(type, name, address) \
	static type &name = *(type *)address
#define DataArray(type, name, address, length) \
	static type *const name = (type *)address; static const int name##_Length = length

DataArray(LoadBlock, LevelLoadBlock, 0x431DC8, 48);
DataArray(void *, SolidIndexes, 0x64EFB4, 48);
DataArray(void *, LevelPtrs, 0x65AA74, 48);
DataArray(PalPtr, PalPoint, 0x6704A6, 66);
DataArray(void *, SSLayoutOffs_RAM, 0x69E3DF, 8);
DataArray(void *, SS_Pal_Map_Ptrs, 0x69EF32, 46);
DataArray(void *, SpriteLocPtrs, 0x7F762C, 48);
DataArray(void *, RingLocPtrs, 0x7F76EC, 48);
DataArray(void *, SpriteLocPtrs3, 0x7F790C, 48);
DataArray(void *, RingLocPtrs3, 0x7F79CC, 48);
DataArray(StartPos, Sonic_Start_Locations, 0x7F7B24, 48);
DataArray(StartPos, Knux_Start_Locations, 0x7F7BE4, 48);
DataArray(void *, SStageLayoutPtrs, 0x7F80F8, 8);
DataArray(void *, SSCompressedLayoutPointers, 0x7FA156, 2);
DataPointer(void *, reg_a0, 0x8547C0);
DataPointer(void *, reg_a1, 0x8547C8);
DataPointer(void *, reg_a2, 0x8547CC);
DataPointer(void *, reg_a3, 0x8547D0);
DataPointer(void *, reg_a4, 0x8547D4);
DataPointer(void *, reg_a5, 0x854998);
DataPointer(void *, reg_a6, 0x85499C);
DataPointer(MDReg, reg_d0, 0x8549A4);
DataPointer(MDReg, reg_d1, 0x8549A8);
DataPointer(MDReg, reg_d2, 0x8549AC);
DataPointer(MDReg, reg_d3, 0x8549B0);
DataPointer(MDReg, reg_d4, 0x8549B4);
DataPointer(MDReg, reg_d5, 0x8549B8);
DataPointer(MDReg, reg_d6, 0x8549BC);
DataPointer(MDReg, reg_d7, 0x8549C0);
DataArray(char, RAM_start, 0x8FF0000, 1);
DataArray(short, Target_water_palette, 0x8FFF000, 64);
DataArray(char, Plane_buffer, 0x8FFF100, 1);
DataArray(short, Normal_palette, 0x8FFFC00, 64);
DataArray(short, Target_palette, 0x8FFFC80, 64);
DataPointer(short, SS_start_x, 0x8FFE422);
DataPointer(short, SS_start_y, 0x8FFE424);
DataPointer(short, SS_start_angle, 0x8FFE426);
DataPointer(short, SS_perfect_count, 0x8FFE442);
DataPointer(Controller, Ctrl_1, 0x8FFF604);
DataPointer(Controller, Ctrl_2, 0x8FFF606);
DataPointer(char, Current_special_stage, 0x8FFFE16);
DataPointer(short, Sound_test_sound, 0x8FFFF84);
DataPointer(int, Blue_sphere_stage_number, 0x8FFFFA2);
DataPointer(short, SK_alone_flag, 0x8FFFFAE);
DataPointer(char, Emerald_count, 0x8FFFFB0);
DataPointer(char, Super_emerald_count, 0x8FFFFB1);
DataArray(char, Collected_emeralds_array, 0x8FFFFB2, 7);
DataPointer(char, SK_special_stage_flag, 0x8FFFFBB);
DataPointer(char, BS_special_stage_flag, 0x8FFFFD4);
DataPointer(char, Debug_cheat_flag, 0x8FFFFE2);

// Function pointer declarations.
#define FunctionPointer(RETURN_TYPE, NAME, ARGS, ADDRESS) \
	static RETURN_TYPE (__cdecl *const NAME)ARGS = (RETURN_TYPE (__cdecl *)ARGS)ADDRESS
#define StdcallFunctionPointer(RETURN_TYPE, NAME, ARGS, ADDRESS) \
	static RETURN_TYPE (__stdcall *const NAME)ARGS = (RETURN_TYPE (__stdcall *)ARGS)ADDRESS
#define FastcallFunctionPointer(RETURN_TYPE, NAME, ARGS, ADDRESS) \
	static RETURN_TYPE (__fastcall *const NAME)ARGS = (RETURN_TYPE (__fastcall *)ARGS)ADDRESS
#define ThiscallFunctionPointer(RETURN_TYPE, NAME, ARGS, ADDRESS) \
	static RETURN_TYPE (__thiscall *const NAME)ARGS = (RETURN_TYPE (__thiscall *)ARGS)ADDRESS
#define VoidFunc(NAME, ADDRESS) FunctionPointer(void,NAME,(void),ADDRESS)

FunctionPointer(void *, Kosinski_Decomp, (void *src, void *dst), 0x41028C);
VoidFunc(LoadSpecialStageMap, 0x69E3FF);

#define patchdecl(address,data) { (void*)address, arrayptrandsize(data) }
#define ptrdecl(address,data) { (void*)address, (void*)data }

// S&KC Mod Loader API version.
static const int ModLoaderVer = 1;

struct PatchInfo
{
	void *address;
	const void *data;
	int datasize;
};

struct PatchList
{
	const PatchInfo *Patches;
	int Count;
};

struct PointerInfo
{
	void *address;
	void *data;
};

struct PointerList
{
	const PointerInfo *Pointers;
	int Count;
};

struct HelperFunctions_v1
{
	// The version of the structure.
	int Version;
	// Registers a new PLC list.
	int (__cdecl *RegisterPLCList)(const PLC *plcs, int length);
	// Retrieves a PLC list.
	const PLC *(__cdecl *GetPLCList)(int index, int &length);
	// Sets a PLC list.
	BOOL (__cdecl *SetPLCList)(int index, const PLC *plcs, int length);
};

typedef HelperFunctions_v1 HelperFunctions;

typedef void(__cdecl *ModInitFunc)(const wchar_t *path, const HelperFunctions &helperFunctions);

typedef void(__cdecl *ModFrameFunc)();

struct ModInfo
{
	int Version;
};

#endif