#define CulturalFestivalFlag true

#ifdef _DEBUG
#define DebugModeFlag true
#else
#define DebugModeFlag false
#endif // !DEBUG

#include "DxLib.h"
//#include "resource.h"
#include <math.h>
#include <string>


const int BlockShift = 8;	//ブロックずらし許容値
const int ChunkSize = 20;
const double FrameRate = 40;

typedef struct {
	int X, Y;			//座標
	int Xspeed, Yspeed;	//速度
	int WalkCount;		//歩いたりした距離（一定時間ごとにリセット）
	char Direction;		//向き
	char GroundFlag;	//地面についているか
	int Death;			//死亡レヴェル（状態）
	bool Big;			//でかいか
} Mob;
#define LEFT 0
#define RIGHT 1
int PadInput;
int Select = 0, Time = -1;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int Start(void);
int Game(int Stage);	//0:強制終了 -1:ロード失敗 1:ゴール/F1 2:死んだ

void My_MessageBox(std::string str[], int str_num, int back_color = GetColor(255, 255, 0), int str_color = GetColor(255, 0, 0)) {
	std::string longest_str = str[0];
	for (int i = 1; i < str_num; i++) {
		if (GetDrawStringWidth(
			str[i].c_str(), strlen(str[i].c_str())
		) > GetDrawStringWidth(
			longest_str.c_str(), strlen(longest_str.c_str())
		)) longest_str = str[i];
	}
	DrawBox(
		(640 - GetDrawStringWidth(
			longest_str.c_str(), strlen(longest_str.c_str())
		)) / 2 - 32,
		(480 - 16 * str_num) / 2 - 32,
		(640 + GetDrawStringWidth(
			longest_str.c_str(), strlen(longest_str.c_str())
		)) / 2 + 32,
		(480 + 16 * str_num) / 2 + 32,
		back_color,
		TRUE
	);
	for (int i = 0; i < str_num; i++) {
		DrawString(
			(640 - GetDrawStringWidth(
				str[i].c_str(), strlen(str[i].c_str())
			)) / 2,
			(480 - 16 * str_num) / 2 + 16 * i,
			str[i].c_str(),
			str_color
		);
	}
	return;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);
	//SetWindowIconID(666);
	//SetWaitVSyncFlag(FALSE);
	SetGraphMode(640, 480, 16);
	if (DxLib_Init() == -1) return -1;
	if (CulturalFestivalFlag) {
		//全画面もどき
		RECT rc;
		GetWindowRect(GetDesktopWindow(), &rc);
		int Width = rc.right - rc.left, Height = rc.bottom - rc.top;
		if (Width * 1.0 / Height >= 640.0 / 480) {
			//左右に余白
			SetWindowSizeExtendRate(Height * 1.0 / 480);
		}
		else {
			//上下に余白
			SetWindowSizeExtendRate(Width * 1.0 / 640);
		}
		SetMouseDispFlag(FALSE);
		SetWindowStyleMode(3);
	}
	SetMainWindowText("真顔のアクションver2.0");
	SetDrawScreen(DX_SCREEN_BACK);
	SetUseTransColor(FALSE);

	if (CulturalFestivalFlag) {
		bool BeforeUP = true, BeforeDOWN = true;
		while (ProcessMessage() != -1 && (PadInput & PAD_INPUT_9) == 0) {
			if (CulturalFestivalFlag) SetMousePoint(640 / 2, 480 / 2);
			//処理
			PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
			if ((PadInput & PAD_INPUT_UP) && !BeforeUP) Select--;
			if ((PadInput & PAD_INPUT_DOWN) && !BeforeDOWN) Select++;
			BeforeUP = (PadInput & PAD_INPUT_UP), BeforeDOWN = (PadInput & PAD_INPUT_DOWN);
			if (Select < 0) Select = 2;
			if (Select == 3) Select = 0;
			if (CheckHitKey(KEY_INPUT_RETURN)) break;
			//描画
			DrawBox(0, 0, 640, 480, GetColor(255, 255, 255), TRUE);
			DrawString(320 - 8 * 21, 120 - 8, "今あなたの後ろには何人並んでいますか?\n上下キーで選んでEnterで決定してください。", GetColor(0, 0, 0));
			if (Select == 0) DrawString(320 - 8 * 19, 200 - 8, "→　１人以下", GetColor(255, 0, 0));
			else DrawString(320 - 8 * 19, 200 - 8, "　　１人以下", GetColor(0, 0, 0));
			if (Select == 1) DrawString(320 - 8 * 19, 280 - 8, "→　２人", GetColor(255, 0, 0));
			else DrawString(320 - 8 * 19, 280 - 8, "　　２人", GetColor(0, 0, 0));
			if (Select == 2) DrawString(320 - 8 * 19, 360 - 8, "→　３人以上", GetColor(255, 0, 0));
			else DrawString(320 - 8 * 19, 360 - 8, "　　３人以上", GetColor(0, 0, 0));
			ScreenFlip();
		}
		LoadGraphScreen(0, 0, "画像/操作説明.jpg", FALSE);
		ScreenFlip();
		WaitKey();
		WaitKey();
	}

	while (true) {
		int Stage = Start();
		if (Stage == 0) break;
		while (true) {
			switch (Game(Stage)) {
			case 0:
				DxLib_End();
				return 0;
			case -1:
				goto START;
				break;
			case 1:
				goto START;
				break;
			}
		} START:;
	}

	DxLib_End();
	return 0;
}

int Start(void) {
	int map[15][20] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0,13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,14, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0,11, 0, 0, 0, 0,12, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
	Mob Player;
	Player.X = 64;
	Player.Y = 352;
	Player.Xspeed = 0;
	Player.Yspeed = 0;
	Player.WalkCount = 0;
	Player.Direction = RIGHT;
	Player.GroundFlag = false;
	char Count = 0;
	int FrameTime;
	int Brightness = 255, Stage = 0;
	//画像などの読み込み
	int SkyPNG = LoadGraph("画像/空.png");
	int Player1PNG = LoadGraph("画像/Player 1.png");
	int Player2PNG = LoadGraph("画像/Player 2.png");
	int PlayerJumpPNG = LoadGraph("画像/Player Jump.png");
	int GrassPNG = LoadGraph("画像/草.png");
	int SoilPNG = LoadGraph("画像/土.png");
	int WoodPNG = LoadGraph("画像/木.png");
	int Stage1PNG = LoadGraph("画像/Stage 1.png");
	int Stage2PNG = LoadGraph("画像/Stage 2.png");
	int Stage3PNG = LoadGraph("画像/Stage 3.png");
	int Stage4PNG = LoadGraph("画像/Stage 4.png");
	int TitlePNG = LoadGraph("画像/タイトル.png");
	int jump = LoadSoundMem("効果音/jump.mp3");

	while (ProcessMessage() != -1 && (PadInput & PAD_INPUT_9) == 0) {
		if (CulturalFestivalFlag) SetMousePoint(640 / 2, 480 / 2);
		FrameTime = GetNowCount();
		if (Count == 2) Count = 0;
		Count++;
		//入力
		PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		if ((PadInput & PAD_INPUT_LEFT) != 0) {
			Player.Xspeed -= 1;
			Player.Direction = LEFT;
		}
		if ((PadInput & PAD_INPUT_RIGHT) != 0) {
			Player.Xspeed += 1;
			Player.Direction = RIGHT;
		}
		if (Player.GroundFlag && ((PadInput & PAD_INPUT_1) != 0 || (PadInput & PAD_INPUT_2) != 0 || (PadInput & PAD_INPUT_3) != 0 || (PadInput & PAD_INPUT_4) != 0 || CheckHitKey(KEY_INPUT_UP) != 0)) {
			Player.Yspeed = -17 - abs(Player.Xspeed) / 2.6;
			PlaySoundMem(jump, DX_PLAYTYPE_BACK, TRUE);
		}
		//処理
		// 移動 {
		//プレイヤーの移動
		if (Player.Xspeed > 0 && Count == 1) Player.Xspeed--;
		if (Player.Xspeed < 0 && Count == 1) Player.Xspeed++;
		if (Player.Xspeed > 8) Player.Xspeed = 8;
		if (Player.Xspeed < -8) Player.Xspeed = -8;
		if (Player.Yspeed < 0 && (PadInput & PAD_INPUT_1) == 0 && (PadInput & PAD_INPUT_2) == 0 && (PadInput & PAD_INPUT_3) == 0 && (PadInput & PAD_INPUT_4) == 0 && CheckHitKey(KEY_INPUT_UP) == 0) Player.Yspeed *= 0.9;
		Player.Y += ++Player.Yspeed;
		Player.X += Player.Xspeed;
		//WalkCount
		Player.WalkCount += abs(Player.Xspeed);
		Player.WalkCount %= 48;
		// } 当たり判定 {
		Player.GroundFlag = false;
		//壁
		if (Player.X < 0) {
			Player.X = 0;
			Player.Xspeed = 0;
		}
		if (Player.X > 640 - 32) {
			Player.X = 640 - 32;
			Player.Xspeed = 0;
		}
		//プレイヤー・ブロック
		//ブロックの上下の当たり判定
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 20; j++) {
				//地面ブロック
				if (map[i][j] == 1 || map[i][j] == 2) {
					if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
						if (Player.Yspeed >= 0) {
							//ブロックに乗っているときの処理
							Player.GroundFlag = true;
							Player.Y = 32 * i - 48;
						}
						if (Player.Yspeed < 0) {
							//ブロックに頭をぶつけた時の処理
							if (Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
							}
						}
					}
				}
				//ステージ選択ブロック
				if (map[i][j] == 11 || map[i][j] == 12 || map[i][j] == 13 || map[i][j] == 14) {
					if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
						if (Player.Yspeed >= 0) {
							//ブロックに乗っているときの処理
							Player.GroundFlag = true;
							Player.Y = 32 * i - 48;
						}
						if (Player.Yspeed < 0) {
							//ブロックに頭をぶつけた時の処理
							if (Player.X + 32 <= 32 * j + BlockShift) Player.X = 32 * j - 32;
							else if (Player.X >= 32 * j + 32 - BlockShift) Player.X = 32 * j + 32;
							else {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								if (Stage == 0) PlaySoundFile("効果音/hintBlock.mp3", DX_PLAYTYPE_BACK);
								Stage = map[i][j] - 10;
							}
						}
					}
				}
			}
		}
		//ブロックの左右の当たり判定
		//画面上
		for (int j = 0; j < 20; j++) {
			//地面ブロック
			if (map[0][j] == 1 || map[0][j] == 2) {
				if (Player.Y < 0 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
					if (Player.Xspeed > 0) {
						//ブロックの左に当たった時の処理
						Player.X = 32 * j - 32;
						Player.Xspeed = 0;
					}
					if (Player.Xspeed < 0) {
						//ブロックの右に当たった時の処理
						Player.X = 32 * j + 32;
						Player.Xspeed = 0;
					}
				}
			}
			//ステージ選択ブロック
			if (map[0][j] == 11 || map[0][j] == 12 || map[0][j] == 13 || map[0][j] == 14) {
				if (Player.Y < 0 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
					if (Player.Xspeed > 0) {
						//ブロックの左に当たった時の処理
						Player.X = 32 * j - 32;
						Player.Xspeed = 0;
					}
					if (Player.Xspeed < 0) {
						//ブロックの右に当たった時の処理
						Player.X = 32 * j + 32;
						Player.Xspeed = 0;
					}
				}
			}
		}
		//画面内
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 20; j++) {
				//地面ブロック
				if (map[i][j] == 1 || map[i][j] == 2) {
					if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32) {
						if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + Player.Xspeed) {
							//ブロックの左に当たった時の処理
							Player.X = 32 * j - 32;
							Player.Xspeed = 0;
						}
						if (Player.X >= 32 * j + 32 + Player.Xspeed && Player.X < 32 * j + 32) {
							//ブロックの右に当たった時の処理
							Player.X = 32 * j + 32;
							Player.Xspeed = 0;
						}
						//ブロックずらし
						//左
						if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + BlockShift) Player.X = 32 * j - 32;
						//右
						if (Player.X >= 32 * j + 32 - BlockShift && Player.X < 32 * j + 32) Player.X = 32 * j + 32;
					}
				}
				//ステージ選択ブロック
				if (map[i][j] == 11 || map[i][j] == 12 || map[i][j] == 13 || map[i][j] == 14) {
					if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32) {
						if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + Player.Xspeed) {
							//ブロックの左に当たった時の処理
							Player.X = 32 * j - 32;
							Player.Xspeed = 0;
						}
						if (Player.X >= 32 * j + 32 + Player.Xspeed && Player.X < 32 * j + 32) {
							//ブロックの右に当たった時の処理
							Player.X = 32 * j + 32;
							Player.Xspeed = 0;
						}
					}
				}
			}
		}
		// } 地面にいたら縦に動かなくする
		if (Player.GroundFlag) Player.Yspeed = 0;
		//描画
		ClearDrawScreen();
		if (Stage != 0) Brightness -= 16;
		SetDrawBright(Brightness, Brightness, Brightness);
		DrawGraph(0, 0, SkyPNG, FALSE);
		for (int i = 0; i < 15; i++) for (int j = 0; j < 20; j++) {
			switch (map[i][j]) {
			case 1:
				if (i == 0) DrawGraph(32 * j, 32 * i, SoilPNG, TRUE);
				else if (map[i - 1][j] == 0) DrawGraph(32 * j, 32 * i, GrassPNG, TRUE);
				else DrawGraph(32 * j, 32 * i, SoilPNG, TRUE);
				break;
			case 2:
				DrawGraph(32 * j, 32 * i, WoodPNG, TRUE);
				break;
			case 11:
				DrawGraph(32 * j, 32 * i, Stage1PNG, TRUE);
				break;
			case 12:
				DrawGraph(32 * j, 32 * i, Stage2PNG, TRUE);
				break;
			case 13:
				DrawGraph(32 * j, 32 * i, Stage3PNG, TRUE);
				break;
			case 14:
				DrawGraph(32 * j, 32 * i, Stage4PNG, TRUE);
				break;
			case -1:
				DrawGraph(32 * j, 32 * i, TitlePNG, TRUE);
				DrawString(32 * j + 28, 32 * (i + 6) + 12, "行きたいステージの番号が\n書かれたブロックを叩け！", GetColor(0, 0, 0));
				break;
			}
		}
		if (Player.GroundFlag == false) {
			if (Player.Direction == RIGHT) DrawGraph(Player.X, Player.Y, PlayerJumpPNG, TRUE);
			else DrawTurnGraph(Player.X, Player.Y, PlayerJumpPNG, TRUE);
		}
		else {
			if (Player.Direction == RIGHT) {
				if (Player.WalkCount / 24) DrawGraph(Player.X, Player.Y, Player1PNG, TRUE);
				else DrawGraph(Player.X, Player.Y, Player2PNG, TRUE);
			}
			else {
				if (Player.WalkCount / 24) DrawTurnGraph(Player.X, Player.Y, Player1PNG, TRUE);
				else DrawTurnGraph(Player.X, Player.Y, Player2PNG, TRUE);
			}
		}
		//残り時間
		if (CulturalFestivalFlag && Time != -1) {
			DrawBox(0, 0, 8 * 18.5, 16, 0xffffff, TRUE);
			DrawFormatString(0, 0, GetColor(0, 0, 0), "残り時間 : %d 秒", (Time - GetNowCount()) / 1000);
			if (GetNowCount() > Time) {
				LoadGraphScreen(0, 0, "画像/終了.png", FALSE);
				ScreenFlip();
				WaitKey();
				return 0;
			}
		}
		ScreenFlip();
		if ((PadInput & PAD_INPUT_10) == 0) while (GetNowCount() - FrameTime < 1000.0 / FrameRate);
		//ステージ決定
		if (Stage != 0 && Brightness <= 0) {
			SetDrawBright(255, 255, 255);
			return Stage;
		}
	}
	return 0;
}

int Game(int Stage) {

	static int Life = 2;

	//ロードスタート
	ClearDrawScreen();
	DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), TRUE);
	LoadGraphScreen(260, 216, "画像/Player Jump.png", TRUE);
	DrawFormatString(320, 232, GetColor(255, 255, 255), "× %d", Life);
	ScreenFlip();
	int LoadStart = GetNowCount();

	// ファイルの読み込み ////----////----////----////----////----//
	//ステージマップ
	char FileName[] = "ステージマップ/Stage0.csv";
	FileName[20] += Stage;
	int File = FileRead_open(FileName);
	if (File == 0 || FileRead_eof(File) != 0) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"マップデータの読み込みに失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "マップデータの読み込みに失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "マップデータの読み込みに失敗しました。\nステージマップファイルをして確認ください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}
	char FirstRow[4 * 1024];
	FileRead_gets(FirstRow, 4 * 1024, File);
	int MapXSize = 0;
	for (int i = 0; i < 4 * 1024; i++) {
		if (FirstRow[i] == '\0') break;
		if (FirstRow[i] == ',') MapXSize++;
	}
	FileRead_seek(File, 0, SEEK_SET);
	int *Map = (int *)malloc(sizeof(int) * 15 * MapXSize);
	if (Map == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		return -1;
	}
#define map(map_y, map_x) Map[MapXSize * (map_y) + (map_x)]
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (FileRead_eof(File)) {
			if (CulturalFestivalFlag) {
				std::string Error[2] = {
					"マップデータの読み込みに失敗しました。",
					"部員を呼んでください。"
				};
				My_MessageBox(Error, 2);
				ScreenFlip();
				WaitKey();
				//MessageBox(GetMainWindowHandle(), "マップデータの読み込みに失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
			}
			else MessageBox(GetMainWindowHandle(), "マップデータの読み込みに失敗しました。\nステージマップファイルを確認してください。", NULL, MB_OK | MB_ICONEXCLAMATION);
			free(Map);
			return -1;
		}
		FileRead_scanf(File, "%d,", &map(i, j));
	}
	FileRead_close(File);
	if (MapXSize < 15) MapXSize = 15;
	//ステージデータ
	char FileName2[] = "ステージマップ/Stage0Data.csv";
	FileName2[20] += Stage;
	int File2 = FileRead_open(FileName2);
	int MapDataSize = (MapXSize % ChunkSize) ? (MapXSize / ChunkSize + 1) : (MapXSize / ChunkSize);
	int *MapDataX = (int *)malloc(sizeof(int) * MapDataSize);
	if (MapDataX == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		return -1;
	}
	int *MapDataY = (int *)malloc(sizeof(int) * MapDataSize);
	if (MapDataY == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		return -1;
	}
	int *MapDataSave = (int *)malloc(sizeof(int) * MapDataSize);
	if (MapDataSave == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(MapDataSave);
		return -1;
	}
	char **MapDataHint = (char **)malloc(sizeof(char *) * MapDataSize);
	for (int i = 0; i < MapDataSize; ++i)
		MapDataHint[i] = (char *)malloc(sizeof(char) * 256);
	if (MapDataHint == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(MapDataSave);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		return -1;
	}
	int *MapDataBack = (int *)malloc(sizeof(int) * MapDataSize);
	if (MapDataBack == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(MapDataSave);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < MapDataSize; i++) {
		if (!FileRead_eof(File2)) {
			char sentence[256] = {};
			FileRead_gets(sentence, 256, File2);
			char tmp[256] = {};
			sscanfDx(sentence, "%d %d, %d %d %s",
				&MapDataBack[i], &MapDataX[i], &MapDataY[i], &MapDataSave[i], tmp);
			MapDataX[i] *= 32 / 2;
			MapDataY[i] *= 32 / 2;
			for (int j = 0; j < 256; j++) if (tmp[j] == '`') tmp[j] = '\n';	// '`' -> '\n'
			strcpyDx(MapDataHint[i], tmp);
		}
		else {
			MapDataX[i] = 2 * 32;
			MapDataY[i] = 11 * 32;
			MapDataSave[i] = true;
			MapDataBack[i] = 0;
			strcpyDx(MapDataHint[i], "No Message.");
		}
	}
	FileRead_close(File2);
	//----////----////----////----////----////----////----////----//

	int Scroll = 0;
	int Count = 0;
	int FrameTime = 0;
	bool F1Flag = false;
	//プレイヤー
	static int BeforeStage = 0;
	static int SaveX, SaveY;
	if (BeforeStage != Stage) SaveX = 64, SaveY = 352;
	BeforeStage = Stage;
	Mob Player;
	Player.X = SaveX;
	Player.Y = SaveY;
	Player.Xspeed = 0;
	Player.Yspeed = 0;
	Player.WalkCount = 0;
	Player.Direction = RIGHT;
	Player.GroundFlag = false;
	Player.Death = 0;
	Player.Big = false;
	int WarpX, WarpY;
	bool HumiFlag;
	int GoalCount = 0;
	int HintCount = 0;
	//スライム
	int GoombaNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -1) GoombaNum++;
	}
	Mob *Goomba = (Mob *)malloc(sizeof(Mob) * GoombaNum);
	if (Goomba == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(MapDataSave);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < GoombaNum; i++) {
		int GoombaCount = 0;
		for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
			if (map(j, k) == -1) GoombaCount++;
			if (GoombaCount > i) {
				Goomba[i].X = 32 * k;
				Goomba[i].Y = 32 * j;
				goto Goomba_loop_end;
			}
		} Goomba_loop_end:;
		Goomba[i].Xspeed = -2;
		Goomba[i].Yspeed = 0;
		Goomba[i].Death = 0;
		Goomba[i].Big = false;
	}
	//テルテル
	int SunnyNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -2 || map(i, j) == -3) SunnyNum++;
	}
	Mob *Sunny = (Mob *)malloc(sizeof(Mob) * SunnyNum);
	if (Sunny == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(Sunny);
		free(MapDataSave);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < SunnyNum; i++) {
		int SunnyCount = 0;
		for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
			if (map(j, k) == -2 || map(j, k) == -3) SunnyCount++;
			if (SunnyCount > i) {
				if (map(j, k) == -2) {
					Sunny[i].X = 32 * k + 16;
					Sunny[i].Yspeed = -12;
				}
				else {
					Sunny[i].X = 32 * k;
					Sunny[i].Yspeed = 12;
				}
				Sunny[i].Y = 32 * j;
				goto Sunny_loop_end;
			}
		} Sunny_loop_end:;
		Sunny[i].Death = -1;
	}
	//落ちてくるレンガ
	int FallingBrickNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 11) FallingBrickNum++;
	}
	Mob *FallingBrick = (Mob *)malloc(sizeof(Mob) * FallingBrickNum);
	if (FallingBrick == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(MapDataSave);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < FallingBrickNum; i++) {
		FallingBrick[i].Death = 1;
	}
	//毒キノコ
	int PoisonNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 12) PoisonNum += 256;
	}
	Mob *Poison = (Mob *)malloc(sizeof(Mob) * PoisonNum);
	if (Poison == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(MapDataSave);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < PoisonNum; i++) Poison[i].Death = 1;
	//まずいキノコ
	int MushroomNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 13) MushroomNum++;
	}
	Mob *Mushroom = (Mob *)malloc(sizeof(Mob) * MushroomNum);
	if (Mushroom == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(Mushroom);
		free(MapDataSave);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < MushroomNum; i++) Mushroom[i].Death = 1;
	//スター
	int StarNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 16) StarNum++;
	}
	Mob *Star = (Mob *)malloc(sizeof(Mob) * StarNum);
	if (Star == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(Mushroom);
		free(MapDataSave);
		free(Star);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < StarNum; i++) Star[i].Death = 1;
	//亀
	int KoopaNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -4) KoopaNum++;
	}
	Mob *Koopa = (Mob *)malloc(sizeof(Mob) * KoopaNum);
	if (Koopa == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(Mushroom);
		free(MapDataSave);
		free(Star);
		free(Koopa);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < KoopaNum; i++) {
		int KoopaCount = 0;
		for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
			if (map(j, k) == -4) KoopaCount++;
			if (KoopaCount > i) {
				Koopa[i].X = 32 * k;
				Koopa[i].Y = 32 * j;
				goto Koopa_loop_end;
			}
		} Koopa_loop_end:;
		Koopa[i].Xspeed = -2;
		Koopa[i].Yspeed = 0;
		Koopa[i].Death = 0;
	}
	//落とし穴
	int HoleNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -5) HoleNum++;
	}
	Mob *Hole = (Mob *)malloc(sizeof(Mob) * HoleNum);
	if (Hole == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(Mushroom);
		free(MapDataSave);
		free(Star);
		free(Koopa);
		free(Hole);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < HoleNum; i++) {
		int HoleCount = 0;
		for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
			if (map(j, k) == -5) HoleCount++;
			if (HoleCount > i) {
				Hole[i].X = 32 * k;
				Hole[i].Y = 32 * j;
				Hole[i].Death = 1;
				if (j > 0 && (map(j - 1, k) <= 0 && map(j - 1, k) != -5 || map(j - 1, k) == 6 || map(j - 1, k) == 7 || map(j - 1, k) == 19 || map(j - 1, k) == 22)) Hole[i].Death = 0;
				goto Hole_loop_end;
			}
		} Hole_loop_end:;
		Hole[i].Yspeed = 0;
	}
	//ビーム
	int BeamNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -6) BeamNum++;
	}
	Mob *Beam = (Mob *)malloc(sizeof(Mob) * BeamNum);
	if (Beam == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(Mushroom);
		free(MapDataSave);
		free(Star);
		free(Koopa);
		free(Hole);
		free(Beam);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < BeamNum; i++) Beam[i].Death = 1;
	//キノコ
	int BigMushroomNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 27) BigMushroomNum++;
	}
	Mob *BigMushroom = (Mob *)malloc(sizeof(Mob) * BigMushroomNum);
	if (BigMushroom == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(Mushroom);
		free(MapDataSave);
		free(Star);
		free(Koopa);
		free(Hole);
		free(Beam);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(BigMushroom);
		free(MapDataBack);
		return -1;
	}
	for (int i = 0; i < BigMushroomNum; i++) BigMushroom[i].Death = 1;
	//とげスライム
	int GoombaThornsNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -7) GoombaThornsNum++;
	}
	Mob *GoombaThorns = (Mob *)malloc(sizeof(Mob) * GoombaThornsNum);
	if (GoombaThorns == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(Mushroom);
		free(MapDataSave);
		free(Star);
		free(Koopa);
		free(Hole);
		free(Beam);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(BigMushroom);
		free(MapDataBack);
		free(GoombaThorns);
		return -1;
	}
	for (int i = 0; i < GoombaThornsNum; i++) {
		int GoombaThornsCount = 0;
		for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
			if (map(j, k) == -7) GoombaThornsCount++;
			if (GoombaThornsCount > i) {
				GoombaThorns[i].X = 32 * k;
				GoombaThorns[i].Y = 32 * j;
				goto GoombaThorns_loop_end;
			}
		} GoombaThorns_loop_end:;
		GoombaThorns[i].Xspeed = -2;
		GoombaThorns[i].Yspeed = 0;
		GoombaThorns[i].Death = 0;
		GoombaThorns[i].Big = false;
	}
	//リフト
	int LiftNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -8 || map(i, j) == -9 || map(i, j) == -10 || map(i, j) == -11) LiftNum++;
	}
	Mob *Lift = (Mob *)malloc(sizeof(Mob) * LiftNum);
	if (Lift == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"メモリの確保に失敗しました。",
				"部員を呼んでください。"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。\n部員を呼んでください。", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "メモリの確保に失敗しました。", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		free(Goomba);
		free(FallingBrick);
		free(Poison);
		free(Mushroom);
		free(MapDataSave);
		free(Star);
		free(Koopa);
		free(Hole);
		free(Beam);
		for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
		free(MapDataHint);
		free(BigMushroom);
		free(MapDataBack);
		free(GoombaThorns);
		free(Lift);
		return -1;
	}
	for (int i = 0; i < LiftNum; i++) {
		int LiftCount = 0;
		for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
			if (map(j, k) == -8 || map(j, k) == -9 || map(j, k) == -10 || map(j, k) == -11) LiftCount++;
			if (LiftCount > i) {
				if (map(j, k) == -8) {
					Lift[i].Death = 0;
					Lift[i].Yspeed = -4;
				}
				if (map(j, k) == -9) {
					Lift[i].Death = 0;
					Lift[i].Yspeed = 4;
				}
				if (map(j, k) == -10) {
					Lift[i].Death = -1;
					Lift[i].Yspeed = 0;
				}
				if (map(j, k) == -11) {
					Lift[i].Death = -2;
					Lift[i].Yspeed = 0;
				}
				Lift[i].X = 32 * k;
				Lift[i].Y = 32 * j;
				goto Lift_loop_end;
			}
		} Lift_loop_end:;
	}
	//ブロックの破片
	Mob Debris[4 * 64];
	for (int i = 0; i < 4 * 64; i++) Debris[i].Death = 1;
	//コインチャリーン
	Mob FloatCoin[16];
	for (int i = 0; i < 16; i++) FloatCoin[i].Death = 1;
	//画像などの読み込み
	int CloudPNG = LoadGraph("画像/雲.png");
	int CavePNG = LoadGraph("画像/洞窟.png");
	int CirclePNG = LoadGraph("画像/サークル.png");
	int Player1PNG = LoadGraph("画像/Player 1.png");
	int Player2PNG = LoadGraph("画像/Player 2.png");
	int PlayerJumpPNG = LoadGraph("画像/Player Jump.png");
	int PlayerOwataPNG = LoadGraph("画像/Player Owata.png");
	int BigPlayerPNG = LoadGraph("画像/Big Player.png");
	int GoombaPNG = LoadGraph("画像/Goomba.png");
	int GoombaOwataPNG = LoadGraph("画像/Goomba Owata.png");
	int SunnyPNG = LoadGraph("画像/テルテル.png");
	int PoisonPNG = LoadGraph("画像/毒キノコ.png");
	int MushroomPNG = LoadGraph("画像/キノコ.png");
	int StarPNG = LoadGraph("画像/スター.png");
	int KoopaPNG = LoadGraph("画像/亀.png");
	int KoopaOwataPNG = LoadGraph("画像/亀 Owata.png");
	int BeamPNG = LoadGraph("画像/ビーム.png");
	int GoombaThornsPNG = LoadGraph("画像/とげスライム.png");
	int LiftPNG = LoadGraph("画像/リフト.png");
	int CrackLiftPNG = LoadGraph("画像/割れリフト.png");
	int MissilePNG = LoadGraph("画像/ミサイル.png");
	int BeePNG = LoadGraph("画像/ハチ.png");
	int GrassPNG = LoadGraph("画像/草.png");
	int SoilPNG = LoadGraph("画像/土.png");
	int WoodPNG = LoadGraph("画像/木.png");
	int BrickPNG = LoadGraph("画像/レンガ.png");
	int BreakPNG = LoadGraph("画像/叩いた.png");
	int GrassCavePNG = LoadGraph("画像/草Cave.png");
	int SoilCavePNG = LoadGraph("画像/土Cave.png");
	int WoodCavePNG = LoadGraph("画像/木Cave.png");
	int BrickCavePNG = LoadGraph("画像/レンガCave.png");
	int BreakCavePNG = LoadGraph("画像/叩いたCave.png");
	int QuestionPNG = LoadGraph("画像/ハテナ.png");
	int CoinPNG = LoadGraph("画像/コイン.png");
	int PipePNG = LoadGraph("画像/土管.png");
	int SavePNG = LoadGraph("画像/中間.png");
	int GoalPNG = LoadGraph("画像/ゴール.png");
	int SplinterPNG = LoadGraph("画像/トゲトゲ.png");
	int HintBlockPNG = LoadGraph("画像/ヒント.png");
	int HorizontalPipePNG = LoadGraph("画像/横土管.png");
	int PSwitchPNG = LoadGraph("画像/Pスイッチ.png");
	int MushroomScaffoldLeftPNG = LoadGraph("画像/キノコ足場 左.png");
	int MushroomScaffoldUpPNG = LoadGraph("画像/キノコ足場 上.png");
	int MushroomScaffoldRightPNG = LoadGraph("画像/キノコ足場 右.png");
	int MushroomScaffoldDownPNG = LoadGraph("画像/キノコ足場 下.png");
	int SpringPNG = LoadGraph("画像/バネ.png");
	int OnPNG = LoadGraph("画像/Onスイッチ .png");
	int OffPNG = LoadGraph("画像/Offスイッチ.png");
	int OnBlockOnPNG = LoadGraph("画像/ONブロック ON.png");
	int OnBlockOffPNG = LoadGraph("画像/ONブロック OFF.png");
	int OffBlockOnPNG = LoadGraph("画像/OFFブロック ON.png");
	int OffBlockOffPNG = LoadGraph("画像/OFFブロック OFF.png");
	int NeedlePNG = LoadGraph("画像/針.png");
	int NeedleCavePNG = LoadGraph("画像/針Cave.png");
	int jump = LoadSoundMem("効果音/jump.mp3");
	int humi = LoadSoundMem("効果音/humi.mp3");
	int brockbreak = LoadSoundMem("効果音/brockbreak.mp3");
	int brockcoin = LoadSoundMem("効果音/brockcoin.mp3");
	int coin = LoadSoundMem("効果音/coin.mp3");
	int dokan = LoadSoundMem("効果音/dokan.mp3");
	int brockkinoko = LoadSoundMem("効果音/brockkinoko.mp3");
	int kirra = LoadSoundMem("効果音/kirra.mp3");
	int koura = LoadSoundMem("効果音/koura.mp3");
	int goal = LoadSoundMem("効果音/goal.mp3");
	//グラフィック
	int Circle = 0;
	int Cloud = 0;

	//ステージスタート
	if (!DebugModeFlag) while (GetNowCount() - LoadStart < 1500) {
		if (CulturalFestivalFlag) SetMousePoint(640 / 2, 480 / 2);
		PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		if (ProcessMessage() == -1 || (PadInput & PAD_INPUT_9)) {
			free(Map);
			free(MapDataX);
			free(MapDataY);
			free(Goomba);
			free(Sunny);
			free(FallingBrick);
			free(Poison);
			free(Mushroom);
			free(MapDataSave);
			free(Star);
			free(Koopa);
			free(Hole);
			free(Beam);
			for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
			free(MapDataHint);
			free(BigMushroom);
			free(MapDataBack);
			return 0;
		}
		if (CheckHitKey(KEY_INPUT_SPACE)) LoadStart -= 1000.0 / 60.0;
		ScreenFlip();
	}

	if (CulturalFestivalFlag) {
		if (Time == -1) switch (Select) {
		case 0:
			Time = GetNowCount() + 1000 * 60 * 5;
			break;
		case 1:
			Time = GetNowCount() + 1000 * 60 * 4;
			break;
		case 2:
			Time = GetNowCount() + 1000 * 60 * 3;
			break;
		}
	}

	while (ProcessMessage() != -1 && (PadInput & PAD_INPUT_9) == 0) {

		if (CulturalFestivalFlag) SetMousePoint(640 / 2, 480 / 2);
		FrameTime = GetNowCount();

		if (Count == 2) Count = 0;
		Count++;
		if (Circle < 64) Circle++;
		if (Cloud == 0) Cloud = 480;
		if (Count == 1) Cloud--;
		if (GoalCount > 0) GoalCount++;
		if (HintCount > 0) HintCount++;

		// 入力 //----////----////----////----////----////----////----//

		PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		if (CheckHitKey(KEY_INPUT_F1) == 1 && Player.Death == 0) {
			F1Flag = true;
			Player.Death = 1;
		}
		if ((PadInput & PAD_INPUT_LEFT) && Player.Death == 0 && GoalCount == 0) {
			Player.Xspeed -= 1;
			Player.Direction = LEFT;
		}
		if ((PadInput & PAD_INPUT_RIGHT) && Player.Death == 0 && GoalCount == 0) {
			Player.Xspeed += 1;
			Player.Direction = RIGHT;
		}
		if (Player.GroundFlag && ((PadInput & PAD_INPUT_1) || (PadInput & PAD_INPUT_2) || (PadInput & PAD_INPUT_3) || (PadInput & PAD_INPUT_4) || CheckHitKey(KEY_INPUT_UP) && GoalCount == 0) && Player.Death == 0) {
			Player.Yspeed = -17 - abs(Player.Xspeed) / 2.6;
			PlaySoundMem(jump, DX_PLAYTYPE_BACK, TRUE);
		}

		// 処理 //----////----////----////----////----////----////----//

		/*プレイヤー*/ {
			//移動
			if (Player.Xspeed > 0 && (Player.Death > 0 || Count == 1 && GoalCount == 0)) Player.Xspeed--;
			if (Player.Xspeed < 0 && (Player.Death > 0 || Count == 1 && GoalCount == 0)) Player.Xspeed++;
			if (Player.Yspeed < 0 && (PadInput & PAD_INPUT_1) == 0 && (PadInput & PAD_INPUT_2) == 0 && (PadInput & PAD_INPUT_3) == 0 && (PadInput & PAD_INPUT_4) == 0 && CheckHitKey(KEY_INPUT_UP) == 0 && Player.Death == 0 && GoalCount == 0) Player.Yspeed *= 0.9;
			if (Player.Xspeed > 8) Player.Xspeed = 8;
			if (Player.Xspeed < -8) Player.Xspeed = -8;
			if (Player.Yspeed > 63) Player.Yspeed = 63;
			if (Player.Death > 0 && Player.Death <= 3 || Player.Death > 30 || Player.Death == 0 && (GoalCount == 0 || GoalCount > 60)) Player.Y += ++Player.Yspeed;
			if (Player.Death >= 0 && Player.Death <= 3) Player.X += Player.Xspeed;
			//WalkCount
			Player.WalkCount += abs(Player.Xspeed);
			Player.WalkCount %= 48;
			//当たり判定
			if (Player.Death >= 0 && Player.Death <= 3) {
				Player.GroundFlag = false;
				//壁
				if (Player.X < Scroll) {
					Player.X = Scroll;
					Player.Xspeed = 0;
				}
				if (Player.X > Scroll + 640 - 32 && Circle >= 2) {
					Player.X = Scroll + 640 - 32;
					Player.Xspeed = 0;
				}
				if (Player.X > 32 * MapXSize - 32) {
					Player.X = 32 * MapXSize - 32;
					Player.Xspeed = 0;
				}
				if (DebugModeFlag && Player.Y > 480 - 48) {
					Player.Y = 480 - 48;
					Player.GroundFlag = true;
				}
				//プレイヤー・ブロック
				//でかいとこわす
				if (Player.Big) {
					for (int i = 0; i < 15; i++) {
						for (int j = 0; j < MapXSize; j++) {
							if (!(map(i, j) <= 0 || map(i, j) == 6 || map(i, j) == 7 || map(i, j) == 19 || map(i, j) == 22)) {
								if (Player.X + 48 > 32 * j && Player.X - 16 < 32 * j + 32 && Player.Y + 48 > 32 * i && Player.Y - 48 < 32 * i + 32) {
									map(i, j) = 0;
									for (int k = 0; k < 4 * 64; k++) {
										if (Debris[k].Death) {
											Debris[k].Death = 0;
											Debris[k].Y = 32 * i;
											Debris[k].X = 32 * j;
											Debris[k].Yspeed = -12;
											Debris[k].Xspeed = -1;
											break;
										}
									}
									for (int k = 0; k < 4 * 64; k++) {
										if (Debris[k].Death) {
											Debris[k].Death = 0;
											Debris[k].Y = 32 * i;
											Debris[k].X = 32 * j + 16;
											Debris[k].Yspeed = -12;
											Debris[k].Xspeed = 1;
											break;
										}
									}
									for (int k = 0; k < 4 * 64; k++) {
										if (Debris[k].Death) {
											Debris[k].Death = 0;
											Debris[k].Y = 32 * i + 16;
											Debris[k].X = 32 * j;
											Debris[k].Yspeed = -10;
											Debris[k].Xspeed = -1;
											break;
										}
									}
									for (int k = 0; k < 4 * 64; k++) {
										if (Debris[k].Death) {
											Debris[k].Death = 0;
											Debris[k].Y = 32 * i + 16;
											Debris[k].X = 32 * j + 16;
											Debris[k].Yspeed = -10;
											Debris[k].Xspeed = 1;
											break;
										}
									}
									PlaySoundMem(brockbreak, DX_PLAYTYPE_BACK, TRUE);
								}
							}
						}
					}
				}
				else {
					//アクションブロックに印す
					for (int i = 0; i < 15; i++) {
						for (int j = 0; j < MapXSize; j++) {
							//レンガブロック
							if (map(i, j) == 3) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 203;
									}
								}
							}
							//ハテナブロック
							if (map(i, j) == 4) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 204;
									}
								}
							}
							//隠しブロック
							if (map(i, j) == 6) {
								if (Player.Y >= 32 * i + 32 + Player.Yspeed && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 204;
									}
								}
							}
							//スライムブロック
							if (map(i, j) == 10) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 210;
									}
								}
							}
							//毒キノコブロック
							if (map(i, j) == 12) {
								if (Player.Y >= 32 * i + 32 + Player.Yspeed && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 212;
									}
								}
							}
							//まずいキノコブロック
							if (map(i, j) == 13) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 213;
									}
								}
							}
							//10コインレンガブロック
							if (map(i, j) % 1000 == 14) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) += 200;
									}
								}
							}
							//スターレンガブロック
							if (map(i, j) == 16) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 216;
									}
								}
							}
							//隠しトゲ
							if (map(i, j) == 19) {
								if (Player.Y >= 32 * i + 32 + Player.Yspeed && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 219;
									}
								}
							}
							//ハテナトゲ
							if (map(i, j) == 20) {
								if (Player.Y >= 32 * i + 32 + Player.Yspeed && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 219;
									}
								}
							}
							//ヒントブロック
							if (map(i, j) == 26) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 226;
									}
								}
							}
							//キノコブロック
							if (map(i, j) == 27) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 227;
									}
								}
							}
							//Pハテナブロック
							if (map(i, j) == 30) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 230;
									}
								}
							}
							//崩れるレンガブロック
							if (map(i, j) == 32) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed >= 0) {
										map(i, j) = 232;
									}
								}
							}
							//とげスライムハテナブロック
							if (map(i, j) == 33) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 233;
									}
								}
							}
						}
					}
					//ブロックの上下の当たり判定
					for (int i = 0; i < 15; i++) {
						for (int j = 0; j < MapXSize; j++) {
							//ブロック
							if (map(i, j) == 1 || map(i, j) == 2 || map(i, j) == 5 || map(i, j) == 11 || (j >= 1 && map(i, j - 1) == 11) || (j >= 2 && map(i, j - 2) == 11) || map(i, j) == 17 || map(i, j) == 31
								|| map(i, j) == 28 || (j >= 1 && map(i, j - 1) == 28) || (j >= 2 && map(i, j - 2) == 28) || (i >= 1 && map(i - 1, j) == 28) || (i >= 1 && j >= 1 && map(i - 1, j - 1) == 28) || (i >= 1 && j >= 2 && map(i - 1, j - 2) == 28)
								|| map(i, j) == 29 || (j >= 1 && map(i, j - 1) == 29) || (j >= 2 && map(i, j - 2) == 29) || (i >= 1 && map(i - 1, j) == 29) || (i >= 1 && j >= 1 && map(i - 1, j - 1) == 29) || (i >= 1 && j >= 2 && map(i - 1, j - 2) == 29)) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed >= 0) {
										//ブロックに乗っているときの処理
										Player.GroundFlag = true;
										Player.Y = 32 * i - 48;
										if (map(i, j) == 31) {
											PlaySoundFile("効果音/Pswitch.mp3", DX_PLAYTYPE_BACK);
											for (int k = 0; k < 15; k++) for (int l = 0; l < MapXSize; l++) {
												if (map(k, l) > 0) map(k, l) = 7;
											}
										}
									}
									if (Player.Yspeed < 0) {
										//ブロックに頭をぶつけた時の処理
										if (Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
											Player.Yspeed = abs(Player.Yspeed);
											Player.Y = 32 * i + 32;
										}
									}
								}
							}
							//乗るブロック
							if (
								map(i, j) == 3 || map(i, j) == 4 || map(i, j) == 8 || (j >= 1 && map(i, j - 1) == 8) || map(i, j) == 9 || (j >= 1 && map(i, j - 1) == 9) || map(i, j) == 10 || map(i, j) == 13 || map(i, j) % 1000 == 14 || map(i, j) == 16 || map(i, j) == 20 || map(i, j) == 26 || map(i, j) == 27 || map(i, j) == 30 || (
									abs(Player.Xspeed) > 4 && j >= 1 && j < MapXSize - 1 &&																																																																					//走ってるとき
									(map(i, j - 1) == 1 || map(i, j - 1) == 2 || map(i, j - 1) == 3 || map(i, j - 1) == 4 || map(i, j - 1) == 5 || map(i, j - 1) == 10 || map(i, j - 1) == 13 || map(i, j - 1) % 1000 == 14 || map(i, j - 1) == 16 || map(i, j - 1) == 20 || map(i, j - 1) == 26 || map(i, j - 1) == 27 || map(i, j - 1) == 30 || map(i, j - 1) == 31) &&	//一マスの隙間には
									(map(i, j + 1) == 1 || map(i, j + 1) == 2 || map(i, j + 1) == 3 || map(i, j + 1) == 4 || map(i, j + 1) == 5 || map(i, j + 1) == 10 || map(i, j + 1) == 13 || map(i, j + 1) % 1000 == 14 || map(i, j + 1) == 16 || map(i, j + 1) == 20 || map(i, j + 1) == 26 || map(i, j + 1) == 27 || map(i, j + 1) == 30 || map(i, j + 1) == 31)		//落ちないようにする
									|| map(i, j) == 33)
								) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed >= 0) {
										//ブロックに乗っているときの処理
										Player.GroundFlag = true;
										Player.Y = 32 * i - 48;
										//土管に入る
										if (((map(i, j) == 9 && Player.X > 32 * j && Player.X < 32 * j + 32) || (j >= 1 && map(i, j - 1) == 9 && Player.X > 32 * j - 32 && Player.X < 32 * j)) && (PadInput & PAD_INPUT_DOWN)) {
											Player.Death = -48;
											WarpX = MapDataX[j / ChunkSize];
											WarpY = MapDataY[j / ChunkSize];
											if (MapDataSave[j / ChunkSize]) SaveX = WarpX, SaveY = WarpY;
											PlaySoundMem(dokan, DX_PLAYTYPE_BACK, TRUE);
										}
									}
								}
							}
							//レンガブロックを壊された
							if (map(i, j) == 203) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 0;
								for (int k = 0; k < 4 * 64; k++) {
									if (Debris[k].Death) {
										Debris[k].Death = 0;
										Debris[k].Y = 32 * i;
										Debris[k].X = 32 * j;
										Debris[k].Yspeed = -12;
										Debris[k].Xspeed = -1;
										break;
									}
								}
								for (int k = 0; k < 4 * 64; k++) {
									if (Debris[k].Death) {
										Debris[k].Death = 0;
										Debris[k].Y = 32 * i;
										Debris[k].X = 32 * j + 16;
										Debris[k].Yspeed = -12;
										Debris[k].Xspeed = 1;
										break;
									}
								}
								for (int k = 0; k < 4 * 64; k++) {
									if (Debris[k].Death) {
										Debris[k].Death = 0;
										Debris[k].Y = 32 * i + 16;
										Debris[k].X = 32 * j;
										Debris[k].Yspeed = -10;
										Debris[k].Xspeed = -1;
										break;
									}
								}
								for (int k = 0; k < 4 * 64; k++) {
									if (Debris[k].Death) {
										Debris[k].Death = 0;
										Debris[k].Y = 32 * i + 16;
										Debris[k].X = 32 * j + 16;
										Debris[k].Yspeed = -10;
										Debris[k].Xspeed = 1;
										break;
									}
								}
								PlaySoundMem(brockbreak, DX_PLAYTYPE_BACK, TRUE);
							}
							//ハテナブロックが叩かれた
							if (map(i, j) == 204) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								for (int k = 0; k < 16; k++) {
									if (FloatCoin[k].Death == 1) {
										FloatCoin[k].Death = -16;
										FloatCoin[k].Y = 32 * i;
										FloatCoin[k].X = 32 * j;
										break;
									}
								}
								PlaySoundMem(brockcoin, DX_PLAYTYPE_BACK, TRUE);
							}
							//スライムブロックが叩かれた
							if (map(i, j) == 210) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								GoombaNum++;
								Goomba = (Mob *)realloc(Goomba, sizeof(Mob) * GoombaNum);
								Goomba[GoombaNum - 1].X = j * 32;
								Goomba[GoombaNum - 1].Y = i * 32 - 32;
								Goomba[GoombaNum - 1].Xspeed = 2;
								Goomba[GoombaNum - 1].Yspeed = 0;
								Goomba[GoombaNum - 1].GroundFlag = true;
								Goomba[GoombaNum - 1].Death = -16;
								Goomba[GoombaNum - 1].Big = false;
								PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
							}
							//毒キノコブロックが叩かれた
							if (map(i, j) == 212) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								for (int k = 0; k < PoisonNum; k++) {
									if (Poison[k].Death == 1) {
										Poison[k].X = j * 32;
										Poison[k].Y = i * 32 - 32;
										Poison[k].Xspeed = 2;
										Poison[k].Yspeed = 0;
										Poison[k].GroundFlag = true;
										Poison[k].Death = -16;
										break;
									}
								}
								PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
							}
							//まずいキノコブロックが叩かれた
							if (map(i, j) == 213) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								for (int k = 0; k < MushroomNum; k++) {
									if (Mushroom[k].Death == 1) {
										Mushroom[k].X = j * 32;
										Mushroom[k].Y = i * 32 - 32;
										Mushroom[k].Xspeed = 2;
										Mushroom[k].Yspeed = 0;
										Mushroom[k].GroundFlag = true;
										Mushroom[k].Death = -16;
										break;
									}
								}
								PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
							}
							//10コインレンガブロックが叩かれた
							if (map(i, j) >= 214 && map(i, j) % 1000 == 214) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								if (map(i, j) == 9214) map(i, j) = 5;
								else map(i, j) += 800;
								for (int k = 0; k < 16; k++) {
									if (FloatCoin[k].Death == 1) {
										FloatCoin[k].Death = -16;
										FloatCoin[k].Y = 32 * i;
										FloatCoin[k].X = 32 * j;
										break;
									}
								}
								PlaySoundMem(brockcoin, DX_PLAYTYPE_BACK, TRUE);
							}
							//まずいキノコブロックが叩かれた
							if (map(i, j) == 216) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								for (int k = 0; k < StarNum; k++) {
									if (Star[k].Death == 1) {
										Star[k].X = j * 32;
										Star[k].Y = i * 32 - 32;
										Star[k].Xspeed = 2;
										Star[k].Yspeed = 0;
										Star[k].GroundFlag = true;
										Star[k].Death = -16;
										break;
									}
								}
								PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
							}
							//隠しトゲ
							if (map(i, j) == 219) {
								map(i, j) = 18;
								for (int k = 0; k < 16; k++) {
									if (FloatCoin[k].Death == 1) {
										FloatCoin[k].Death = -16;
										FloatCoin[k].Y = 32 * i;
										FloatCoin[k].X = 32 * j;
										break;
									}
								}
								Player.Death = 1;
								PlaySoundMem(brockcoin, DX_PLAYTYPE_BACK, TRUE);
							}
							//ヒントブロック
							if (map(i, j) == 226) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 26;
								PlaySoundFile("効果音/hintBlock.mp3", DX_PLAYTYPE_BACK);
								HintCount = 1;
							}
							//キノコブロックが叩かれた
							if (map(i, j) == 227) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								for (int k = 0; k < BigMushroomNum; k++) {
									if (BigMushroom[k].Death == 1) {
										BigMushroom[k].X = j * 32;
										BigMushroom[k].Y = i * 32 - 32;
										BigMushroom[k].Xspeed = 2;
										BigMushroom[k].Yspeed = 0;
										BigMushroom[k].GroundFlag = true;
										BigMushroom[k].Death = -16;
										break;
									}
								}
								PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
							}
							//Pスイッチハテナブロック
							if (map(i, j) == 230) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								if (i > 0) map(i - 1, j) = 31;
								PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
							}
							//崩れるレンガブロックを壊された
							if (map(i, j) == 232) {
								map(i, j) = 0;
								for (int k = 0; k < 4 * 64; k++) {
									if (Debris[k].Death) {
										Debris[k].Death = 0;
										Debris[k].Y = 32 * i;
										Debris[k].X = 32 * j;
										Debris[k].Yspeed = -12;
										Debris[k].Xspeed = -1;
										break;
									}
								}
								for (int k = 0; k < 4 * 64; k++) {
									if (Debris[k].Death) {
										Debris[k].Death = 0;
										Debris[k].Y = 32 * i;
										Debris[k].X = 32 * j + 16;
										Debris[k].Yspeed = -12;
										Debris[k].Xspeed = 1;
										break;
									}
								}
								for (int k = 0; k < 4 * 64; k++) {
									if (Debris[k].Death) {
										Debris[k].Death = 0;
										Debris[k].Y = 32 * i + 16;
										Debris[k].X = 32 * j;
										Debris[k].Yspeed = -10;
										Debris[k].Xspeed = -1;
										break;
									}
								}
								for (int k = 0; k < 4 * 64; k++) {
									if (Debris[k].Death) {
										Debris[k].Death = 0;
										Debris[k].Y = 32 * i + 16;
										Debris[k].X = 32 * j + 16;
										Debris[k].Yspeed = -10;
										Debris[k].Xspeed = 1;
										break;
									}
								}
								PlaySoundMem(brockbreak, DX_PLAYTYPE_BACK, TRUE);
							}
							//とげスライムブロックが叩かれた
							if (map(i, j) == 233) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								GoombaThornsNum++;
								GoombaThorns = (Mob *)realloc(GoombaThorns, sizeof(Mob) * GoombaThornsNum);
								GoombaThorns[GoombaThornsNum - 1].X = j * 32;
								GoombaThorns[GoombaThornsNum - 1].Y = i * 32 - 32;
								GoombaThorns[GoombaThornsNum - 1].Xspeed = 2;
								GoombaThorns[GoombaThornsNum - 1].Yspeed = 0;
								GoombaThorns[GoombaThornsNum - 1].GroundFlag = true;
								GoombaThorns[GoombaThornsNum - 1].Death = -16;
								GoombaThorns[GoombaThornsNum - 1].Big = false;
								PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
							}
						}
					}
					//ブロックの左右の当たり判定
					//画面上
					for (int j = 0; j < MapXSize; j++) {
						//ブロック
						if (map(0, j) == 1 || map(0, j) == 2 || map(0, j) == 3 || map(0, j) == 4 || map(0, j) == 5 || map(0, j) == 10 || map(0, j) == 13 || map(0, j) % 1000 == 14 || map(0, j) == 16 || map(0, j) == 20 || map(0, j) == 26 || map(0, j) == 30 || map(0, j) == 32
							|| map(0, j) == 27 || map(0, j) == 36) {
							if (Player.Y < 0 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
								if (Player.Xspeed > 0) {
									//ブロックの左に当たった時の処理
									Player.X = 32 * j - 32;
									Player.Xspeed = 0;
								}
								if (Player.Xspeed < 0) {
									//ブロックの右に当たった時の処理
									Player.X = 32 * j + 32;
									Player.Xspeed = 0;
								}
							}
						}
					}
					//画面内
					for (int i = 0; i < 15; i++) {
						for (int j = 0; j < MapXSize; j++) {
							//ブロック
							if (map(i, j) == 1 || map(i, j) == 2 || map(i, j) == 3 || map(i, j) == 4 || map(i, j) == 5 || map(i, j) == 10 || map(i, j) == 11 || (j >= 2 && map(i, j - 2) == 11) || map(i, j) == 13 || map(i, j) % 1000 == 14
								|| map(i, j) == 16 || map(i, j) == 17 || map(i, j) == 20 || map(i, j) == 26 || map(i, j) == 27
								|| map(i, j) == 28 || (j >= 2 && map(i, j - 2) == 28) || (i >= 1 && map(i - 1, j) == 28) || (i >= 1 && j >= 2 && map(i - 1, j - 2) == 28)
								|| map(i, j) == 29 || (j >= 2 && map(i, j - 2) == 29) || (i >= 1 && map(i - 1, j) == 29) || (i >= 1 && j >= 2 && map(i - 1, j - 2) == 29)
								|| map(i, j) == 30 || map(i, j) == 31 || map(i, j) == 32 || map(i, j) == 33 || map(i, j) == 36) {
								if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32) {
									if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + Player.Xspeed) {
										//ブロックの左に当たった時の処理
										Player.X = 32 * j - 32;
										Player.Xspeed = 0;
										//土管に入る
										if (Player.GroundFlag && ((map(i, j) == 29 && Player.Y > 32 * i && Player.Y + 48 <= 32 * i + 64) || (j >= 1 && map(i, j - 1) == 29 && Player.Y > 32 * i - 32 && Player.Y + 48 <= 32 * i + 32))) {
											Player.Death = -96;
											WarpX = MapDataX[j / ChunkSize];
											WarpY = MapDataY[j / ChunkSize];
											if (MapDataSave[j / ChunkSize]) SaveX = WarpX, SaveY = WarpY;
											PlaySoundMem(dokan, DX_PLAYTYPE_BACK, TRUE);
										}
									}
									if (Player.X >= 32 * j + 32 + Player.Xspeed && Player.X < 32 * j + 32) {
										//ブロックの右に当たった時の処理
										Player.X = 32 * j + 32;
										Player.Xspeed = 0;
									}
									//ブロックずらし
									//左
									if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + BlockShift) Player.X = 32 * j - 32;
									//右
									if (Player.X >= 32 * j + 32 - BlockShift && Player.X < 32 * j + 32) Player.X = 32 * j + 32;
								}
							}
							//土管
							if (map(i, j) == 8 || map(i, j) == 9) {
								if (Player.Y + 48 > 32 * i && Player.Y < 640) {
									if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + Player.Xspeed) {
										//左に当たった時の処理
										Player.X = 32 * j - 32;
										Player.Xspeed = 0;
									}
									if (Player.X >= 32 + 32 * j + 32 + Player.Xspeed && Player.X < 32 + 32 * j + 32) {
										//右に当たった時の処理
										Player.X = 32 + 32 * j + 32;
										Player.Xspeed = 0;
									}
								}
							}
						}
					}
				}
			}
			//地面にいたら縦に動かなくする
			if (Player.GroundFlag && Player.Death >= 0 && Player.Death <= 3) Player.Yspeed = 0;
			//最終当たり判定
			for (int i = 0; i < 15; i++) {
				for (int j = 0; j < MapXSize; j++) {
					//コイン
					if (map(i, j) == 7) {
						if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
							map(i, j) = 0;
							PlaySoundMem(coin, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//落ちてくるレンガ
					if (map(i, j) == 11) {
						if (Player.Death == 0 && Player.X > 32 * j && Player.X < 32 * j + 64 && Player.Y > 32 * i && Player.Death == 0) {
							map(i, j) = 0;
							for (int k = 0; k < FallingBrickNum; k++) {
								if (FallingBrick[k].Death == 1) {
									FallingBrick[k].Death = 0;
									FallingBrick[k].X = 32 * j;
									FallingBrick[k].Y = 32 * i;
									FallingBrick[k].Yspeed = 0;
									break;
								}
							}
						}
					}
					//中間
					if (map(i, j) == 15) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i - 32 && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
							map(i, j) = 0;
							SaveX = 32 * j, SaveY = 32 * i - 32;
						}
					}
					//ビーム
					if (map(i, j) == -6) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * (j - 6) && Player.X < 32 * j + 32) {
							for (int k = 0; k < BeamNum; k++) {
								if (Beam[k].Death == 1) {
									Beam[k].X = 32 * j;
									Beam[k].Y = 32 * i;
									Beam[k].Death = 0;
									map(i, j) = 0;
								}
							}
						}
					}
					//ゴール
					if (map(i, j) == 17) {
						if (Player.Death == 0 && GoalCount == 0 && Player.Y + 48 > 32 * (i - 10) + 8 && Player.Y < 32 * i && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							GoalCount = 1;
							Player.X = 32 * j - 16;
							Player.Xspeed = 0;
							Player.Yspeed = 0;
							Player.Direction = RIGHT;
							PlaySoundMem(goal, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//トゲトゲ
					if (map(i, j) == 18) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i + 8 && Player.Y < 32 * i + 32 - 8 && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							Player.Death = 1;
						}
					}
					//木トゲ
					if (map(i, j) == 21) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i + 8 && Player.Y < 32 * i + 32 - 8 && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							Player.Death = 1;
							map(i, j) = 18;
						}
					}
					//超隠しトゲ
					if (map(i, j) == 22) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i + 8 && Player.Y < 32 * i + 32 - 8 && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							Player.Death = 1;
							map(i, j) = 18;
						}
					}
					//土トゲ
					if (map(i, j) == 23) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i + 8 && Player.Y < 32 * i + 32 - 8 && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							Player.Death = 1;
							map(i, j) = 18;
						}
					}
					//キノコ足場
					if (map(i, j) == 34) {
						if (Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
							if (Player.Y - Player.Yspeed + 48 <= 32 * i && Player.Y + 48 > 32 * i) {
								//ブロックに乗っているときの処理
								Player.GroundFlag = true;
								Player.Y = 32 * i - 48;
								Player.Yspeed = 0;
							}
						}
					}
					//バネ
					if (map(i, j) == 35) {
						if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
							Player.Yspeed = -192;
						}
					}
					//針ブロック
					if (map(i, j) == 36) {
						if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
							if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
								if (Player.Death == 0) {
									Player.Y = 32 * i + 24;
									Player.Yspeed = -1;
									Player.Death = 1;
								}
							}
						}
					}
				}
			}
			//天空に行ったら奈良に落とされる
			if (Player.Y < -480) {
				Player.Y = 960;
				Player.Death = 1;
			}
			//ゴール
			if (GoalCount > 0 && GoalCount <= 60) {
				Player.Y += 6;
				for (int i = 0; i < 15; i++) {
					for (int j = 0; j < MapXSize; j++) {
						if (map(i, j) == 17) {
							if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
								Player.Y = 32 * i - 48;
							}
						}
					}
				}
			}
			if (GoalCount == 66) {
				Player.Xspeed = 3;
			}
			if (GoalCount == 135) {
				Player.Xspeed = 0;
			}
			if (GoalCount == 270) {
				SaveX = 64, SaveY = 352;
				free(Map);
				free(MapDataX);
				free(MapDataY);
				free(Goomba);
				free(Sunny);
				free(FallingBrick);
				free(Poison);
				free(Mushroom);
				free(MapDataSave);
				free(Star);
				free(Koopa);
				free(Hole);
				free(Beam);
				for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
				free(MapDataHint);
				free(BigMushroom);
				free(MapDataBack);
				return 1;
			}
			//ヒント
			if (HintCount == 2) {
				HintCount = 0;
				SaveDrawScreen(160, 120, 480, 120 + 30 * 6, "画像/Screeen.bmp");
				for (int i = 1; i <= 30; i++) {
					PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
					if (ProcessMessage() == -1 || (PadInput & PAD_INPUT_9)) {
						free(Map);
						free(MapDataX);
						free(MapDataY);
						free(Goomba);
						free(Sunny);
						free(FallingBrick);
						free(Poison);
						free(Mushroom);
						free(MapDataSave);
						free(Star);
						free(Koopa);
						free(Hole);
						free(Beam);
						for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
						free(MapDataHint);
						free(BigMushroom);
						free(MapDataBack);
						return 0;
					}
					DrawBox(160, 120, 480, 120 + i * 6, GetColor(0, 0, 0), TRUE);
					DrawBox(160, 120, 480, 120 + i * 6, GetColor(255, 255, 255), FALSE);
					SetDrawArea(160 + 32, 120 + 32, 480 - 32, 120 + i * 6 - 32);
					DrawString(160 + 32, 120 + 32, MapDataHint[(Player.X + 16) / 32 / ChunkSize], GetColor(255, 255, 255));
					SetDrawArea(0, 0, 640, 480);
					ScreenFlip();
				}
				for (int i = 0; i < 6 * (int)strlen(MapDataHint[(Player.X + 16) / 32 / ChunkSize]); i++) {
					PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
					if (ProcessMessage() == -1 || (PadInput & PAD_INPUT_9)) {
						free(Map);
						free(MapDataX);
						free(MapDataY);
						free(Goomba);
						free(Sunny);
						free(FallingBrick);
						free(Poison);
						free(Mushroom);
						free(MapDataSave);
						free(Star);
						free(Koopa);
						free(Hole);
						free(Beam);
						for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
						free(MapDataHint);
						free(BigMushroom);
						free(MapDataBack);
						return 0;
					}
					if ((PadInput & PAD_INPUT_10) || CheckHitKey(KEY_INPUT_RETURN)) break;
					ScreenFlip();
				}
				for (int i = 0; i < 30; i++) {
					PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
					if (ProcessMessage() == -1 || (PadInput & PAD_INPUT_9)) {
						free(Map);
						free(MapDataX);
						free(MapDataY);
						free(Goomba);
						free(Sunny);
						free(FallingBrick);
						free(Poison);
						free(Mushroom);
						free(MapDataSave);
						free(Star);
						free(Koopa);
						free(Hole);
						free(Beam);
						for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
						free(MapDataHint);
						free(BigMushroom);
						free(MapDataBack);
						return 0;
					}
					LoadGraphScreen(160, 120, "画像/Screeen.bmp", FALSE);
					DrawBox(160, 120 + i * 6, 480, 120 + 30 * 6, GetColor(0, 0, 0), TRUE);
					DrawBox(160, 120 + i * 6, 480, 120 + 30 * 6, GetColor(255, 255, 255), FALSE);
					SetDrawArea(160 + 32, 120 + i * 6 + 32, 480 - 32, 120 + 30 * 6 - 32);
					DrawString(160 + 32, 120 + 32, MapDataHint[(Player.X + 16) / 32 / ChunkSize], GetColor(255, 255, 255));
					SetDrawArea(0, 0, 640, 480);
					ScreenFlip();
				}
			}
			//スクロール
			if (Player.X - Scroll > 320 - 32 && Player.Y < 540 && !(DebugModeFlag && (GetMouseInput() & MOUSE_INPUT_LEFT))) {
				Scroll += Player.X - Scroll - (320 - 32);
				if (Scroll + 640 > 32 * MapXSize) Scroll = 32 * MapXSize - 640;
			}
			//背景の境目でスクロールを止める。
			while (MapDataBack[(Player.X + 16) / 32 / ChunkSize] != MapDataBack[(Scroll + 640) / 32 / ChunkSize]) {
				Scroll--;
			}
			while (MapDataBack[(Player.X + 16) / 32 / ChunkSize] != MapDataBack[(Scroll) / 32 / ChunkSize]) {
				Scroll++;
			}
			if (Scroll < 0) Scroll = 0;
			//奈落死
			if (Player.Y > 960 && Player.Death == 0) Player.Death = 1;
			//デバック死
			if (CheckHitKey(KEY_INPUT_O) && Player.Death == 0) Player.Death = 1;
			//死んだ
			if (Player.Death == 1) {
				//MessageBox(NULL, "<= to be Continue ///", "つづく", NULL);
				GoalCount = 0;
				StopSoundMem(goal);
				PlaySoundFile("効果音/death.mp3", DX_PLAYTYPE_BACK);
			}
			if (Player.Death == 3) Player.Yspeed = -17;
			if (Player.Death == 111 || (DebugModeFlag && Player.Death == 26)) {
				Life--;
				free(Map);
				free(MapDataX);
				free(MapDataY);
				free(Goomba);
				free(Sunny);
				free(FallingBrick);
				free(Poison);
				free(Mushroom);
				free(MapDataSave);
				free(Star);
				free(Koopa);
				free(Hole);
				free(Beam);
				for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
				free(MapDataHint);
				free(BigMushroom);
				free(MapDataBack);
				if (F1Flag) return 1;
				else return 2;
			}
			//土管プルップルップルッ
			if (Player.Death == -1 || Player.Death == -49) {
				Player.X = WarpX;
				Player.Y = WarpY;
				Player.Xspeed = 0;
				Player.Yspeed = 0;
				Player.Direction = RIGHT;
				Player.WalkCount = 0;
				Scroll = Player.X - (320 - 32);
				//エンティティのリセット
				for (int i = 0; i < GoombaNum; i++) {
					int GoombaCount = 0;
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == -1) GoombaCount++;
						if (GoombaCount > i) {
							Goomba[i].X = 32 * k;
							Goomba[i].Y = 32 * j;
							goto Goomba_loop_end2;
						}
					}
				Goomba_loop_end2:
					Goomba[i].Xspeed = -2;
					Goomba[i].Yspeed = 0;
					Goomba[i].Death = 0;
				}
				for (int i = 0; i < SunnyNum; i++) {
					int SunnyCount = 0;
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == -2 || map(j, k) == -3) SunnyCount++;
						if (SunnyCount > i) {
							if (map(j, k) == -2) {
								Sunny[i].X = 32 * k + 16;
								Sunny[i].Yspeed = -12;
							}
							else {
								Sunny[i].X = 32 * k;
								Sunny[i].Yspeed = 12;
							}
							Sunny[i].Y = 32 * j;
							goto Sunny_loop_end2;
						}
					} Sunny_loop_end2:
					Sunny[i].Xspeed = 0;
					Sunny[i].Death = -1;
				}
				for (int i = 0; i < KoopaNum; i++) {
					int KoopaCount = 0;
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == -4) KoopaCount++;
						if (KoopaCount > i) {
							Koopa[i].X = 32 * k;
							Koopa[i].Y = 32 * j;
							goto Koopa_loop_end2;
						}
					} Koopa_loop_end2:;
					Koopa[i].Xspeed = -2;
					Koopa[i].Yspeed = 0;
					Koopa[i].Death = 0;
				}
				for (int i = 0; i < 4 * 64; i++) Debris[i].Death = 1;
				for (int i = 0; i < 16; i++) FloatCoin[i].Death = 1;
				//サークル
				Circle = 0;
			}
			if (Player.Death) Player.Death++;
			if (Player.Death == -48) Player.Death = 0;
		}
		HumiFlag = false;
		/*スライム*/	for (int i = 0; i < GoombaNum; i++) {
			if (Goomba[i].Death == 0 && Goomba[i].X > Scroll - 320 && Goomba[i].X < Scroll + 640 + 32 * 4) {
				if (Goomba[i].Yspeed > 64) Goomba[i].Yspeed = 64;
				Goomba[i].X += Goomba[i].Xspeed;
				Goomba[i].Y += ++Goomba[i].Yspeed;
				//スライム・ブロック
				Goomba[i].GroundFlag = false;
				//でかいと壊す
				if (Goomba[i].Big) {
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							if (!(map(j, k) <= 0 || map(j, k) == 6 || map(j, k) == 7 || map(j, k) == 19 || map(j, k) == 22)) {
								if (Goomba[i].X + 48 > 32 * k && Goomba[i].X - 16 < 32 * k + 32 && Goomba[i].Y + 32 > 32 * j && Goomba[i].Y - 32 < 32 * j + 32) {
									map(j, k) = 0;
									for (int l = 0; l < 4 * 64; l++) {
										if (Debris[l].Death) {
											Debris[l].Death = 0;
											Debris[l].Y = 32 * j;
											Debris[l].X = 32 * k;
											Debris[l].Yspeed = -12;
											Debris[l].Xspeed = -1;
											break;
										}
									}
									for (int l = 0; l < 4 * 64; l++) {
										if (Debris[l].Death) {
											Debris[l].Death = 0;
											Debris[l].Y = 32 * j;
											Debris[l].X = 32 * k + 16;
											Debris[l].Yspeed = -12;
											Debris[l].Xspeed = 1;
											break;
										}
									}
									for (int l = 0; l < 4 * 64; l++) {
										if (Debris[l].Death) {
											Debris[l].Death = 0;
											Debris[l].Y = 32 * j + 16;
											Debris[l].X = 32 * k;
											Debris[l].Yspeed = -10;
											Debris[l].Xspeed = -1;
											break;
										}
									}
									for (int l = 0; l < 4 * 64; l++) {
										if (Debris[l].Death) {
											Debris[l].Death = 0;
											Debris[l].Y = 32 * j + 16;
											Debris[l].X = 32 * k + 16;
											Debris[l].Yspeed = -10;
											Debris[l].Xspeed = 1;
											break;
										}
									}
									PlaySoundMem(brockbreak, DX_PLAYTYPE_BACK, TRUE);
								}
							}
						}
					}
				}
				else {
					//ブロックの上下の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Goomba[i].Y + 32 >= 32 * j && Goomba[i].Y < 32 * j + 32 && Goomba[i].X + 32 > 32 * k + Goomba[i].Xspeed && Goomba[i].X < 32 * k + 32 + Goomba[i].Xspeed) {
									if (Goomba[i].Yspeed >= 0) {
										//ブロックに乗っているときの処理
										Goomba[i].GroundFlag = true;
										Goomba[i].Y = 32 * j - 32;
									}
									if (Goomba[i].Yspeed < 0) {
										//ブロックに頭をぶつけた時の処理
										if (Goomba[i].X + 32 > 32 * k + BlockShift && Goomba[i].X < 32 * k + 32 - BlockShift) {
											Goomba[i].Yspeed = abs(Goomba[i].Yspeed);
											Goomba[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//ブロックの左右の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Goomba[i].Y + 32 > 32 * j && Goomba[i].Y < 32 * j + 32) {
									if (Goomba[i].X + 32 > 32 * k && Goomba[i].X + 32 <= 32 * k + Goomba[i].Xspeed) {
										//ブロックの左に当たった時の処理
										Goomba[i].X = 32 * k - 32;
										if (Goomba[i].GroundFlag) Goomba[i].Xspeed = -2;
									}
									if (Goomba[i].X >= 32 * k + 32 + Goomba[i].Xspeed && Goomba[i].X < 32 * k + 32) {
										//ブロックの右に当たった時の処理
										Goomba[i].X = 32 * k + 32;
										if (Goomba[i].GroundFlag) Goomba[i].Xspeed = 2;
									}
									//ブロックずらし
									//左
									if (Goomba[i].X + 32 > 32 * k && Goomba[i].X + 32 <= 32 * k + BlockShift) Goomba[i].X = 32 * k - 32;
									//右
									if (Goomba[i].X >= 32 * k + 32 - BlockShift && Goomba[i].X < 32 * k + 32) Goomba[i].X = 32 * k + 32;
								}
							}
							//土管
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (Goomba[i].Y + 32 > 32 * j && Goomba[i].Y < 640) {
									if (Goomba[i].X + 32 > 32 * k && Goomba[i].X + 32 <= 32 * k + Goomba[i].Xspeed) {
										//左に当たった時の処理
										Goomba[i].X = 32 * k - 32;
										if (Goomba[i].GroundFlag) Goomba[i].Xspeed = -2;
									}
									if (Goomba[i].X >= 32 + 32 * k + 32 + Goomba[i].Xspeed && Goomba[i].X < 32 + 32 * k + 32) {
										//右に当たった時の処理
										Goomba[i].X = 32 + 32 * k + 32;
										if (Goomba[i].GroundFlag) Goomba[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//キノコ足場
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (Goomba[i].X + 32 > 32 * k && Goomba[i].X < 32 * k + 32) {
								if (Goomba[i].Y - Goomba[i].Yspeed + 32 <= 32 * j && Goomba[i].Y + 32 > 32 * j) {
									//ブロックに乗っているときの処理
									Goomba[i].GroundFlag = true;
									Goomba[i].Y = 32 * j - 32;
								}
							}
						}
					}
				}
				//地面にいたら縦に動かなくする
				if (Goomba[i].GroundFlag && Goomba[i].Death == 0) Goomba[i].Yspeed = 0;
				//スライム・プレイヤー
				if (Goomba[i].Big) {
					if (Player.X + 32 > Goomba[i].X - 16 && Player.X < Goomba[i].X + 48 && Player.Y + 48 > Goomba[i].Y - 32 && Player.Y < Goomba[i].Y + 32) Player.Death = 1;
				}
				else {
					//左右
					if (Player.Death == 0) {
						if (Player.Y + 32 > Goomba[i].Y && Player.Y < Goomba[i].Y + 32) {
							if (Player.X + 32 >= Goomba[i].X && Player.X + 32 + Goomba[i].Xspeed <= Goomba[i].X + Player.Xspeed) {
								//スライムの左に当たった時の処理
								Player.Death = 1;
							}
							if (Player.X + Goomba[i].Xspeed >= Goomba[i].X + 32 + Player.Xspeed && Player.X <= Goomba[i].X + 32) {
								//スライムの右に当たった時の処理
								Player.Death = 1;
							}
						}
					}
					//上下
					if (Player.Death == 0) {
						if (Player.Y + 48 > Goomba[i].Y && Player.Y < Goomba[i].Y + 32 && Player.X + 32 + Goomba[i].Xspeed > Goomba[i].X + Player.Xspeed + Goomba[i].Xspeed && Player.X < Goomba[i].X + 32 + Player.Xspeed) {
							if (Player.Y + 24 <= Goomba[i].Y + 32 && !HumiFlag) {
								//スライムに乗っているときの処理
								Player.Yspeed = -17 - abs(Player.Xspeed) / 4;
								Player.Y = Goomba[i].Y - 48;
								Goomba[i].Death = 1;
								PlaySoundMem(humi, DX_PLAYTYPE_BACK, TRUE);
								HumiFlag = true;
							}
							if (Player.Y + 24 > Goomba[i].Y + 32) {
								//スライムに頭をぶつけた時の処理
								Player.Death = 1;
							}
						}
					}
				}
			}
			//つぶれる
			if (Goomba[i].Death > 0 && Goomba[i].Death <= 16) Goomba[i].Death++;
			//出てくる
			if (Goomba[i].Death < 0) Goomba[i].Death++;
		}
		/*テルテル*/	for (int i = 0; i < SunnyNum; i++) {
			if (Sunny[i].Death == 0) {
				if (Player.Death == 0 && Player.X + 32 > Sunny[i].X && Player.X < Sunny[i].X + 32 && Player.Y + 48 > Sunny[i].Y && Player.Y < Sunny[i].Y + 48) Player.Death = 1;
				Sunny[i].Y += Sunny[i].Yspeed;
				if (Sunny[i].Y < -64 && Sunny[i].Y > 480 + 64) Sunny[i].Death = 1;
			}
			if (Sunny[i].Death == -1) {
				if (Sunny[i].Yspeed < 0 && Player.X > Sunny[i].X - 32 - 10 && Player.X < Sunny[i].X + 32 + 10) {
					Sunny[i].Death = 0;
					PlaySoundMem(kirra, DX_PLAYTYPE_BACK, TRUE);
				}
				if (Sunny[i].Yspeed > 0 && Player.X > Sunny[i].X - 96 && Player.X < Sunny[i].X + 96) {
					Sunny[i].Death = 0;
					PlaySoundMem(kirra, DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}
		/*毒キノコ*/	for (int i = 0; i < PoisonNum; i++) {
			if (Poison[i].X > Scroll - 32 && Poison[i].X < Scroll + 640 + 32 * 4 && Poison[i].Y < 480) {
				if (Poison[i].Death == 0) {
					if (Poison[i].Yspeed > 64) Poison[i].Yspeed = 64;
					Poison[i].X += Poison[i].Xspeed;
					Poison[i].Y += ++Poison[i].Yspeed;
					//スライム・ブロック
					Poison[i].GroundFlag = false;
					//ブロックの上下の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Poison[i].Y + 32 >= 32 * j && Poison[i].Y < 32 * j + 32 && Poison[i].X + 32 > 32 * k + Poison[i].Xspeed && Poison[i].X < 32 * k + 32 + Poison[i].Xspeed) {
									if (Poison[i].Yspeed >= 0) {
										//ブロックに乗っているときの処理
										Poison[i].GroundFlag = true;
										Poison[i].Y = 32 * j - 32;
									}
									if (Poison[i].Yspeed < 0) {
										//ブロックに頭をぶつけた時の処理
										if (Poison[i].X + 32 > 32 * k + BlockShift && Poison[i].X < 32 * k + 32 - BlockShift) {
											Poison[i].Yspeed = abs(Poison[i].Yspeed);
											Poison[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//ブロックの左右の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
								|| map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Poison[i].Y + 32 > 32 * j && Poison[i].Y < 32 * j + 32) {
									if (Poison[i].X + 32 > 32 * k && Poison[i].X + 32 <= 32 * k + Poison[i].Xspeed) {
										//ブロックの左に当たった時の処理
										Poison[i].X = 32 * k - 32;
										if (Poison[i].GroundFlag) Poison[i].Xspeed = -2;
									}
									if (Poison[i].X >= 32 * k + 32 + Poison[i].Xspeed && Poison[i].X < 32 * k + 32) {
										//ブロックの右に当たった時の処理
										Poison[i].X = 32 * k + 32;
										if (Poison[i].GroundFlag) Poison[i].Xspeed = 2;
									}
									//ブロックずらし
									//左
									if (Poison[i].X + 32 > 32 * k && Poison[i].X + 32 <= 32 * k + BlockShift) Poison[i].X = 32 * k - 32;
									//右
									if (Poison[i].X >= 32 * k + 32 - BlockShift && Poison[i].X < 32 * k + 32) Poison[i].X = 32 * k + 32;
								}
							}
							//土管
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (Poison[i].Y + 32 > 32 * j && Poison[i].Y < 640) {
									if (Poison[i].X + 32 > 32 * k && Poison[i].X + 32 <= 32 * k + Poison[i].Xspeed) {
										//左に当たった時の処理
										Poison[i].X = 32 * k - 32;
										if (Poison[i].GroundFlag) Poison[i].Xspeed = -2;
									}
									if (Poison[i].X >= 32 + 32 * k + 32 + Poison[i].Xspeed && Poison[i].X < 32 + 32 * k + 32) {
										//右に当たった時の処理
										Poison[i].X = 32 + 32 * k + 32;
										if (Poison[i].GroundFlag) Poison[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//キノコ足場
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (Poison[i].X + 32 > 32 * k && Poison[i].X < 32 * k + 32) {
								if (Poison[i].Y - Poison[i].Yspeed + 32 <= 32 * j && Poison[i].Y + 32 > 32 * j) {
									//ブロックに乗っているときの処理
									Poison[i].GroundFlag = true;
									Poison[i].Y = 32 * j - 32;
								}
							}
						}
					}
					//地面にいたら縦に動かなくする
					if (Poison[i].GroundFlag && Poison[i].Death == 0) Poison[i].Yspeed = 0;
					//食べると死ぬ
					if (Player.X + 32 > Poison[i].X && Player.X < Poison[i].X + 32 && Player.Y + 48 > Poison[i].Y && Player.Y < Poison[i].Y + 32) {
						if (Player.Death == 0) Player.Death = 1;
						Poison[i].Death = 1;
					}
				}
				if (Poison[i].Death < 0) {
					Poison[i].Death++;
					if (Poison[i].Death == 0) {
						for (int j = 0; j < PoisonNum; j++) {
							if (Poison[j].Death == 1) {
								Poison[j].X = Poison[i].X;
								Poison[j].Y = Poison[i].Y;
								Poison[j].Xspeed = 2;
								Poison[j].Yspeed = 0;
								Poison[j].GroundFlag = true;
								Poison[j].Death = -16;
								break;
							}
							if (j == PoisonNum - 1) {
								int Random = GetRand(PoisonNum - 1);
								Poison[Random].X = Poison[i].X;
								Poison[Random].Y = Poison[i].Y;
								Poison[Random].Xspeed = 2;
								Poison[Random].Yspeed = 0;
								Poison[Random].GroundFlag = true;
								Poison[Random].Death = -16;
							}
						}
						PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
					}
				}
			}
			else Poison[i].Death = 1;
		}
		/*まずいキノコ*/	for (int i = 0; i < MushroomNum; i++) {
			if (Mushroom[i].X > Scroll - 32 && Mushroom[i].X < Scroll + 640 + 32 * 4 && Mushroom[i].Y < 480) {
				if (Mushroom[i].Death == 0) {
					if (Mushroom[i].Yspeed > 64) Mushroom[i].Yspeed = 64;
					Mushroom[i].X += Mushroom[i].Xspeed;
					Mushroom[i].Y += ++Mushroom[i].Yspeed;
					//スライム・ブロック
					Mushroom[i].GroundFlag = false;
					//ブロックの上下の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Mushroom[i].Y + 32 >= 32 * j && Mushroom[i].Y < 32 * j + 32 && Mushroom[i].X + 32 > 32 * k + Mushroom[i].Xspeed && Mushroom[i].X < 32 * k + 32 + Mushroom[i].Xspeed) {
									if (Mushroom[i].Yspeed >= 0) {
										//ブロックに乗っているときの処理
										Mushroom[i].GroundFlag = true;
										Mushroom[i].Y = 32 * j - 32;
									}
									if (Mushroom[i].Yspeed < 0) {
										//ブロックに頭をぶつけた時の処理
										if (Mushroom[i].X + 32 > 32 * k + BlockShift && Mushroom[i].X < 32 * k + 32 - BlockShift) {
											Mushroom[i].Yspeed = abs(Mushroom[i].Yspeed);
											Mushroom[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//ブロックの左右の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
								|| map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Mushroom[i].Y + 32 > 32 * j && Mushroom[i].Y < 32 * j + 32) {
									if (Mushroom[i].X + 32 > 32 * k && Mushroom[i].X + 32 <= 32 * k + Mushroom[i].Xspeed) {
										//ブロックの左に当たった時の処理
										Mushroom[i].X = 32 * k - 32;
										if (Mushroom[i].GroundFlag) Mushroom[i].Xspeed = -2;
									}
									if (Mushroom[i].X >= 32 * k + 32 + Mushroom[i].Xspeed && Mushroom[i].X < 32 * k + 32) {
										//ブロックの右に当たった時の処理
										Mushroom[i].X = 32 * k + 32;
										if (Mushroom[i].GroundFlag) Mushroom[i].Xspeed = 2;
									}
									//ブロックずらし
									//左
									if (Mushroom[i].X + 32 > 32 * k && Mushroom[i].X + 32 <= 32 * k + BlockShift) Mushroom[i].X = 32 * k - 32;
									//右
									if (Mushroom[i].X >= 32 * k + 32 - BlockShift && Mushroom[i].X < 32 * k + 32) Mushroom[i].X = 32 * k + 32;
								}
							}
							//土管
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (Mushroom[i].Y + 32 > 32 * j && Mushroom[i].Y < 640) {
									if (Mushroom[i].X + 32 > 32 * k && Mushroom[i].X + 32 <= 32 * k + Mushroom[i].Xspeed) {
										//左に当たった時の処理
										Mushroom[i].X = 32 * k - 32;
										if (Mushroom[i].GroundFlag) Mushroom[i].Xspeed = -2;
									}
									if (Mushroom[i].X >= 32 + 32 * k + 32 + Mushroom[i].Xspeed && Mushroom[i].X < 32 + 32 * k + 32) {
										//右に当たった時の処理
										Mushroom[i].X = 32 + 32 * k + 32;
										if (Mushroom[i].GroundFlag) Mushroom[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//キノコ足場
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (Mushroom[i].X + 32 > 32 * k && Mushroom[i].X < 32 * k + 32) {
								if (Mushroom[i].Y - Mushroom[i].Yspeed + 32 <= 32 * j && Mushroom[i].Y + 32 > 32 * j) {
									//ブロックに乗っているときの処理
									Mushroom[i].GroundFlag = true;
									Mushroom[i].Y = 32 * j - 32;
								}
							}
						}
					}
					//地面にいたら縦に動かなくする
					if (Mushroom[i].GroundFlag && Mushroom[i].Death == 0) Mushroom[i].Yspeed = 0;
					//食べると死ぬ
					if (Player.X + 32 > Mushroom[i].X && Player.X < Mushroom[i].X + 32 && Player.Y + 48 > Mushroom[i].Y && Player.Y < Mushroom[i].Y + 32) {
						if (Player.Death == 0) Player.Death = 1;
						Mushroom[i].Death = 1;
					}
				}
				if (Mushroom[i].Death < 0) {
					Mushroom[i].Death++;
				}
			}
			else Mushroom[i].Death = 1;
		}
		/*キノコ*/	for (int i = 0; i < BigMushroomNum; i++) {
			if (BigMushroom[i].X > Scroll - 32 && BigMushroom[i].X < Scroll + 640 + 32 * 4 && BigMushroom[i].Y < 480) {
				if (BigMushroom[i].Death == 0) {
					if (BigMushroom[i].Yspeed > 64) BigMushroom[i].Yspeed = 64;
					BigMushroom[i].X += BigMushroom[i].Xspeed;
					BigMushroom[i].Y += ++BigMushroom[i].Yspeed;
					//スライム・ブロック
					BigMushroom[i].GroundFlag = false;
					//ブロックの上下の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (BigMushroom[i].Y + 32 >= 32 * j && BigMushroom[i].Y < 32 * j + 32 && BigMushroom[i].X + 32 > 32 * k + BigMushroom[i].Xspeed && BigMushroom[i].X < 32 * k + 32 + BigMushroom[i].Xspeed) {
									if (BigMushroom[i].Yspeed >= 0) {
										//ブロックに乗っているときの処理
										BigMushroom[i].GroundFlag = true;
										BigMushroom[i].Y = 32 * j - 32;
									}
									if (BigMushroom[i].Yspeed < 0) {
										//ブロックに頭をぶつけた時の処理
										if (BigMushroom[i].X + 32 > 32 * k + BlockShift && BigMushroom[i].X < 32 * k + 32 - BlockShift) {
											BigMushroom[i].Yspeed = abs(BigMushroom[i].Yspeed);
											BigMushroom[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//ブロックの左右の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
								|| map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (BigMushroom[i].Y + 32 > 32 * j && BigMushroom[i].Y < 32 * j + 32) {
									if (BigMushroom[i].X + 32 > 32 * k && BigMushroom[i].X + 32 <= 32 * k + BigMushroom[i].Xspeed) {
										//ブロックの左に当たった時の処理
										BigMushroom[i].X = 32 * k - 32;
										if (BigMushroom[i].GroundFlag) BigMushroom[i].Xspeed = -2;
									}
									if (BigMushroom[i].X >= 32 * k + 32 + BigMushroom[i].Xspeed && BigMushroom[i].X < 32 * k + 32) {
										//ブロックの右に当たった時の処理
										BigMushroom[i].X = 32 * k + 32;
										if (BigMushroom[i].GroundFlag) BigMushroom[i].Xspeed = 2;
									}
									//ブロックずらし
									//左
									if (BigMushroom[i].X + 32 > 32 * k && BigMushroom[i].X + 32 <= 32 * k + BlockShift) BigMushroom[i].X = 32 * k - 32;
									//右
									if (BigMushroom[i].X >= 32 * k + 32 - BlockShift && BigMushroom[i].X < 32 * k + 32) BigMushroom[i].X = 32 * k + 32;
								}
							}
							//土管
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (BigMushroom[i].Y + 32 > 32 * j && BigMushroom[i].Y < 640) {
									if (BigMushroom[i].X + 32 > 32 * k && BigMushroom[i].X + 32 <= 32 * k + BigMushroom[i].Xspeed) {
										//左に当たった時の処理
										BigMushroom[i].X = 32 * k - 32;
										if (BigMushroom[i].GroundFlag) BigMushroom[i].Xspeed = -2;
									}
									if (BigMushroom[i].X >= 32 + 32 * k + 32 + BigMushroom[i].Xspeed && BigMushroom[i].X < 32 + 32 * k + 32) {
										//右に当たった時の処理
										BigMushroom[i].X = 32 + 32 * k + 32;
										if (BigMushroom[i].GroundFlag) BigMushroom[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//キノコ足場
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (BigMushroom[i].X + 32 > 32 * k && BigMushroom[i].X < 32 * k + 32) {
								if (BigMushroom[i].Y - BigMushroom[i].Yspeed + 32 <= 32 * j && BigMushroom[i].Y + 32 > 32 * j) {
									//ブロックに乗っているときの処理
									BigMushroom[i].GroundFlag = true;
									BigMushroom[i].Y = 32 * j - 32;
								}
							}
						}
					}
					//地面にいたら縦に動かなくする
					if (BigMushroom[i].GroundFlag && BigMushroom[i].Death == 0) BigMushroom[i].Yspeed = 0;
					//食べるとでかくなる
					//プレイヤー
					if (Player.X + 32 > BigMushroom[i].X && Player.X < BigMushroom[i].X + 32 && Player.Y + 48 > BigMushroom[i].Y && Player.Y < BigMushroom[i].Y + 32) {
						Player.Big = true;
						BigMushroom[i].Death = 1;
					}
					//スライム
					for (int j = 0; j < GoombaNum; j++) {
						if (Goomba[j].Death == 0 && Goomba[j].X + 32 > BigMushroom[i].X && Goomba[j].X < BigMushroom[i].X + 32 && Goomba[j].Y + 32 > BigMushroom[i].Y && Goomba[j].Y < BigMushroom[i].Y + 32) {
							Goomba[j].Big = true;
							BigMushroom[i].Death = 1;
						}
					}
					//とげスライム
					for (int j = 0; j < GoombaThornsNum; j++) {
						if (GoombaThorns[j].Death == 0 && GoombaThorns[j].X + 32 > BigMushroom[i].X && GoombaThorns[j].X < BigMushroom[i].X + 32 && GoombaThorns[j].Y + 32 > BigMushroom[i].Y && GoombaThorns[j].Y < BigMushroom[i].Y + 32) {
							GoombaThorns[j].Big = true;
							BigMushroom[i].Death = 1;
						}
					}
				}
				if (BigMushroom[i].Death < 0) {
					BigMushroom[i].Death++;
				}
			}
			else BigMushroom[i].Death = 1;
		}
		/*スター*/	for (int i = 0; i < StarNum; i++) {
			if (Star[i].X > Scroll - 32 && Star[i].X < Scroll + 640 + 32 * 4 && Star[i].Y < 480) {
				if (Star[i].Death == 0) {
					if (Star[i].Yspeed > 64) Star[i].Yspeed = 64;
					Star[i].X += Star[i].Xspeed;
					Star[i].Y += ++Star[i].Yspeed;
					//スライム・ブロック
					Star[i].GroundFlag = false;
					//ブロックの上下の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Star[i].Y + 32 >= 32 * j && Star[i].Y < 32 * j + 32 && Star[i].X + 32 > 32 * k + Star[i].Xspeed && Star[i].X < 32 * k + 32 + Star[i].Xspeed) {
									if (Star[i].Yspeed >= 0) {
										//ブロックに乗っているときの処理
										Star[i].GroundFlag = true;
										Star[i].Y = 32 * j - 32;
									}
									if (Star[i].Yspeed < 0) {
										//ブロックに頭をぶつけた時の処理
										if (Star[i].X + 32 > 32 * k + BlockShift && Star[i].X < 32 * k + 32 - BlockShift) {
											Star[i].Yspeed = abs(Star[i].Yspeed);
											Star[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//ブロックの左右の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
								|| map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Star[i].Y + 32 > 32 * j && Star[i].Y < 32 * j + 32) {
									if (Star[i].X + 32 > 32 * k && Star[i].X + 32 <= 32 * k + Star[i].Xspeed) {
										//ブロックの左に当たった時の処理
										Star[i].X = 32 * k - 32;
										Star[i].Xspeed = -2;
									}
									if (Star[i].X >= 32 * k + 32 + Star[i].Xspeed && Star[i].X < 32 * k + 32) {
										//ブロックの右に当たった時の処理
										Star[i].X = 32 * k + 32;
										Star[i].Xspeed = 2;
									}
									//ブロックずらし
									//左
									if (Star[i].X + 32 > 32 * k && Star[i].X + 32 <= 32 * k + BlockShift) Star[i].X = 32 * k - 32;
									//右
									if (Star[i].X >= 32 * k + 32 - BlockShift && Star[i].X < 32 * k + 32) Star[i].X = 32 * k + 32;
								}
							}
							//土管
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (Star[i].Y + 32 > 32 * j && Star[i].Y < 640) {
									if (Star[i].X + 32 > 32 * k && Star[i].X + 32 <= 32 * k + Star[i].Xspeed) {
										//左に当たった時の処理
										Star[i].X = 32 * k - 32;
										Star[i].Xspeed = -2;
									}
									if (Star[i].X >= 32 + 32 * k + 32 + Star[i].Xspeed && Star[i].X < 32 + 32 * k + 32) {
										//右に当たった時の処理
										Star[i].X = 32 + 32 * k + 32;
										Star[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//キノコ足場
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (Star[i].X + 32 > 32 * k && Star[i].X < 32 * k + 32) {
								if (Star[i].Y - Star[i].Yspeed + 32 <= 32 * j && Star[i].Y + 32 > 32 * j) {
									//ブロックに乗っているときの処理
									Star[i].GroundFlag = true;
									Star[i].Y = 32 * j - 32;
								}
							}
						}
					}
					//地面にいたらジャンプ
					if (Star[i].GroundFlag && Star[i].Death == 0) Star[i].Yspeed = -15;
					//食べると死ぬ
					if (Player.X + 32 > Star[i].X && Player.X < Star[i].X + 32 && Player.Y + 48 > Star[i].Y && Player.Y < Star[i].Y + 32) {
						if (Player.Death == 0) Player.Death = 1;
						Star[i].Death = 1;
					}
				}
				if (Star[i].Death < 0) {
					Star[i].Death++;
				}
			}
			else Star[i].Death = 1;
		}
		/*亀*/	for (int i = 0; i < KoopaNum; i++) {
			if (Koopa[i].Death >= 0 && Koopa[i].Death <= 1 && Koopa[i].X > Scroll - 320 && Koopa[i].X < Scroll + 640 + 32 * 4 && Koopa[i].Y < 480) {
				if (Koopa[i].Yspeed > 64) Koopa[i].Yspeed = 64;
				Koopa[i].X += Koopa[i].Xspeed;
				Koopa[i].Y += ++Koopa[i].Yspeed;
				//亀・ブロック
				Koopa[i].GroundFlag = false;
				//ブロックの上下の当たり判定
				for (int j = 0; j < 15; j++) {
					for (int k = 0; k < MapXSize; k++) {
						//ブロック
						if (
							map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
							|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
							|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
							|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
							|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
							|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
							if (Koopa[i].Y + 32 >= 32 * j && Koopa[i].Y < 32 * j + 32 && Koopa[i].X + 32 > 32 * k + Koopa[i].Xspeed && Koopa[i].X < 32 * k + 32 + Koopa[i].Xspeed) {
								if (Koopa[i].Yspeed >= 0) {
									//ブロックに乗っているときの処理
									Koopa[i].GroundFlag = true;
									Koopa[i].Y = 32 * j - 32;
								}
								if (Koopa[i].Yspeed < 0) {
									//ブロックに頭をぶつけた時の処理
									if (Koopa[i].X + 32 > 32 * k + BlockShift && Koopa[i].X < 32 * k + 32 - BlockShift) {
										Koopa[i].Yspeed = abs(Koopa[i].Yspeed);
										Koopa[i].Y = 32 * j + 32;
									}
								}
							}
						}
					}
				}
				//ブロックの左右の当たり判定
				for (int j = 0; j < 15; j++) {
					for (int k = 0; k < MapXSize; k++) {
						//ブロック
						if (
							map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 6 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
							|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 16 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
							|| map(j, k) == 27
							|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j, k) == 28) || (j >= 1 && k >= 2 && map(j, k - 2) == 28)
							|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j, k) == 29) || (j >= 1 && k >= 2 && map(j, k - 2) == 29)
							|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
							if (Koopa[i].Y + 32 > 32 * j && Koopa[i].Y < 32 * j + 32) {
								if (Koopa[i].X + 32 > 32 * k && Koopa[i].X + 32 <= 32 * k + Koopa[i].Xspeed) {
									//ブロックの左に当たった時の処理
									Koopa[i].X = 32 * k - 32;
									Koopa[i].Xspeed = -Koopa[i].Xspeed;
									//特定のブロックの壁にあたったらぽん！
									if (Koopa[i].Death == 1) {
										if (map(j, k) == 3) map(j, k) = 203;
										if (map(j, k) == 4) map(j, k) = 204;
										if (map(j, k) == 6) map(j, k) = 206;
										if (map(j, k) == 10) map(j, k) = 210;
										if (map(j, k) == 12) map(j, k) = 212;
										if (map(j, k) == 13) map(j, k) = 213;
										if (map(j, k) % 1000 == 14) map(j, k) += 200;
										if (map(j, k) == 16) map(j, k) = 216;
									}
								}
								if (Koopa[i].X >= 32 * k + 32 + Koopa[i].Xspeed && Koopa[i].X < 32 * k + 32) {
									//ブロックの右に当たった時の処理
									Koopa[i].X = 32 * k + 32;
									Koopa[i].Xspeed = -Koopa[i].Xspeed;
									//特定のブロックの壁にあたったらぽん！
									if (Koopa[i].Death == 1) {
										if (map(j, k) == 3) map(j, k) = 203;
										if (map(j, k) == 4) map(j, k) = 204;
										if (map(j, k) == 6) map(j, k) = 206;
										if (map(j, k) == 10) map(j, k) = 210;
										if (map(j, k) == 12) map(j, k) = 212;
										if (map(j, k) == 13) map(j, k) = 213;
										if (map(j, k) % 1000 == 14) map(j, k) += 200;
										if (map(j, k) == 16) map(j, k) = 216;
									}
								}
								//ブロックずらし
								//左
								if (Koopa[i].X + 32 > 32 * k && Koopa[i].X + 32 <= 32 * k + BlockShift) Koopa[i].X = 32 * k - 32;
								//右
								if (Koopa[i].X >= 32 * k + 32 - BlockShift && Koopa[i].X < 32 * k + 32) Koopa[i].X = 32 * k + 32;
							}
						}
						//土管
						if (map(j, k) == 8 || map(j, k) == 9) {
							if (Koopa[i].Y + 32 > 32 * j && Koopa[i].Y < 640) {
								if (Koopa[i].X + 32 > 32 * k && Koopa[i].X + 32 <= 32 * k + Koopa[i].Xspeed) {
									//左に当たった時の処理
									Koopa[i].X = 32 * k - 32;
									if (Koopa[i].GroundFlag) Koopa[i].Xspeed = -Koopa[i].Xspeed;
								}
								if (Koopa[i].X >= 32 + 32 * k + 32 + Koopa[i].Xspeed && Koopa[i].X < 32 + 32 * k + 32) {
									//右に当たった時の処理
									Koopa[i].X = 32 + 32 * k + 32;
									if (Koopa[i].GroundFlag) Koopa[i].Xspeed = -Koopa[i].Xspeed;
								}
							}
						}
						//レンガブロックを壊された
						if (map(j, k) == 203) {
							map(j, k) = 0;
							for (int l = 0; l < 4 * 64; l++) {
								if (Debris[l].Death) {
									Debris[l].Death = 0;
									Debris[l].Y = 32 * j;
									Debris[l].X = 32 * k;
									Debris[l].Yspeed = -12;
									Debris[l].Xspeed = -1;
									break;
								}
							}
							for (int l = 0; l < 4 * 64; l++) {
								if (Debris[l].Death) {
									Debris[l].Death = 0;
									Debris[l].Y = 32 * j;
									Debris[l].X = 32 * k + 16;
									Debris[l].Yspeed = -12;
									Debris[l].Xspeed = 1;
									break;
								}
							}
							for (int l = 0; l < 4 * 64; l++) {
								if (Debris[l].Death) {
									Debris[l].Death = 0;
									Debris[l].Y = 32 * j + 16;
									Debris[l].X = 32 * k;
									Debris[l].Yspeed = -10;
									Debris[l].Xspeed = -1;
									break;
								}
							}
							for (int l = 0; l < 4 * 64; l++) {
								if (Debris[l].Death) {
									Debris[l].Death = 0;
									Debris[l].Y = 32 * j + 16;
									Debris[l].X = 32 * k + 16;
									Debris[l].Yspeed = -10;
									Debris[l].Xspeed = 1;
									break;
								}
							}
							PlaySoundMem(brockbreak, DX_PLAYTYPE_BACK, TRUE);
						}
						//ハテナブロックが叩かれた
						if (map(j, k) == 204) {
							map(j, k) = 5;
							for (int l = 0; l < 16; l++) {
								if (FloatCoin[l].Death == 1) {
									FloatCoin[l].Death = -16;
									FloatCoin[l].Y = 32 * j;
									FloatCoin[l].X = 32 * k;
									break;
								}
							}
							PlaySoundMem(brockcoin, DX_PLAYTYPE_BACK, TRUE);
						}
						//スライムブロックが叩かれた
						if (map(j, k) == 210) {
							map(j, k) = 5;
							GoombaNum++;
							Goomba = (Mob *)realloc(Goomba, sizeof(Mob) * GoombaNum);
							Goomba[GoombaNum - 1].X = k * 32;
							Goomba[GoombaNum - 1].Y = j * 32 - 32;
							Goomba[GoombaNum - 1].Xspeed = 2;
							Goomba[GoombaNum - 1].Yspeed = 0;
							Goomba[GoombaNum - 1].GroundFlag = true;
							Goomba[GoombaNum - 1].Death = -16;
							PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
						}
						//毒キノコブロックが叩かれた
						if (map(j, k) == 212) {
							map(j, k) = 5;
							for (int l = 0; l < PoisonNum; l++) {
								if (Poison[l].Death == 1) {
									Poison[l].X = k * 32;
									Poison[l].Y = j * 32 - 32;
									Poison[l].Xspeed = 2;
									Poison[l].Yspeed = 0;
									Poison[l].GroundFlag = true;
									Poison[l].Death = -16;
									break;
								}
							}
							PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
						}
						//まずいキノコブロックが叩かれた
						if (map(j, k) == 213) {
							map(j, k) = 5;
							for (int l = 0; l < MushroomNum; l++) {
								if (Mushroom[l].Death == 1) {
									Mushroom[l].X = k * 32;
									Mushroom[l].Y = j * 32 - 32;
									Mushroom[l].Xspeed = 2;
									Mushroom[l].Yspeed = 0;
									Mushroom[l].GroundFlag = true;
									Mushroom[l].Death = -16;
									break;
								}
							}
							PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
						}
						//10コインレンガブロックが叩かれた
						if (map(j, k) >= 214 && map(j, k) % 1000 == 214) {
							if (map(j, k) == 9214) map(j, k) = 5;
							else map(j, k) += 800;
							for (int l = 0; l < 16; l++) {
								if (FloatCoin[l].Death == 1) {
									FloatCoin[l].Death = -16;
									FloatCoin[l].Y = 32 * j;
									FloatCoin[l].X = 32 * k;
									break;
								}
							}
							PlaySoundMem(brockcoin, DX_PLAYTYPE_BACK, TRUE);
						}
						//まずいキノコブロックが叩かれた
						if (map(j, k) == 216) {
							map(j, k) = 5;
							for (int l = 0; l < StarNum; l++) {
								if (Star[l].Death == 1) {
									Star[l].X = k * 32;
									Star[l].Y = j * 32 - 32;
									Star[l].Xspeed = 2;
									Star[l].Yspeed = 0;
									Star[l].GroundFlag = true;
									Star[l].Death = -16;
									break;
								}
							}
							PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
						}
					}
				}
				//キノコ足場
				for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
					if (map(j, k) == 34) {
						if (Koopa[i].X + 32 > 32 * k && Koopa[i].X < 32 * k + 32) {
							if (Koopa[i].Y - Koopa[i].Yspeed + 32 <= 32 * j && Koopa[i].Y + 32 > 32 * j) {
								//ブロックに乗っているときの処理
								Koopa[i].GroundFlag = true;
								Koopa[i].Y = 32 * j - 32;
							}
						}
					}
				}
				//地面にいたら縦に動かなくする
				if (Koopa[i].GroundFlag && Koopa[i].Death >= 0 && Koopa[i].Death <= 1) Koopa[i].Yspeed = 0;
				//亀・プレイヤー
				//左右
				if (Player.Death == 0) {
					if (Player.Y + 32 > Koopa[i].Y && Player.Y < Koopa[i].Y + 32) {
						if (Player.X + 32 >= Koopa[i].X && Player.X + 32 + Koopa[i].Xspeed <= Koopa[i].X + Player.Xspeed) {
							//亀の左に当たった時の処理
							if (Koopa[i].Death == 0) Player.Death = 1;
							else if (Koopa[i].Xspeed == 0) {
								Koopa[i].Xspeed = 8;
								PlaySoundMem(koura, DX_PLAYTYPE_BACK);
							}
							else Player.Death = 1;
						}
						if (Player.X + Koopa[i].Xspeed >= Koopa[i].X + 32 + Player.Xspeed && Player.X <= Koopa[i].X + 32) {
							//亀の右に当たった時の処理
							if (Koopa[i].Death == 0) Player.Death = 1;
							else if (Koopa[i].Xspeed == 0) {
								Koopa[i].Xspeed = -8;
								PlaySoundMem(koura, DX_PLAYTYPE_BACK);
							}
							else Player.Death = 1;
						}
					}
				}
				//上下
				if (Player.Death == 0) {
					if (Player.Y + 48 > Koopa[i].Y && Player.Y < Koopa[i].Y + 32 && Player.X + 32 + Koopa[i].Xspeed > Koopa[i].X + Player.Xspeed + Koopa[i].Xspeed && Player.X < Koopa[i].X + 32 + Player.Xspeed) {
						if (Player.Y + 24 <= Koopa[i].Y + 32 && !HumiFlag) {
							//亀に乗っているときの処理
							Player.Yspeed = -17 - abs(Player.Xspeed) / 4;
							Player.Y = Koopa[i].Y - 48;
							if (Koopa[i].Death == 0) {
								Koopa[i].Death = 1;
								Koopa[i].Xspeed = 0;
								PlaySoundMem(humi, DX_PLAYTYPE_BACK, TRUE);
							}
							else if (Koopa[i].Xspeed == 0) {
								if (Player.X <= Koopa[i].X) Koopa[i].Xspeed = 8;
								else Koopa[i].Xspeed = -8;
								PlaySoundMem(koura, DX_PLAYTYPE_BACK);
							}
							else {
								Koopa[i].Xspeed = 0;
								PlaySoundMem(humi, DX_PLAYTYPE_BACK, TRUE);
							}
							HumiFlag = true;
						}
						if (Player.Y + 24 > Koopa[i].Y + 32) {
							//亀に頭をぶつけた時の処理
							Player.Death = 1;
						}
					}
				}
				if (abs(Koopa[i].Xspeed) > 5) {
					//亀・スライム
					for (int j = 0; j < GoombaNum; j++) {
						if (Goomba[j].Death == 0 && Koopa[i].X < Goomba[j].X + 32 && Koopa[i].X + 32 > Goomba[j].X && Koopa[i].Y < Goomba[j].Y + 32 && Koopa[i].Y + 32 > Goomba[j].Y) {
							Goomba[j].Death = 17;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//亀・テルテル
					for (int j = 0; j < SunnyNum; j++) {
						if (Sunny[j].Death == 0 && Koopa[i].X < Sunny[j].X + 32 && Koopa[i].X + 32 > Sunny[j].X && Koopa[i].Y < Sunny[j].Y + 48 && Koopa[i].Y + 32 > Sunny[j].Y) {
							Sunny[j].Death = 1;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//亀・毒キノコ
					for (int j = 0; j < PoisonNum; j++) {
						if (Poison[j].Death == 0 && Koopa[i].X < Poison[j].X + 32 && Koopa[i].X + 32 > Poison[j].X && Koopa[i].Y < Poison[j].Y + 48 && Koopa[i].Y + 32 > Poison[j].Y) {
							Poison[j].Death = 1;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//亀・まずいキノコ
					for (int j = 0; j < MushroomNum; j++) {
						if (Mushroom[j].Death == 0 && Koopa[i].X < Mushroom[j].X + 32 && Koopa[i].X + 32 > Mushroom[j].X && Koopa[i].Y < Mushroom[j].Y + 48 && Koopa[i].Y + 32 > Mushroom[j].Y) {
							Mushroom[j].Death = 1;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//亀・スター
					for (int j = 0; j < StarNum; j++) {
						if (Star[j].Death == 0 && Koopa[i].X < Star[j].X + 32 && Koopa[i].X + 32 > Star[j].X && Koopa[i].Y < Star[j].Y + 48 && Koopa[i].Y + 32 > Star[j].Y) {
							Star[j].Death = 1;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//亀・亀
					for (int j = 0; j < KoopaNum; j++) {
						if (j != i && Koopa[j].Death == 0) {
							if (Koopa[i].X < Koopa[j].X + 32 && Koopa[i].X + 32 > Koopa[j].X && Koopa[i].Y < Koopa[j].Y + 48 && Koopa[i].Y + 32 > Koopa[j].Y) {
								Koopa[j].Death = 1;
								PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
							}
						}
					}
				}
			}
			//出てくる
			if (Koopa[i].Death < 0) Koopa[i].Death++;
		}
		/*落ちるレンガ*/	for (int i = 0; i < FallingBrickNum; i++) {
			if (FallingBrick[i].Death == 0) {
				if (Player.Death == 0 && Player.X + 32 > FallingBrick[i].X && Player.X < FallingBrick[i].X + 96 && Player.Y + 48 > FallingBrick[i].Y && Player.Y < FallingBrick[i].Y + 32) {
					Player.Death = 1;
					FallingBrick[i].Death = -1;
				}
				FallingBrick[i].Y += FallingBrick[i].Yspeed++;
				if (FallingBrick[i].Y > 480 + 64) FallingBrick[i].Death = 1;
			}
		}
		/*落とし穴*/ {
			//落ちる
			for (int i = 0; i < HoleNum; i++) {
				if (Hole[i].Yspeed > 0) {
					Hole[i].Y += Hole[i].Yspeed++;
				}
			}
			//プレイヤーとの当たり判定
			for (int i = 0; i < HoleNum; i++) {
				if (Hole[i].Yspeed == 0) {
					if (Player.Y + 48 > Hole[i].Y && Player.Y < Hole[i].Y + 32 && Player.X + 32 > Hole[i].X && Player.X < Hole[i].X + 32) {
						Player.Y = Hole[i].Y - 48;
						Player.GroundFlag = true;
						Hole[i].Yspeed = 1;
					}
				}
				else {
					if (Player.Y + 48 >= Hole[i].Y && Player.Y < Hole[i].Y + 32 && Player.X + 32 > Hole[i].X && Player.X < Hole[i].X + 32) {
						Player.Y = Hole[i].Y - 48;
						Player.GroundFlag = true;
					}
				}
			}
			//連鎖
			for (int i = 0; i < HoleNum; i++) {				//くり返し調べる
				for (int j = 0; j < HoleNum; j++) {			//全部のホールを調べる
					if (Hole[j].Yspeed == 1) {
						for (int k = 0; k < HoleNum; k++) {	//周りのやつ
							if (Hole[k].Yspeed == 0) {
								//上
								if (Hole[k].X == Hole[j].X && Hole[k].Y - 32 == Hole[j].Y) {
									Hole[k].Yspeed = 1;
								}
								//下
								if (Hole[k].X == Hole[j].X && Hole[k].Y + 32 == Hole[j].Y) {
									Hole[k].Yspeed = 1;
								}
								//左
								if (Hole[k].X - 32 == Hole[j].X && Hole[k].Y == Hole[j].Y) {
									Hole[k].Yspeed = 1;
								}
								//右
								if (Hole[k].X + 32 == Hole[j].X && Hole[k].Y == Hole[j].Y) {
									Hole[k].Yspeed = 1;
								}
							}
						}
					}
				}
			}
		}
		/*ビーム*/	for (int i = 0; i < BeamNum; i++) {
			if (Beam[i].Death == 0) {
				Beam[i].X -= 32;
				if (Player.X + 32 > Beam[i].X && Player.X < Beam[i].X + 128 && Player.Y + 48 > Beam[i].Y + 8 && Player.Y < Beam[i].Y + 24) {
					Player.Death = 1;
				}
				if (Beam[i].X < Scroll - 256) Beam[i].Death = 1;
			}
		}
		/*とげスライム*/	for (int i = 0; i < GoombaThornsNum; i++) {
			if (GoombaThorns[i].Death == 0 && GoombaThorns[i].X > Scroll - 320 && GoombaThorns[i].X < Scroll + 640 + 32 * 4) {
				if (GoombaThorns[i].Yspeed > 64) GoombaThorns[i].Yspeed = 64;
				GoombaThorns[i].X += GoombaThorns[i].Xspeed;
				GoombaThorns[i].Y += ++GoombaThorns[i].Yspeed;
				//とげスライム・ブロック
				GoombaThorns[i].GroundFlag = false;
				//でかいと壊す
				if (GoombaThorns[i].Big) {
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							if (!(map(j, k) <= 0 || map(j, k) == 6 || map(j, k) == 7 || map(j, k) == 19 || map(j, k) == 22)) {
								if (GoombaThorns[i].X + 48 > 32 * k && GoombaThorns[i].X - 16 < 32 * k + 32 && GoombaThorns[i].Y + 32 > 32 * j && GoombaThorns[i].Y - 32 < 32 * j + 32) {
									map(j, k) = 0;
									for (int l = 0; l < 4 * 64; l++) {
										if (Debris[l].Death) {
											Debris[l].Death = 0;
											Debris[l].Y = 32 * j;
											Debris[l].X = 32 * k;
											Debris[l].Yspeed = -12;
											Debris[l].Xspeed = -1;
											break;
										}
									}
									for (int l = 0; l < 4 * 64; l++) {
										if (Debris[l].Death) {
											Debris[l].Death = 0;
											Debris[l].Y = 32 * j;
											Debris[l].X = 32 * k + 16;
											Debris[l].Yspeed = -12;
											Debris[l].Xspeed = 1;
											break;
										}
									}
									for (int l = 0; l < 4 * 64; l++) {
										if (Debris[l].Death) {
											Debris[l].Death = 0;
											Debris[l].Y = 32 * j + 16;
											Debris[l].X = 32 * k;
											Debris[l].Yspeed = -10;
											Debris[l].Xspeed = -1;
											break;
										}
									}
									for (int l = 0; l < 4 * 64; l++) {
										if (Debris[l].Death) {
											Debris[l].Death = 0;
											Debris[l].Y = 32 * j + 16;
											Debris[l].X = 32 * k + 16;
											Debris[l].Yspeed = -10;
											Debris[l].Xspeed = 1;
											break;
										}
									}
									PlaySoundMem(brockbreak, DX_PLAYTYPE_BACK, TRUE);
								}
							}
						}
					}
				}
				else {
					//ブロックの上下の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (GoombaThorns[i].Y + 32 >= 32 * j && GoombaThorns[i].Y < 32 * j + 32 && GoombaThorns[i].X + 32 > 32 * k + GoombaThorns[i].Xspeed && GoombaThorns[i].X < 32 * k + 32 + GoombaThorns[i].Xspeed) {
									if (GoombaThorns[i].Yspeed >= 0) {
										//ブロックに乗っているときの処理
										GoombaThorns[i].GroundFlag = true;
										GoombaThorns[i].Y = 32 * j - 32;
									}
									if (GoombaThorns[i].Yspeed < 0) {
										//ブロックに頭をぶつけた時の処理
										if (GoombaThorns[i].X + 32 > 32 * k + BlockShift && GoombaThorns[i].X < 32 * k + 32 - BlockShift) {
											GoombaThorns[i].Yspeed = abs(GoombaThorns[i].Yspeed);
											GoombaThorns[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//ブロックの左右の当たり判定
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//ブロック
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (GoombaThorns[i].Y + 32 > 32 * j && GoombaThorns[i].Y < 32 * j + 32) {
									if (GoombaThorns[i].X + 32 > 32 * k && GoombaThorns[i].X + 32 <= 32 * k + GoombaThorns[i].Xspeed) {
										//ブロックの左に当たった時の処理
										GoombaThorns[i].X = 32 * k - 32;
										if (GoombaThorns[i].GroundFlag) GoombaThorns[i].Xspeed = -2;
									}
									if (GoombaThorns[i].X >= 32 * k + 32 + GoombaThorns[i].Xspeed && GoombaThorns[i].X < 32 * k + 32) {
										//ブロックの右に当たった時の処理
										GoombaThorns[i].X = 32 * k + 32;
										if (GoombaThorns[i].GroundFlag) GoombaThorns[i].Xspeed = 2;
									}
									//ブロックずらし
									//左
									if (GoombaThorns[i].X + 32 > 32 * k && GoombaThorns[i].X + 32 <= 32 * k + BlockShift) GoombaThorns[i].X = 32 * k - 32;
									//右
									if (GoombaThorns[i].X >= 32 * k + 32 - BlockShift && GoombaThorns[i].X < 32 * k + 32) GoombaThorns[i].X = 32 * k + 32;
								}
							}
							//土管
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (GoombaThorns[i].Y + 32 > 32 * j && GoombaThorns[i].Y < 640) {
									if (GoombaThorns[i].X + 32 > 32 * k && GoombaThorns[i].X + 32 <= 32 * k + GoombaThorns[i].Xspeed) {
										//左に当たった時の処理
										GoombaThorns[i].X = 32 * k - 32;
										if (GoombaThorns[i].GroundFlag) GoombaThorns[i].Xspeed = -2;
									}
									if (GoombaThorns[i].X >= 32 + 32 * k + 32 + GoombaThorns[i].Xspeed && GoombaThorns[i].X < 32 + 32 * k + 32) {
										//右に当たった時の処理
										GoombaThorns[i].X = 32 + 32 * k + 32;
										if (GoombaThorns[i].GroundFlag) GoombaThorns[i].Xspeed = 2;
									}
								}
							}
						}
					}
				}
				//キノコ足場
				for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
					if (map(j, k) == 34) {
						if (GoombaThorns[i].X + 32 > 32 * k && GoombaThorns[i].X < 32 * k + 32) {
							if (GoombaThorns[i].Y - GoombaThorns[i].Yspeed + 32 <= 32 * j && GoombaThorns[i].Y + 32 > 32 * j) {
								//ブロックに乗っているときの処理
								GoombaThorns[i].GroundFlag = true;
								GoombaThorns[i].Y = 32 * j - 32;
							}
						}
					}
				}
				//地面にいたら縦に動かなくする
				if (GoombaThorns[i].GroundFlag && GoombaThorns[i].Death == 0) GoombaThorns[i].Yspeed = 0;
				//とげスライム・プレイヤー
				if (GoombaThorns[i].Big) {
					if (Player.X + 32 > GoombaThorns[i].X - 16 && Player.X < GoombaThorns[i].X + 48 && Player.Y + 48 > GoombaThorns[i].Y - 32 && Player.Y < GoombaThorns[i].Y + 32) Player.Death = 1;
				}
				else {
					if (Player.Death == 0) {
						if (Player.Y + 48 > GoombaThorns[i].Y && Player.Y < GoombaThorns[i].Y + 32 && Player.X + 32 + GoombaThorns[i].Xspeed > GoombaThorns[i].X + Player.Xspeed + GoombaThorns[i].Xspeed && Player.X < GoombaThorns[i].X + 32 + Player.Xspeed) {
							Player.Death = 1;
						}
					}
				}
			}
			//出てくる
			if (GoombaThorns[i].Death < 0) GoombaThorns[i].Death++;
		}
		/*リフト*/	for (int i = 0; i < LiftNum; i++) {
			if (!(Lift[i].Death == -2 && Lift[i].Yspeed != 0)) {
				Lift[i].Y += Lift[i].Yspeed;
				if (Lift[i].Death == -1 && Lift[i].Yspeed > 0) {
					++Lift[i].Yspeed;
					if (Lift[i].Yspeed >= 8) Lift[i].Yspeed = 7;
				}
				if (Lift[i].Death == 0 && Lift[i].Yspeed < 0 && Lift[i].Y < -8) Lift[i].Y = 640;
				if (Lift[i].Death == 0 && Lift[i].Yspeed > 0 && Lift[i].Y > 640) Lift[i].Y = -8;
				//当たり判定
				if (Player.X + 32 > Lift[i].X && Player.X < Lift[i].X + 32 * 4) {
					if (Player.Y - Player.Yspeed + 48 <= Lift[i].Y - Lift[i].Yspeed) {	//さっき上にいて
						if (Player.Y + 48 >= Lift[i].Y) {								//今下にいる
							if (Lift[i].Death != -2) {
								Player.Y = Lift[i].Y - 48;
								Player.GroundFlag = true;
								Player.Yspeed = Lift[i].Yspeed;
								if (Lift[i].Death == -1 && Lift[i].Yspeed == 0) Lift[i].Yspeed = 1;
							}
							else if (Lift[i].Yspeed == 0) {
								Lift[i].Yspeed = 1;
								Player.Yspeed = -11;
							}
						}
						//if (Lift[i].Death == -1 && Lift[i].Yspeed != 0) {
						//	if (Player.Y + 48 >= Lift[i].Y - 128) Player.GroundFlag = true;
						//}
					}
				}
			}
			else if (Lift[i].Yspeed <= 32) Lift[i].Yspeed++;
		}
		/*ブロックの破片*/	for (int i = 0; i < 4 * 64; i++) {
			if (Debris[i].Death == 0) {
				if (Debris[i].Yspeed > 64) Debris[i].Yspeed = 64;
				Debris[i].X += Debris[i].Xspeed;
				Debris[i].Y += ++Debris[i].Yspeed;
				if (Debris[i].Y > 480) Debris[i].Death = 1;
			}
		}
		/*コインチャリーン*/	for (int i = 0; i < 16; i++) {
			if (FloatCoin[i].Death <= 0) {
				FloatCoin[i].Y -= 6;
				FloatCoin[i].Death++;
			}
		}
		//デバッグワープ
		if (DebugModeFlag && (GetMouseInput() & MOUSE_INPUT_LEFT)) {
			GetMousePoint(&Player.X, &Player.Y);
			Player.X -= 16;
			Player.Y -= 24;
			Player.X += Scroll;
			Player.Yspeed = 0;
		}
		//デバッグキル
		if (DebugModeFlag && CheckHitKey(KEY_INPUT_K)) {
			for (int i = 0; i < GoombaNum; i++) {
				if (Goomba[i].Death == 0 && Goomba[i].X > Scroll - 320 && Goomba[i].X < Scroll + 640 + 32 * 4 && Goomba[i].Y < 480) Goomba[i].Death = 1;
			}
			for (int i = 0; i < SunnyNum; i++) {
				if (Sunny[i].Death == 0 && Sunny[i].X > Scroll - 320 && Sunny[i].X < Scroll + 640 + 32 * 4) Sunny[i].Death = 1;
			}
			for (int i = 0; i < PoisonNum; i++) {
				if (Poison[i].Death == 0 && Poison[i].X > Scroll - 320 && Poison[i].X < Scroll + 640 + 32 * 4) Poison[i].Death = 1;
			}
			for (int i = 0; i < MushroomNum; i++) {
				if (Mushroom[i].Death == 0 && Mushroom[i].X > Scroll - 320 && Mushroom[i].X < Scroll + 640 + 32 * 4) Mushroom[i].Death = 1;
			}
			for (int i = 0; i < StarNum; i++) {
				if (Star[i].Death == 0 && Star[i].X > Scroll - 320 && Star[i].X < Scroll + 640 + 32 * 4) Star[i].Death = 1;
			}
			for (int i = 0; i < KoopaNum; i++) {
				if (Koopa[i].Death >= 0 && Koopa[i].Death <= 1 && Koopa[i].X > Scroll - 320 && Koopa[i].X < Scroll + 640 + 32 * 4) Koopa[i].Death = 2;
			}
		}

		int CalculationTime = GetNowCount() - FrameTime;

		// 描画 //----////----////----////----////----////----////----//

		ClearDrawScreen();
		//土管のグラデ
		if (Player.Death < -48) SetDrawBright(-(Player.Death + 48) * 8 - 64, -(Player.Death + 48) * 8 - 64, -(Player.Death + 48) * 8 - 64);
		else if (Player.Death < 0) SetDrawBright(-Player.Death * 8 - 64, -Player.Death * 8 - 64, -Player.Death * 8 - 64);
		//サークル１
		if (Circle < 64) SetDrawArea(Player.X - Scroll + 16 - Circle * 32, Player.Y + 24 - Circle * 32, Player.X - Scroll + 16 + Circle * 32, Player.Y + 24 + Circle * 32);
		//背景
		switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
		case 0:
			for (int i = 0; i < 480; i++) DrawLine(0, i, 640, i, GetColor(0 + (44 - 0) * i / 480, 110 + (143 - 110) * i / 480, 187 + (208 - 187) * i / 480));
			DrawGraph(Cloud + 480, 0, CloudPNG, TRUE);
			DrawGraph(Cloud, 0, CloudPNG, TRUE);
			DrawGraph(Cloud - 480, 0, CloudPNG, TRUE);
			DrawGraph(Cloud - 480 * 2, 0, CloudPNG, TRUE);
			break;
		case 1:
			DrawGraph(0, 0, CavePNG, FALSE);
			break;
		case 2:

			break;
		}
		//キノコ足場
		for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
			if (map(i, j) == 34) {
				if (j == 0 || map(i, j - 1) != 34) DrawGraph(32 * j - Scroll, 32 * i, MushroomScaffoldLeftPNG, TRUE);
				else if (j == MapXSize - 1 || map(i, j + 1) != 34) DrawGraph(32 * j - Scroll, 32 * i, MushroomScaffoldRightPNG, TRUE);
				else {
					DrawGraph(32 * j - Scroll, 32 * i, MushroomScaffoldUpPNG, TRUE);
					for (int k = i + 1; k < 15; k++) DrawGraph(32 * j - Scroll, 32 * k, MushroomScaffoldDownPNG, TRUE);
				}
			}
		}
		//ゴール
		for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
			if (map(i, j) == 17) DrawGraph(32 * j - Scroll, 32 * (i - 10), GoalPNG, TRUE);
		}
		//プレイヤー
		if (GoalCount <= 135) {
			if (Player.Big) DrawGraph(Player.X - 16 - Scroll, Player.Y - 48, BigPlayerPNG, TRUE);
			else if (Player.Death > 3) DrawGraph(Player.X + 1 - Player.Direction - Scroll, Player.Y, PlayerOwataPNG, TRUE);
			else if (Player.Death < -48) {
				DrawGraph(Player.X + (Player.Death + 48) * 2 + 96 - Scroll, Player.Y, PlayerJumpPNG, TRUE);
			}
			else if (Player.Death < 0) {
				if (Player.Direction == RIGHT) DrawGraph(Player.X - Scroll, Player.Y + Player.Death * 2 + 96, PlayerJumpPNG, TRUE);
				else DrawTurnGraph(Player.X - Scroll, Player.Y + Player.Death * 2 + 96, PlayerJumpPNG, TRUE);
			}
			else if (Player.GroundFlag == false) {
				if (Player.Direction == RIGHT) DrawGraph(Player.X - Scroll, Player.Y, PlayerJumpPNG, TRUE);
				else DrawTurnGraph(Player.X - Scroll, Player.Y, PlayerJumpPNG, TRUE);
			}
			else {
				if (Player.Direction == RIGHT) {
					if (Player.WalkCount / 24) DrawGraph(Player.X - Scroll, Player.Y, Player1PNG, TRUE);
					else DrawGraph(Player.X - Scroll, Player.Y, Player2PNG, TRUE);
				}
				else {
					if (Player.WalkCount / 24) DrawTurnGraph(Player.X - Scroll, Player.Y, Player1PNG, TRUE);
					else DrawTurnGraph(Player.X - Scroll, Player.Y, Player2PNG, TRUE);
				}
			}
		}
		//スライム
		for (int i = 0; i < GoombaNum; i++) {
			if (Goomba[i].Big) {
				if (Goomba[i].Xspeed <= 0) DrawRotaGraph(Goomba[i].X + 16 - Scroll, Goomba[i].Y, 2.0, 0.0, GoombaPNG, TRUE, TRUE);
				else DrawRotaGraph(Goomba[i].X + 16 - Scroll, Goomba[i].Y, 2.0, 0.0, GoombaPNG, TRUE);
			}
			else {
				if (Goomba[i].Death == 0) {
					if (Goomba[i].Xspeed <= 0) DrawTurnGraph(Goomba[i].X - Scroll, Goomba[i].Y, GoombaPNG, TRUE);
					else DrawGraph(Goomba[i].X - Scroll, Goomba[i].Y, GoombaPNG, TRUE);
				}
				if (Goomba[i].Death > 0 && Goomba[i].Death <= 16) {
					if (Goomba[i].Xspeed <= 0) DrawTurnGraph(Goomba[i].X - Scroll, Goomba[i].Y, GoombaOwataPNG, TRUE);
					else DrawGraph(Goomba[i].X - Scroll, Goomba[i].Y, GoombaOwataPNG, TRUE);
				}
				if (Goomba[i].Death < 0) DrawGraph(Goomba[i].X - Scroll, Goomba[i].Y - Goomba[i].Death * 2, GoombaPNG, TRUE);
			}
		}
		//テルテル
		for (int i = 0; i < SunnyNum; i++) {
			if (Sunny[i].Death == 0) {
				if (Sunny[i].Yspeed < 0) DrawGraph(Sunny[i].X - Scroll, Sunny[i].Y, SunnyPNG, TRUE);
				else DrawRotaGraph(Sunny[i].X - Scroll + 16, Sunny[i].Y + 24, 1.0, DX_PI, SunnyPNG, TRUE, TRUE);
			}
		}
		//毒キノコ
		for (int i = 0; i < PoisonNum; i++) {
			if (Poison[i].Death == 0) DrawGraph(Poison[i].X - Scroll, Poison[i].Y, PoisonPNG, TRUE);
			if (Poison[i].Death < 0) DrawGraph(Poison[i].X - Scroll, Poison[i].Y - Poison[i].Death * 2, PoisonPNG, TRUE);
		}
		//まずいキノコ
		for (int i = 0; i < MushroomNum; i++) {
			if (Mushroom[i].Death == 0) DrawGraph(Mushroom[i].X - Scroll, Mushroom[i].Y, MushroomPNG, TRUE);
			if (Mushroom[i].Death < 0) DrawGraph(Mushroom[i].X - Scroll, Mushroom[i].Y - Mushroom[i].Death * 2, MushroomPNG, TRUE);
		}
		//キノコ
		for (int i = 0; i < BigMushroomNum; i++) {
			if (BigMushroom[i].Death == 0) DrawGraph(BigMushroom[i].X - Scroll, BigMushroom[i].Y, MushroomPNG, TRUE);
			if (BigMushroom[i].Death < 0) DrawGraph(BigMushroom[i].X - Scroll, BigMushroom[i].Y - BigMushroom[i].Death * 2, MushroomPNG, TRUE);
		}
		//スター
		for (int i = 0; i < StarNum; i++) {
			if (Star[i].Death == 0) DrawGraph(Star[i].X - Scroll, Star[i].Y, StarPNG, TRUE);
			if (Star[i].Death < 0) DrawGraph(Star[i].X - Scroll, Star[i].Y - Star[i].Death * 2, StarPNG, TRUE);
		}
		//亀
		for (int i = 0; i < KoopaNum; i++) {
			if (Koopa[i].Death == 0) {
				if (Koopa[i].Xspeed <= 0) DrawTurnGraph(Koopa[i].X - Scroll, Koopa[i].Y, KoopaPNG, TRUE);
				else DrawGraph(Koopa[i].X - Scroll, Koopa[i].Y, KoopaPNG, TRUE);
			}
			if (Koopa[i].Death == 1) {
				if (Koopa[i].Xspeed <= 0) DrawTurnGraph(Koopa[i].X - Scroll, Koopa[i].Y, KoopaOwataPNG, TRUE);
				else DrawGraph(Koopa[i].X - Scroll, Koopa[i].Y, KoopaOwataPNG, TRUE);
			}
			if (Koopa[i].Death < 0) DrawGraph(Koopa[i].X - Scroll, Koopa[i].Y - Koopa[i].Death * 2, KoopaPNG, TRUE);
		}
		//ビーム
		for (int i = 0; i < BeamNum; i++) {
			if (Beam[i].Death == 0) DrawGraph(Beam[i].X - Scroll, Beam[i].Y, BeamPNG, TRUE);
		}
		//とげスライム
		for (int i = 0; i < GoombaThornsNum; i++) {
			if (GoombaThorns[i].Big) {
				if (Goomba[i].Xspeed <= 0) DrawRotaGraph(Goomba[i].X + 16 - Scroll, Goomba[i].Y, 2.0, 0.0, GoombaPNG, TRUE, TRUE);
				else DrawRotaGraph(Goomba[i].X + 16 - Scroll, Goomba[i].Y, 2.0, 0.0, GoombaPNG, TRUE);
			}
			else {
				if (GoombaThorns[i].Death == 0) {
					if (GoombaThorns[i].Xspeed <= 0) DrawTurnGraph(GoombaThorns[i].X - Scroll, GoombaThorns[i].Y, GoombaThornsPNG, TRUE);
					else DrawGraph(GoombaThorns[i].X - Scroll, GoombaThorns[i].Y, GoombaThornsPNG, TRUE);
				}
				if (GoombaThorns[i].Death < 0) DrawGraph(GoombaThorns[i].X - Scroll, GoombaThorns[i].Y - GoombaThorns[i].Death * 2, GoombaThornsPNG, TRUE);
			}
		}
		//リフト
		for (int i = 0; i < LiftNum; i++) {
			if (Lift[i].Death == -2) {
				if (Lift[i].Yspeed == 0) {
					DrawGraph(Lift[i].X - Scroll, Lift[i].Y, LiftPNG, TRUE);
					DrawTurnGraph(Lift[i].X + 64 - Scroll, Lift[i].Y, LiftPNG, TRUE);
				}
				if (Lift[i].Yspeed > 0 && Lift[i].Yspeed <= 32) {
					DrawRotaGraph(Lift[i].X + 32 - Scroll, Lift[i].Y + 24, 1.0, DX_PI / 4, LiftPNG, TRUE, FALSE);
					DrawRotaGraph(Lift[i].X + 64 + 32 - Scroll, Lift[i].Y + 24, 1.0, DX_PI * 7 / 4, LiftPNG, TRUE, TRUE);
				}
			}
			else {
				DrawGraph(Lift[i].X - Scroll, Lift[i].Y, LiftPNG, TRUE);
				DrawTurnGraph(Lift[i].X + 64 - Scroll, Lift[i].Y, LiftPNG, TRUE);
			}
		}
		//ブロックの破片
		for (int i = 0; i < 4 * 64; i++) if (Debris[i].Death == 0) {
			DrawCircle(Debris[i].X + 8 - Scroll, Debris[i].Y + 8, 7, GetColor(143, 96, 48), TRUE);
			DrawCircle(Debris[i].X + 8 - Scroll, Debris[i].Y + 8, 7, GetColor(0, 0, 0), FALSE);
		}
		//コインチャリーン
		for (int i = 0; i < 16; i++) if (FloatCoin[i].Death <= 0) {
			DrawGraph(FloatCoin[i].X - Scroll, FloatCoin[i].Y, CoinPNG, TRUE);
		}
		//横土管
		for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
			if (map(i, j) == 28 || map(i, j) == 29) DrawGraph(32 * j - Scroll, 32 * i, HorizontalPipePNG, TRUE);
		}
		//ブロック
		for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
			switch (map(i, j) % 1000) {
			case 1:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					if (i == 0) DrawGraph(32 * j - Scroll, 32 * i, SoilPNG, TRUE);
					else if (map(i - 1, j) <= 0 || map(i - 1, j) == 6 || map(i - 1, j) == 7 || map(i - 1, j) == 19 || map(i - 1, j) == 22) DrawGraph(32 * j - Scroll, 32 * i, GrassPNG, TRUE);
					else DrawGraph(32 * j - Scroll, 32 * i, SoilPNG, TRUE);
					break;
				case 1:
					if (i == 0) DrawGraph(32 * j - Scroll, 32 * i, SoilCavePNG, TRUE);
					else if (map(i - 1, j) <= 0 || map(i - 1, j) == 6 || map(i - 1, j) == 7 || map(i - 1, j) == 19 || map(i - 1, j) == 22) DrawGraph(32 * j - Scroll, 32 * i, GrassCavePNG, TRUE);
					else DrawGraph(32 * j - Scroll, 32 * i, SoilCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 2:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, WoodPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, WoodCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 3:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, BrickPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, BrickCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 4:
				DrawGraph(32 * j - Scroll, 32 * i, QuestionPNG, TRUE);
				break;
			case 5:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, BreakPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, BreakCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 7:
				DrawGraph(32 * j - Scroll, 32 * i, CoinPNG, TRUE);
				break;
			case 8:
				DrawGraph(32 * j - Scroll, 32 * i, PipePNG, TRUE);
				break;
			case 9:
				DrawGraph(32 * j - Scroll, 32 * i, PipePNG, TRUE);
				break;
			case 10:
				DrawGraph(32 * j - Scroll, 32 * i, QuestionPNG, TRUE);
				break;
			case 11:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, BrickPNG, TRUE);
					DrawGraph(32 * j + 32 - Scroll, 32 * i, BrickPNG, TRUE);
					DrawGraph(32 * j + 32 + 32 - Scroll, 32 * i, BrickPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, BrickCavePNG, TRUE);
					DrawGraph(32 * j + 32 - Scroll, 32 * i, BrickCavePNG, TRUE);
					DrawGraph(32 * j + 32 + 32 - Scroll, 32 * i, BrickCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 13:
				DrawGraph(32 * j - Scroll, 32 * i, QuestionPNG, TRUE);
				break;
			case 14:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, BrickPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, BrickCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 15:
				DrawGraph(32 * j - Scroll, 32 * i - 32, SavePNG, TRUE);
				break;
			case 16:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, BrickPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, BrickCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 18:
				DrawGraph(32 * j - Scroll, 32 * i, SplinterPNG, TRUE);
				break;
			case 20:
				DrawGraph(32 * j - Scroll, 32 * i, QuestionPNG, TRUE);
				break;
			case 21:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, WoodPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, WoodCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 23:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, SoilPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, SoilCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 24:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					if (i == 0) DrawGraph(32 * j - Scroll, 32 * i, SoilPNG, TRUE);
					else if (map(i - 1, j) <= 0 || map(i - 1, j) == 6 || map(i - 1, j) == 7 || map(i - 1, j) == 19 || map(i - 1, j) == 22) DrawGraph(32 * j - Scroll, 32 * i, GrassPNG, TRUE);
					else DrawGraph(32 * j - Scroll, 32 * i, SoilPNG, TRUE);
					break;
				case 1:
					if (i == 0) DrawGraph(32 * j - Scroll, 32 * i, SoilCavePNG, TRUE);
					else if (map(i - 1, j) <= 0 || map(i - 1, j) == 6 || map(i - 1, j) == 7 || map(i - 1, j) == 19 || map(i - 1, j) == 22) DrawGraph(32 * j - Scroll, 32 * i, GrassCavePNG, TRUE);
					else DrawGraph(32 * j - Scroll, 32 * i, SoilCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 25:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, WoodPNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, WoodCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			case 26:
				DrawGraph(32 * j - Scroll, 32 * i, HintBlockPNG, TRUE);
				break;
			case 27:
				DrawGraph(32 * j - Scroll, 32 * i, QuestionPNG, TRUE);
				break;
			case 30:
				DrawGraph(32 * j - Scroll, 32 * i, QuestionPNG, TRUE);
				break;
			case 31:
				DrawGraph(32 * j - Scroll, 32 * i, PSwitchPNG, TRUE);
				break;
			case 32:
				DrawGraph(32 * j - Scroll, 32 * i, BrickPNG, TRUE);
				break;
			case 33:
				DrawGraph(32 * j - Scroll, 32 * i, QuestionPNG, TRUE);
				break;
			case 35:
				DrawGraph(32 * j - Scroll, 32 * i, SpringPNG, TRUE);
				break;
			case 36:
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(32 * j - Scroll, 32 * i, NeedlePNG, TRUE);
					break;
				case 1:
					DrawGraph(32 * j - Scroll, 32 * i, NeedleCavePNG, TRUE);
					break;
				case 2:

					break;
				}
				break;
			}
		}
		//落ちるレンガ
		for (int i = 0; i < FallingBrickNum; i++) {
			if (FallingBrick[i].Death != 1) {
				switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
				case 0:
					DrawGraph(FallingBrick[i].X - Scroll, FallingBrick[i].Y, BrickPNG, TRUE);
					DrawGraph(FallingBrick[i].X + 32 - Scroll, FallingBrick[i].Y, BrickPNG, TRUE);
					DrawGraph(FallingBrick[i].X + 32 + 32 - Scroll, FallingBrick[i].Y, BrickPNG, TRUE);
					break;
				case 1:
					DrawGraph(FallingBrick[i].X - Scroll, FallingBrick[i].Y, BrickCavePNG, TRUE);
					DrawGraph(FallingBrick[i].X + 32 - Scroll, FallingBrick[i].Y, BrickCavePNG, TRUE);
					DrawGraph(FallingBrick[i].X + 32 + 32 - Scroll, FallingBrick[i].Y, BrickCavePNG, TRUE);
					break;
				case 2:

					break;
				}
			}
		}
		//落ちる床
		for (int i = 0; i < HoleNum; i++) {
			switch (MapDataBack[(Player.X + 16) / 32 / ChunkSize]) {
			case 0:
				if (Hole[i].Death) DrawGraph(Hole[i].X - Scroll, Hole[i].Y, SoilPNG, TRUE);
				else DrawGraph(Hole[i].X - Scroll, Hole[i].Y, GrassPNG, TRUE);
				break;
			case 1:
				if (Hole[i].Death) DrawGraph(Hole[i].X - Scroll, Hole[i].Y, SoilCavePNG, TRUE);
				else DrawGraph(Hole[i].X - Scroll, Hole[i].Y, GrassCavePNG, TRUE);
				break;
			case 2:

				break;
			}
		}
		//サークル２
		if (Circle < 64) DrawExtendGraph(Player.X - Scroll + 16 - Circle * 32, Player.Y + 24 - Circle * 32, Player.X - Scroll + 16 + Circle * 32, Player.Y + 24 + Circle * 32, CirclePNG, TRUE);
		//いろいろリセット
		SetDrawArea(0, 0, 640, 480);
		SetDrawBright(255, 255, 255);
		//デバッグ情報
		if (DebugModeFlag && CheckHitKey(KEY_INPUT_F3)) {
			//マス
			for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
				if ((Player.X + 16) / 32 / ChunkSize == j / ChunkSize && j * 32 >= Scroll - 32 && j * 32 <= Scroll + 640) {
					DrawBox(32 * j - Scroll, 32 * i, 32 * j + 32 - Scroll, 32 * i + 32, GetColor(0, 0, 255), FALSE);
					if (map(i, j)) DrawFormatString(4 + 32 * j - Scroll, 14 + 32 * i, GetColor(0, 0, 255), "%d", map(i, j));
				}
			}
			//チャンク
			for (int i = 0; i < MapDataSize; i++) {
				DrawBox(32 * ChunkSize * i - Scroll, 0, 32 * ChunkSize * (i + 1) - Scroll, 480, GetColor(255, 0, 0), FALSE);
				DrawFormatString(4 + 32 * ChunkSize * i - Scroll, 4, GetColor(255, 0, 0), "%d %s", i, MapDataHint[i]);
			}
			//ステータス
			DrawFormatString(0, 0, GetColor(0, 0, 0), "Player.X :         %4d j:%d", Player.X, (int)((Player.X + 16) / 32));
			DrawFormatString(0, 16, GetColor(0, 0, 0), "Player.Y :         %4d i:%d", Player.Y, (int)((Player.Y + 16) / 32));
			DrawFormatString(0, 32, GetColor(0, 0, 0), "Player.Xspeed :     %d", Player.Xspeed);
			DrawFormatString(0, 48, GetColor(0, 0, 0), "Player.Yspeed :     %d", Player.Yspeed);
			DrawFormatString(0, 64, GetColor(0, 0, 0), "Player.WalkCount :  %d", Player.WalkCount);
			DrawFormatString(0, 80, GetColor(0, 0, 0), "Player.Direction :  %d", Player.Direction);
			DrawFormatString(0, 96, GetColor(0, 0, 0), Player.GroundFlag ? "Player.GroundFlag : true" :
				"Player.GroundFlag : false");
			DrawFormatString(0, 112, GetColor(0, 0, 0), "Player.Death :      %d", Player.Death);
			DrawFormatString(0, 128, GetColor(0, 0, 0), "Scroll ;            %d", Scroll);
			DrawFormatString(0, 144, GetColor(0, 0, 0), "Save :              %d, %d", SaveX, SaveY);
			DrawFormatString(0, 160, GetColor(0, 0, 0), "LiftNum :           %d", LiftNum);
		}
		//残り時間
		if (CulturalFestivalFlag) {
			DrawBox(0, 0, 8 * 18.5, 16, 0xffffff, TRUE);
			DrawFormatString(0, 0, GetColor(0, 0, 0), "残り時間 : %d 秒", (Time - GetNowCount()) / 1000);
			if (GetNowCount() > Time) {
				LoadGraphScreen(0, 0, "画像/終了.png", FALSE);
				ScreenFlip();
				WaitKey();
				return -1;
			}
		}
		ScreenFlip();

		//----////----////----////----////----////----////----////----//

		if ((PadInput & PAD_INPUT_10) == 0) while (GetNowCount() - FrameTime < 1000.0 / FrameRate);

	}

	free(Map);
	free(MapDataX);
	free(MapDataY);
	free(Goomba);
	free(Sunny);
	free(FallingBrick);
	free(Poison);
	free(Mushroom);
	free(MapDataSave);
	free(Star);
	free(Koopa);
	free(Hole);
	free(Beam);
	for (int i = 0; i < MapDataSize; ++i)free(MapDataHint[i]);
	free(MapDataHint);
	free(BigMushroom);
	free(MapDataBack);
	return 0;
}
