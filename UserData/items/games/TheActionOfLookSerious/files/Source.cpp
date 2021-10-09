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


const int BlockShift = 8;	//�u���b�N���炵���e�l
const int ChunkSize = 20;
const double FrameRate = 40;

typedef struct {
	int X, Y;			//���W
	int Xspeed, Yspeed;	//���x
	int WalkCount;		//�������肵�������i��莞�Ԃ��ƂɃ��Z�b�g�j
	char Direction;		//����
	char GroundFlag;	//�n�ʂɂ��Ă��邩
	int Death;			//���S�����F���i��ԁj
	bool Big;			//�ł�����
} Mob;
#define LEFT 0
#define RIGHT 1
int PadInput;
int Select = 0, Time = -1;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int Start(void);
int Game(int Stage);	//0:�����I�� -1:���[�h���s 1:�S�[��/F1 2:����

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
		//�S��ʂ��ǂ�
		RECT rc;
		GetWindowRect(GetDesktopWindow(), &rc);
		int Width = rc.right - rc.left, Height = rc.bottom - rc.top;
		if (Width * 1.0 / Height >= 640.0 / 480) {
			//���E�ɗ]��
			SetWindowSizeExtendRate(Height * 1.0 / 480);
		}
		else {
			//�㉺�ɗ]��
			SetWindowSizeExtendRate(Width * 1.0 / 640);
		}
		SetMouseDispFlag(FALSE);
		SetWindowStyleMode(3);
	}
	SetMainWindowText("�^��̃A�N�V����ver2.0");
	SetDrawScreen(DX_SCREEN_BACK);
	SetUseTransColor(FALSE);

	if (CulturalFestivalFlag) {
		bool BeforeUP = true, BeforeDOWN = true;
		while (ProcessMessage() != -1 && (PadInput & PAD_INPUT_9) == 0) {
			if (CulturalFestivalFlag) SetMousePoint(640 / 2, 480 / 2);
			//����
			PadInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);
			if ((PadInput & PAD_INPUT_UP) && !BeforeUP) Select--;
			if ((PadInput & PAD_INPUT_DOWN) && !BeforeDOWN) Select++;
			BeforeUP = (PadInput & PAD_INPUT_UP), BeforeDOWN = (PadInput & PAD_INPUT_DOWN);
			if (Select < 0) Select = 2;
			if (Select == 3) Select = 0;
			if (CheckHitKey(KEY_INPUT_RETURN)) break;
			//�`��
			DrawBox(0, 0, 640, 480, GetColor(255, 255, 255), TRUE);
			DrawString(320 - 8 * 21, 120 - 8, "�����Ȃ��̌��ɂ͉��l����ł��܂���?\n�㉺�L�[�őI���Enter�Ō��肵�Ă��������B", GetColor(0, 0, 0));
			if (Select == 0) DrawString(320 - 8 * 19, 200 - 8, "���@�P�l�ȉ�", GetColor(255, 0, 0));
			else DrawString(320 - 8 * 19, 200 - 8, "�@�@�P�l�ȉ�", GetColor(0, 0, 0));
			if (Select == 1) DrawString(320 - 8 * 19, 280 - 8, "���@�Q�l", GetColor(255, 0, 0));
			else DrawString(320 - 8 * 19, 280 - 8, "�@�@�Q�l", GetColor(0, 0, 0));
			if (Select == 2) DrawString(320 - 8 * 19, 360 - 8, "���@�R�l�ȏ�", GetColor(255, 0, 0));
			else DrawString(320 - 8 * 19, 360 - 8, "�@�@�R�l�ȏ�", GetColor(0, 0, 0));
			ScreenFlip();
		}
		LoadGraphScreen(0, 0, "�摜/�������.jpg", FALSE);
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
	//�摜�Ȃǂ̓ǂݍ���
	int SkyPNG = LoadGraph("�摜/��.png");
	int Player1PNG = LoadGraph("�摜/Player 1.png");
	int Player2PNG = LoadGraph("�摜/Player 2.png");
	int PlayerJumpPNG = LoadGraph("�摜/Player Jump.png");
	int GrassPNG = LoadGraph("�摜/��.png");
	int SoilPNG = LoadGraph("�摜/�y.png");
	int WoodPNG = LoadGraph("�摜/��.png");
	int Stage1PNG = LoadGraph("�摜/Stage 1.png");
	int Stage2PNG = LoadGraph("�摜/Stage 2.png");
	int Stage3PNG = LoadGraph("�摜/Stage 3.png");
	int Stage4PNG = LoadGraph("�摜/Stage 4.png");
	int TitlePNG = LoadGraph("�摜/�^�C�g��.png");
	int jump = LoadSoundMem("���ʉ�/jump.mp3");

	while (ProcessMessage() != -1 && (PadInput & PAD_INPUT_9) == 0) {
		if (CulturalFestivalFlag) SetMousePoint(640 / 2, 480 / 2);
		FrameTime = GetNowCount();
		if (Count == 2) Count = 0;
		Count++;
		//����
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
		//����
		// �ړ� {
		//�v���C���[�̈ړ�
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
		// } �����蔻�� {
		Player.GroundFlag = false;
		//��
		if (Player.X < 0) {
			Player.X = 0;
			Player.Xspeed = 0;
		}
		if (Player.X > 640 - 32) {
			Player.X = 640 - 32;
			Player.Xspeed = 0;
		}
		//�v���C���[�E�u���b�N
		//�u���b�N�̏㉺�̓����蔻��
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 20; j++) {
				//�n�ʃu���b�N
				if (map[i][j] == 1 || map[i][j] == 2) {
					if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
						if (Player.Yspeed >= 0) {
							//�u���b�N�ɏ���Ă���Ƃ��̏���
							Player.GroundFlag = true;
							Player.Y = 32 * i - 48;
						}
						if (Player.Yspeed < 0) {
							//�u���b�N�ɓ����Ԃ������̏���
							if (Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
							}
						}
					}
				}
				//�X�e�[�W�I���u���b�N
				if (map[i][j] == 11 || map[i][j] == 12 || map[i][j] == 13 || map[i][j] == 14) {
					if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
						if (Player.Yspeed >= 0) {
							//�u���b�N�ɏ���Ă���Ƃ��̏���
							Player.GroundFlag = true;
							Player.Y = 32 * i - 48;
						}
						if (Player.Yspeed < 0) {
							//�u���b�N�ɓ����Ԃ������̏���
							if (Player.X + 32 <= 32 * j + BlockShift) Player.X = 32 * j - 32;
							else if (Player.X >= 32 * j + 32 - BlockShift) Player.X = 32 * j + 32;
							else {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								if (Stage == 0) PlaySoundFile("���ʉ�/hintBlock.mp3", DX_PLAYTYPE_BACK);
								Stage = map[i][j] - 10;
							}
						}
					}
				}
			}
		}
		//�u���b�N�̍��E�̓����蔻��
		//��ʏ�
		for (int j = 0; j < 20; j++) {
			//�n�ʃu���b�N
			if (map[0][j] == 1 || map[0][j] == 2) {
				if (Player.Y < 0 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
					if (Player.Xspeed > 0) {
						//�u���b�N�̍��ɓ����������̏���
						Player.X = 32 * j - 32;
						Player.Xspeed = 0;
					}
					if (Player.Xspeed < 0) {
						//�u���b�N�̉E�ɓ����������̏���
						Player.X = 32 * j + 32;
						Player.Xspeed = 0;
					}
				}
			}
			//�X�e�[�W�I���u���b�N
			if (map[0][j] == 11 || map[0][j] == 12 || map[0][j] == 13 || map[0][j] == 14) {
				if (Player.Y < 0 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
					if (Player.Xspeed > 0) {
						//�u���b�N�̍��ɓ����������̏���
						Player.X = 32 * j - 32;
						Player.Xspeed = 0;
					}
					if (Player.Xspeed < 0) {
						//�u���b�N�̉E�ɓ����������̏���
						Player.X = 32 * j + 32;
						Player.Xspeed = 0;
					}
				}
			}
		}
		//��ʓ�
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 20; j++) {
				//�n�ʃu���b�N
				if (map[i][j] == 1 || map[i][j] == 2) {
					if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32) {
						if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + Player.Xspeed) {
							//�u���b�N�̍��ɓ����������̏���
							Player.X = 32 * j - 32;
							Player.Xspeed = 0;
						}
						if (Player.X >= 32 * j + 32 + Player.Xspeed && Player.X < 32 * j + 32) {
							//�u���b�N�̉E�ɓ����������̏���
							Player.X = 32 * j + 32;
							Player.Xspeed = 0;
						}
						//�u���b�N���炵
						//��
						if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + BlockShift) Player.X = 32 * j - 32;
						//�E
						if (Player.X >= 32 * j + 32 - BlockShift && Player.X < 32 * j + 32) Player.X = 32 * j + 32;
					}
				}
				//�X�e�[�W�I���u���b�N
				if (map[i][j] == 11 || map[i][j] == 12 || map[i][j] == 13 || map[i][j] == 14) {
					if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32) {
						if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + Player.Xspeed) {
							//�u���b�N�̍��ɓ����������̏���
							Player.X = 32 * j - 32;
							Player.Xspeed = 0;
						}
						if (Player.X >= 32 * j + 32 + Player.Xspeed && Player.X < 32 * j + 32) {
							//�u���b�N�̉E�ɓ����������̏���
							Player.X = 32 * j + 32;
							Player.Xspeed = 0;
						}
					}
				}
			}
		}
		// } �n�ʂɂ�����c�ɓ����Ȃ�����
		if (Player.GroundFlag) Player.Yspeed = 0;
		//�`��
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
				DrawString(32 * j + 28, 32 * (i + 6) + 12, "�s�������X�e�[�W�̔ԍ���\n�����ꂽ�u���b�N��@���I", GetColor(0, 0, 0));
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
		//�c�莞��
		if (CulturalFestivalFlag && Time != -1) {
			DrawBox(0, 0, 8 * 18.5, 16, 0xffffff, TRUE);
			DrawFormatString(0, 0, GetColor(0, 0, 0), "�c�莞�� : %d �b", (Time - GetNowCount()) / 1000);
			if (GetNowCount() > Time) {
				LoadGraphScreen(0, 0, "�摜/�I��.png", FALSE);
				ScreenFlip();
				WaitKey();
				return 0;
			}
		}
		ScreenFlip();
		if ((PadInput & PAD_INPUT_10) == 0) while (GetNowCount() - FrameTime < 1000.0 / FrameRate);
		//�X�e�[�W����
		if (Stage != 0 && Brightness <= 0) {
			SetDrawBright(255, 255, 255);
			return Stage;
		}
	}
	return 0;
}

int Game(int Stage) {

	static int Life = 2;

	//���[�h�X�^�[�g
	ClearDrawScreen();
	DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), TRUE);
	LoadGraphScreen(260, 216, "�摜/Player Jump.png", TRUE);
	DrawFormatString(320, 232, GetColor(255, 255, 255), "�~ %d", Life);
	ScreenFlip();
	int LoadStart = GetNowCount();

	// �t�@�C���̓ǂݍ��� ////----////----////----////----////----//
	//�X�e�[�W�}�b�v
	char FileName[] = "�X�e�[�W�}�b�v/Stage0.csv";
	FileName[20] += Stage;
	int File = FileRead_open(FileName);
	if (File == 0 || FileRead_eof(File) != 0) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�}�b�v�f�[�^�̓ǂݍ��݂Ɏ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�}�b�v�f�[�^�̓ǂݍ��݂Ɏ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�}�b�v�f�[�^�̓ǂݍ��݂Ɏ��s���܂����B\n�X�e�[�W�}�b�v�t�@�C�������Ċm�F���������B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		return -1;
	}
#define map(map_y, map_x) Map[MapXSize * (map_y) + (map_x)]
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (FileRead_eof(File)) {
			if (CulturalFestivalFlag) {
				std::string Error[2] = {
					"�}�b�v�f�[�^�̓ǂݍ��݂Ɏ��s���܂����B",
					"�������Ă�ł��������B"
				};
				My_MessageBox(Error, 2);
				ScreenFlip();
				WaitKey();
				//MessageBox(GetMainWindowHandle(), "�}�b�v�f�[�^�̓ǂݍ��݂Ɏ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
			}
			else MessageBox(GetMainWindowHandle(), "�}�b�v�f�[�^�̓ǂݍ��݂Ɏ��s���܂����B\n�X�e�[�W�}�b�v�t�@�C�����m�F���Ă��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
			free(Map);
			return -1;
		}
		FileRead_scanf(File, "%d,", &map(i, j));
	}
	FileRead_close(File);
	if (MapXSize < 15) MapXSize = 15;
	//�X�e�[�W�f�[�^
	char FileName2[] = "�X�e�[�W�}�b�v/Stage0Data.csv";
	FileName2[20] += Stage;
	int File2 = FileRead_open(FileName2);
	int MapDataSize = (MapXSize % ChunkSize) ? (MapXSize / ChunkSize + 1) : (MapXSize / ChunkSize);
	int *MapDataX = (int *)malloc(sizeof(int) * MapDataSize);
	if (MapDataX == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		return -1;
	}
	int *MapDataY = (int *)malloc(sizeof(int) * MapDataSize);
	if (MapDataY == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
		free(Map);
		free(MapDataX);
		free(MapDataY);
		return -1;
	}
	int *MapDataSave = (int *)malloc(sizeof(int) * MapDataSize);
	if (MapDataSave == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�v���C���[
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
	//�X���C��
	int GoombaNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -1) GoombaNum++;
	}
	Mob *Goomba = (Mob *)malloc(sizeof(Mob) * GoombaNum);
	if (Goomba == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�e���e��
	int SunnyNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -2 || map(i, j) == -3) SunnyNum++;
	}
	Mob *Sunny = (Mob *)malloc(sizeof(Mob) * SunnyNum);
	if (Sunny == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�����Ă��郌���K
	int FallingBrickNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 11) FallingBrickNum++;
	}
	Mob *FallingBrick = (Mob *)malloc(sizeof(Mob) * FallingBrickNum);
	if (FallingBrick == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�ŃL�m�R
	int PoisonNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 12) PoisonNum += 256;
	}
	Mob *Poison = (Mob *)malloc(sizeof(Mob) * PoisonNum);
	if (Poison == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�܂����L�m�R
	int MushroomNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 13) MushroomNum++;
	}
	Mob *Mushroom = (Mob *)malloc(sizeof(Mob) * MushroomNum);
	if (Mushroom == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�X�^�[
	int StarNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 16) StarNum++;
	}
	Mob *Star = (Mob *)malloc(sizeof(Mob) * StarNum);
	if (Star == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�T
	int KoopaNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -4) KoopaNum++;
	}
	Mob *Koopa = (Mob *)malloc(sizeof(Mob) * KoopaNum);
	if (Koopa == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//���Ƃ���
	int HoleNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -5) HoleNum++;
	}
	Mob *Hole = (Mob *)malloc(sizeof(Mob) * HoleNum);
	if (Hole == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�r�[��
	int BeamNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -6) BeamNum++;
	}
	Mob *Beam = (Mob *)malloc(sizeof(Mob) * BeamNum);
	if (Beam == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�L�m�R
	int BigMushroomNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == 27) BigMushroomNum++;
	}
	Mob *BigMushroom = (Mob *)malloc(sizeof(Mob) * BigMushroomNum);
	if (BigMushroom == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�Ƃ��X���C��
	int GoombaThornsNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -7) GoombaThornsNum++;
	}
	Mob *GoombaThorns = (Mob *)malloc(sizeof(Mob) * GoombaThornsNum);
	if (GoombaThorns == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//���t�g
	int LiftNum = 0;
	for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
		if (map(i, j) == -8 || map(i, j) == -9 || map(i, j) == -10 || map(i, j) == -11) LiftNum++;
	}
	Mob *Lift = (Mob *)malloc(sizeof(Mob) * LiftNum);
	if (Lift == NULL) {
		if (CulturalFestivalFlag) {
			std::string Error[2] = {
				"�������̊m�ۂɎ��s���܂����B",
				"�������Ă�ł��������B"
			};
			My_MessageBox(Error, 2);
			ScreenFlip();
			WaitKey();
			//MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B\n�������Ă�ł��������B", NULL, MB_OK | MB_ICONEXCLAMATION);
		}
		else MessageBox(GetMainWindowHandle(), "�������̊m�ۂɎ��s���܂����B", NULL, MB_OK | MB_ICONEXCLAMATION);
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
	//�u���b�N�̔j��
	Mob Debris[4 * 64];
	for (int i = 0; i < 4 * 64; i++) Debris[i].Death = 1;
	//�R�C���`�����[��
	Mob FloatCoin[16];
	for (int i = 0; i < 16; i++) FloatCoin[i].Death = 1;
	//�摜�Ȃǂ̓ǂݍ���
	int CloudPNG = LoadGraph("�摜/�_.png");
	int CavePNG = LoadGraph("�摜/���A.png");
	int CirclePNG = LoadGraph("�摜/�T�[�N��.png");
	int Player1PNG = LoadGraph("�摜/Player 1.png");
	int Player2PNG = LoadGraph("�摜/Player 2.png");
	int PlayerJumpPNG = LoadGraph("�摜/Player Jump.png");
	int PlayerOwataPNG = LoadGraph("�摜/Player Owata.png");
	int BigPlayerPNG = LoadGraph("�摜/Big Player.png");
	int GoombaPNG = LoadGraph("�摜/Goomba.png");
	int GoombaOwataPNG = LoadGraph("�摜/Goomba Owata.png");
	int SunnyPNG = LoadGraph("�摜/�e���e��.png");
	int PoisonPNG = LoadGraph("�摜/�ŃL�m�R.png");
	int MushroomPNG = LoadGraph("�摜/�L�m�R.png");
	int StarPNG = LoadGraph("�摜/�X�^�[.png");
	int KoopaPNG = LoadGraph("�摜/�T.png");
	int KoopaOwataPNG = LoadGraph("�摜/�T Owata.png");
	int BeamPNG = LoadGraph("�摜/�r�[��.png");
	int GoombaThornsPNG = LoadGraph("�摜/�Ƃ��X���C��.png");
	int LiftPNG = LoadGraph("�摜/���t�g.png");
	int CrackLiftPNG = LoadGraph("�摜/���ꃊ�t�g.png");
	int MissilePNG = LoadGraph("�摜/�~�T�C��.png");
	int BeePNG = LoadGraph("�摜/�n�`.png");
	int GrassPNG = LoadGraph("�摜/��.png");
	int SoilPNG = LoadGraph("�摜/�y.png");
	int WoodPNG = LoadGraph("�摜/��.png");
	int BrickPNG = LoadGraph("�摜/�����K.png");
	int BreakPNG = LoadGraph("�摜/�@����.png");
	int GrassCavePNG = LoadGraph("�摜/��Cave.png");
	int SoilCavePNG = LoadGraph("�摜/�yCave.png");
	int WoodCavePNG = LoadGraph("�摜/��Cave.png");
	int BrickCavePNG = LoadGraph("�摜/�����KCave.png");
	int BreakCavePNG = LoadGraph("�摜/�@����Cave.png");
	int QuestionPNG = LoadGraph("�摜/�n�e�i.png");
	int CoinPNG = LoadGraph("�摜/�R�C��.png");
	int PipePNG = LoadGraph("�摜/�y��.png");
	int SavePNG = LoadGraph("�摜/����.png");
	int GoalPNG = LoadGraph("�摜/�S�[��.png");
	int SplinterPNG = LoadGraph("�摜/�g�Q�g�Q.png");
	int HintBlockPNG = LoadGraph("�摜/�q���g.png");
	int HorizontalPipePNG = LoadGraph("�摜/���y��.png");
	int PSwitchPNG = LoadGraph("�摜/P�X�C�b�`.png");
	int MushroomScaffoldLeftPNG = LoadGraph("�摜/�L�m�R���� ��.png");
	int MushroomScaffoldUpPNG = LoadGraph("�摜/�L�m�R���� ��.png");
	int MushroomScaffoldRightPNG = LoadGraph("�摜/�L�m�R���� �E.png");
	int MushroomScaffoldDownPNG = LoadGraph("�摜/�L�m�R���� ��.png");
	int SpringPNG = LoadGraph("�摜/�o�l.png");
	int OnPNG = LoadGraph("�摜/On�X�C�b�` .png");
	int OffPNG = LoadGraph("�摜/Off�X�C�b�`.png");
	int OnBlockOnPNG = LoadGraph("�摜/ON�u���b�N ON.png");
	int OnBlockOffPNG = LoadGraph("�摜/ON�u���b�N OFF.png");
	int OffBlockOnPNG = LoadGraph("�摜/OFF�u���b�N ON.png");
	int OffBlockOffPNG = LoadGraph("�摜/OFF�u���b�N OFF.png");
	int NeedlePNG = LoadGraph("�摜/�j.png");
	int NeedleCavePNG = LoadGraph("�摜/�jCave.png");
	int jump = LoadSoundMem("���ʉ�/jump.mp3");
	int humi = LoadSoundMem("���ʉ�/humi.mp3");
	int brockbreak = LoadSoundMem("���ʉ�/brockbreak.mp3");
	int brockcoin = LoadSoundMem("���ʉ�/brockcoin.mp3");
	int coin = LoadSoundMem("���ʉ�/coin.mp3");
	int dokan = LoadSoundMem("���ʉ�/dokan.mp3");
	int brockkinoko = LoadSoundMem("���ʉ�/brockkinoko.mp3");
	int kirra = LoadSoundMem("���ʉ�/kirra.mp3");
	int koura = LoadSoundMem("���ʉ�/koura.mp3");
	int goal = LoadSoundMem("���ʉ�/goal.mp3");
	//�O���t�B�b�N
	int Circle = 0;
	int Cloud = 0;

	//�X�e�[�W�X�^�[�g
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

		// ���� //----////----////----////----////----////----////----//

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

		// ���� //----////----////----////----////----////----////----//

		/*�v���C���[*/ {
			//�ړ�
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
			//�����蔻��
			if (Player.Death >= 0 && Player.Death <= 3) {
				Player.GroundFlag = false;
				//��
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
				//�v���C���[�E�u���b�N
				//�ł����Ƃ��킷
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
					//�A�N�V�����u���b�N�Ɉ�
					for (int i = 0; i < 15; i++) {
						for (int j = 0; j < MapXSize; j++) {
							//�����K�u���b�N
							if (map(i, j) == 3) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 203;
									}
								}
							}
							//�n�e�i�u���b�N
							if (map(i, j) == 4) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 204;
									}
								}
							}
							//�B���u���b�N
							if (map(i, j) == 6) {
								if (Player.Y >= 32 * i + 32 + Player.Yspeed && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 204;
									}
								}
							}
							//�X���C���u���b�N
							if (map(i, j) == 10) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 210;
									}
								}
							}
							//�ŃL�m�R�u���b�N
							if (map(i, j) == 12) {
								if (Player.Y >= 32 * i + 32 + Player.Yspeed && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 212;
									}
								}
							}
							//�܂����L�m�R�u���b�N
							if (map(i, j) == 13) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 213;
									}
								}
							}
							//10�R�C�������K�u���b�N
							if (map(i, j) % 1000 == 14) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) += 200;
									}
								}
							}
							//�X�^�[�����K�u���b�N
							if (map(i, j) == 16) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 216;
									}
								}
							}
							//�B���g�Q
							if (map(i, j) == 19) {
								if (Player.Y >= 32 * i + 32 + Player.Yspeed && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 219;
									}
								}
							}
							//�n�e�i�g�Q
							if (map(i, j) == 20) {
								if (Player.Y >= 32 * i + 32 + Player.Yspeed && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 219;
									}
								}
							}
							//�q���g�u���b�N
							if (map(i, j) == 26) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 226;
									}
								}
							}
							//�L�m�R�u���b�N
							if (map(i, j) == 27) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 227;
									}
								}
							}
							//P�n�e�i�u���b�N
							if (map(i, j) == 30) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 230;
									}
								}
							}
							//����郌���K�u���b�N
							if (map(i, j) == 32) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed >= 0) {
										map(i, j) = 232;
									}
								}
							}
							//�Ƃ��X���C���n�e�i�u���b�N
							if (map(i, j) == 33) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed < 0 && Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
										map(i, j) = 233;
									}
								}
							}
						}
					}
					//�u���b�N�̏㉺�̓����蔻��
					for (int i = 0; i < 15; i++) {
						for (int j = 0; j < MapXSize; j++) {
							//�u���b�N
							if (map(i, j) == 1 || map(i, j) == 2 || map(i, j) == 5 || map(i, j) == 11 || (j >= 1 && map(i, j - 1) == 11) || (j >= 2 && map(i, j - 2) == 11) || map(i, j) == 17 || map(i, j) == 31
								|| map(i, j) == 28 || (j >= 1 && map(i, j - 1) == 28) || (j >= 2 && map(i, j - 2) == 28) || (i >= 1 && map(i - 1, j) == 28) || (i >= 1 && j >= 1 && map(i - 1, j - 1) == 28) || (i >= 1 && j >= 2 && map(i - 1, j - 2) == 28)
								|| map(i, j) == 29 || (j >= 1 && map(i, j - 1) == 29) || (j >= 2 && map(i, j - 2) == 29) || (i >= 1 && map(i - 1, j) == 29) || (i >= 1 && j >= 1 && map(i - 1, j - 1) == 29) || (i >= 1 && j >= 2 && map(i - 1, j - 2) == 29)) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed >= 0) {
										//�u���b�N�ɏ���Ă���Ƃ��̏���
										Player.GroundFlag = true;
										Player.Y = 32 * i - 48;
										if (map(i, j) == 31) {
											PlaySoundFile("���ʉ�/Pswitch.mp3", DX_PLAYTYPE_BACK);
											for (int k = 0; k < 15; k++) for (int l = 0; l < MapXSize; l++) {
												if (map(k, l) > 0) map(k, l) = 7;
											}
										}
									}
									if (Player.Yspeed < 0) {
										//�u���b�N�ɓ����Ԃ������̏���
										if (Player.X + 32 > 32 * j + BlockShift && Player.X < 32 * j + 32 - BlockShift) {
											Player.Yspeed = abs(Player.Yspeed);
											Player.Y = 32 * i + 32;
										}
									}
								}
							}
							//���u���b�N
							if (
								map(i, j) == 3 || map(i, j) == 4 || map(i, j) == 8 || (j >= 1 && map(i, j - 1) == 8) || map(i, j) == 9 || (j >= 1 && map(i, j - 1) == 9) || map(i, j) == 10 || map(i, j) == 13 || map(i, j) % 1000 == 14 || map(i, j) == 16 || map(i, j) == 20 || map(i, j) == 26 || map(i, j) == 27 || map(i, j) == 30 || (
									abs(Player.Xspeed) > 4 && j >= 1 && j < MapXSize - 1 &&																																																																					//�����Ă�Ƃ�
									(map(i, j - 1) == 1 || map(i, j - 1) == 2 || map(i, j - 1) == 3 || map(i, j - 1) == 4 || map(i, j - 1) == 5 || map(i, j - 1) == 10 || map(i, j - 1) == 13 || map(i, j - 1) % 1000 == 14 || map(i, j - 1) == 16 || map(i, j - 1) == 20 || map(i, j - 1) == 26 || map(i, j - 1) == 27 || map(i, j - 1) == 30 || map(i, j - 1) == 31) &&	//��}�X�̌��Ԃɂ�
									(map(i, j + 1) == 1 || map(i, j + 1) == 2 || map(i, j + 1) == 3 || map(i, j + 1) == 4 || map(i, j + 1) == 5 || map(i, j + 1) == 10 || map(i, j + 1) == 13 || map(i, j + 1) % 1000 == 14 || map(i, j + 1) == 16 || map(i, j + 1) == 20 || map(i, j + 1) == 26 || map(i, j + 1) == 27 || map(i, j + 1) == 30 || map(i, j + 1) == 31)		//�����Ȃ��悤�ɂ���
									|| map(i, j) == 33)
								) {
								if (Player.Y + 48 >= 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j + Player.Xspeed && Player.X < 32 * j + 32 + Player.Xspeed) {
									if (Player.Yspeed >= 0) {
										//�u���b�N�ɏ���Ă���Ƃ��̏���
										Player.GroundFlag = true;
										Player.Y = 32 * i - 48;
										//�y�ǂɓ���
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
							//�����K�u���b�N���󂳂ꂽ
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
							//�n�e�i�u���b�N���@���ꂽ
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
							//�X���C���u���b�N���@���ꂽ
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
							//�ŃL�m�R�u���b�N���@���ꂽ
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
							//�܂����L�m�R�u���b�N���@���ꂽ
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
							//10�R�C�������K�u���b�N���@���ꂽ
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
							//�܂����L�m�R�u���b�N���@���ꂽ
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
							//�B���g�Q
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
							//�q���g�u���b�N
							if (map(i, j) == 226) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 26;
								PlaySoundFile("���ʉ�/hintBlock.mp3", DX_PLAYTYPE_BACK);
								HintCount = 1;
							}
							//�L�m�R�u���b�N���@���ꂽ
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
							//P�X�C�b�`�n�e�i�u���b�N
							if (map(i, j) == 230) {
								Player.Yspeed = abs(Player.Yspeed);
								Player.Y = 32 * i + 32;
								map(i, j) = 5;
								if (i > 0) map(i - 1, j) = 31;
								PlaySoundMem(brockkinoko, DX_PLAYTYPE_BACK, TRUE);
							}
							//����郌���K�u���b�N���󂳂ꂽ
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
							//�Ƃ��X���C���u���b�N���@���ꂽ
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
					//�u���b�N�̍��E�̓����蔻��
					//��ʏ�
					for (int j = 0; j < MapXSize; j++) {
						//�u���b�N
						if (map(0, j) == 1 || map(0, j) == 2 || map(0, j) == 3 || map(0, j) == 4 || map(0, j) == 5 || map(0, j) == 10 || map(0, j) == 13 || map(0, j) % 1000 == 14 || map(0, j) == 16 || map(0, j) == 20 || map(0, j) == 26 || map(0, j) == 30 || map(0, j) == 32
							|| map(0, j) == 27 || map(0, j) == 36) {
							if (Player.Y < 0 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
								if (Player.Xspeed > 0) {
									//�u���b�N�̍��ɓ����������̏���
									Player.X = 32 * j - 32;
									Player.Xspeed = 0;
								}
								if (Player.Xspeed < 0) {
									//�u���b�N�̉E�ɓ����������̏���
									Player.X = 32 * j + 32;
									Player.Xspeed = 0;
								}
							}
						}
					}
					//��ʓ�
					for (int i = 0; i < 15; i++) {
						for (int j = 0; j < MapXSize; j++) {
							//�u���b�N
							if (map(i, j) == 1 || map(i, j) == 2 || map(i, j) == 3 || map(i, j) == 4 || map(i, j) == 5 || map(i, j) == 10 || map(i, j) == 11 || (j >= 2 && map(i, j - 2) == 11) || map(i, j) == 13 || map(i, j) % 1000 == 14
								|| map(i, j) == 16 || map(i, j) == 17 || map(i, j) == 20 || map(i, j) == 26 || map(i, j) == 27
								|| map(i, j) == 28 || (j >= 2 && map(i, j - 2) == 28) || (i >= 1 && map(i - 1, j) == 28) || (i >= 1 && j >= 2 && map(i - 1, j - 2) == 28)
								|| map(i, j) == 29 || (j >= 2 && map(i, j - 2) == 29) || (i >= 1 && map(i - 1, j) == 29) || (i >= 1 && j >= 2 && map(i - 1, j - 2) == 29)
								|| map(i, j) == 30 || map(i, j) == 31 || map(i, j) == 32 || map(i, j) == 33 || map(i, j) == 36) {
								if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32) {
									if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + Player.Xspeed) {
										//�u���b�N�̍��ɓ����������̏���
										Player.X = 32 * j - 32;
										Player.Xspeed = 0;
										//�y�ǂɓ���
										if (Player.GroundFlag && ((map(i, j) == 29 && Player.Y > 32 * i && Player.Y + 48 <= 32 * i + 64) || (j >= 1 && map(i, j - 1) == 29 && Player.Y > 32 * i - 32 && Player.Y + 48 <= 32 * i + 32))) {
											Player.Death = -96;
											WarpX = MapDataX[j / ChunkSize];
											WarpY = MapDataY[j / ChunkSize];
											if (MapDataSave[j / ChunkSize]) SaveX = WarpX, SaveY = WarpY;
											PlaySoundMem(dokan, DX_PLAYTYPE_BACK, TRUE);
										}
									}
									if (Player.X >= 32 * j + 32 + Player.Xspeed && Player.X < 32 * j + 32) {
										//�u���b�N�̉E�ɓ����������̏���
										Player.X = 32 * j + 32;
										Player.Xspeed = 0;
									}
									//�u���b�N���炵
									//��
									if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + BlockShift) Player.X = 32 * j - 32;
									//�E
									if (Player.X >= 32 * j + 32 - BlockShift && Player.X < 32 * j + 32) Player.X = 32 * j + 32;
								}
							}
							//�y��
							if (map(i, j) == 8 || map(i, j) == 9) {
								if (Player.Y + 48 > 32 * i && Player.Y < 640) {
									if (Player.X + 32 > 32 * j && Player.X + 32 <= 32 * j + Player.Xspeed) {
										//���ɓ����������̏���
										Player.X = 32 * j - 32;
										Player.Xspeed = 0;
									}
									if (Player.X >= 32 + 32 * j + 32 + Player.Xspeed && Player.X < 32 + 32 * j + 32) {
										//�E�ɓ����������̏���
										Player.X = 32 + 32 * j + 32;
										Player.Xspeed = 0;
									}
								}
							}
						}
					}
				}
			}
			//�n�ʂɂ�����c�ɓ����Ȃ�����
			if (Player.GroundFlag && Player.Death >= 0 && Player.Death <= 3) Player.Yspeed = 0;
			//�ŏI�����蔻��
			for (int i = 0; i < 15; i++) {
				for (int j = 0; j < MapXSize; j++) {
					//�R�C��
					if (map(i, j) == 7) {
						if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
							map(i, j) = 0;
							PlaySoundMem(coin, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//�����Ă��郌���K
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
					//����
					if (map(i, j) == 15) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i - 32 && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
							map(i, j) = 0;
							SaveX = 32 * j, SaveY = 32 * i - 32;
						}
					}
					//�r�[��
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
					//�S�[��
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
					//�g�Q�g�Q
					if (map(i, j) == 18) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i + 8 && Player.Y < 32 * i + 32 - 8 && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							Player.Death = 1;
						}
					}
					//�؃g�Q
					if (map(i, j) == 21) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i + 8 && Player.Y < 32 * i + 32 - 8 && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							Player.Death = 1;
							map(i, j) = 18;
						}
					}
					//���B���g�Q
					if (map(i, j) == 22) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i + 8 && Player.Y < 32 * i + 32 - 8 && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							Player.Death = 1;
							map(i, j) = 18;
						}
					}
					//�y�g�Q
					if (map(i, j) == 23) {
						if (Player.Death == 0 && Player.Y + 48 > 32 * i + 8 && Player.Y < 32 * i + 32 - 8 && Player.X + 32 > 32 * j + 8 && Player.X < 32 * j + 32 - 8) {
							Player.Death = 1;
							map(i, j) = 18;
						}
					}
					//�L�m�R����
					if (map(i, j) == 34) {
						if (Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
							if (Player.Y - Player.Yspeed + 48 <= 32 * i && Player.Y + 48 > 32 * i) {
								//�u���b�N�ɏ���Ă���Ƃ��̏���
								Player.GroundFlag = true;
								Player.Y = 32 * i - 48;
								Player.Yspeed = 0;
							}
						}
					}
					//�o�l
					if (map(i, j) == 35) {
						if (Player.Y + 48 > 32 * i && Player.Y < 32 * i + 32 && Player.X + 32 > 32 * j && Player.X < 32 * j + 32) {
							Player.Yspeed = -192;
						}
					}
					//�j�u���b�N
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
			//�V��ɍs������ޗǂɗ��Ƃ����
			if (Player.Y < -480) {
				Player.Y = 960;
				Player.Death = 1;
			}
			//�S�[��
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
			//�q���g
			if (HintCount == 2) {
				HintCount = 0;
				SaveDrawScreen(160, 120, 480, 120 + 30 * 6, "�摜/Screeen.bmp");
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
					LoadGraphScreen(160, 120, "�摜/Screeen.bmp", FALSE);
					DrawBox(160, 120 + i * 6, 480, 120 + 30 * 6, GetColor(0, 0, 0), TRUE);
					DrawBox(160, 120 + i * 6, 480, 120 + 30 * 6, GetColor(255, 255, 255), FALSE);
					SetDrawArea(160 + 32, 120 + i * 6 + 32, 480 - 32, 120 + 30 * 6 - 32);
					DrawString(160 + 32, 120 + 32, MapDataHint[(Player.X + 16) / 32 / ChunkSize], GetColor(255, 255, 255));
					SetDrawArea(0, 0, 640, 480);
					ScreenFlip();
				}
			}
			//�X�N���[��
			if (Player.X - Scroll > 320 - 32 && Player.Y < 540 && !(DebugModeFlag && (GetMouseInput() & MOUSE_INPUT_LEFT))) {
				Scroll += Player.X - Scroll - (320 - 32);
				if (Scroll + 640 > 32 * MapXSize) Scroll = 32 * MapXSize - 640;
			}
			//�w�i�̋��ڂŃX�N���[�����~�߂�B
			while (MapDataBack[(Player.X + 16) / 32 / ChunkSize] != MapDataBack[(Scroll + 640) / 32 / ChunkSize]) {
				Scroll--;
			}
			while (MapDataBack[(Player.X + 16) / 32 / ChunkSize] != MapDataBack[(Scroll) / 32 / ChunkSize]) {
				Scroll++;
			}
			if (Scroll < 0) Scroll = 0;
			//�ޗ���
			if (Player.Y > 960 && Player.Death == 0) Player.Death = 1;
			//�f�o�b�N��
			if (CheckHitKey(KEY_INPUT_O) && Player.Death == 0) Player.Death = 1;
			//����
			if (Player.Death == 1) {
				//MessageBox(NULL, "<= to be Continue ///", "�Â�", NULL);
				GoalCount = 0;
				StopSoundMem(goal);
				PlaySoundFile("���ʉ�/death.mp3", DX_PLAYTYPE_BACK);
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
			//�y�ǃv���b�v���b�v���b
			if (Player.Death == -1 || Player.Death == -49) {
				Player.X = WarpX;
				Player.Y = WarpY;
				Player.Xspeed = 0;
				Player.Yspeed = 0;
				Player.Direction = RIGHT;
				Player.WalkCount = 0;
				Scroll = Player.X - (320 - 32);
				//�G���e�B�e�B�̃��Z�b�g
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
				//�T�[�N��
				Circle = 0;
			}
			if (Player.Death) Player.Death++;
			if (Player.Death == -48) Player.Death = 0;
		}
		HumiFlag = false;
		/*�X���C��*/	for (int i = 0; i < GoombaNum; i++) {
			if (Goomba[i].Death == 0 && Goomba[i].X > Scroll - 320 && Goomba[i].X < Scroll + 640 + 32 * 4) {
				if (Goomba[i].Yspeed > 64) Goomba[i].Yspeed = 64;
				Goomba[i].X += Goomba[i].Xspeed;
				Goomba[i].Y += ++Goomba[i].Yspeed;
				//�X���C���E�u���b�N
				Goomba[i].GroundFlag = false;
				//�ł����Ɖ�
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
					//�u���b�N�̏㉺�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Goomba[i].Y + 32 >= 32 * j && Goomba[i].Y < 32 * j + 32 && Goomba[i].X + 32 > 32 * k + Goomba[i].Xspeed && Goomba[i].X < 32 * k + 32 + Goomba[i].Xspeed) {
									if (Goomba[i].Yspeed >= 0) {
										//�u���b�N�ɏ���Ă���Ƃ��̏���
										Goomba[i].GroundFlag = true;
										Goomba[i].Y = 32 * j - 32;
									}
									if (Goomba[i].Yspeed < 0) {
										//�u���b�N�ɓ����Ԃ������̏���
										if (Goomba[i].X + 32 > 32 * k + BlockShift && Goomba[i].X < 32 * k + 32 - BlockShift) {
											Goomba[i].Yspeed = abs(Goomba[i].Yspeed);
											Goomba[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//�u���b�N�̍��E�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Goomba[i].Y + 32 > 32 * j && Goomba[i].Y < 32 * j + 32) {
									if (Goomba[i].X + 32 > 32 * k && Goomba[i].X + 32 <= 32 * k + Goomba[i].Xspeed) {
										//�u���b�N�̍��ɓ����������̏���
										Goomba[i].X = 32 * k - 32;
										if (Goomba[i].GroundFlag) Goomba[i].Xspeed = -2;
									}
									if (Goomba[i].X >= 32 * k + 32 + Goomba[i].Xspeed && Goomba[i].X < 32 * k + 32) {
										//�u���b�N�̉E�ɓ����������̏���
										Goomba[i].X = 32 * k + 32;
										if (Goomba[i].GroundFlag) Goomba[i].Xspeed = 2;
									}
									//�u���b�N���炵
									//��
									if (Goomba[i].X + 32 > 32 * k && Goomba[i].X + 32 <= 32 * k + BlockShift) Goomba[i].X = 32 * k - 32;
									//�E
									if (Goomba[i].X >= 32 * k + 32 - BlockShift && Goomba[i].X < 32 * k + 32) Goomba[i].X = 32 * k + 32;
								}
							}
							//�y��
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (Goomba[i].Y + 32 > 32 * j && Goomba[i].Y < 640) {
									if (Goomba[i].X + 32 > 32 * k && Goomba[i].X + 32 <= 32 * k + Goomba[i].Xspeed) {
										//���ɓ����������̏���
										Goomba[i].X = 32 * k - 32;
										if (Goomba[i].GroundFlag) Goomba[i].Xspeed = -2;
									}
									if (Goomba[i].X >= 32 + 32 * k + 32 + Goomba[i].Xspeed && Goomba[i].X < 32 + 32 * k + 32) {
										//�E�ɓ����������̏���
										Goomba[i].X = 32 + 32 * k + 32;
										if (Goomba[i].GroundFlag) Goomba[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//�L�m�R����
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (Goomba[i].X + 32 > 32 * k && Goomba[i].X < 32 * k + 32) {
								if (Goomba[i].Y - Goomba[i].Yspeed + 32 <= 32 * j && Goomba[i].Y + 32 > 32 * j) {
									//�u���b�N�ɏ���Ă���Ƃ��̏���
									Goomba[i].GroundFlag = true;
									Goomba[i].Y = 32 * j - 32;
								}
							}
						}
					}
				}
				//�n�ʂɂ�����c�ɓ����Ȃ�����
				if (Goomba[i].GroundFlag && Goomba[i].Death == 0) Goomba[i].Yspeed = 0;
				//�X���C���E�v���C���[
				if (Goomba[i].Big) {
					if (Player.X + 32 > Goomba[i].X - 16 && Player.X < Goomba[i].X + 48 && Player.Y + 48 > Goomba[i].Y - 32 && Player.Y < Goomba[i].Y + 32) Player.Death = 1;
				}
				else {
					//���E
					if (Player.Death == 0) {
						if (Player.Y + 32 > Goomba[i].Y && Player.Y < Goomba[i].Y + 32) {
							if (Player.X + 32 >= Goomba[i].X && Player.X + 32 + Goomba[i].Xspeed <= Goomba[i].X + Player.Xspeed) {
								//�X���C���̍��ɓ����������̏���
								Player.Death = 1;
							}
							if (Player.X + Goomba[i].Xspeed >= Goomba[i].X + 32 + Player.Xspeed && Player.X <= Goomba[i].X + 32) {
								//�X���C���̉E�ɓ����������̏���
								Player.Death = 1;
							}
						}
					}
					//�㉺
					if (Player.Death == 0) {
						if (Player.Y + 48 > Goomba[i].Y && Player.Y < Goomba[i].Y + 32 && Player.X + 32 + Goomba[i].Xspeed > Goomba[i].X + Player.Xspeed + Goomba[i].Xspeed && Player.X < Goomba[i].X + 32 + Player.Xspeed) {
							if (Player.Y + 24 <= Goomba[i].Y + 32 && !HumiFlag) {
								//�X���C���ɏ���Ă���Ƃ��̏���
								Player.Yspeed = -17 - abs(Player.Xspeed) / 4;
								Player.Y = Goomba[i].Y - 48;
								Goomba[i].Death = 1;
								PlaySoundMem(humi, DX_PLAYTYPE_BACK, TRUE);
								HumiFlag = true;
							}
							if (Player.Y + 24 > Goomba[i].Y + 32) {
								//�X���C���ɓ����Ԃ������̏���
								Player.Death = 1;
							}
						}
					}
				}
			}
			//�Ԃ��
			if (Goomba[i].Death > 0 && Goomba[i].Death <= 16) Goomba[i].Death++;
			//�o�Ă���
			if (Goomba[i].Death < 0) Goomba[i].Death++;
		}
		/*�e���e��*/	for (int i = 0; i < SunnyNum; i++) {
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
		/*�ŃL�m�R*/	for (int i = 0; i < PoisonNum; i++) {
			if (Poison[i].X > Scroll - 32 && Poison[i].X < Scroll + 640 + 32 * 4 && Poison[i].Y < 480) {
				if (Poison[i].Death == 0) {
					if (Poison[i].Yspeed > 64) Poison[i].Yspeed = 64;
					Poison[i].X += Poison[i].Xspeed;
					Poison[i].Y += ++Poison[i].Yspeed;
					//�X���C���E�u���b�N
					Poison[i].GroundFlag = false;
					//�u���b�N�̏㉺�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Poison[i].Y + 32 >= 32 * j && Poison[i].Y < 32 * j + 32 && Poison[i].X + 32 > 32 * k + Poison[i].Xspeed && Poison[i].X < 32 * k + 32 + Poison[i].Xspeed) {
									if (Poison[i].Yspeed >= 0) {
										//�u���b�N�ɏ���Ă���Ƃ��̏���
										Poison[i].GroundFlag = true;
										Poison[i].Y = 32 * j - 32;
									}
									if (Poison[i].Yspeed < 0) {
										//�u���b�N�ɓ����Ԃ������̏���
										if (Poison[i].X + 32 > 32 * k + BlockShift && Poison[i].X < 32 * k + 32 - BlockShift) {
											Poison[i].Yspeed = abs(Poison[i].Yspeed);
											Poison[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//�u���b�N�̍��E�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
								|| map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Poison[i].Y + 32 > 32 * j && Poison[i].Y < 32 * j + 32) {
									if (Poison[i].X + 32 > 32 * k && Poison[i].X + 32 <= 32 * k + Poison[i].Xspeed) {
										//�u���b�N�̍��ɓ����������̏���
										Poison[i].X = 32 * k - 32;
										if (Poison[i].GroundFlag) Poison[i].Xspeed = -2;
									}
									if (Poison[i].X >= 32 * k + 32 + Poison[i].Xspeed && Poison[i].X < 32 * k + 32) {
										//�u���b�N�̉E�ɓ����������̏���
										Poison[i].X = 32 * k + 32;
										if (Poison[i].GroundFlag) Poison[i].Xspeed = 2;
									}
									//�u���b�N���炵
									//��
									if (Poison[i].X + 32 > 32 * k && Poison[i].X + 32 <= 32 * k + BlockShift) Poison[i].X = 32 * k - 32;
									//�E
									if (Poison[i].X >= 32 * k + 32 - BlockShift && Poison[i].X < 32 * k + 32) Poison[i].X = 32 * k + 32;
								}
							}
							//�y��
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (Poison[i].Y + 32 > 32 * j && Poison[i].Y < 640) {
									if (Poison[i].X + 32 > 32 * k && Poison[i].X + 32 <= 32 * k + Poison[i].Xspeed) {
										//���ɓ����������̏���
										Poison[i].X = 32 * k - 32;
										if (Poison[i].GroundFlag) Poison[i].Xspeed = -2;
									}
									if (Poison[i].X >= 32 + 32 * k + 32 + Poison[i].Xspeed && Poison[i].X < 32 + 32 * k + 32) {
										//�E�ɓ����������̏���
										Poison[i].X = 32 + 32 * k + 32;
										if (Poison[i].GroundFlag) Poison[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//�L�m�R����
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (Poison[i].X + 32 > 32 * k && Poison[i].X < 32 * k + 32) {
								if (Poison[i].Y - Poison[i].Yspeed + 32 <= 32 * j && Poison[i].Y + 32 > 32 * j) {
									//�u���b�N�ɏ���Ă���Ƃ��̏���
									Poison[i].GroundFlag = true;
									Poison[i].Y = 32 * j - 32;
								}
							}
						}
					}
					//�n�ʂɂ�����c�ɓ����Ȃ�����
					if (Poison[i].GroundFlag && Poison[i].Death == 0) Poison[i].Yspeed = 0;
					//�H�ׂ�Ǝ���
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
		/*�܂����L�m�R*/	for (int i = 0; i < MushroomNum; i++) {
			if (Mushroom[i].X > Scroll - 32 && Mushroom[i].X < Scroll + 640 + 32 * 4 && Mushroom[i].Y < 480) {
				if (Mushroom[i].Death == 0) {
					if (Mushroom[i].Yspeed > 64) Mushroom[i].Yspeed = 64;
					Mushroom[i].X += Mushroom[i].Xspeed;
					Mushroom[i].Y += ++Mushroom[i].Yspeed;
					//�X���C���E�u���b�N
					Mushroom[i].GroundFlag = false;
					//�u���b�N�̏㉺�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Mushroom[i].Y + 32 >= 32 * j && Mushroom[i].Y < 32 * j + 32 && Mushroom[i].X + 32 > 32 * k + Mushroom[i].Xspeed && Mushroom[i].X < 32 * k + 32 + Mushroom[i].Xspeed) {
									if (Mushroom[i].Yspeed >= 0) {
										//�u���b�N�ɏ���Ă���Ƃ��̏���
										Mushroom[i].GroundFlag = true;
										Mushroom[i].Y = 32 * j - 32;
									}
									if (Mushroom[i].Yspeed < 0) {
										//�u���b�N�ɓ����Ԃ������̏���
										if (Mushroom[i].X + 32 > 32 * k + BlockShift && Mushroom[i].X < 32 * k + 32 - BlockShift) {
											Mushroom[i].Yspeed = abs(Mushroom[i].Yspeed);
											Mushroom[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//�u���b�N�̍��E�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
								|| map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Mushroom[i].Y + 32 > 32 * j && Mushroom[i].Y < 32 * j + 32) {
									if (Mushroom[i].X + 32 > 32 * k && Mushroom[i].X + 32 <= 32 * k + Mushroom[i].Xspeed) {
										//�u���b�N�̍��ɓ����������̏���
										Mushroom[i].X = 32 * k - 32;
										if (Mushroom[i].GroundFlag) Mushroom[i].Xspeed = -2;
									}
									if (Mushroom[i].X >= 32 * k + 32 + Mushroom[i].Xspeed && Mushroom[i].X < 32 * k + 32) {
										//�u���b�N�̉E�ɓ����������̏���
										Mushroom[i].X = 32 * k + 32;
										if (Mushroom[i].GroundFlag) Mushroom[i].Xspeed = 2;
									}
									//�u���b�N���炵
									//��
									if (Mushroom[i].X + 32 > 32 * k && Mushroom[i].X + 32 <= 32 * k + BlockShift) Mushroom[i].X = 32 * k - 32;
									//�E
									if (Mushroom[i].X >= 32 * k + 32 - BlockShift && Mushroom[i].X < 32 * k + 32) Mushroom[i].X = 32 * k + 32;
								}
							}
							//�y��
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (Mushroom[i].Y + 32 > 32 * j && Mushroom[i].Y < 640) {
									if (Mushroom[i].X + 32 > 32 * k && Mushroom[i].X + 32 <= 32 * k + Mushroom[i].Xspeed) {
										//���ɓ����������̏���
										Mushroom[i].X = 32 * k - 32;
										if (Mushroom[i].GroundFlag) Mushroom[i].Xspeed = -2;
									}
									if (Mushroom[i].X >= 32 + 32 * k + 32 + Mushroom[i].Xspeed && Mushroom[i].X < 32 + 32 * k + 32) {
										//�E�ɓ����������̏���
										Mushroom[i].X = 32 + 32 * k + 32;
										if (Mushroom[i].GroundFlag) Mushroom[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//�L�m�R����
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (Mushroom[i].X + 32 > 32 * k && Mushroom[i].X < 32 * k + 32) {
								if (Mushroom[i].Y - Mushroom[i].Yspeed + 32 <= 32 * j && Mushroom[i].Y + 32 > 32 * j) {
									//�u���b�N�ɏ���Ă���Ƃ��̏���
									Mushroom[i].GroundFlag = true;
									Mushroom[i].Y = 32 * j - 32;
								}
							}
						}
					}
					//�n�ʂɂ�����c�ɓ����Ȃ�����
					if (Mushroom[i].GroundFlag && Mushroom[i].Death == 0) Mushroom[i].Yspeed = 0;
					//�H�ׂ�Ǝ���
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
		/*�L�m�R*/	for (int i = 0; i < BigMushroomNum; i++) {
			if (BigMushroom[i].X > Scroll - 32 && BigMushroom[i].X < Scroll + 640 + 32 * 4 && BigMushroom[i].Y < 480) {
				if (BigMushroom[i].Death == 0) {
					if (BigMushroom[i].Yspeed > 64) BigMushroom[i].Yspeed = 64;
					BigMushroom[i].X += BigMushroom[i].Xspeed;
					BigMushroom[i].Y += ++BigMushroom[i].Yspeed;
					//�X���C���E�u���b�N
					BigMushroom[i].GroundFlag = false;
					//�u���b�N�̏㉺�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (BigMushroom[i].Y + 32 >= 32 * j && BigMushroom[i].Y < 32 * j + 32 && BigMushroom[i].X + 32 > 32 * k + BigMushroom[i].Xspeed && BigMushroom[i].X < 32 * k + 32 + BigMushroom[i].Xspeed) {
									if (BigMushroom[i].Yspeed >= 0) {
										//�u���b�N�ɏ���Ă���Ƃ��̏���
										BigMushroom[i].GroundFlag = true;
										BigMushroom[i].Y = 32 * j - 32;
									}
									if (BigMushroom[i].Yspeed < 0) {
										//�u���b�N�ɓ����Ԃ������̏���
										if (BigMushroom[i].X + 32 > 32 * k + BlockShift && BigMushroom[i].X < 32 * k + 32 - BlockShift) {
											BigMushroom[i].Yspeed = abs(BigMushroom[i].Yspeed);
											BigMushroom[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//�u���b�N�̍��E�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
								|| map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (BigMushroom[i].Y + 32 > 32 * j && BigMushroom[i].Y < 32 * j + 32) {
									if (BigMushroom[i].X + 32 > 32 * k && BigMushroom[i].X + 32 <= 32 * k + BigMushroom[i].Xspeed) {
										//�u���b�N�̍��ɓ����������̏���
										BigMushroom[i].X = 32 * k - 32;
										if (BigMushroom[i].GroundFlag) BigMushroom[i].Xspeed = -2;
									}
									if (BigMushroom[i].X >= 32 * k + 32 + BigMushroom[i].Xspeed && BigMushroom[i].X < 32 * k + 32) {
										//�u���b�N�̉E�ɓ����������̏���
										BigMushroom[i].X = 32 * k + 32;
										if (BigMushroom[i].GroundFlag) BigMushroom[i].Xspeed = 2;
									}
									//�u���b�N���炵
									//��
									if (BigMushroom[i].X + 32 > 32 * k && BigMushroom[i].X + 32 <= 32 * k + BlockShift) BigMushroom[i].X = 32 * k - 32;
									//�E
									if (BigMushroom[i].X >= 32 * k + 32 - BlockShift && BigMushroom[i].X < 32 * k + 32) BigMushroom[i].X = 32 * k + 32;
								}
							}
							//�y��
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (BigMushroom[i].Y + 32 > 32 * j && BigMushroom[i].Y < 640) {
									if (BigMushroom[i].X + 32 > 32 * k && BigMushroom[i].X + 32 <= 32 * k + BigMushroom[i].Xspeed) {
										//���ɓ����������̏���
										BigMushroom[i].X = 32 * k - 32;
										if (BigMushroom[i].GroundFlag) BigMushroom[i].Xspeed = -2;
									}
									if (BigMushroom[i].X >= 32 + 32 * k + 32 + BigMushroom[i].Xspeed && BigMushroom[i].X < 32 + 32 * k + 32) {
										//�E�ɓ����������̏���
										BigMushroom[i].X = 32 + 32 * k + 32;
										if (BigMushroom[i].GroundFlag) BigMushroom[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//�L�m�R����
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (BigMushroom[i].X + 32 > 32 * k && BigMushroom[i].X < 32 * k + 32) {
								if (BigMushroom[i].Y - BigMushroom[i].Yspeed + 32 <= 32 * j && BigMushroom[i].Y + 32 > 32 * j) {
									//�u���b�N�ɏ���Ă���Ƃ��̏���
									BigMushroom[i].GroundFlag = true;
									BigMushroom[i].Y = 32 * j - 32;
								}
							}
						}
					}
					//�n�ʂɂ�����c�ɓ����Ȃ�����
					if (BigMushroom[i].GroundFlag && BigMushroom[i].Death == 0) BigMushroom[i].Yspeed = 0;
					//�H�ׂ�Ƃł����Ȃ�
					//�v���C���[
					if (Player.X + 32 > BigMushroom[i].X && Player.X < BigMushroom[i].X + 32 && Player.Y + 48 > BigMushroom[i].Y && Player.Y < BigMushroom[i].Y + 32) {
						Player.Big = true;
						BigMushroom[i].Death = 1;
					}
					//�X���C��
					for (int j = 0; j < GoombaNum; j++) {
						if (Goomba[j].Death == 0 && Goomba[j].X + 32 > BigMushroom[i].X && Goomba[j].X < BigMushroom[i].X + 32 && Goomba[j].Y + 32 > BigMushroom[i].Y && Goomba[j].Y < BigMushroom[i].Y + 32) {
							Goomba[j].Big = true;
							BigMushroom[i].Death = 1;
						}
					}
					//�Ƃ��X���C��
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
		/*�X�^�[*/	for (int i = 0; i < StarNum; i++) {
			if (Star[i].X > Scroll - 32 && Star[i].X < Scroll + 640 + 32 * 4 && Star[i].Y < 480) {
				if (Star[i].Death == 0) {
					if (Star[i].Yspeed > 64) Star[i].Yspeed = 64;
					Star[i].X += Star[i].Xspeed;
					Star[i].Y += ++Star[i].Yspeed;
					//�X���C���E�u���b�N
					Star[i].GroundFlag = false;
					//�u���b�N�̏㉺�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Star[i].Y + 32 >= 32 * j && Star[i].Y < 32 * j + 32 && Star[i].X + 32 > 32 * k + Star[i].Xspeed && Star[i].X < 32 * k + 32 + Star[i].Xspeed) {
									if (Star[i].Yspeed >= 0) {
										//�u���b�N�ɏ���Ă���Ƃ��̏���
										Star[i].GroundFlag = true;
										Star[i].Y = 32 * j - 32;
									}
									if (Star[i].Yspeed < 0) {
										//�u���b�N�ɓ����Ԃ������̏���
										if (Star[i].X + 32 > 32 * k + BlockShift && Star[i].X < 32 * k + 32 - BlockShift) {
											Star[i].Yspeed = abs(Star[i].Yspeed);
											Star[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//�u���b�N�̍��E�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
								|| map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (Star[i].Y + 32 > 32 * j && Star[i].Y < 32 * j + 32) {
									if (Star[i].X + 32 > 32 * k && Star[i].X + 32 <= 32 * k + Star[i].Xspeed) {
										//�u���b�N�̍��ɓ����������̏���
										Star[i].X = 32 * k - 32;
										Star[i].Xspeed = -2;
									}
									if (Star[i].X >= 32 * k + 32 + Star[i].Xspeed && Star[i].X < 32 * k + 32) {
										//�u���b�N�̉E�ɓ����������̏���
										Star[i].X = 32 * k + 32;
										Star[i].Xspeed = 2;
									}
									//�u���b�N���炵
									//��
									if (Star[i].X + 32 > 32 * k && Star[i].X + 32 <= 32 * k + BlockShift) Star[i].X = 32 * k - 32;
									//�E
									if (Star[i].X >= 32 * k + 32 - BlockShift && Star[i].X < 32 * k + 32) Star[i].X = 32 * k + 32;
								}
							}
							//�y��
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (Star[i].Y + 32 > 32 * j && Star[i].Y < 640) {
									if (Star[i].X + 32 > 32 * k && Star[i].X + 32 <= 32 * k + Star[i].Xspeed) {
										//���ɓ����������̏���
										Star[i].X = 32 * k - 32;
										Star[i].Xspeed = -2;
									}
									if (Star[i].X >= 32 + 32 * k + 32 + Star[i].Xspeed && Star[i].X < 32 + 32 * k + 32) {
										//�E�ɓ����������̏���
										Star[i].X = 32 + 32 * k + 32;
										Star[i].Xspeed = 2;
									}
								}
							}
						}
					}
					//�L�m�R����
					for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
						if (map(j, k) == 34) {
							if (Star[i].X + 32 > 32 * k && Star[i].X < 32 * k + 32) {
								if (Star[i].Y - Star[i].Yspeed + 32 <= 32 * j && Star[i].Y + 32 > 32 * j) {
									//�u���b�N�ɏ���Ă���Ƃ��̏���
									Star[i].GroundFlag = true;
									Star[i].Y = 32 * j - 32;
								}
							}
						}
					}
					//�n�ʂɂ�����W�����v
					if (Star[i].GroundFlag && Star[i].Death == 0) Star[i].Yspeed = -15;
					//�H�ׂ�Ǝ���
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
		/*�T*/	for (int i = 0; i < KoopaNum; i++) {
			if (Koopa[i].Death >= 0 && Koopa[i].Death <= 1 && Koopa[i].X > Scroll - 320 && Koopa[i].X < Scroll + 640 + 32 * 4 && Koopa[i].Y < 480) {
				if (Koopa[i].Yspeed > 64) Koopa[i].Yspeed = 64;
				Koopa[i].X += Koopa[i].Xspeed;
				Koopa[i].Y += ++Koopa[i].Yspeed;
				//�T�E�u���b�N
				Koopa[i].GroundFlag = false;
				//�u���b�N�̏㉺�̓����蔻��
				for (int j = 0; j < 15; j++) {
					for (int k = 0; k < MapXSize; k++) {
						//�u���b�N
						if (
							map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
							|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
							|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
							|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
							|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
							|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
							if (Koopa[i].Y + 32 >= 32 * j && Koopa[i].Y < 32 * j + 32 && Koopa[i].X + 32 > 32 * k + Koopa[i].Xspeed && Koopa[i].X < 32 * k + 32 + Koopa[i].Xspeed) {
								if (Koopa[i].Yspeed >= 0) {
									//�u���b�N�ɏ���Ă���Ƃ��̏���
									Koopa[i].GroundFlag = true;
									Koopa[i].Y = 32 * j - 32;
								}
								if (Koopa[i].Yspeed < 0) {
									//�u���b�N�ɓ����Ԃ������̏���
									if (Koopa[i].X + 32 > 32 * k + BlockShift && Koopa[i].X < 32 * k + 32 - BlockShift) {
										Koopa[i].Yspeed = abs(Koopa[i].Yspeed);
										Koopa[i].Y = 32 * j + 32;
									}
								}
							}
						}
					}
				}
				//�u���b�N�̍��E�̓����蔻��
				for (int j = 0; j < 15; j++) {
					for (int k = 0; k < MapXSize; k++) {
						//�u���b�N
						if (
							map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 6 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
							|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 16 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26
							|| map(j, k) == 27
							|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j, k) == 28) || (j >= 1 && k >= 2 && map(j, k - 2) == 28)
							|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j, k) == 29) || (j >= 1 && k >= 2 && map(j, k - 2) == 29)
							|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
							if (Koopa[i].Y + 32 > 32 * j && Koopa[i].Y < 32 * j + 32) {
								if (Koopa[i].X + 32 > 32 * k && Koopa[i].X + 32 <= 32 * k + Koopa[i].Xspeed) {
									//�u���b�N�̍��ɓ����������̏���
									Koopa[i].X = 32 * k - 32;
									Koopa[i].Xspeed = -Koopa[i].Xspeed;
									//����̃u���b�N�̕ǂɂ���������ۂ�I
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
									//�u���b�N�̉E�ɓ����������̏���
									Koopa[i].X = 32 * k + 32;
									Koopa[i].Xspeed = -Koopa[i].Xspeed;
									//����̃u���b�N�̕ǂɂ���������ۂ�I
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
								//�u���b�N���炵
								//��
								if (Koopa[i].X + 32 > 32 * k && Koopa[i].X + 32 <= 32 * k + BlockShift) Koopa[i].X = 32 * k - 32;
								//�E
								if (Koopa[i].X >= 32 * k + 32 - BlockShift && Koopa[i].X < 32 * k + 32) Koopa[i].X = 32 * k + 32;
							}
						}
						//�y��
						if (map(j, k) == 8 || map(j, k) == 9) {
							if (Koopa[i].Y + 32 > 32 * j && Koopa[i].Y < 640) {
								if (Koopa[i].X + 32 > 32 * k && Koopa[i].X + 32 <= 32 * k + Koopa[i].Xspeed) {
									//���ɓ����������̏���
									Koopa[i].X = 32 * k - 32;
									if (Koopa[i].GroundFlag) Koopa[i].Xspeed = -Koopa[i].Xspeed;
								}
								if (Koopa[i].X >= 32 + 32 * k + 32 + Koopa[i].Xspeed && Koopa[i].X < 32 + 32 * k + 32) {
									//�E�ɓ����������̏���
									Koopa[i].X = 32 + 32 * k + 32;
									if (Koopa[i].GroundFlag) Koopa[i].Xspeed = -Koopa[i].Xspeed;
								}
							}
						}
						//�����K�u���b�N���󂳂ꂽ
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
						//�n�e�i�u���b�N���@���ꂽ
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
						//�X���C���u���b�N���@���ꂽ
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
						//�ŃL�m�R�u���b�N���@���ꂽ
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
						//�܂����L�m�R�u���b�N���@���ꂽ
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
						//10�R�C�������K�u���b�N���@���ꂽ
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
						//�܂����L�m�R�u���b�N���@���ꂽ
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
				//�L�m�R����
				for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
					if (map(j, k) == 34) {
						if (Koopa[i].X + 32 > 32 * k && Koopa[i].X < 32 * k + 32) {
							if (Koopa[i].Y - Koopa[i].Yspeed + 32 <= 32 * j && Koopa[i].Y + 32 > 32 * j) {
								//�u���b�N�ɏ���Ă���Ƃ��̏���
								Koopa[i].GroundFlag = true;
								Koopa[i].Y = 32 * j - 32;
							}
						}
					}
				}
				//�n�ʂɂ�����c�ɓ����Ȃ�����
				if (Koopa[i].GroundFlag && Koopa[i].Death >= 0 && Koopa[i].Death <= 1) Koopa[i].Yspeed = 0;
				//�T�E�v���C���[
				//���E
				if (Player.Death == 0) {
					if (Player.Y + 32 > Koopa[i].Y && Player.Y < Koopa[i].Y + 32) {
						if (Player.X + 32 >= Koopa[i].X && Player.X + 32 + Koopa[i].Xspeed <= Koopa[i].X + Player.Xspeed) {
							//�T�̍��ɓ����������̏���
							if (Koopa[i].Death == 0) Player.Death = 1;
							else if (Koopa[i].Xspeed == 0) {
								Koopa[i].Xspeed = 8;
								PlaySoundMem(koura, DX_PLAYTYPE_BACK);
							}
							else Player.Death = 1;
						}
						if (Player.X + Koopa[i].Xspeed >= Koopa[i].X + 32 + Player.Xspeed && Player.X <= Koopa[i].X + 32) {
							//�T�̉E�ɓ����������̏���
							if (Koopa[i].Death == 0) Player.Death = 1;
							else if (Koopa[i].Xspeed == 0) {
								Koopa[i].Xspeed = -8;
								PlaySoundMem(koura, DX_PLAYTYPE_BACK);
							}
							else Player.Death = 1;
						}
					}
				}
				//�㉺
				if (Player.Death == 0) {
					if (Player.Y + 48 > Koopa[i].Y && Player.Y < Koopa[i].Y + 32 && Player.X + 32 + Koopa[i].Xspeed > Koopa[i].X + Player.Xspeed + Koopa[i].Xspeed && Player.X < Koopa[i].X + 32 + Player.Xspeed) {
						if (Player.Y + 24 <= Koopa[i].Y + 32 && !HumiFlag) {
							//�T�ɏ���Ă���Ƃ��̏���
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
							//�T�ɓ����Ԃ������̏���
							Player.Death = 1;
						}
					}
				}
				if (abs(Koopa[i].Xspeed) > 5) {
					//�T�E�X���C��
					for (int j = 0; j < GoombaNum; j++) {
						if (Goomba[j].Death == 0 && Koopa[i].X < Goomba[j].X + 32 && Koopa[i].X + 32 > Goomba[j].X && Koopa[i].Y < Goomba[j].Y + 32 && Koopa[i].Y + 32 > Goomba[j].Y) {
							Goomba[j].Death = 17;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//�T�E�e���e��
					for (int j = 0; j < SunnyNum; j++) {
						if (Sunny[j].Death == 0 && Koopa[i].X < Sunny[j].X + 32 && Koopa[i].X + 32 > Sunny[j].X && Koopa[i].Y < Sunny[j].Y + 48 && Koopa[i].Y + 32 > Sunny[j].Y) {
							Sunny[j].Death = 1;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//�T�E�ŃL�m�R
					for (int j = 0; j < PoisonNum; j++) {
						if (Poison[j].Death == 0 && Koopa[i].X < Poison[j].X + 32 && Koopa[i].X + 32 > Poison[j].X && Koopa[i].Y < Poison[j].Y + 48 && Koopa[i].Y + 32 > Poison[j].Y) {
							Poison[j].Death = 1;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//�T�E�܂����L�m�R
					for (int j = 0; j < MushroomNum; j++) {
						if (Mushroom[j].Death == 0 && Koopa[i].X < Mushroom[j].X + 32 && Koopa[i].X + 32 > Mushroom[j].X && Koopa[i].Y < Mushroom[j].Y + 48 && Koopa[i].Y + 32 > Mushroom[j].Y) {
							Mushroom[j].Death = 1;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//�T�E�X�^�[
					for (int j = 0; j < StarNum; j++) {
						if (Star[j].Death == 0 && Koopa[i].X < Star[j].X + 32 && Koopa[i].X + 32 > Star[j].X && Koopa[i].Y < Star[j].Y + 48 && Koopa[i].Y + 32 > Star[j].Y) {
							Star[j].Death = 1;
							PlaySoundMem(koura, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					//�T�E�T
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
			//�o�Ă���
			if (Koopa[i].Death < 0) Koopa[i].Death++;
		}
		/*�����郌���K*/	for (int i = 0; i < FallingBrickNum; i++) {
			if (FallingBrick[i].Death == 0) {
				if (Player.Death == 0 && Player.X + 32 > FallingBrick[i].X && Player.X < FallingBrick[i].X + 96 && Player.Y + 48 > FallingBrick[i].Y && Player.Y < FallingBrick[i].Y + 32) {
					Player.Death = 1;
					FallingBrick[i].Death = -1;
				}
				FallingBrick[i].Y += FallingBrick[i].Yspeed++;
				if (FallingBrick[i].Y > 480 + 64) FallingBrick[i].Death = 1;
			}
		}
		/*���Ƃ���*/ {
			//������
			for (int i = 0; i < HoleNum; i++) {
				if (Hole[i].Yspeed > 0) {
					Hole[i].Y += Hole[i].Yspeed++;
				}
			}
			//�v���C���[�Ƃ̓����蔻��
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
			//�A��
			for (int i = 0; i < HoleNum; i++) {				//����Ԃ����ׂ�
				for (int j = 0; j < HoleNum; j++) {			//�S���̃z�[���𒲂ׂ�
					if (Hole[j].Yspeed == 1) {
						for (int k = 0; k < HoleNum; k++) {	//����̂��
							if (Hole[k].Yspeed == 0) {
								//��
								if (Hole[k].X == Hole[j].X && Hole[k].Y - 32 == Hole[j].Y) {
									Hole[k].Yspeed = 1;
								}
								//��
								if (Hole[k].X == Hole[j].X && Hole[k].Y + 32 == Hole[j].Y) {
									Hole[k].Yspeed = 1;
								}
								//��
								if (Hole[k].X - 32 == Hole[j].X && Hole[k].Y == Hole[j].Y) {
									Hole[k].Yspeed = 1;
								}
								//�E
								if (Hole[k].X + 32 == Hole[j].X && Hole[k].Y == Hole[j].Y) {
									Hole[k].Yspeed = 1;
								}
							}
						}
					}
				}
			}
		}
		/*�r�[��*/	for (int i = 0; i < BeamNum; i++) {
			if (Beam[i].Death == 0) {
				Beam[i].X -= 32;
				if (Player.X + 32 > Beam[i].X && Player.X < Beam[i].X + 128 && Player.Y + 48 > Beam[i].Y + 8 && Player.Y < Beam[i].Y + 24) {
					Player.Death = 1;
				}
				if (Beam[i].X < Scroll - 256) Beam[i].Death = 1;
			}
		}
		/*�Ƃ��X���C��*/	for (int i = 0; i < GoombaThornsNum; i++) {
			if (GoombaThorns[i].Death == 0 && GoombaThorns[i].X > Scroll - 320 && GoombaThorns[i].X < Scroll + 640 + 32 * 4) {
				if (GoombaThorns[i].Yspeed > 64) GoombaThorns[i].Yspeed = 64;
				GoombaThorns[i].X += GoombaThorns[i].Xspeed;
				GoombaThorns[i].Y += ++GoombaThorns[i].Yspeed;
				//�Ƃ��X���C���E�u���b�N
				GoombaThorns[i].GroundFlag = false;
				//�ł����Ɖ�
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
					//�u���b�N�̏㉺�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 8 || (k >= 1 && map(j, k - 1) == 8)
								|| map(j, k) == 9 || (k >= 1 && map(j, k - 1) == 9) || map(j, k) == 10 || map(j, k) == 11 || (k >= 1 && map(j, k - 1) == 11) || (k >= 2 && map(j, k - 2) == 11) || map(j, k) == 13
								|| map(j, k) % 1000 == 14 || map(j, k) == 17 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 1 && map(j, k - 1) == 28) || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 1 && map(j, k - 1) == 29) || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 1 && map(j - 1, k - 1) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (GoombaThorns[i].Y + 32 >= 32 * j && GoombaThorns[i].Y < 32 * j + 32 && GoombaThorns[i].X + 32 > 32 * k + GoombaThorns[i].Xspeed && GoombaThorns[i].X < 32 * k + 32 + GoombaThorns[i].Xspeed) {
									if (GoombaThorns[i].Yspeed >= 0) {
										//�u���b�N�ɏ���Ă���Ƃ��̏���
										GoombaThorns[i].GroundFlag = true;
										GoombaThorns[i].Y = 32 * j - 32;
									}
									if (GoombaThorns[i].Yspeed < 0) {
										//�u���b�N�ɓ����Ԃ������̏���
										if (GoombaThorns[i].X + 32 > 32 * k + BlockShift && GoombaThorns[i].X < 32 * k + 32 - BlockShift) {
											GoombaThorns[i].Yspeed = abs(GoombaThorns[i].Yspeed);
											GoombaThorns[i].Y = 32 * j + 32;
										}
									}
								}
							}
						}
					}
					//�u���b�N�̍��E�̓����蔻��
					for (int j = 0; j < 15; j++) {
						for (int k = 0; k < MapXSize; k++) {
							//�u���b�N
							if (
								map(j, k) == 1 || map(j, k) == 2 || map(j, k) == 3 || map(j, k) == 4 || map(j, k) == 5 || map(j, k) == 10 || map(j, k) == 11 || (k >= 2 && map(j, k - 2) == 11)
								|| map(j, k) == 13 || map(j, k) % 1000 == 14 || map(j, k) == 18 || map(j, k) == 20 || map(j, k) == 21 || map(j, k) == 23 || map(j, k) == 26 || map(j, k) == 27
								|| map(j, k) == 28 || (k >= 2 && map(j, k - 2) == 28) || (j >= 1 && map(j - 1, k) == 28) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 28)
								|| map(j, k) == 29 || (k >= 2 && map(j, k - 2) == 29) || (j >= 1 && map(j - 1, k) == 29) || (j >= 1 && k >= 2 && map(j - 1, k - 2) == 29)
								|| map(j, k) == 30 || map(j, k) == 31 || map(j, k) == 33) {
								if (GoombaThorns[i].Y + 32 > 32 * j && GoombaThorns[i].Y < 32 * j + 32) {
									if (GoombaThorns[i].X + 32 > 32 * k && GoombaThorns[i].X + 32 <= 32 * k + GoombaThorns[i].Xspeed) {
										//�u���b�N�̍��ɓ����������̏���
										GoombaThorns[i].X = 32 * k - 32;
										if (GoombaThorns[i].GroundFlag) GoombaThorns[i].Xspeed = -2;
									}
									if (GoombaThorns[i].X >= 32 * k + 32 + GoombaThorns[i].Xspeed && GoombaThorns[i].X < 32 * k + 32) {
										//�u���b�N�̉E�ɓ����������̏���
										GoombaThorns[i].X = 32 * k + 32;
										if (GoombaThorns[i].GroundFlag) GoombaThorns[i].Xspeed = 2;
									}
									//�u���b�N���炵
									//��
									if (GoombaThorns[i].X + 32 > 32 * k && GoombaThorns[i].X + 32 <= 32 * k + BlockShift) GoombaThorns[i].X = 32 * k - 32;
									//�E
									if (GoombaThorns[i].X >= 32 * k + 32 - BlockShift && GoombaThorns[i].X < 32 * k + 32) GoombaThorns[i].X = 32 * k + 32;
								}
							}
							//�y��
							if (map(j, k) == 8 || map(j, k) == 9) {
								if (GoombaThorns[i].Y + 32 > 32 * j && GoombaThorns[i].Y < 640) {
									if (GoombaThorns[i].X + 32 > 32 * k && GoombaThorns[i].X + 32 <= 32 * k + GoombaThorns[i].Xspeed) {
										//���ɓ����������̏���
										GoombaThorns[i].X = 32 * k - 32;
										if (GoombaThorns[i].GroundFlag) GoombaThorns[i].Xspeed = -2;
									}
									if (GoombaThorns[i].X >= 32 + 32 * k + 32 + GoombaThorns[i].Xspeed && GoombaThorns[i].X < 32 + 32 * k + 32) {
										//�E�ɓ����������̏���
										GoombaThorns[i].X = 32 + 32 * k + 32;
										if (GoombaThorns[i].GroundFlag) GoombaThorns[i].Xspeed = 2;
									}
								}
							}
						}
					}
				}
				//�L�m�R����
				for (int j = 0; j < 15; j++) for (int k = 0; k < MapXSize; k++) {
					if (map(j, k) == 34) {
						if (GoombaThorns[i].X + 32 > 32 * k && GoombaThorns[i].X < 32 * k + 32) {
							if (GoombaThorns[i].Y - GoombaThorns[i].Yspeed + 32 <= 32 * j && GoombaThorns[i].Y + 32 > 32 * j) {
								//�u���b�N�ɏ���Ă���Ƃ��̏���
								GoombaThorns[i].GroundFlag = true;
								GoombaThorns[i].Y = 32 * j - 32;
							}
						}
					}
				}
				//�n�ʂɂ�����c�ɓ����Ȃ�����
				if (GoombaThorns[i].GroundFlag && GoombaThorns[i].Death == 0) GoombaThorns[i].Yspeed = 0;
				//�Ƃ��X���C���E�v���C���[
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
			//�o�Ă���
			if (GoombaThorns[i].Death < 0) GoombaThorns[i].Death++;
		}
		/*���t�g*/	for (int i = 0; i < LiftNum; i++) {
			if (!(Lift[i].Death == -2 && Lift[i].Yspeed != 0)) {
				Lift[i].Y += Lift[i].Yspeed;
				if (Lift[i].Death == -1 && Lift[i].Yspeed > 0) {
					++Lift[i].Yspeed;
					if (Lift[i].Yspeed >= 8) Lift[i].Yspeed = 7;
				}
				if (Lift[i].Death == 0 && Lift[i].Yspeed < 0 && Lift[i].Y < -8) Lift[i].Y = 640;
				if (Lift[i].Death == 0 && Lift[i].Yspeed > 0 && Lift[i].Y > 640) Lift[i].Y = -8;
				//�����蔻��
				if (Player.X + 32 > Lift[i].X && Player.X < Lift[i].X + 32 * 4) {
					if (Player.Y - Player.Yspeed + 48 <= Lift[i].Y - Lift[i].Yspeed) {	//��������ɂ���
						if (Player.Y + 48 >= Lift[i].Y) {								//�����ɂ���
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
		/*�u���b�N�̔j��*/	for (int i = 0; i < 4 * 64; i++) {
			if (Debris[i].Death == 0) {
				if (Debris[i].Yspeed > 64) Debris[i].Yspeed = 64;
				Debris[i].X += Debris[i].Xspeed;
				Debris[i].Y += ++Debris[i].Yspeed;
				if (Debris[i].Y > 480) Debris[i].Death = 1;
			}
		}
		/*�R�C���`�����[��*/	for (int i = 0; i < 16; i++) {
			if (FloatCoin[i].Death <= 0) {
				FloatCoin[i].Y -= 6;
				FloatCoin[i].Death++;
			}
		}
		//�f�o�b�O���[�v
		if (DebugModeFlag && (GetMouseInput() & MOUSE_INPUT_LEFT)) {
			GetMousePoint(&Player.X, &Player.Y);
			Player.X -= 16;
			Player.Y -= 24;
			Player.X += Scroll;
			Player.Yspeed = 0;
		}
		//�f�o�b�O�L��
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

		// �`�� //----////----////----////----////----////----////----//

		ClearDrawScreen();
		//�y�ǂ̃O���f
		if (Player.Death < -48) SetDrawBright(-(Player.Death + 48) * 8 - 64, -(Player.Death + 48) * 8 - 64, -(Player.Death + 48) * 8 - 64);
		else if (Player.Death < 0) SetDrawBright(-Player.Death * 8 - 64, -Player.Death * 8 - 64, -Player.Death * 8 - 64);
		//�T�[�N���P
		if (Circle < 64) SetDrawArea(Player.X - Scroll + 16 - Circle * 32, Player.Y + 24 - Circle * 32, Player.X - Scroll + 16 + Circle * 32, Player.Y + 24 + Circle * 32);
		//�w�i
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
		//�L�m�R����
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
		//�S�[��
		for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
			if (map(i, j) == 17) DrawGraph(32 * j - Scroll, 32 * (i - 10), GoalPNG, TRUE);
		}
		//�v���C���[
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
		//�X���C��
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
		//�e���e��
		for (int i = 0; i < SunnyNum; i++) {
			if (Sunny[i].Death == 0) {
				if (Sunny[i].Yspeed < 0) DrawGraph(Sunny[i].X - Scroll, Sunny[i].Y, SunnyPNG, TRUE);
				else DrawRotaGraph(Sunny[i].X - Scroll + 16, Sunny[i].Y + 24, 1.0, DX_PI, SunnyPNG, TRUE, TRUE);
			}
		}
		//�ŃL�m�R
		for (int i = 0; i < PoisonNum; i++) {
			if (Poison[i].Death == 0) DrawGraph(Poison[i].X - Scroll, Poison[i].Y, PoisonPNG, TRUE);
			if (Poison[i].Death < 0) DrawGraph(Poison[i].X - Scroll, Poison[i].Y - Poison[i].Death * 2, PoisonPNG, TRUE);
		}
		//�܂����L�m�R
		for (int i = 0; i < MushroomNum; i++) {
			if (Mushroom[i].Death == 0) DrawGraph(Mushroom[i].X - Scroll, Mushroom[i].Y, MushroomPNG, TRUE);
			if (Mushroom[i].Death < 0) DrawGraph(Mushroom[i].X - Scroll, Mushroom[i].Y - Mushroom[i].Death * 2, MushroomPNG, TRUE);
		}
		//�L�m�R
		for (int i = 0; i < BigMushroomNum; i++) {
			if (BigMushroom[i].Death == 0) DrawGraph(BigMushroom[i].X - Scroll, BigMushroom[i].Y, MushroomPNG, TRUE);
			if (BigMushroom[i].Death < 0) DrawGraph(BigMushroom[i].X - Scroll, BigMushroom[i].Y - BigMushroom[i].Death * 2, MushroomPNG, TRUE);
		}
		//�X�^�[
		for (int i = 0; i < StarNum; i++) {
			if (Star[i].Death == 0) DrawGraph(Star[i].X - Scroll, Star[i].Y, StarPNG, TRUE);
			if (Star[i].Death < 0) DrawGraph(Star[i].X - Scroll, Star[i].Y - Star[i].Death * 2, StarPNG, TRUE);
		}
		//�T
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
		//�r�[��
		for (int i = 0; i < BeamNum; i++) {
			if (Beam[i].Death == 0) DrawGraph(Beam[i].X - Scroll, Beam[i].Y, BeamPNG, TRUE);
		}
		//�Ƃ��X���C��
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
		//���t�g
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
		//�u���b�N�̔j��
		for (int i = 0; i < 4 * 64; i++) if (Debris[i].Death == 0) {
			DrawCircle(Debris[i].X + 8 - Scroll, Debris[i].Y + 8, 7, GetColor(143, 96, 48), TRUE);
			DrawCircle(Debris[i].X + 8 - Scroll, Debris[i].Y + 8, 7, GetColor(0, 0, 0), FALSE);
		}
		//�R�C���`�����[��
		for (int i = 0; i < 16; i++) if (FloatCoin[i].Death <= 0) {
			DrawGraph(FloatCoin[i].X - Scroll, FloatCoin[i].Y, CoinPNG, TRUE);
		}
		//���y��
		for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
			if (map(i, j) == 28 || map(i, j) == 29) DrawGraph(32 * j - Scroll, 32 * i, HorizontalPipePNG, TRUE);
		}
		//�u���b�N
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
		//�����郌���K
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
		//�����鏰
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
		//�T�[�N���Q
		if (Circle < 64) DrawExtendGraph(Player.X - Scroll + 16 - Circle * 32, Player.Y + 24 - Circle * 32, Player.X - Scroll + 16 + Circle * 32, Player.Y + 24 + Circle * 32, CirclePNG, TRUE);
		//���낢�냊�Z�b�g
		SetDrawArea(0, 0, 640, 480);
		SetDrawBright(255, 255, 255);
		//�f�o�b�O���
		if (DebugModeFlag && CheckHitKey(KEY_INPUT_F3)) {
			//�}�X
			for (int i = 0; i < 15; i++) for (int j = 0; j < MapXSize; j++) {
				if ((Player.X + 16) / 32 / ChunkSize == j / ChunkSize && j * 32 >= Scroll - 32 && j * 32 <= Scroll + 640) {
					DrawBox(32 * j - Scroll, 32 * i, 32 * j + 32 - Scroll, 32 * i + 32, GetColor(0, 0, 255), FALSE);
					if (map(i, j)) DrawFormatString(4 + 32 * j - Scroll, 14 + 32 * i, GetColor(0, 0, 255), "%d", map(i, j));
				}
			}
			//�`�����N
			for (int i = 0; i < MapDataSize; i++) {
				DrawBox(32 * ChunkSize * i - Scroll, 0, 32 * ChunkSize * (i + 1) - Scroll, 480, GetColor(255, 0, 0), FALSE);
				DrawFormatString(4 + 32 * ChunkSize * i - Scroll, 4, GetColor(255, 0, 0), "%d %s", i, MapDataHint[i]);
			}
			//�X�e�[�^�X
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
		//�c�莞��
		if (CulturalFestivalFlag) {
			DrawBox(0, 0, 8 * 18.5, 16, 0xffffff, TRUE);
			DrawFormatString(0, 0, GetColor(0, 0, 0), "�c�莞�� : %d �b", (Time - GetNowCount()) / 1000);
			if (GetNowCount() > Time) {
				LoadGraphScreen(0, 0, "�摜/�I��.png", FALSE);
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
