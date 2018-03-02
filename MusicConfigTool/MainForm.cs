using IniFile;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace MusicConfigTool
{
	public partial class MainForm : Form
	{
		static MainForm Instance;

		public MainForm()
		{
			InitializeComponent();
			Instance = this;
		}

		static readonly string[] GameIDs = { "All", "S3K", "S3", "S&K" };

		readonly string[] smpsSongs = {
			"S3Title",
			"AngelIsland1",
			"AngelIsland2",
			"Hydrocity1",
			"Hydrocity2",
			"MarbleGarden1",
			"MarbleGarden2",
			"CarnivalNight1",
			"CarnivalNight2",
			"FlyingBattery1",
			"FlyingBattery2",
			"IceCap1",
			"IceCap2",
			"LaunchBase1",
			"LaunchBase2",
			"MushroomHill1",
			"MushroomHill2",
			"Sandopolis1",
			"Sandopolis2",
			"LavaReef1",
			"LavaReef2",
			"SkySanctuary",
			"DeathEgg1",
			"DeathEgg2",
			"SKMidboss",
			"Boss",
			"Doomsday",
			"MagneticOrbs",
			"SpecialStage",
			"SlotMachine",
			"GumballMachine",
			"S3Knuckles",
			"AzureLake",
			"BalloonPark",
			"DesertPalace",
			"ChromeGadget",
			"EndlessMine",
			"GameOver",
			"Continue",
			"ActClear",
			"S31Up",
			"ChaosEmerald",
			"S3Invincibility",
			"CompetitionMenu",
			"S3Midboss",
			"LevelSelect",
			"FinalBoss",
			"Drowning",
			"S3AllClear",
			"S3Credits",
			"SKKnuckles",
			"SKTitle",
			"SK1Up",
			"SKInvincibility",
			"SKAllClear",
			"SKCredits",
			"S3CCredits",
			"S3Continue",
			"SKTitle0525",
			"SKAllClear0525",
			"SKCredits0525",
			"GreenGrove1",
			"GreenGrove2",
			"RustyRuin1",
			"RustyRuin2",
			"VolcanoValley2",
			"VolcanoValley1",
			"SpringStadium1",
			"SpringStadium2",
			"DiamondDust1",
			"DiamondDust2",
			"GeneGadget1",
			"GeneGadget2",
			"PanicPuppet2",
			"FinalFight",
			"S3DEnding",
			"S3DSpecialStage",
			"PanicPuppet1",
			"S3DBoss2",
			"S3DBoss1",
			"S3DIntro",
			"S3DCredits",
			"S3DInvincibility",
			"S3DMenu",
			"S4E1Boss",
			"GreenHill",
			"Labyrinth",
			"Marble",
			"StarLight",
			"SpringYard",
			"ScrapBrain",
			"S1Invincibility",
			"S11Up",
			"S1SpecialStage",
			"S1Title",
			"S1Ending",
			"S1Boss",
			"FinalZone",
			"S1ActClear",
			"S1GameOver",
			"S1Continue",
			"S1Credits",
			"S1Drowning",
			"S1ChaosEmerald",
			"CasinoNight2P",
			"EmeraldHill",
			"Metropolis",
			"CasinoNight",
			"MysticCave",
			"MysticCave2P",
			"AquaticRuin",
			"S2DeathEgg",
			"S2SpecialStage",
			"S2Options",
			"S2Ending",
			"S2FinalBoss",
			"ChemicalPlant",
			"S2Boss",
			"SkyChase",
			"OilOcean",
			"WingFortress",
			"EmeraldHill2P",
			"S22PResults",
			"S2SuperSonic",
			"HillTop",
			"S2Title",
			"S2Invincibility",
			"S2HiddenPalace",
			"S2Credits",
			"CasinoNight2PBeta",
			"EmeraldHillBeta",
			"MetropolisBeta",
			"CasinoNightBeta",
			"MysticCaveBeta",
			"MysticCave2PBeta",
			"AquaticRuinBeta",
			"S2DeathEggBeta",
			"S2SpecialStageBeta",
			"S2OptionsBeta",
			"S2FinalBossBeta",
			"ChemicalPlantBeta",
			"S2BossBeta",
			"SkyChaseBeta",
			"OilOceanBeta",
			"WingFortressBeta",
			"EmeraldHill2PBeta",
			"S22PResultsBeta",
			"S2SuperSonicBeta",
			"HillTopBeta",
			"S3DSpecialStageBeta",
			"CarnivalNight1PC",
			"CarnivalNight2PC",
			"IceCap1PC",
			"IceCap2PC",
			"LaunchBase1PC",
			"LaunchBase2PC",
			"KnucklesPC",
			"CompetitionMenuPC",
			"UnusedPC",
			"CreditsPC",
			"S3InvincibilityPC"
		};

		Settings settings;

		List<TrackInfoInternal> TrackOptions = new List<TrackInfoInternal>();

		static readonly List<ComboBox>[] comboboxes = new List<ComboBox>[4];

		private void MainForm_Load(object sender, EventArgs e)
		{
			TableLayoutPanel[] tables = { tableLayoutPanel1, tableLayoutPanel2, tableLayoutPanel3, tableLayoutPanel4 };
			if (!File.Exists("opts.ini"))
			{
				MessageBox.Show(this, "Failed to load opts.ini.", Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
				Environment.Exit(-1);
			}
			foreach (KeyValuePair<string, TrackOptInfo> item in IniSerializer.Deserialize<Dictionary<string, TrackOptInfo>>("opts.ini"))
				TrackOptions.Add(new TrackInfoInternal(item.Key, item.Value.Name, item.Value.EnableByZone, item.Value.EnableByCharacter));
			if (File.Exists("Music.ini"))
				settings = IniSerializer.Deserialize<Settings>("Music.ini");
			else
				settings = new Settings();
			soundfontTextBox.Text = settings.MIDISoundFont;
			for (int gn = 0; gn < GameIDs.Length; gn++)
			{
				string game = GameIDs[gn];
				if (!settings.Music.ContainsKey(game))
					settings.Music.Add(game, new SettingsGame());
				SettingsGame group = settings.Music[game];
				TableLayoutPanel table = tables[gn];
				table.SuspendLayout();
				comboboxes[gn] = new List<ComboBox>();
				for (int tn = 0; tn < TrackOptions.Count; tn++)
				{
					string track = TrackOptions[tn].Key;
					table.Controls.Add(new Label() { Text = TrackOptions[tn].Name + ":", AutoSize = true, TextAlign = ContentAlignment.MiddleLeft }, 0, tn);
					ComboBox cb = new ComboBox() { DropDownStyle = ComboBoxStyle.DropDown, AutoCompleteMode = AutoCompleteMode.SuggestAppend, AutoCompleteSource = AutoCompleteSource.ListItems, Width = 300 };
					cb.Items.AddRange(new[] { "Default", "MIDI" });
					if (gn == 1)
					{
						if (TrackOptions[tn].EnableByCharacter)
							cb.Items.Add("ByCharacter");
						if (TrackOptions[tn].EnableByZone)
							cb.Items.Add("ByZone");
					}
					cb.Items.AddRange(smpsSongs);
					table.Controls.Add(cb, 1, tn);
					comboboxes[gn].Add(cb);
					Button btn = new Button() { AutoSize = true, AutoSizeMode = AutoSizeMode.GrowAndShrink, Text = "File..." };
					btn.Click += new EventHandler((s, ev) => 
					{
						using (OpenFileDialog dlg = new OpenFileDialog() { Filter = "Common Music Files|*.mid;*.midi;*.adx;*.aax;*.brstm;*.bcstm;*.ogg;*.mp3;*.logg;*.wav|All Files|*.*", InitialDirectory = Environment.CurrentDirectory, RestoreDirectory = true })
							if (dlg.ShowDialog(this) == DialogResult.OK)
							{
								string fn = dlg.FileName;
								if (fn.StartsWith(Environment.CurrentDirectory))
									fn = fn.Substring(Environment.CurrentDirectory.Length + 1);
								cb.Text = fn;
							}
					});
					table.Controls.Add(btn, 2, tn);
					if (!group.Tracks.ContainsKey(track))
					{
						group.Tracks.Add(track, "Default");
						cb.SelectedIndex = 0;
					}
					else
						cb.Text = group.Tracks[track];
					if (tn > 0)
						table.RowStyles.Add(new RowStyle(SizeType.AutoSize));
					cb.TextChanged += new EventHandler((s, ev) => group.Tracks[track] = cb.Text);
				}
				table.ResumeLayout();
			}
		}

		private void setAllToDefaultToolStripMenuItem_Click(object sender, EventArgs e)
		{
			foreach (ComboBox cb in comboboxes[tabControl1.SelectedIndex])
				cb.SelectedIndex = 0;
		}

		private void setAllToMIDIToolStripMenuItem_Click(object sender, EventArgs e)
		{
			foreach (ComboBox cb in comboboxes[tabControl1.SelectedIndex])
				cb.SelectedIndex = 1;
		}

		private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
		{
			foreach (string game in GameIDs)
			{
				SettingsGame group = settings.Music[game];
				foreach (string track in TrackOptions.Select(a => a.Key))
					if (group.Tracks[track].Equals("Default", StringComparison.OrdinalIgnoreCase) || group.Tracks[track].Length == 0)
						group.Tracks.Remove(track);
				if (group.Tracks.Count == 0)
					settings.Music.Remove(game);
			}
			if (string.IsNullOrWhiteSpace(settings.MIDISoundFont))
				settings.MIDISoundFont = null;
			IniSerializer.Serialize(settings, "Music.ini");
		}

		private void soundfontTextBox_TextChanged(object sender, EventArgs e)
		{
			settings.MIDISoundFont = soundfontTextBox.Text;
		}

		private void soundfontBrowseButton_Click(object sender, EventArgs e)
		{
			using (OpenFileDialog dlg = new OpenFileDialog() { Filter = "Soundfonts|*.sf2|All Files|*.*", InitialDirectory = Environment.CurrentDirectory, RestoreDirectory = true })
				if (dlg.ShowDialog(this) == DialogResult.OK)
				{
					string fn = dlg.FileName;
					if (fn.StartsWith(Environment.CurrentDirectory))
						fn = fn.Substring(Environment.CurrentDirectory.Length + 1);
					soundfontTextBox.Text = fn;
				}
		}
	}

	class Settings
	{
		public string MIDISoundFont { get; set; }
		[IniCollection(IniCollectionMode.IndexOnly)]
		public Dictionary<string, SettingsGame> Music { get; set; } = new Dictionary<string, SettingsGame>();
	}

	class SettingsGame
	{
		[IniCollection(IniCollectionMode.IndexOnly)]
		public Dictionary<string, string> Tracks { get; set; } = new Dictionary<string, string>();
	}

	class TrackInfoInternal
	{
		public string Key { get; set; }
		public string Name { get; set; }
		public bool EnableByZone { get; set; }
		public bool EnableByCharacter { get; set; }

		public TrackInfoInternal(string key, string name, bool enableByZone, bool enableByCharacter)
		{
			Key = key;
			Name = name;
			EnableByZone = enableByZone;
			EnableByCharacter = enableByCharacter;
		}
	}

	class TrackOptInfo
	{
		public string Name { get; set; }
		public bool EnableByZone { get; set; }
		public bool EnableByCharacter { get; set; }
	}
}