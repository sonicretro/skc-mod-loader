using ModManagerCommon;
using System.Collections.Generic;

namespace SKCModManager
{
	enum Game
	{
		S3K,
		SK,
		S3
	}

	enum PaletteSetting
	{
		Accurate,
		Linear,
		SKC,
		Old
	}

	class SKCLoaderInfo : LoaderInfo
	{
		public bool DebugConsole { get; set; }
		public bool DebugFile { get; set; }
		public Game Game { get; set; }
		public PaletteSetting Palette { get; set; }

		public SKCLoaderInfo()
		{
			Mods = new List<string>();
			EnabledCodes = new List<string>();
		}
	}
}
