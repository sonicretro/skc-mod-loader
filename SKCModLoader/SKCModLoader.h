#ifndef SKCMODLOADER_H
#define SKCMODLOADER_H

#if !defined(_M_IX86) && !defined(__i386__)
#error Mods must be built targeting 32-bit x86, change your settings.
#endif

#include <stdint.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <vector>
#include <stdexcept>

// C++ version.

/**
* Get the number of elements in an array.
* @return Number of elements in the array.
*/
template <typename T, size_t N>
static constexpr size_t LengthOfArray(const T(&)[N])
{
	return N;
}

/**
* Get the size of an array.
* @return Size of the array, in bytes.
*/
template <typename T, size_t N>
static constexpr size_t SizeOfArray(const T(&)[N])
{
	return N * sizeof(T);
}

// Macros for functions that need both an array
// and the array length or size.
#define arrayptrandlength(data) data, LengthOfArray(data)
#define arraylengthandptr(data) LengthOfArray(data), data
#define arrayptrandsize(data) data, SizeOfArray(data)
#define arraysizeandptr(data) SizeOfArray(data), data

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
#define GameModeID_SegaScreen 0
#define GameModeID_TitleScreen 4
#define GameModeID_Demo 8
#define GameModeID_Level 0xC
#define GameModeID_SegaScreen2 0x10
#define GameModeID_ContinueScreen 0x14
#define GameModeID_SegaScreen3 0x18
#define GameModeID_LevelSelect 0x1C
#define GameModeID_S3Credits 0x20
#define GameModeID_LevelSelect2 0x24
#define GameModeID_LevelSelect3 0x28
#define GameModeID_BlueSpheresTitle 0x2C
#define GameModeID_BlueSpheresDifficulty 0x2C
#define GameModeID_BlueSpheresResults 0x30
#define GameModeID_SpecialStage 0x34
#define GameModeID_CompetitionMenu 0x38
#define GameModeID_CompetitionPlayerSelect 0x3C
#define GameModeID_CompetitionLevelSelect 0x40
#define GameModeID_CompetitionResults 0x44
#define GameModeID_SpecialStageResults 0x48
#define GameModeID_SaveScreen 0x4C
#define GameModeID_TimeAttackRecords 0x50

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

enum PlayerModes
{
	sonic_and_tails,
	sonic_alone,
	tails_alone,
	knuckles_alone
};

enum ArtTile
{
	ArtTile_ArtKos_S3MenuBG = 0x0001,

	ArtTile_ArtKos_Competition_LevSel = 0x029F,
	ArtTile_ArtKos_Competition_ModeSel = 0x034A,
	ArtTile_ArtKos_Competition_Results = 0x034A,
	ArtTile_ArtKos_Competition_CharSel = 0x05C9,

	ArtTile_ArtKos_Save_Misc = 0x029F,
	ArtTile_ArtKos_Save_Extra = 0x0454,

	ArtTile_Monitors = 0x04C4,
	ArtTile_CutsceneKnux = 0x04DA,
	ArtTile_Player_1 = 0x0680,
	ArtTile_Player_2 = 0x06A0,
	ArtTile_Player_2_Tail = 0x06B0,
	ArtTile_Ring = 0x06BC,
	ArtTile_Shield = 0x079C,
	ArtTile_Shield_Sparks = 0x07BB,
	ArtTile_DashDust = 0x07E0,
	ArtTile_DashDust_P2 = 0x07F0,
};

enum SoundIDs
{
	mus_AIZ1 = 1,
	mus_AIZ2,
	mus_HCZ1,
	mus_HCZ2,
	mus_MGZ1,
	mus_MGZ2,
	mus_CNZ1,
	mus_CNZ2,
	mus_FBZ1,
	mus_FBZ2,
	mus_ICZ1,
	mus_ICZ2,
	mus_LBZ1,
	mus_LBZ2,
	mus_MHZ1,
	mus_MHZ2,
	mus_SOZ1,
	mus_SOZ2,
	mus_LRZ1,
	mus_HPZ,
	mus_SSZ,
	mus_DEZ1,
	mus_DEZ2,
	mus_MinibossK,
	mus_EndBoss,
	mus_DDZ,
	mus_MagneticOrbs,
	mus_SpecialStage,
	mus_SlotMachine,
	mus_Gumball,
	mus_Knuckles,
	mus_ALZ,
	mus_BPZ,
	mus_DPZ,
	mus_CGZ,
	mus_EMZ,
	mus_TitleScreen,
	mus_Credits3,
	mus_GameOver,
	mus_Continue,
	mus_GotThroughAct,
	mus_ExtraLife,
	mus_Emerald,
	mus_Invincibility,
	mus_CompetitionMenu,
	mus_Miniboss,
	mus_DataSelect,
	mus_FinalBoss,
	mus_Drowning,
	mus_Ending,

	sfx_RingRight,
	sfx_RingLeft,
	sfx_Death,
	sfx_Skid,
	sfx_SpikeHit,
	sfx_Bubble,
	sfx_Splash,
	sfx_Shield,
	sfx_Drown,
	sfx_Roll,
	sfx_Break,
	sfx_FireShield,
	sfx_BubbleShield,
	sfx_UnknownShield,
	sfx_ElectricShield,
	sfx_InstaAttack,
	sfx_FireAttack,
	sfx_BubbleAttack,
	sfx_ElectricAttack,
	sfx_SuperAlt,
	sfx_SandwallRise,
	sfx_Blast,
	sfx_Thump,
	sfx_Grab,
	sfx_WaterfallSplash,
	sfx_GlideLand,
	sfx_Projectile,
	sfx_MissileExplode,
	sfx_FlamethrowerQuiet,
	sfx_BossActivate,
	sfx_MissileThrow,
	sfx_SpikeMove,
	sfx_Charging,
	sfx_BossLazer,
	sfx_BlockConveyor,
	sfx_FlipBridge,
	sfx_Geyser,
	sfx_FanLatch,
	sfx_Collapse,
	sfx_UnknownCharge,
	sfx_Switch,
	sfx_MetalSpark,
	sfx_FloorThump,
	sfx_Lazer,
	sfx_Crash,
	sfx_BossZoom,
	sfx_BossHitFloor,
	sfx_Jump,
	sfx_Starpost,
	sfx_PulleyGrab,
	sfx_BlueSphere,
	sfx_AllSpheres,
	sfx_LevelProjectile,
	sfx_Perfect,
	sfx_PushBlock,
	sfx_Goal,
	sfx_ActionBlock,
	sfx_Splash2,
	sfx_UnknownShift,
	sfx_BossHit,
	sfx_Rumble2,
	sfx_LavaBall,
	sfx_Shield2,
	sfx_Hoverpad,
	sfx_Transporter,
	sfx_TunnelBooster,
	sfx_BalloonPlatform,
	sfx_TrapDoor,
	sfx_Balloon,
	sfx_GravityMachine,
	sfx_Lightning,
	sfx_BossMagma,
	sfx_SmallBumpers,
	sfx_ChainTension,
	sfx_UnknownPump,
	sfx_GroundSlide,
	sfx_FrostPuff,
	sfx_IceSpikes,
	sfx_TubeLauncher,
	sfx_SandSplash,
	sfx_BridgeCollapse,
	sfx_UnknownPowerUp,
	sfx_UnknownPowerDown,
	sfx_Alarm,
	sfx_MushroomBounce,
	sfx_PulleyMove,
	sfx_WeatherMachine,
	sfx_Bouncy,
	sfx_ChopTree,
	sfx_ChopStuck,
	sfx_UnknownFlutter,
	sfx_UnknownRevving,
	sfx_DoorOpen,
	sfx_DoorMove,
	sfx_DoorClose,
	sfx_GhostAppear,
	sfx_BossRecovery,
	sfx_ChainTick,
	sfx_BossHand,
	sfx_MetalLand,
	sfx_EnemyBreath,
	sfx_BossProjectile,
	sfx_UnknownPlink,
	sfx_SpringLatch,
	sfx_ThumpBoss,
	sfx_SuperEmerald,
	sfx_Targeting,
	sfx_Clank,
	sfx_SuperTransform,
	sfx_MissileShoot,
	sfx_UnknownOminous,
	sfx_FloorLauncher,
	sfx_GravityLift,
	sfx_MetalTransform,
	sfx_UnknownRise,
	sfx_LaunchGrab,
	sfx_LaunchReady,
	sfx_EnergyZap,
	sfx_AirDing,
	sfx_Bumper,
	sfx_Spindash,
	sfx_Continue,
	sfx_LaunchGo,
	sfx_Flipper,
	sfx_EnterSS,
	sfx_Register,
	sfx_Spring,
	sfx_Error,
	sfx_BigRing,
	sfx_Explode,
	sfx_Diamonds,
	sfx_Dash,
	sfx_SlotMachine,
	sfx_Signpost,
	sfx_RingLoss,
	sfx_Flying,
	sfx_FlyTired,
	sfx_SlideSkidLoud,
	sfx_LargeShip,
	sfx_EggmanSiren,
	sfx_BossRotate,
	sfx_FanBig,
	sfx_FanSmall,
	sfx_FlamethrowerLoud,
	sfx_GravityTunnel,
	sfx_BossPanic,
	sfx_UnknownSpin,
	sfx_WaveHover,
	sfx_CannonTurn,
	sfx_SlideSkidQuiet,
	sfx_SpikeBalls,
	sfx_LightTunnel,
	sfx_Rumble,
	sfx_BigRumble,
	sfx_DeathEggRiseLoud,
	sfx_WindQuiet,
	sfx_WindLoud,
	sfx_Rising,
	sfx_UnknownFlutter2,
	sfx_GumballTab,
	sfx_DeathEggRiseQuiet,
	sfx_TurbineHum,
	sfx_LavaFall,
	sfx_UnknownZap,
	sfx_ConveyorPlatform,
	sfx_UnknownSaw,
	sfx_MagneticSpike,
	sfx_LeafBlower,
	sfx_WaterSkid,

	mus_CreditsK,

	mus_FadeOut = 0xE1,
	mus_Stop,
	mus_MutePSG,
	mus_StopSFX,
	mus_FadeOut2,

	mus_S2SEGA = 0xFA,
	mus_StopSEGA = 0xFE,
	mus_SEGA
};

// Structures
#pragma pack(1)
template<typename T, size_t len>
struct OffsetTable
{
	short offs[len];

	constexpr T& operator [](size_t i)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<intptr_t>(this) + offs[i]);
	}
};

template<size_t len>
using JumpTable = OffsetTable<void(), len>;

template<typename T>
struct MDRamPtr
{
	short off;

	MDRamPtr() = default;
	MDRamPtr(short off) { this.off = off; }
	MDRamPtr(T* ptr) { off = reinterpret_cast<intptr_t>(ptr) - 0x9000000; }

	constexpr operator T* () { return reinterpret_cast<T*>(0x9000000 + off); }
};

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

struct Position
{
	int16_t X, Y;
};

struct PalPtr
{
	short *Colors;
	MDRamPtr<short> Location;
	short Size;
};

struct PatternIndex
{
	int16_t Data;

	PatternIndex() {}
	PatternIndex(int16_t data) { Data = data; }
	operator int16_t() { return Data; }

	bool GetPriority() { return Data < 0; }
	void SetPriority(bool pri) { Data = (Data & 0x7FFF) | (pri ? 0x8000 : 0); }
	uint8_t GetPalette() { return (Data >> 13) & 3; }
	void SetPalette(uint8_t pal) { Data = (Data & 0x9FFF) | ((pal & 3) << 13); }
	bool GetYFlip() { return Data & 0x1000; }
	void SetYFlip(bool yfl) { Data = (Data & 0xEFFF) | (yfl ? 0x1000 : 0); }
	bool GetXFlip() { return Data & 0x800; }
	void SetXFlip(bool xfl) { Data = (Data & 0xF7FF) | (xfl ? 0x800 : 0); }
	uint16_t GetTile() { return Data & 0x7FF; }
	void SetTile(uint16_t til) { Data = (Data & 0xF800) | (til & 0x7FF); }
#ifdef _MSC_VER
	__declspec(property(get = GetPriority, put = SetPriority)) bool Priority;
	__declspec(property(get = GetPalette, put = SetPalette)) uint8_t Palette;
	__declspec(property(get = GetYFlip, put = SetYFlip)) bool YFlip;
	__declspec(property(get = GetXFlip, put = SetXFlip)) bool XFlip;
	__declspec(property(get = GetTile, put = SetTile)) uint16_t Tile;
#endif
};

struct SpritePiece
{
	int8_t Y;
	uint8_t Size;
	PatternIndex Tile;
	int16_t X;

	SpritePiece() {}

	uint8_t GetWidth() { return (Size >> 2) & 3; }
	void SetWidth(uint8_t width) { Size = (Size & 3) | ((width & 3) << 2); }
	uint8_t GetHeight() { return Size & 3; }
	void SetHeight(uint8_t height) { Size = (Size & 0xC) | (height & 3); }
#ifdef _MSC_VER
	__declspec(property(get = GetWidth, put = SetWidth)) uint8_t Width;
	__declspec(property(get = GetHeight, put = SetHeight)) uint8_t Height;
#endif
};

struct DPLCEntry
{
	uint16_t Data;

	DPLCEntry() {}
	DPLCEntry(uint16_t data) { Data = data; }
	operator uint16_t() { return Data; }

	uint8_t GetLength() { return (Data & 0xF) + 1; }
	void SetLength(uint8_t len) { Data = (Data & 0xFFF0) | ((len - 1) & 0xF); }
	uint16_t GetTile() { return (Data >> 4) & 0xFFF; }
	void SetTile(uint16_t til) { Data = (Data & 0xF) | ((til & 0xFFF) << 4); }
#ifdef _MSC_VER
	__declspec(property(get = GetLength, put = SetLength)) uint8_t Length;
	__declspec(property(get = GetTile, put = SetTile)) uint16_t Tile;
#endif
};

struct PlayerDPLCEntry
{
	uint16_t Data;

	PlayerDPLCEntry() {}
	PlayerDPLCEntry(uint16_t data) { Data = data; }
	operator uint16_t() { return Data; }

	uint8_t GetLength() { return ((Data >> 12) & 0xF) + 1; }
	void SetLength(uint8_t len) { Data = (Data & 0xFFF) | (((len - 1) & 0xF) << 12); }
	uint16_t GetTile() { return Data & 0xFFF; }
	void SetTile(uint16_t til) { Data = (Data & 0xF000) | (til & 0xFFF); }
#ifdef _MSC_VER
	__declspec(property(get = GetLength, put = SetLength)) uint8_t Length;
	__declspec(property(get = GetTile, put = SetTile)) uint16_t Tile;
#endif
};

struct Object
{
	void(__cdecl *code)();
	char render_flags;
	char routine;
	char height_pixels;
	char width_pixels;
	__int16 priority;
	PatternIndex art_tile;
	void *mappings;
	__int16 x_pos;
	__int16 x_sub;
	__int16 y_pos;
	__int16 y_sub;
	__int16 x_vel;
	__int16 y_vel;
	__int16 ground_vel;
	char y_radius;
	char x_radius;
	char anim;
	char prev_anim;
	char mapping_frame;
	char anim_frame;
	char anim_frame_timer;
	char double_jump_property;
	char angle;
	char flip_angle;
	char collision_flags;
	char collision_property;
	char status;
	char status_secondary;
	char air_left;
	char flip_type;
	char object_control;
	char double_jump_flag;
	char flips_remaining;
	char flip_speed;
	__int16 move_lock;
	char invulnerability_timer;
	char invincibility_timer;
	char speed_shoes_timer;
	char status_tertiary;
	char character_id;
	char scroll_delay_counter;
	char next_tilt;
	char tilt;
	char stick_to_convex;
	char spin_dash_flag;
	__int16 spin_dash_counter;
	char jumping;
	char field_41;
	MDRamPtr<Object> interact;
	char default_y_radius;
	char default_x_radius;
	char top_solid_bit;
	char lrb_solid_bit;
	MDRamPtr<char> respawn_addr;
};

union DReg
{
	char Byte;
	unsigned char UByte;
	short Word;
	unsigned short UWord;
	int Long;
	unsigned int ULong;
};

union AReg
{
	void *Unknown;
	char *Byte;
	unsigned char *UByte;
	short *Word;
	unsigned short *UWord;
	int *Long;
	unsigned int *ULong;
	Object *Object;
};

struct Controller
{
	char Held;
	char Press;
};

struct MDSpriteTableEntry
{
	__int16 y_pos;
	char sprite_size;
	char link_field;
	__int16 render_info;
	__int16 x_pos;
};

struct PCSpriteTableEntry
{
	int x_pos;
	int y_pos;
	int h_cells;
	int v_pixels;
	int render_flags;
	char* art_addr;
	int unused[2];
};

class LayoutRow
{
public:
	LayoutRow(uint8_t* data)
	{
		this->data = data;
	}

	uint8_t& operator[](int X)
	{
		return data[X ^ 1];
	}

private:
	uint8_t* data;
};

struct Level_Layout
{
	uint16_t FGWidth;
	uint16_t BGWidth;
	uint16_t FGHeight;
	uint16_t BGHeight;
	uint16_t RowOffs[64];
	uint8_t Data[0xF78];

	LayoutRow GetFGRow(char Y)
	{
		return LayoutRow(reinterpret_cast<uint8_t*>(reinterpret_cast<intptr_t>(this) + RowOffs[Y * 2] - 0x8000));
	}

	LayoutRow GetBGRow(char Y)
	{
		return LayoutRow(reinterpret_cast<uint8_t*>(reinterpret_cast<intptr_t>(this) + RowOffs[Y * 2 + 1] - 0x8000));
	}

	bool Resize(uint16_t fgwidth, uint16_t fgheight, uint16_t bgwidth, uint16_t bgheight)
	{
		if (fgheight > 32 || bgheight > 32) return false;
		if (fgwidth * fgheight + bgwidth * bgheight > (int)LengthOfArray(Data))
			return false;
		FGWidth = fgwidth;
		FGHeight = fgheight;
		BGWidth = bgwidth;
		BGHeight = bgheight;
		uint16_t off = 0x8000 + offsetof(Level_Layout, Data);
		for (int i = 0; i < 32; ++i)
		{
			if (i < FGHeight)
			{
				RowOffs[i * 2] = off;
				off += FGWidth;
			}
			else
				RowOffs[i * 2] = 0;
			if (i < BGHeight)
			{
				RowOffs[i * 2 + 1] = off;
				off += BGWidth;
			}
			else
				RowOffs[i * 2 + 1] = 0;
		}
		return true;
	}
};

struct Comp_Save_Stage
{
	int Times[3];
	char Characters[4];
};

struct Comp_Save_Data
{
	Comp_Save_Stage Azure_Lake;
	Comp_Save_Stage Balloon_Park;
	Comp_Save_Stage Desert_Palace;
	Comp_Save_Stage Chrome_Gadget;
	Comp_Save_Stage Endless_Mine;
	__int16 Magic;
};

struct Main_Save_Slot
{
	char Status;
	char pad1;
	char Character_Emeralds;
	char Zone;
	char Big_Rings;
	char pad2;
	__int16 Got_Emeralds;
	char Lives;
	char Continues;
};

struct Main_Save_Data
{
	Main_Save_Slot Slots[8];
	__int16 Magic;
};

struct Main_Save_Slot_S3
{
	char Status;
	char pad;
	char Character;
	char Zone;
	char Next_Special_Stage;
	char Emerald_Count;
	char Got_Emeralds;
	char Big_Rings;
};

struct Main_Save_Data_S3
{
	Main_Save_Slot_S3 Slots[6];
	__int16 Magic;
};

struct ObjectEntry
{
	short X;
	short Y_Flags;
	unsigned char ID;
	unsigned char SubType;

	bool GetLoadAtAnyYPos() { return Y_Flags < 0; }
	void SetLoadAtAnyYPos(bool flag) { Y_Flags = (Y_Flags & 0x7FFF) | (flag ? 0x8000 : 0); }
	bool GetYFlip() { return Y_Flags & 0x4000; }
	void SetYFlip(bool yfl) { Y_Flags = (Y_Flags & 0xBFFF) | (yfl ? 0x4000 : 0); }
	bool GetXFlip() { return Y_Flags & 0x2000; }
	void SetXFlip(bool xfl) { Y_Flags = (Y_Flags & 0xDFFF) | (xfl ? 0x2000 : 0); }
	short GetY() { return Y_Flags & 0xFFF; }
	void SetY(short y) { Y_Flags = (Y_Flags & 0xF000) | (y & 0xFFF); }
#ifdef _MSC_VER
	__declspec(property(get = GetLoadAtAnyYPos, put = SetLoadAtAnyYPos)) bool LoadAtAnyYPos;
	__declspec(property(get = GetYFlip, put = SetYFlip)) bool YFlip;
	__declspec(property(get = GetXFlip, put = SetXFlip)) bool XFlip;
	__declspec(property(get = GetY, put = SetY)) short Y;
#endif
};

// Data pointer and array declarations.
#define DataPointer(type, name, address) \
	static type &name = *(type *)address
#define DataArray(type, name, address, len) \
	static DataArray_t<type, address, len> name

template<typename T, intptr_t addr, size_t len>
struct DataArray_t final
{
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef pointer iterator;
	typedef const_pointer const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	DataArray_t() = default;
	DataArray_t(const DataArray_t&) = delete; // object cannot be copied, prevents accidentally using DataArray in a function call
	DataArray_t(const DataArray_t&&) = delete; // object cannot be moved

	// Gets the underlying data for the array.
	constexpr pointer data() const noexcept { return reinterpret_cast<pointer>(addr); }
	// Gets the underlying data for the array.
	constexpr const_pointer cdata() const noexcept { return reinterpret_cast<const_pointer>(addr); }

	// Checks if the array is empty (no elements).
	constexpr bool empty() const noexcept { return len == 0; }

	// Gets the size of the array, in elements.
	constexpr size_type size() const noexcept { return len; }

	// Gets the maximum size of the array, in elements.
	constexpr size_type max_size() const noexcept { return len; }

	constexpr pointer operator&() const noexcept { return data(); }

	constexpr operator pointer() const noexcept { return data(); }

	// Gets an item from the array, with bounds checking.
	constexpr reference at(size_type i)
	{
		if (i < len)
			return data()[i];
		throw std::out_of_range("Data access out of range.");
	}

	// Gets an item from the array, with bounds checking.
	constexpr const_reference at(size_type i) const
	{
		if (i < len)
			return cdata()[i];
		throw std::out_of_range("Data access out of range.");
	}

	template<size_type I>
	// Gets an item from the array, with compile-time bounds checking.
	constexpr reference get() noexcept
	{
		static_assert(I < len, "index is within bounds");
		return data()[I];
	}

	template<size_type I>
	// Gets an item from the array, with compile-time bounds checking.
	constexpr const_reference get() const noexcept
	{
		static_assert(I < len, "index is within bounds");
		return cdata()[I];
	}

	// Gets the first item in the array.
	constexpr reference front() { return *data(); }
	// Gets the first item in the array.
	constexpr const_reference front() const { return *cdata(); }

	// Gets the last item in the array.
	constexpr reference back() { return data()[len - 1]; }
	// Gets the last item in the array.
	constexpr const_reference back() const { return cdata()[len - 1]; }

	// Gets an iterator to the beginning of the array.
	constexpr iterator begin() noexcept { return data(); }
	// Gets an iterator to the beginning of the array.
	constexpr const_iterator begin() const noexcept { return cdata(); }
	// Gets an iterator to the beginning of the array.
	constexpr const_iterator cbegin() const noexcept { return cdata(); }

	// Gets an iterator to the end of the array.
	constexpr iterator end() noexcept { return data() + len; }
	// Gets an iterator to the end of the array.
	constexpr const_iterator end() const noexcept { return cdata() + len; }
	// Gets an iterator to the end of the array.
	constexpr const_iterator cend() const noexcept { return cdata() + len; }

	// Gets a reverse iterator to the beginning of the array.
	constexpr reverse_iterator rbegin() noexcept { return data() + len; }
	// Gets a reverse iterator to the beginning of the array.
	constexpr const_reverse_iterator rbegin() const noexcept { return cdata() + len; }
	// Gets a reverse iterator to the beginning of the array.
	constexpr const_reverse_iterator crbegin() const noexcept { return cdata() + len; }

	// Gets a reverse iterator to the end of the array.
	constexpr reverse_iterator rend() noexcept { return data(); }
	// Gets a reverse iterator to the end of the array.
	constexpr const_reverse_iterator rend() const noexcept { return cdata(); }
	// Gets a reverse iterator to the end of the array.
	constexpr const_reverse_iterator crend() const noexcept { return cdata(); }
};

DataArray(LoadBlock, LevelLoadBlock, 0x431DC8, 48);
DataArray(void *, SolidIndexes, 0x64EFB4, 48);
DataArray(Level_Layout *, LevelPtrs, 0x65AA74, 48);
DataArray(PalPtr, PalPoint, 0x6704A6, 66);
DataArray(void *, SSLayoutOffs_RAM, 0x69E3DF, 8);
DataArray(void *, SS_Pal_Map_Ptrs, 0x69EF32, 46);
DataArray(ObjectEntry *, SpriteLocPtrs, 0x7F762C, 48);
DataArray(Position *, RingLocPtrs, 0x7F76EC, 48);
DataArray(ObjectEntry *, SpriteLocPtrs3, 0x7F790C, 48);
DataArray(Position *, RingLocPtrs3, 0x7F79CC, 48);
DataArray(Position, Sonic_Start_Locations, 0x7F7B24, 48);
DataArray(Position, Knux_Start_Locations, 0x7F7BE4, 48);
DataArray(void *, SStageLayoutPtrs, 0x7F80F8, 8);
DataArray(void *, SSCompressedLayoutPointers, 0x7FA156, 2);
DataPointer(int, Sonic3Mode, 0x831180);
// 0 - normal, 1 - special stage mode, 2 - debug mode
DataPointer(int, ModeSelection, 0x831184);
// 0 - S3K, 1 - S&K, 2 - S3
DataPointer(int, GameSelection, 0x831188);
DataPointer(int, SpritesProcessed, 0x831740);
DataPointer(int, P2SpriteTableProcessed, 0x831750);
DataPointer(AReg, reg_a0, 0x8547C0);
DataPointer(AReg, reg_a1, 0x8547C8);
DataPointer(AReg, reg_a2, 0x8547CC);
DataPointer(AReg, reg_a3, 0x8547D0);
DataPointer(AReg, reg_a4, 0x8547D4);
DataPointer(AReg, reg_a5, 0x854998);
DataPointer(AReg, reg_a6, 0x85499C);
DataPointer(DReg, reg_d0, 0x8549A4);
DataPointer(DReg, reg_d1, 0x8549A8);
DataPointer(DReg, reg_d2, 0x8549AC);
DataPointer(DReg, reg_d3, 0x8549B0);
DataPointer(DReg, reg_d4, 0x8549B4);
DataPointer(DReg, reg_d5, 0x8549B8);
DataPointer(DReg, reg_d6, 0x8549BC);
DataPointer(DReg, reg_d7, 0x8549C0);
DataArray(char, VRAM, 0x8000000, 0x10000);
DataArray(PatternIndex, VRAM_Plane_A_Name_Table, 0x800C000, 0x800);
DataArray(PatternIndex, VRAM_Plane_B_Name_Table, 0x800E000, 0x800);
DataArray(char, RAM_start, 0x8FF0000, 0x10000);
// chunk (128x128) definitions, $80 bytes per definition
DataArray(short, Chunk_table, 0x8FF0000, 0x4000);
// alternate sprite table for player 1 in competition mode
DataArray(MDSpriteTableEntry, Sprite_table_buffer_2, 0x8FF7880, 80);
// sprite table for player 2 in competition mode
DataArray(MDSpriteTableEntry, Sprite_table_buffer_P2, 0x8FF7B00, 80);
// alternate sprite table for player 2 in competition mode
DataArray(MDSpriteTableEntry, Sprite_table_buffer_P2_2, 0x8FF7D80, 80);
DataPointer(Level_Layout, Level_layout, 0x8FF8000);
// respawn table used by glowing spheres bonus stage, because... Reasons?
DataArray(char, Object_respawn_table_2, 0x8FF8400, 0x200);
// ring status table used by glowing spheres bonus stage, because... Reasons?
DataArray(short, Ring_status_table_2, 0x8FF8600, 0x200);
// block (16x16) definitions, 8 bytes per definition, space for $300 blocks
DataArray(short, Block_table, 0x8FF9000, 0xC00);
// sprite collision list during a special stage
DataArray(char, SStage_collision_response_list, 0x8FFA400, 0x100);
// unknown special stage array
DataArray(char, SStage_unkA500, 0x8FFA500, 0x100);
// unknown special stage array
DataArray(char, SStage_unkA600, 0x8FFA600, 0x200);
// array of background scroll positions for the level
DataArray(int, HScroll_table, 0x8FFA800, 0x80);
// code table is built up here and then used during decompression
DataArray(char, Nem_code_table, 0x8FFAA00, 0x200);
// 8 priority levels, $80 bytes per level
DataArray(char, Sprite_table_input, 0x8FFAC00, 0x400);
DataArray(Object, Object_RAM, 0x8FFB000, 110);
// main character in 1 player mode, player 1 in Competition mode
DataPointer(Object, Player_1, 0x8FFB000);
// Tails in a Sonic and Tails game, player 2 in Competition mode
DataPointer(Object, Player_2, 0x8FFB04A);
// during a level, an object whose sole purpose is to clear the collision response list is stored here
DataPointer(Object, Reserved_object_3, 0x8FFB094);
DataArray(Object, Dynamic_Object_RAM, 0x8FFB0DE, 90);
// various fixed in-level objects
DataArray(Object, Level_Object_RAM, 0x8FFCAE2, 17);
// for the main character
DataPointer(Object, Breathing_bubbles, 0x8FFCB2C);
// for Tails in a Sonic and Tails game
DataPointer(Object, Breathing_bubbles_P2, 0x8FFCB76);
// for Super Sonic and Super Knuckles
DataPointer(Object, Super_stars, 0x8FFCBC0);
// Tails' tails in Competition mode
DataPointer(Object, Tails_tails_2P, 0x8FFCBC0);
// Tails' tails
DataPointer(Object, Tails_tails, 0x8FFCC0A);
DataPointer(Object, Dust, 0x8FFCC54);
DataPointer(Object, Dust_P2, 0x8FFCC9E);
DataPointer(Object, Shield, 0x8FFCCE8);
// left over from Sonic 2 I'm guessing
DataPointer(Object, Shield_P2, 0x8FFCD32);
DataArray(Object, Invincibility_stars, 0x8FFCD7C, 4);
DataArray(Object, Invincibility_stars_P2, 0x8FFCEA4, 3);
// Obj_HCZWaveSplash is loaded here
DataPointer(Object, Wave_Splash, 0x8FFCF82);
// each subtype of hcz conveyor belt uses a different byte to check if it's already loaded. Since they're so wide, the object loader may try loading them multiple times
DataArray(char, Conveyor_belt_load_array, 0x8FFCFE0, 14);
// each module in a KosM archive is decompressed here and then DMAed to VRAM
DataArray(char, Kos_decomp_buffer, 0x8FFD000, 0x1000);
// horizontal scroll table is built up here and then DMAed to VRAM
DataArray(short, H_scroll_buffer, 0x8FFE000, 448);
// only objects in this list are processed by the collision response routines
DataArray(MDRamPtr<Object>, Collision_response_list, 0x8FFE380, 64);
// used by Tails' AI in a Sonic and Tails game
DataArray(char, Stat_table, 0x8FFE400, 0x100);
// used by Player 2 in competition mode
DataArray(char, Pos_table_P2, 0x8FFE400, 0x100);
// unknown scalar table index value
DataPointer(short, SStage_scalar_index_0, 0x8FFE400);
// unknown scalar table index value
DataPointer(short, SStage_scalar_index_1, 0x8FFE402);
// unknown scalar table index value
DataPointer(short, SStage_scalar_index_2, 0x8FFE404);
// unknown scalar table results values
DataPointer(short, SStage_scalar_result_0, 0x8FFE406);
// unknown scalar table results values
DataPointer(short, SStage_scalar_result_1, 0x8FFE408);
// unknown scalar table results values
DataPointer(short, SStage_scalar_result_2, 0x8FFE40A);
// unknown scalar table results values
DataPointer(short, SStage_scalar_result_3, 0x8FFE41E);
// special stage globe's current animation frame, $10 and higher is turning
DataPointer(short, Special_stage_anim_frame, 0x8FFE420);
DataPointer(short, Special_stage_X_pos, 0x8FFE422);
DataPointer(short, Special_stage_Y_pos, 0x8FFE424);
// $00 = north, $40 = west, $80 = south, $C0 = east
DataPointer(short, Special_stage_angle, 0x8FFE426);
// player's movement speed, negative when going backwards
DataPointer(short, Special_stage_velocity, 0x8FFE428);
// direction of next turn, 4 = left, -4 = right
DataPointer(char, Special_stage_turning, 0x8FFE42A);
// if set, the player can't start advancing by pressing up
DataPointer(char, Special_stage_bumper_lock, 0x8FFE42B);
DataPointer(char, Special_stage_prev_anim_frame, 0x8FFE42C);
// same as Special_stage_anim_frame, but set to 0 while turning
DataPointer(char, Special_stage_palette_frame, 0x8FFE42F);
// if set, the player can't turn
DataPointer(char, Special_stage_turn_lock, 0x8FFE430);
// set when the player player presses up
DataPointer(char, Special_stage_advancing, 0x8FFE431);
// $80 = normal jump, $81 = spring
DataPointer(char, Special_stage_jumping, 0x8FFE432);
// counts up when leaving the special stage
DataPointer(char, Special_stage_fade_timer, 0x8FFE433);
DataPointer(short, Special_stage_prev_X_pos, 0x8FFE434);
DataPointer(short, Special_stage_prev_Y_pos, 0x8FFE436);
DataPointer(short, Special_stage_spheres_left, 0x8FFE438);
DataPointer(short, Special_stage_ring_count, 0x8FFE43A);
DataPointer(char, Special_stage_sphere_HUD_flag, 0x8FFE43C);
// when bit 7 is set, the ring HUD is updated
DataPointer(char, Special_stage_extra_life_flags, 0x8FFE43D);
// when this reaches 0, the special stage speeds up
DataPointer(short, Special_stage_rate_timer, 0x8FFE43E);
// $80 = normal jump, $81 = spring
DataPointer(char, Special_stage_jumping_P2, 0x8FFE440);
DataPointer(short, Special_stage_rings_left, 0x8FFE442);
// player's maximum speed in either direction
DataPointer(short, Special_stage_rate, 0x8FFE444);
// ROM address of the stage's color palette
DataPointer(short*, Special_stage_palette_addr, 0x8FFE446);
// counts up after getting the last sphere, when it reaches $100 the emerald appears
DataPointer(short, Special_stage_clear_timer, 0x8FFE44A);
// if set, the player can't jump
DataPointer(char, Special_stage_clear_routine, 0x8FFE44C);
// counts down when the emerald appears, when it reaches 0 the emerald sound plays
DataPointer(char, Special_stage_emerald_timer, 0x8FFE44D);
// address of the last bumper touched, or the emerald at the end of the stage
DataPointer(MDRamPtr<Object>, Special_stage_interact, 0x8FFE44E);
// set when the player begins moving at the start of the stage
DataPointer(char, Special_stage_started, 0x8FFE450);
// some extra sprite info for special stages
DataArray(char, SStage_extra_sprites, 0x8FFE480, 0x70);
DataArray(char, Pos_table, 0x8FFE500, 0x100);
// saved data from Competition Mode
DataPointer(Comp_Save_Data, Competition_saved_data, 0x8FFE600);
// pointer to the active save slot in 1 player mode
DataPointer(Main_Save_Slot*, Save_pointer, 0x8FFE660);
// controls the emerald flicker in save screen and special stage results.
DataPointer(short, Emerald_flicker_flag, 0x8FFE666);
// saved data from 1 player mode
DataPointer(Main_Save_Data, Saved_data, 0x8FFE6AC);
// pointer to the active save slot in 1 player mode, when Sonic3Mode is set
DataPointer(Main_Save_Slot_S3*, Save_pointer_S3, 0x8FFE6B8);
// saved data from 1 player mode, when Sonic3Mode is set
DataPointer(Main_Save_Data_S3, Saved_data_S3, 0x8FFE6CC);
// 1 word per ring
DataArray(short, Ring_status_table, 0x8FFE700, 0x200);
// 1 byte per object, every object in the level gets an entry
DataArray(char, Object_respawn_table, 0x8FFEB00, 0x300);
// number of pixels camera scrolled horizontally in the last frame * $100
DataPointer(short, H_scroll_amount, 0x8FFEE00);
// number of pixels camera scrolled vertically in the last frame * $100
DataPointer(short, V_scroll_amount, 0x8FFEE02);
DataPointer(short, H_scroll_amount_P2, 0x8FFEE04);
DataPointer(short, V_scroll_amount_P2, 0x8FFEE06);
// if this is set scrolling routines aren't called
DataPointer(char, Scroll_lock, 0x8FFEE0A);
DataPointer(char, Scroll_lock_P2, 0x8FFEE0B);
DataPointer(short, Camera_target_min_X_pos, 0x8FFEE0C);
DataPointer(short, Camera_target_max_X_pos, 0x8FFEE0E);
DataPointer(short, Camera_target_min_Y_pos, 0x8FFEE10);
DataPointer(short, Camera_target_max_Y_pos, 0x8FFEE12);
DataPointer(short, Camera_min_X_pos, 0x8FFEE14);
DataPointer(short, Camera_max_X_pos, 0x8FFEE16);
DataPointer(short, Camera_min_Y_pos, 0x8FFEE18);
// this is the only one which ever differs from its target value
DataPointer(short, Camera_max_Y_pos, 0x8FFEE1A);
DataPointer(short, Camera_min_X_pos_P2, 0x8FFEE1C);
DataPointer(short, Camera_max_X_pos_P2, 0x8FFEE1E);
DataPointer(short, Camera_min_Y_pos_P2, 0x8FFEE20);
DataPointer(short, Camera_max_Y_pos_P2, 0x8FFEE22);
// if this is non-zero with value x, horizontal scrolling will be based on the player's position x / $100 + 1 frames ago
DataPointer(short, H_scroll_frame_offset, 0x8FFEE24);
// goes up in increments of 4
DataPointer(short, Pos_table_index, 0x8FFEE26);
DataPointer(short, H_scroll_frame_offset_P2, 0x8FFEE28);
DataPointer(short, Pos_table_index_P2, 0x8FFEE2A);
// the vertical scroll manager scrolls the screen until the player's distance from the top of the screen is equal to this (or between this and this + $40 when in the air). $60 by default
DataPointer(short, Distance_from_top, 0x8FFEE2C);
DataPointer(short, Distance_from_top_P2, 0x8FFEE2E);
DataPointer(char, Deform_lock, 0x8FFEE30);
// set when the maximum camera Y pos is undergoing a change
DataPointer(char, Camera_max_Y_pos_changing, 0x8FFEE32);
DataPointer(char, Dynamic_resize_routine, 0x8FFEE33);
// if this is set vertical scroll when the player is on the ground and has a speed of less than $800 is capped at 24 pixels per frame instead of 6
DataPointer(char, Fast_V_scroll_flag, 0x8FFEE39);
// upper word for foreground, lower word for background
DataPointer(int, V_scroll_value_P2_copy, 0x8FFEE3A);
// difference between Camera_X_pos_copy and Camera_X_pos_BG_copy, used for background collision in SSZ and other levels
DataPointer(short, Camera_X_diff, 0x8FFEE3E);
// difference between Camera_Y_pos_copy and Camera_Y_pos_BG_copy, used for background collision in SSZ and other levels
DataPointer(short, Camera_Y_diff, 0x8FFEE40);
// address in the ring layout of the first ring whose X position is >= camera X position - 8
DataPointer(Position*, Ring_start_addr_ROM, 0x8FFEE42);
// address in the ring layout of the first ring whose X position is >= camera X position + 328
DataPointer(Position*, Ring_end_addr_ROM, 0x8FFEE46);
// address in the ring status table of the first ring whose X position is >= camera X position - 8
DataPointer(MDRamPtr<short>, Ring_start_addr_RAM, 0x8FFEE4A);
// for example, after AIZ gets burnt, this indicates act 1 even though it's actually act 2
DataPointer(unsigned short, Apparent_zone_and_act, 0x8FFEE4E);
// for example, after AIZ gets burnt, this indicates act 1 even though it's actually act 2
DataPointer(unsigned char, Apparent_act, 0x8FFEE4E);
// always equal to actual zone
DataPointer(unsigned char, Apparent_zone, 0x8FFEE4F);
// the palette gets faded in until this timer expires
DataPointer(short, Palette_fade_timer, 0x8FFEE50);
// player 1's recorded time for the current run, to be displayed in menus and the result screen
DataPointer(int, Competition_time_record, 0x8FFEE52);
// player 2's recorded time for the current run, to be displayed in menus and the result screen
DataPointer(int, Competition_time_record_P2, 0x8FFEE56);
// signifies new time records in time attack mode. set: no new records, clear: 1st place, $1: 2nd place, $2: 3rd place record.
DataPointer(char, Competition_time_attack_new_top_record, 0x8FFEE5A);
// number of laps that player 1 has completed
DataPointer(char, Competition_lap_count, 0x8FFEE5C);
// number of laps that player 2 has completed
DataPointer(char, Competition_lap_count_P2, 0x8FFEE5D);
// set when entering LRZ 3 or DEZ 3 directly from previous act. Prevents title card from loading
DataPointer(char, Act3_flag, 0x8FFEE5E);
DataPointer(short, Camera_X_pos_P2, 0x8FFEE60);
DataPointer(short, Camera_Y_pos_P2, 0x8FFEE64);
DataPointer(short, Camera_X_pos_P2_copy, 0x8FFEE68);
DataPointer(short, Camera_Y_pos_P2_copy, 0x8FFEE6C);
DataPointer(short, Camera_X_pos, 0x8FFEE78);
DataPointer(short, Camera_Y_pos, 0x8FFEE7C);
DataPointer(short, Camera_X_pos_copy, 0x8FFEE80);
DataPointer(short, Camera_Y_pos_copy, 0x8FFEE84);
// rounded down to the nearest block boundary ($10th pixel)
DataPointer(short, Camera_X_pos_rounded, 0x8FFEE88);
// rounded down to the nearest block boundary ($10th pixel)
DataPointer(short, Camera_Y_pos_rounded, 0x8FFEE8A);
DataPointer(short, Camera_X_pos_BG_copy, 0x8FFEE8C);
DataPointer(short, Camera_Y_pos_BG_copy, 0x8FFEE90);
// rounded down to the nearest block boundary ($10th pixel)
DataPointer(short, Camera_X_pos_BG_rounded, 0x8FFEE94);
// rounded down to the nearest block boundary ($10th pixel)
DataPointer(short, Camera_Y_pos_BG_rounded, 0x8FFEE96);
// set when two block are to be updated instead of one (i.e. the camera's scrolled by more than $10 pixels)
DataPointer(short, Plane_double_update_flag, 0x8FFEEA4);
DataPointer(short, Special_V_int_routine, 0x8FFEEA6);
// set to $FFFF
DataPointer(short, Screen_X_wrap_value, 0x8FFEEA8);
// either $7FF or $FFF
DataPointer(short, Screen_Y_wrap_value, 0x8FFEEAA);
// either $7F0 or $FF0
DataPointer(short, Camera_Y_pos_mask, 0x8FFEEAC);
// either $3C or $7C
DataPointer(short, Layout_row_index_mask, 0x8FFEEAE);
// routine counter for various special events. Used for example with LBZ2 Death Egg sequence
DataPointer(short, Special_events_routine, 0x8FFEEB2);
// various flags used by screen events
DataPointer(short, Events_fg_0, 0x8FFEEB4);
// various flags used by screen events
DataPointer(short, Events_fg_1, 0x8FFEEB6);
// various flags used by screen events
DataPointer(short, Events_fg_2, 0x8FFEEB8);
// the number of pixels the screen was moved this frame, used to offset level objects horizontally. Used only for level repeat sections, such as AIZ airship.
DataPointer(short, Level_repeat_offset, 0x8FFEEBC);
// various flags used by screen events
DataPointer(short, Events_fg_3, 0x8FFEEBE);
// screen events routine counter
DataPointer(short, Events_routine_fg, 0x8FFEEC0);
// background events routine counter
DataPointer(short, Events_routine_bg, 0x8FFEEC2);
// various flags used by screen events
DataPointer(short, Events_fg_4, 0x8FFEEC4);
// various flags used by screen events
DataPointer(short, Events_fg_5, 0x8FFEEC6);
// position to redraw screen from. Screen is reloaded 1 row at a time to avoid game lag
DataPointer(short, Draw_delayed_position, 0x8FFEEC8);
// number of rows for screen redrawing. Screen is reloaded 1 row at a time to avoid game lag
DataPointer(short, Draw_delayed_rowcount, 0x8FFEECA);
// flag for enabling screen shake. Negative values cause screen to shake infinitely, positive values make the screen shake for a short amount of time
DataPointer(short, Screen_shake_flag, 0x8FFEECC);
// vertical offset when screen_shake_flag is enabled. This is added to camera position later
DataPointer(short, Screen_shake_offset, 0x8FFEECE);
// value of Screen_shake_offset for the previous frame
DataPointer(short, Screen_shake_last_offset, 0x8FFEED0);
// $18 bytes ; various flags used by background events
DataArray(char, Events_bg, 0x8FFEED2, 0x18);
// word ; RAM address of the special stage results object
DataPointer(MDRamPtr<Object>, SStage_results_object_addr, 0x8FFEEE0);
// addresses for cloud objects in FBZ2
DataArray(char, FBZ_cloud_addr, 0x8FFEEEA, 0x14);
// vertical scroll buffer used in various levels
DataArray(char, Vscroll_buffer, 0x8FFEEEA, 0x50);
// when set, indicates that special sprites are used for sprite masking
DataPointer(short, Spritemask_flag, 0x8FFEF3A);
// if this is set Sprite_table_buffer and Sprite_table_buffer_P2 will be DMAed instead of Sprite_table_buffer_2 and Sprite_table_buffer_P2_2
DataPointer(short, Use_normal_sprite_table, 0x8FFEF3C);
// if set, switches the state of Use_normal_sprite_table
DataPointer(short, Switch_sprite_table, 0x8FFEF3E);
// if set, Launch Base 2 Death Egg is currently rising
DataPointer(char, Event_LBZ2_DeathEgg, 0x8FFEF40);
// 0 = Grandprix, 1 = Matchrace, 2 = Timeattack. 3 = Exit
DataPointer(char, Competition_menu_selection, 0x8FFEF48);
// set if Player 2 in competition mode isn't a ghost of player 1
DataPointer(char, Not_ghost_flag, 0x8FFEF49);
// competition mode zone id. This is different from the zone order in game
DataPointer(char, Competition_menu_zone, 0x8FFEF4A);
// the selected save entry in data select menu. This includes no save and delete options, too
DataPointer(char, Dataselect_entry, 0x8FFEF4B);
// Player mode for NO SAVE option in data select menu
DataPointer(short, Dataselect_nosave_player, 0x8FFEF4C);
// 0 = Enabled, FF = Disabled
DataPointer(char, Competition_menu_monitors, 0x8FFEF4E);
// keeps track of whether controller 1 has pressed the start button. May be used by the demo data itself
DataPointer(char, Demo_start_button, 0x8FFEF50);
// keeps getting incremented as the demo progresses
DataPointer(void*, Demo_data_addr, 0x8FFEF52);
// if this is set SRAM routines will mask all interrupts (by setting the SR to $2700)
DataPointer(short, SRAM_mask_interrupts_flag, 0x8FFEF56);
// points to either the object index for S3 levels or that for S&K levels
DataPointer(void*, Object_index_addr, 0x8FFEF5A);
// stores ring count during act 3 transition
DataPointer(short, Act3_ring_count, 0x8FFEF5E);
// stores timer during act 3 transition
DataPointer(int, Act3_timer, 0x8FFEF60);
// Camera_Y_pos_coarse - $80
DataPointer(short, Camera_Y_pos_coarse_back, 0x8FFEF64);
// alternate screen shaking flag only used when hyper knuckles hits a wall after gliding
DataPointer(short, Glide_screen_shake, 0x8FFEF66);
// stored zone and act during special stage results screen?
DataPointer(short, Special_stage_zone_and_act, 0x8FFEF6A);
// set if special stage was completed. This determines which cutscene to play when entering HPZS
DataPointer(short, HPZ_special_stage_completed, 0x8FFEF6C);
// seems to be just a copy of Current_special_stage
DataPointer(char, Current_special_stage_2, 0x8FFEF6E);
// seems to be just a copy of Current_special_stage used specifically for HPZS
DataPointer(char, HPZ_current_special_stage, 0x8FFEF70);
// the only thing this does is prevent the game from pausing
DataPointer(short, Ending_running_flag, 0x8FFEF72);
// the address of the second plane buffer to process, if applicable
DataPointer(short*, Plane_buffer_2_addr, 0x8FFEF74);
// the number of frames to hold the current buttons. This only applies to S&K demos
DataPointer(char, Demo_hold_counter, 0x8FFEF78);
// the buttons to hold. This only applies to S&K demos
DataPointer(char, Demo_hold_buttons, 0x8FFEF79);
// the currently running demo
DataPointer(short, Demo_number, 0x8FFEF7A);
// the number of rings being consumed currently
DataPointer(short, Ring_consumption_count, 0x8FFEF80);
// the remaining part of the ring consumption table
DataArray(MDRamPtr<short>, Ring_consumption_list, 0x8FFEF82, 0x3F);
// $600 bytes ; yes, this area is used to for special stage layouts!
DataArray(char, SStage_layout_buffer, 0x8FFF000, 0x600);
// used by palette fading routines
DataArray(short, Target_water_palette, 0x8FFF000, 64);
// this is what actually gets displayed
DataArray(short, Water_palette, 0x8FFF080, 64);
DataArray(short, Water_palette_line_2, 0x8FFF0A0, 16);
DataArray(short, Water_palette_line_3, 0x8FFF0C0, 16);
DataArray(short, Water_palette_line_4, 0x8FFF0E0, 16);
// used by level drawing routines
DataArray(char, Plane_buffer, 0x8FFF100, 0x480);
// used to temporarily hold data while it is being transferred from one VRAM location to another
DataArray(char, VRAM_buffer, 0x8FFF580, 0x80);
DataPointer(char, Game_mode, 0x8FFF600);
DataPointer(Controller, Ctrl_1_logical, 0x8FFF602);
DataPointer(Controller, Ctrl_1, 0x8FFF604);
DataPointer(Controller, Ctrl_2, 0x8FFF606);
// AND the lower byte by $BF and write to VDP control port to disable display, OR by $40 to enable
DataPointer(short, VDP_reg_1_command, 0x8FFF60E);
// the time left for a demo to start/run
DataPointer(short, Demo_timer, 0x8FFF614);
// both foreground and background
DataPointer(int, V_scroll_value, 0x8FFF616);
DataPointer(short, V_scroll_value_FG, 0x8FFF616);
DataPointer(short, V_scroll_value_BG, 0x8FFF618);
DataPointer(int, V_scroll_value_P2, 0x8FFF61E);
DataPointer(short, V_scroll_value_FG_P2, 0x8FFF61E);
DataPointer(short, V_scroll_value_BG_P2, 0x8FFF620);
// left over from Sonic 2
DataPointer(char, Teleport_active_timer, 0x8FFF622);
// left over from Sonic 2
DataPointer(char, Teleport_active_flag, 0x8FFF623);
// contains a command to write to VDP register $0A (line interrupt counter)
DataPointer(short, H_int_counter_command, 0x8FFF624);
// just the counter part of the command
DataPointer(char, H_int_counter, 0x8FFF624);
// colour to start fading from
DataPointer(char, Palette_fade_index, 0x8FFF626);
// the number of colours to fade
DataPointer(char, Palette_fade_count, 0x8FFF627);
// more specifically, the number of times V-int routine 0 has run. Reset at the end of a normal frame
DataPointer(short, Lag_frame_count, 0x8FFF628);
DataPointer(char, V_int_routine, 0x8FFF62A);
// used to ensure the sprite limit isn't exceeded
DataPointer(char, Sprites_drawn, 0x8FFF62C);
// points to the water palette data for the current level
DataPointer(short*, Water_palette_data_addr, 0x8FFF62E);
// various counters and variables for palette cycles
DataPointer(short, Palette_cycle_counter0, 0x8FFF632);
// various counters and variables for palette cycles
DataPointer(short, Palette_cycle_counter1, 0x8FFF634);
// used by the random number generator
DataPointer(int, RNG_seed, 0x8FFF636);
DataPointer(short, Game_paused, 0x8FFF63A);
// transferred from RAM to avoid crashing the Mega Drive
DataPointer(short, DMA_trigger_word, 0x8FFF640);
// unless this is set H-int will return immediately
DataPointer(short, H_int_flag, 0x8FFF644);
// keeps fluctuating
DataPointer(short, Water_level, 0x8FFF646);
// the steady central value of the water level
DataPointer(short, Mean_water_level, 0x8FFF648);
DataPointer(short, Target_water_level, 0x8FFF64A);
// this is added to or subtracted from Mean_water_level every frame till it reaches Target_water_level
DataPointer(char, Water_speed, 0x8FFF64C);
// incremented when entering and exiting water, read by the the floating AIZ spike log, cleared on level initialisation and dynamic events of certain levels
DataPointer(char, Water_entered_counter, 0x8FFF64D);
// set if water covers the entire screen (i.e. the underwater palette should be DMAed during V-int rather than the normal palette)
DataPointer(char, Water_full_screen_flag, 0x8FFF64E);
// if this is set Do_Updates will be called from H-int instead of V-int
DataPointer(char, Do_Updates_in_H_int, 0x8FFF64F);
// various counters and variables for palette cycles
DataArray(char, Palette_cycle_counters, 0x8FFF650, 0xC);
DataPointer(short, Palette_frame, 0x8FFF65C);
DataPointer(char, Palette_timer, 0x8FFF65E);
// appears to be a flag for the palette's current status: '0' for 'off', '1' for 'fading', -1 for 'fading done'
DataPointer(char, Super_palette_status, 0x8FFF65F);
// if set, background collision is enabled
DataPointer(char, Background_collision_flag, 0x8FFF664);
// if set, going below the screen wont kill the player
DataPointer(char, Disable_death_plane, 0x8FFF665);
// used for Hyper Sonic's double jump move
DataPointer(char, Hyper_Sonic_flash_timer, 0x8FFF666);
DataPointer(char, Super_Tails_flag, 0x8FFF667);
// Tails would use Palette_frame and Palette_timer, but they're reserved for his Super Flickies
DataPointer(char, Palette_frame_Tails, 0x8FFF668);
DataPointer(char, Palette_timer_Tails, 0x8FFF669);
DataPointer(Controller, Ctrl_2_logical, 0x8FFF66A);
DataPointer(short, Super_frame_count, 0x8FFF670);
// if this is set scrolling will be based on the two variables below rather than the player's actual position
DataPointer(char, Scroll_force_positions, 0x8FFF676);
DataPointer(short, Scroll_forced_X_pos, 0x8FFF678);
DataPointer(short, Scroll_forced_Y_pos, 0x8FFF67C);
DataArray(PLC, Nem_decomp_queue, 0x8FFF680, 0x10);
// the compressed data location for the first entry in the queue
DataPointer(void*, Nem_decomp_source, 0x8FFF680);
// destination in VRAM for the first entry in the queue
DataPointer(short, Nem_decomp_destination, 0x8FFF684);
// various variables used by the Nemesis decompression queue processor
DataArray(char, Nem_decomp_vars, 0x8FFF6E0, 0x20);
// points to either Nem_PCD_WriteRowToVDP or Nem_PCD_WriteRowToVDP_XOR
DataPointer(void*, Nem_write_routine, 0x8FFF6E0);
// stored repeat count for the current palette index
DataPointer(int, Nem_repeat_count, 0x8FFF6E4);
// the current palette index
DataPointer(int, Nem_palette_index, 0x8FFF6E8);
// used in XOR mode
DataPointer(int, Nem_previous_row, 0x8FFF6EC);
// contains the current compressed word being processed
DataPointer(int, Nem_data_word, 0x8FFF6F0);
// the number of bits the data word needs to be shifted by
DataPointer(int, Nem_shift_value, 0x8FFF6F4);
// the number of patterns remaining to be decompressed
DataPointer(short, Nem_patterns_left, 0x8FFF6F8);
// the number of patterns remaining to be decompressed in the current frame
DataPointer(short, Nem_frame_patterns_left, 0x8FFF6FA);
// RAM address of the last object Tails stood on while controlled by AI
DataPointer(MDRamPtr<Object>, Tails_CPU_interact, 0x8FFF700);
// counts down while controller 2 is idle, when it reaches 0 the AI takes over
DataPointer(short, Tails_CPU_idle_timer, 0x8FFF702);
// counts up while Tails is respawning, when it reaches 300 he drops into the level
DataPointer(short, Tails_CPU_flight_timer, 0x8FFF704);
// Tails' current AI routine in a Sonic and Tails game
DataPointer(short, Tails_CPU_routine, 0x8FFF708);
// Tails' target x-position
DataPointer(short, Tails_CPU_target_X, 0x8FFF70A);
// Tails' target y-position
DataPointer(short, Tails_CPU_target_Y, 0x8FFF70C);
// counts up until AI Tails automatically flies up to maintain altitude, while grabbing Sonic in Marble Garden Act 2's boss
DataPointer(char, Tails_CPU_auto_fly_timer, 0x8FFF70E);
// set to #1 when AI Tails needs to jump of his own accord, regardless of whether Sonic jumped or not
DataPointer(char, Tails_CPU_auto_jump_flag, 0x8FFF70F);
DataPointer(char, Rings_manager_routine, 0x8FFF710);
DataPointer(char, Level_started_flag, 0x8FFF711);
// ??? ; unknown object respawn table
DataArray(char, _unkF712, 0x8FFF712, 0x1C);
// selects which palette cycles are used in AIZ1
DataPointer(char, AIZ1_palette_cycle_flag, 0x8FFF72E);
DataPointer(char, Water_flag, 0x8FFF730);
// set when Tails carries Sonic in a Sonic and Tails game
DataPointer(char, Flying_carrying_Sonic_flag, 0x8FFF73E);
// until this is 0 Tails can't pick Sonic up
DataPointer(char, Flying_picking_Sonic_timer, 0x8FFF73F);
// copy of Last_star_post_hit, sets Tails' starting behavior in a Sonic and Tails game
DataPointer(char, Tails_CPU_star_post_flag, 0x8FFF746);
// copy of Ctrl_1, used on the title screen
DataPointer(Controller, Ctrl_1_title, 0x8FFF748);
// if set, disables Knuckles wall grab
DataPointer(char, Disable_wall_grab, 0x8FFF74F);
DataPointer(short, Max_speed, 0x8FFF760);
DataPointer(short, Acceleration, 0x8FFF762);
DataPointer(short, Deceleration, 0x8FFF764);
// used by DPLC routines to detect whether a DMA transfer is required
DataPointer(char, Player_prev_frame, 0x8FFF766);
DataPointer(char, Primary_Angle, 0x8FFF768);
// Used in FindFloor/FindWall
DataPointer(char, Primary_Angle_save, 0x8FFF769);
DataPointer(char, Secondary_Angle, 0x8FFF76A);
// Used in FindFloor/FindWall
DataPointer(char, Secondary_Angle_save, 0x8FFF76B);
// routine counter for the object loading manager
DataPointer(char, Object_load_routine, 0x8FFF76C);
// rounded down to the nearest chunk boundary (128th pixel)
DataPointer(short, Camera_X_pos_coarse, 0x8FFF76E);
// rounded down to the nearest chunk boundary (128th pixel)
DataPointer(short, Camera_Y_pos_coarse, 0x8FFF770);
// the address inside the object placement data of the first object whose X pos is >= Camera_X_pos_coarse + $280
DataPointer(ObjectEntry*, Object_load_addr_front, 0x8FFF772);
// the address inside the object placement data of the first object whose X pos is >= Camera_X_pos_coarse - $80
DataPointer(ObjectEntry*, Object_load_addr_back, 0x8FFF776);
// the object respawn table index for the object at Obj_load_addr_front
DataPointer(MDRamPtr<char>, Object_respawn_index_front, 0x8FFF77A);
// the object respawn table index for the object at Obj_load_addr_back
DataPointer(MDRamPtr<char>, Object_respawn_index_back, 0x8FFF77C);
// timer for palette fade routines
DataPointer(short, Pal_fade_delay, 0x8FFF794);
// points to the primary or secondary collision data as appropriate
DataPointer(void*, Collision_addr, 0x8FFF796);
// set if a boss fight is going on
DataPointer(char, Boss_flag, 0x8FFF7AA);
DataPointer(void*, Primary_collision_addr, 0x8FFF7B4);
DataPointer(void*, Secondary_collision_addr, 0x8FFF7B8);
// number of currently active pollen or leaves in MHZ
DataPointer(char, MHZ_pollen_counter, 0x8FFF7C0);
DataPointer(char, Reverse_gravity_flag, 0x8FFF7C6);
DataPointer(char, WindTunnel_flag, 0x8FFF7C8);
DataPointer(char, WindTunnel_flag_P2, 0x8FFF7C9);
DataPointer(char, Ctrl_1_locked, 0x8FFF7CA);
DataPointer(char, Ctrl_2_locked, 0x8FFF7CB);
DataPointer(short, Chain_bonus_counter, 0x8FFF7D0);
// used on the results screen
DataPointer(short, Time_bonus_countdown, 0x8FFF7D2);
// used on the results screen
DataPointer(short, Ring_bonus_countdown, 0x8FFF7D4);
// Camera_X_pos_coarse - $80
DataPointer(short, Camera_X_pos_coarse_back, 0x8FFF7DA);
// used by DPLC routines to detect whether a DMA transfer is required
DataPointer(char, Player_prev_frame_P2, 0x8FFF7DE);
// used by DPLC routines to detect whether a DMA transfer is required
DataPointer(char, Player_prev_frame_P2_tail, 0x8FFF7DF);
// used by buttons, etc.
DataArray(char, Level_trigger_array, 0x8FFF7E0, 0x10);
// each word stores data on animated level art, including duration and current frame
DataArray(char, Anim_Counters, 0x8FFF7F0, 0x10);
DataArray(MDSpriteTableEntry, Sprite_table_buffer, 0x8FFF800, 80);
// the target camera maximum x-position
DataPointer(short, Target_camera_max_X_pos, 0x8FFFA92);
// the target camera minimum x-position
DataPointer(short, Target_camera_min_X_pos, 0x8FFFA94);
// the target camera minimum y-position
DataPointer(short, Target_camera_min_Y_pos, 0x8FFFA96);
// the target camera maximum y-position
DataPointer(short, Target_camera_max_Y_pos, 0x8FFFA98);
// bits to determine which slots are used for slotted objects
DataPointer(short, Slotted_object_bits, 0x8FFFA9A);
// address of the currently active signpost object. Hidden monitors use this for bouncing
DataPointer(MDRamPtr<Object>, Signpost_addr, 0x8FFFAA6);
// set if end of level is current active, for example after beating a boss
DataPointer(char, End_of_level_flag, 0x8FFFAAA);
// if set, palette rotation scripts are disabled
DataPointer(char, Palette_rotation_disable, 0x8FFFABF);
// timer for palette fade from white routine
DataPointer(short, Pal_fade_delay2, 0x8FFFACE);
// custom routine for palette rotation scripts
DataPointer(void*, Palette_rotation_custom, 0x8FFFADA);
// data used by palette rotation scripts. Last word must always be 0
DataArray(short, Palette_rotation_data, 0x8FFFADE, 9);
// horizontal position of the Metropolis Zone boss in Sky Sanctuary
DataPointer(short, SSZ_MTZ_boss_X_pos, 0x8FFFAF0);
// vertical position of the Metropolis Zone boss in Sky Sanctuary
DataPointer(short, SSZ_MTZ_boss_Y_pos, 0x8FFFAF4);
// horizontal velocity of the Metropolis Zone boss in Sky Sanctuary
DataPointer(short, SSZ_MTZ_boss_X_vel, 0x8FFFAF8);
// vertical velocity of the Metropolis Zone boss in Sky Sanctuary
DataPointer(short, SSZ_MTZ_boss_Y_vel, 0x8FFFAFA);
// counts down until a laser is fired in Sky Sanctuary's Metropolis boss fight
DataPointer(short, SSZ_MTZ_boss_laser_timer, 0x8FFFAFC);
// stores all the VDP commands necessary to initiate a DMA transfer
DataArray(short, DMA_queue, 0x8FFFB00, 0x12 * 7);
// points to the next free slot on the queue
DataPointer(int, DMA_queue_slot, 0x8FFFBFC);
DataArray(short, Normal_palette, 0x8FFFC00, 64);
DataArray(short, Normal_palette_line_2, 0x8FFFC20, 16);
DataArray(short, Normal_palette_line_3, 0x8FFFC40, 16);
DataArray(short, Normal_palette_line_4, 0x8FFFC60, 16);
// used by palette fading routines
DataArray(short, Target_palette, 0x8FFFC80, 64);
DataArray(short, Target_palette_line_2, 0x8FFFCA0, 16);
DataArray(short, Target_palette_line_3, 0x8FFFCC0, 16);
DataArray(short, Target_palette_line_4, 0x8FFFCE0, 16);
// stack contents
DataArray(char, Stack_contents, 0x8FFFD00, 0x100);
// this is the top of the stack, it grows downwards
DataPointer(char, System_stack, 0x8FFFE00);
DataPointer(short, Restart_level_flag, 0x8FFFE02);
// the number of frames which have elapsed since the level started
DataPointer(short, Level_frame_counter, 0x8FFFE04);
// the current position in the debug mode object list
DataPointer(char, Debug_object, 0x8FFFE06);
DataPointer(char, Debug_placement_routine, 0x8FFFE08);
// 0 = normal gameplay, 1 = normal object placement, 2 = frame cycling
DataPointer(char, Debug_placement_type, 0x8FFFE09);
DataPointer(char, Debug_camera_delay, 0x8FFFE0A);
DataPointer(char, Debug_camera_speed, 0x8FFFE0B);
// the number of times V-int has run
DataPointer(int, V_int_run_count, 0x8FFFE0C);
DataPointer(short, Current_zone_and_act, 0x8FFFE10);
DataPointer(char, Current_act, 0x8FFFE10);
DataPointer(char, Current_zone, 0x8FFFE11);
DataPointer(char, Life_count, 0x8FFFE12);
DataPointer(char, Current_special_stage, 0x8FFFE16);
DataPointer(char, Continue_count, 0x8FFFE18);
DataPointer(char, Super_Sonic_Knux_flag, 0x8FFFE19);
DataPointer(char, Time_over_flag, 0x8FFFE1A);
DataPointer(char, Extra_life_flags, 0x8FFFE1B);
DataPointer(char, Update_HUD_life_count, 0x8FFFE1C);
DataPointer(char, Update_HUD_ring_count, 0x8FFFE1D);
DataPointer(char, Update_HUD_timer, 0x8FFFE1E);
DataPointer(char, Update_HUD_score, 0x8FFFE1F);
DataPointer(short, Ring_count, 0x8FFFE20);
DataPointer(int, Timer, 0x8FFFE22);
DataPointer(int, Score, 0x8FFFE26);
DataPointer(char, Last_star_post_hit, 0x8FFFE2A);
DataPointer(char, Saved_last_star_post_hit, 0x8FFFE2B);
DataPointer(short, Saved_zone_and_act, 0x8FFFE2C);
DataPointer(short, Saved_X_pos, 0x8FFFE2E);
DataPointer(short, Saved_Y_pos, 0x8FFFE30);
DataPointer(short, Saved_ring_count, 0x8FFFE32);
DataPointer(int, Saved_timer, 0x8FFFE34);
DataPointer(short, Saved_art_tile, 0x8FFFE38);
// copy of Player 1's top_solid_bit and lrb_solid_bit
DataPointer(short, Saved_solid_bits, 0x8FFFE3A);
DataPointer(short, Saved_camera_X_pos, 0x8FFFE3C);
DataPointer(short, Saved_camera_Y_pos, 0x8FFFE3E);
DataPointer(short, Saved_mean_water_level, 0x8FFFE40);
DataPointer(char, Saved_water_full_screen_flag, 0x8FFFE42);
DataPointer(char, Saved_extra_life_flags, 0x8FFFE43);
DataPointer(short, Saved_camera_max_Y_pos, 0x8FFFE44);
DataPointer(char, Saved_dynamic_resize_routine, 0x8FFFE46);
DataPointer(char, Saved_status_secondary, 0x8FFFE47);
// 1 for entering a Special Stage, 2 for entering a Bonus Stage
DataPointer(char, Special_bonus_entry_flag, 0x8FFFE48);
DataPointer(char, Saved2_last_star_post_hit, 0x8FFFE49);
DataPointer(short, Saved2_zone_and_act, 0x8FFFE4A);
DataPointer(short, Saved2_X_pos, 0x8FFFE4C);
DataPointer(short, Saved2_Y_pos, 0x8FFFE4E);
DataPointer(short, Saved2_ring_count, 0x8FFFE50);
DataPointer(int, Saved2_timer, 0x8FFFE52);
DataPointer(short, Saved2_art_tile, 0x8FFFE56);
DataPointer(short, Saved2_solid_bits, 0x8FFFE58);
DataPointer(short, Saved2_camera_X_pos, 0x8FFFE5A);
DataPointer(short, Saved2_camera_Y_pos, 0x8FFFE5C);
DataPointer(short, Saved2_mean_water_level, 0x8FFFE5E);
DataPointer(char, Saved2_water_full_screen_flag, 0x8FFFE60);
DataPointer(char, Saved2_extra_life_flags, 0x8FFFE61);
DataPointer(short, Saved2_camera_max_Y_pos, 0x8FFFE62);
DataPointer(char, Saved2_dynamic_resize_routine, 0x8FFFE64);
// routine counter for lrz special rocks
DataPointer(char, LRZ_rocks_routine, 0x8FFFE65);
// the address inside the lrz rocks data of the first rock whose X pos is >= Camera_X_pos_coarse + $280
DataPointer(void*, LRZ_rocks_addr_front, 0x8FFFE66);
// the address inside the lrz rocks data of the first rock whose X pos is >= Camera_X_pos_coarse - $80
DataPointer(void*, LRZ_rocks_addr_back, 0x8FFFE6A);
// various oscillating variables
DataArray(char, Oscillating_table, 0x8FFFE6E, 0x42);
DataPointer(char, Slot_machine_goal_frame_timer, 0x8FFFEB0);
DataPointer(char, Slot_machine_goal_frame, 0x8FFFEB1);
DataPointer(char, Rings_frame_timer, 0x8FFFEB2);
DataPointer(char, Rings_frame, 0x8FFFEB3);
DataPointer(char, Slot_machine_peppermint_frame_timer, 0x8FFFEB4);
DataPointer(char, Slot_machine_peppermint_frame, 0x8FFFEB5);
DataPointer(char, Ring_spill_anim_counter, 0x8FFFEB6);
DataPointer(char, Ring_spill_anim_frame, 0x8FFFEB7);
DataPointer(char, Ring_spill_anim_accum, 0x8FFFEB8);
// controls the angle of AIZ giant vines
DataPointer(short, AIZ_vine_angle, 0x8FFFEBA);
DataPointer(char, Extra_life_flags_P2, 0x8FFFEBF);
DataPointer(short, Max_speed_P2, 0x8FFFEC0);
DataPointer(short, Acceleration_P2, 0x8FFFEC2);
DataPointer(short, Deceleration_P2, 0x8FFFEC4);
// left over from Sonic 2
DataPointer(char, Life_count_P2, 0x8FFFEC6);
// left over from Sonic 2
DataPointer(short, Total_ring_count, 0x8FFFEC8);
// left over from Sonic 2
DataPointer(short, Total_ring_count_P2, 0x8FFFECA);
// left over from Sonic 2. Apparently Sonic 3 developers liked copypasting, since gaining a life from rings also increments this counter
DataPointer(short, Monitors_broken, 0x8FFFECC);
// left over from Sonic 2
DataPointer(short, Monitors_broken_P2, 0x8FFFECE);
// left over from Sonic 2
DataPointer(short, Ring_count_P2, 0x8FFFED0);
// used in competition mode
DataPointer(int, Timer_P2, 0x8FFFED2);
// left over from Sonic 2
DataPointer(int, Score_P2, 0x8FFFED6);
// total number of laps in competition mode (typically 5)
DataPointer(char, Competition_total_laps, 0x8FFFEDA);
// current lap number for player 1 in competition mode
DataPointer(char, Competition_current_lap, 0x8FFFEDC);
// current lap number for player 2 in competition mode
DataPointer(char, Competition_current_lap_2P, 0x8FFFEDD);
// left over from Sonic 2
DataPointer(char, Loser_time_left, 0x8FFFEDE);
// left over from Sonic 2
DataPointer(short, Results_screen_2P, 0x8FFFF02);
// left over from Sonic 2
DataPointer(short, Perfect_rings_left, 0x8FFFF04);
// 0 = Sonic and Tails, 1 = Sonic alone, 2 = Tails alone, 3 = Knuckles alone
DataPointer(short, Player_mode, 0x8FFFF08);
// option selected on level select, data select screen or Sonic & Knuckles title screen
DataPointer(short, Player_option, 0x8FFFF0A);
// the number of pieces of data on the queue. Sign bit set indicates a decompression is in progress
DataPointer(short, Kos_decomp_queue_count, 0x8FFFF0E);
// allows decompression to be spread over multiple frames
DataArray(short, Kos_decomp_stored_registers, 0x8FFFF10, 20);
DataPointer(short, Kos_decomp_stored_SR, 0x8FFFF38);
// the address within the Kosinski queue processor at which processing is to be resumed
DataPointer(void*, Kos_decomp_bookmark, 0x8FFFF3A);
// used by the Kosinski queue processor the same way the stack is used by the normal Kosinski decompression routine
DataPointer(short, Kos_description_field, 0x8FFFF3E);
// 2 longwords per entry, first is source location and second is decompression location
DataArray(void*, Kos_decomp_queue, 0x8FFFF40, 2 * 4);
// the compressed data location for the first entry in the queue
DataPointer(void*, Kos_decomp_source, 0x8FFFF40);
// the decompression location for the first entry in the queue
DataPointer(void*, Kos_decomp_destination, 0x8FFFF44);
// the number of modules left to decompresses. Sign bit set indicates a module is being decompressed/has been decompressed
DataPointer(char, Kos_modules_left, 0x8FFFF60);
// the uncompressed size of the last module in words. All other modules are $800 words
DataPointer(short, Kos_last_module_size, 0x8FFFF62);
// 6 bytes per entry, first longword is source location and next word is VRAM destination
DataArray(PLC, Kos_module_queue, 0x8FFFF64, 4);
// the compressed data location for the first module in the queue
DataPointer(void*, Kos_module_source, 0x8FFFF64);
// the VRAM destination for the first module in the queue
DataPointer(short, Kos_module_destination, 0x8FFFF68);
// delay counter for repeating the button press. Allows the menu move even when up/down is held down
DataPointer(short, Level_select_repeat, 0x8FFFF80);
// the current selected option in the level select
DataPointer(short, Level_select_option, 0x8FFFF82);
DataPointer(short, Sound_test_sound, 0x8FFFF84);
DataPointer(char, Title_screen_option, 0x8FFFF86);
// 0 = Enabled, FF = Disabled.
DataPointer(char, Competition_mode_monitors, 0x8FFFF8A);
// 0 = grand prix, 3 = match race, -1 = time attack
DataPointer(char, Competition_mode_type, 0x8FFFF8B);
// the total points to be added due to various bonuses this frame in the end of level results screen
DataPointer(short, Total_bonus_countup, 0x8FFFF8E);
DataPointer(short, Level_music, 0x8FFFF90);
// each bit indicates a special stage entry ring in the current zone
DataPointer(int, Collected_special_ring_array, 0x8FFFF92);
DataPointer(char, Saved2_status_secondary, 0x8FFFF96);
// if set, respawn table is not reset during level load
DataPointer(char, Respawn_table_keep, 0x8FFFF97);
DataPointer(short, Saved_apparent_zone_and_act, 0x8FFFF9A);
DataPointer(short, Saved2_apparent_zone_and_act, 0x8FFFF9C);
// 0 = SEGA GENESIS, 1 = SEGA MEGA DRIVE
DataPointer(char, Blue_spheres_header_flag, 0x8FFFF9F);
// 0 = single stage, 1 = full game
DataPointer(char, Blue_spheres_mode, 0x8FFFFA0);
// 0 = NO WAY!, 1 = normal, bit 7 set = entering a code
DataPointer(char, Blue_spheres_menu_flag, 0x8FFFFA1);
// the layout parts that make up the current stage
DataArray(char, Blue_spheres_current_stage, 0x8FFFFA2, 4);
// number shown at the top of the full game menu
DataPointer(int, Blue_spheres_current_level, 0x8FFFFA6);
// 0 = level, 1 = start, 2 = code
DataPointer(char, Blue_spheres_option, 0x8FFFFAA);
// 0 = normal, -1 = disabled (single stage mode or using a code from single stage mode)
DataPointer(char, Blue_spheres_progress_flag, 0x8FFFFAB);
// value currently displayed
DataPointer(char, Blue_spheres_difficulty, 0x8FFFFAC);
// byte ; value read from the layout
DataPointer(char, Blue_spheres_target_difficulty, 0x8FFFFAD);
// -1 if Sonic 3 isn't locked on
DataPointer(short, SK_alone_flag, 0x8FFFFAE);
DataPointer(char, Chaos_emerald_count, 0x8FFFFB0);
DataPointer(char, Super_emerald_count, 0x8FFFFB1);
// 1 byte per emerald, 0 = not collected, 1 = chaos emerald collected, 2 = grey super emerald, 3 = super emerald collected
DataArray(char, Collected_emeralds_array, 0x8FFFFB2, 7);
// set if at least one emerald has been converted to a super emerald
DataPointer(char, Emeralds_converted_flag, 0x8FFFFBA);
// set if a Sonic & Knuckles special stage is being run
DataPointer(char, SK_special_stage_flag, 0x8FFFFBB);
// status of the title animation buffer. Changes 2 different nametables in VDP while the other is being processed
DataPointer(char, Title_anim_buffer, 0x8FFFFBC);
// title animation delay counter
DataPointer(char, Title_anim_delay, 0x8FFFFBD);
// title animation frame number
DataPointer(char, Title_anim_frame, 0x8FFFFBE);
DataPointer(int, Next_extra_life_score, 0x8FFFFC0);
// left over from Sonic 2
DataPointer(int, Next_extra_life_score_P2, 0x8FFFFC4);
// player 1 mappings while in debug mode
DataPointer(void*, Debug_P1_mappings, 0x8FFFFCA);
// player 2 mappings while in debug mode
DataPointer(void*, Debug_P2_mappings, 0x8FFFFCE);
DataPointer(short, Demo_mode_flag, 0x8FFFFD0);
DataPointer(short, Next_demo_number, 0x8FFFFD2);
// set if a Blue Sphere special stage is being run
DataPointer(char, Blue_spheres_stage_flag, 0x8FFFFD4);
// the number of cycles between V-blanks
DataPointer(short, V_blank_cycles, 0x8FFFFD6);
// bit 7 set := English system, bit 6 set := PAL system
DataPointer(char, Graphics_flags, 0x8FFFFD8);
DataPointer(short, Debug_mode_flag, 0x8FFFFDA);
DataPointer(char, Level_select_flag, 0x8FFFFE0);
DataPointer(char, Slow_motion_flag, 0x8FFFFE1);
// set if the debug cheat's been entered
DataPointer(short, Debug_cheat_flag, 0x8FFFFE2);
// progress entering level select cheat, unused
DataPointer(short, Level_select_cheat_counter, 0x8FFFFE4);
// progress entering debug mode cheat, unused
DataPointer(short, Debug_mode_cheat_counter, 0x8FFFFE6);
DataPointer(short, Competition_mode, 0x8FFFFE8);
// 0 := Sonic, 1 := Tails, 2 := Knuckles
DataPointer(char, P1_character, 0x8FFFFEA);
DataPointer(char, P2_character, 0x8FFFFEB);
// contains an instruction to jump to the V-int handler
DataArray(char, V_int_jump, 0x8FFFFF0, 6);
DataPointer(void*, V_int_addr, 0x8FFFFF2);
// contains an instruction to jump to the H-int handler
DataArray(char, H_int_jump, 0x8FFFFF6, 6);
DataPointer(void*, H_int_addr, 0x8FFFFF8);
// set to 'SM&K' once the checksum routine has run
DataPointer(int, Checksum_string, 0x8FFFFFC);

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

VoidFunc(VInt, 0x4050C0);
FunctionPointer(void *, Kosinski_Decomp, (void *src, void *dst), 0x41028C);
VoidFunc(LoadSpecialStageMap, 0x69E3FF);

class SpriteMappings
{
public:
	SpriteMappings(intptr_t ptr, int numentries = INT16_MAX / 2) { LoadMappings(ptr, numentries); }
	SpriteMappings(const void* ptr, int numentries = INT16_MAX / 2) { LoadMappings(ptr, numentries); }
	void LoadMappings(const void* ptr, int numentries = INT16_MAX / 2) { LoadMappings(reinterpret_cast<intptr_t>(ptr), numentries); }

	void LoadMappings(intptr_t ptr, int numentries = INT16_MAX / 2)
	{
		MappingData.clear();
		const int16_t* off = reinterpret_cast<const int16_t*>(ptr);
		for (int i = 0; i < numentries; ++i)
		{
			intptr_t addr = off[i] + ptr;
			if (off[i] > 0)
				numentries = min(numentries, off[i] / 2);
			int piececnt = *reinterpret_cast<uint16_t*>(addr);
			std::vector<SpritePiece> pieces(piececnt);
			if (piececnt > 0)
				memcpy(pieces.data(), reinterpret_cast<SpritePiece*>(addr + 2), piececnt * sizeof(SpritePiece));
			MappingData.push_back(pieces);
		}
	}

	const void* GetMappingData()
	{
		if (mapdata != nullptr)
		{
			delete[] mapdata;
			mapdata = nullptr;
		}
		if (MappingData.empty()) return nullptr;
		size_t size = MappingData.size() * 4;
		for (auto& item : MappingData)
			size += item.size() * sizeof(SpritePiece);
		mapdata = new uint8_t[size];
		int16_t* offs = reinterpret_cast<int16_t*>(mapdata);
		uint8_t* spd = mapdata + MappingData.size() * 2;
		for (auto& item : MappingData)
		{
			*(offs++) = spd - mapdata;
			*reinterpret_cast<uint16_t*>(spd) = static_cast<uint16_t>(item.size());
			spd += 2;
			if (item.size() > 0)
			{
				memcpy(spd, item.data(), item.size() * sizeof(SpritePiece));
				spd += item.size() * sizeof(SpritePiece);
			}
		}
		return mapdata;
	}

	~SpriteMappings()
	{
		if (mapdata != nullptr)
			delete[] mapdata;
	}

	std::vector<std::vector<SpritePiece>> MappingData;

private:
	uint8_t* mapdata = nullptr;
};

class SpriteDPLCs
{
public:
	SpriteDPLCs(intptr_t ptr, int numentries = INT16_MAX / 2) { LoadDPLCs(ptr, numentries); }
	SpriteDPLCs(const void* ptr, int numentries = INT16_MAX / 2) { LoadDPLCs(ptr, numentries); }
	void LoadDPLCs(const void* ptr, int numentries = INT16_MAX / 2) { LoadDPLCs(reinterpret_cast<intptr_t>(ptr), numentries); }

	void LoadDPLCs(intptr_t ptr, int numentries = INT16_MAX / 2)
	{
		DPLCData.clear();
		const int16_t* off = reinterpret_cast<const int16_t*>(ptr);
		for (int i = 0; i < numentries; ++i)
		{
			intptr_t addr = off[i] + ptr;
			if (off[i] > 0)
				numentries = min(numentries, off[i] / 2);
			int piececnt = *reinterpret_cast<uint16_t*>(addr) + 1;
			std::vector<DPLCEntry> pieces(piececnt);
			memcpy(pieces.data(), reinterpret_cast<DPLCEntry*>(addr + 2), piececnt * sizeof(DPLCEntry));
			DPLCData.push_back(pieces);
		}
	}

	const void* GetDPLCData()
	{
		if (mapdata != nullptr)
		{
			delete[] mapdata;
			mapdata = nullptr;
		}
		if (DPLCData.empty()) return nullptr;
		size_t size = DPLCData.size() * 4;
		for (auto& item : DPLCData)
			size += item.size() * sizeof(DPLCEntry);
		mapdata = new uint8_t[size];
		int16_t* offs = reinterpret_cast<int16_t*>(mapdata);
		uint8_t* spd = mapdata + DPLCData.size() * 2;
		for (auto& item : DPLCData)
		{
			*(offs++) = spd - mapdata;
			*reinterpret_cast<uint16_t*>(spd) = static_cast<uint16_t>(item.size() - 1);
			spd += 2;
			memcpy(spd, item.data(), item.size() * sizeof(DPLCEntry));
			spd += item.size() * sizeof(DPLCEntry);
		}
		return mapdata;
	}

	~SpriteDPLCs()
	{
		if (mapdata != nullptr)
			delete[] mapdata;
	}

	std::vector<std::vector<DPLCEntry>> DPLCData;

private:
	uint8_t* mapdata = nullptr;
};

class SpritePlayerDPLCs
{
public:
	SpritePlayerDPLCs(intptr_t ptr, int numentries = INT16_MAX / 2) { LoadDPLCs(ptr, numentries); }
	SpritePlayerDPLCs(const void* ptr, int numentries = INT16_MAX / 2) { LoadDPLCs(ptr, numentries); }
	void LoadDPLCs(const void* ptr, int numentries = INT16_MAX / 2) { LoadDPLCs(reinterpret_cast<intptr_t>(ptr), numentries); }

	void LoadDPLCs(intptr_t ptr, int numentries = INT16_MAX / 2)
	{
		DPLCData.clear();
		const int16_t* off = reinterpret_cast<const int16_t*>(ptr);
		for (int i = 0; i < numentries; ++i)
		{
			intptr_t addr = off[i] + ptr;
			if (off[i] > 0)
				numentries = min(numentries, off[i] / 2);
			int piececnt = *reinterpret_cast<uint16_t*>(addr);
			std::vector<PlayerDPLCEntry> pieces(piececnt);
			if (piececnt > 0)
				memcpy(pieces.data(), reinterpret_cast<PlayerDPLCEntry*>(addr + 2), piececnt * sizeof(PlayerDPLCEntry));
			DPLCData.push_back(pieces);
		}
	}

	const void* GetDPLCData()
	{
		if (mapdata != nullptr)
		{
			delete[] mapdata;
			mapdata = nullptr;
		}
		if (DPLCData.empty()) return nullptr;
		size_t size = DPLCData.size() * 4;
		for (auto& item : DPLCData)
			size += item.size() * sizeof(PlayerDPLCEntry);
		mapdata = new uint8_t[size];
		int16_t* offs = reinterpret_cast<int16_t*>(mapdata);
		uint8_t* spd = mapdata + DPLCData.size() * 2;
		for (auto& item : DPLCData)
		{
			*(offs++) = spd - mapdata;
			*reinterpret_cast<uint16_t*>(spd) = static_cast<uint16_t>(item.size() - 1);
			spd += 2;
			if (item.size() > 0)
			{
				memcpy(spd, item.data(), item.size() * sizeof(PlayerDPLCEntry));
				spd += item.size() * sizeof(PlayerDPLCEntry);
			}
		}
		return mapdata;
	}

	~SpritePlayerDPLCs()
	{
		if (mapdata != nullptr)
			delete[] mapdata;
	}

	std::vector<std::vector<PlayerDPLCEntry>> DPLCData;

private:
	uint8_t* mapdata = nullptr;
};

#define patchdecl(address,data) { (void*)address, arrayptrandsize(data) }
#define ptrdecl(address,data) { (void*)address, (void*)data }

// S&KC Mod Loader API version.
static const int ModLoaderVer = 3;

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

struct HelperFunctions
{
	// The version of the structure.
	int Version;
	// Registers a new PLC list.
	int(__cdecl *RegisterPLCList)(const PLC *plcs, int length);
	// Retrieves a PLC list.
	const PLC* (__cdecl* GetPLCList)(int index, int& length);
	// Sets a PLC list.
	BOOL(__cdecl *SetPLCList)(int index, const PLC *plcs, int length);
	// Prints a message to the log.
	// Requires version 2 or higher.
	int(__cdecl *PrintDebug)(const char *format, ...);
	// Registers a custom art pointer for an object.
	// Requires version 3 or higher.
	void(__cdecl *RegisterObjArtPtr)(const void *art);
};

typedef void(__cdecl *ModInitFunc)(const wchar_t *path, const HelperFunctions &helperFunctions);

typedef void(__cdecl *ModFrameFunc)();

struct ModInfo
{
	int Version;
};

#endif