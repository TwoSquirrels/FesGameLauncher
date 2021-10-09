#include "DxLib.h"
#include <stdlib.h>
//#include "resource.h"
#define RED 0
#define BLUE 1
int ProcessMessage_and_End() {
	if (ProcessMessage() == -1 || CheckHitKey(KEY_INPUT_ESCAPE == 1)) {
		DxLib_End();
		exit(EXIT_SUCCESS);
	}
	return 0;
}

typedef struct {
	float x, y, x_speed, y_speed;
} ball;

int start(void);
int select(void);
int game(int block_map[], int block_y, int block_x, int mode);

int WINAPI WinMain(HINSTANCE hImst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {

	//���O�t�@�C�����Ȃ�
	SetOutApplicationLogValidFlag(FALSE);
	//�t���X�N���[��Flag
	//switch (MessageBox(NULL, "�t���X�N���[���ŕ\�����܂����H", "�ΐ�^�u���b�N���� ver0.5", (MB_YESNOCANCEL | MB_ICONINFORMATION | MB_DEFBUTTON2))) {
	//case 0:
	//	DxLib_End();
	//	return 0;
	//case IDCANCEL:
	//	DxLib_End();
	//	return 0;
	//case IDNO:
	//	ChangeWindowMode(TRUE);
	//	break;
	//case IDYES:
	//	ChangeWindowMode(FALSE);
	//	break;
	//}
	ChangeWindowMode(TRUE);
	//�w�i�F
	SetBackgroundColor(0, 255, 0);
	//���O�ς���
	SetMainWindowText("�ΐ�^�u���b�N���� ver0.5");
	//�A�C�R���ς���
	//SetWindowIconID(333);
	//�𑜓x�ς���
	SetGraphMode(1136, 640, 16);
	//DxLib�̏�����
	if (DxLib_Init() == -1)return -1;
	//����ʃZ�b�g
	SetDrawScreen(DX_SCREEN_BACK);
	//�S��ʂ��ǂ�
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	int Width = rc.right - rc.left, Height = rc.bottom - rc.top;
	if (Width * 1.0 / Height >= 1136.0 / 640) {
		//���E�ɗ]��
		SetWindowSizeExtendRate(Height * 1.0 / 640);
	}
	else {
		//�㉺�ɗ]��
		SetWindowSizeExtendRate(Width * 1.0 / 1136);
	}
	SetMouseDispFlag(FALSE);
	SetWindowStyleMode(3);

	start();
	game(0, 0, 0, 0);

	DxLib_End();
	return 0;
}


//�X�^�[�g���
int start(void) {
	DrawBox(0, 0, 1136, 640, 0x000000, TRUE);
	//�u���b�N�̕`��
	int block[8][7]{
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 }
	};
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 7; j++) {
			if (block[i][j] == 1) DrawBox(40 * j + 4, 80 * i + 4, 40 * j + 36, 80 * i + 76, GetColor(237, 28, 36), TRUE);
			if (block[i][j] == 2) DrawBox(40 * j + 4, 80 * i + 4, 40 * j + 36, 80 * i + 76, GetColor(255, 127, 39), TRUE);
			if (block[i][j] == 1) DrawBox(40 * j + 860, 80 * i + 4, 40 * j + 892, 80 * i + 76, GetColor(0, 162, 232), TRUE);
			if (block[i][j] == 2) DrawBox(40 * j + 860, 80 * i + 4, 40 * j + 892, 80 * i + 76, GetColor(153, 217, 234), TRUE);
		}
	}
	//�o�[�̕`��
	DrawBox(776, 260, 784, 380, GetColor(255, 0, 0), TRUE);
	DrawBox(352, 260, 360, 380, GetColor(0, 127, 255), TRUE);
	//�t�H���g�ǂݍ���
	int Title1 = CreateFontToHandle(NULL, 32, -1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	int Title2 = CreateFontToHandle(NULL, 64, -1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	//�^�C�g���̕`��
	DrawStringToHandle(512, 32, "�ΐ�^", GetColor(255, 255, 0), Title1, GetColor(0, 0, 255));
	DrawStringToHandle(372, 64, "�u���b�N����", GetColor(0, 255, 0), Title2, GetColor(255, 0, 255));
	//������
	DrawString(360, 240, "\
���[������\n\
����͓�l�p�̃Q�[���ł��B\n\
���̐l�͐��o�[��\n\
w, s�L�[�i�L�[�{�[�h�̍��̕��ɂ���j�œ������A\n\
�E�̐l�͐Ԃ��o�[��\n\
�����L�[�i�E�̂ق��ɂ���j�œ������܂��B\n\
�Ȃ�ׂ��{�[���𑊎�̕��ɑł��Ԃ���\n\
�ŏI�I�Ƀ{�[������ʂ̉E�̕ӂɓ�������Ԃ̏����ŁA\n\
���ɓ�������̏����ł��B\n\
�������i�ނɂ�ǂ�ǂ�o�[���Z���Ȃ��Ă��܂��B\n\
\n\
�Q�l�̏������o������X�y�[�X�L�[�������Ă��������B\n\
����ƃQ�[�����X�^�[�g���܂��I\n\
�ł͊撣���Đ���Ă��������I\n\
", 0xffffff);
	//����ʂ�\��ʂɂ���
	ScreenFlip();
	//�X�y�[�X�L�[�̓��͑҂�
	while (CheckHitKey(KEY_INPUT_SPACE) == 0) {
		ProcessMessage_and_End();
		SetMousePoint(1136 / 2, 640 / 2);
	}
	return 0;
}


//�Q�[�����
int game(int block_map[], int block_y, int block_x, int mode) {

	int Red_bar_y = 320, Blue_bar_y = 320;
	int Red_bar_long_half = 60, Blue_bar_long_half = 60;
	ball one, two;
	one.x = 568, one.x_speed = 2, one.y = 320, one.y_speed = 2;
	two.x = 568, two.x_speed = -2, two.y = 320, two.y_speed = -2;
	int Blue_block_left_x = 856;
	int Red_block[8][7]{
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 }
	};
	int Blue_block[8][7]{
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 },
		{ 1,2,1,2,1,2,1 },
		{ 2,1,2,1,2,1,2 }
	};
	int Red_block_sum, Blue_block_sum;
	int Red_block_count, Blue_block_count;
	int Turbo_flag;
	int win;

	Red_block_sum = 0;
	Blue_block_sum = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 7; j++) {
			if (Red_block[i][j] != 0) Red_block_sum++;
			if (Blue_block[i][j] != 0) Blue_block_sum++;
		}
	}

	while (1) {

		ProcessMessage_and_End();

		SetMousePoint(1136 / 2, 640 / 2);

		//����

		//��������
		if (one.x < 0 || two.x < 0) {
			win = RED;
			break;
		}
		if (one.x > 1136 || two.x > 1136) {
			win = BLUE;
			break;
		}

		//�u���b�N�̎c����𒲂ׂ�
		Red_block_count = 0;
		Blue_block_count = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 7; j++) {
				if (Red_block[i][j] != 0) Red_block_count++;
				if (Blue_block[i][j] != 0) Blue_block_count++;
			}
		}

		//�o�[�̒���
		Red_bar_long_half = 60 * Red_block_count / Red_block_sum;
		Blue_bar_long_half = 60 * Blue_block_count / Blue_block_sum;

		//�o�[�̈ړ�
		if (CheckHitKey(KEY_INPUT_UP) == 1 && CheckHitKey(KEY_INPUT_DOWN) == 0) Red_bar_y -= 5;
		if (CheckHitKey(KEY_INPUT_UP) == 0 && CheckHitKey(KEY_INPUT_DOWN) == 1) Red_bar_y += 5;
		if (CheckHitKey(KEY_INPUT_W) == 1 && CheckHitKey(KEY_INPUT_S) == 0) Blue_bar_y -= 5;
		if (CheckHitKey(KEY_INPUT_W) == 0 && CheckHitKey(KEY_INPUT_S) == 1) Blue_bar_y += 5;

		//�o�[�̏㉺���E
		if (Red_bar_y < Red_bar_long_half) Red_bar_y = Red_bar_long_half;
		if (Red_bar_y > 640 - Red_bar_long_half) Red_bar_y = 640 - Red_bar_long_half;
		if (Blue_bar_y < Blue_bar_long_half) Blue_bar_y = Blue_bar_long_half;
		if (Blue_bar_y > 640 - Blue_bar_long_half) Blue_bar_y = 640 - Blue_bar_long_half;

		//�{�[���̈ړ�
		one.x += one.x_speed;
		one.y += one.y_speed;
		two.x += two.x_speed;
		two.y += two.y_speed;

		//�{�[���̒��˕Ԃ�(�㉺)
		if (one.y < 6) {
			one.y = 12 - one.y;
			one.y_speed = -one.y_speed;
		}
		if (one.y > 634) {
			one.y = 1268 - one.y;
			one.y_speed = -one.y_speed;
		}
		if (two.y < 6) {
			two.y = 12 - two.y;
			two.y_speed = -two.y_speed;
		}
		if (two.y > 634) {
			two.y = 1268 - two.y;
			two.y_speed = -two.y_speed;
		}

		//�{�[���̒��˕Ԃ�(�o�[)
		if (one.x > 346 && one.x < 366 && one.y > Blue_bar_y - Blue_bar_long_half && one.y < Blue_bar_y + Blue_bar_long_half) {
			if (one.x_speed > 0) {
				one.x = 692 - one.x;
				one.x_speed = -one.x_speed - 0.2;
			}
			else {
				one.x = 732 - one.x;
				one.x_speed = -one.x_speed + 0.2;
			}
			one.y_speed += (one.y - Blue_bar_y) * 0.025;
		}
		if (two.x > 346 && two.x < 366 && two.y > Blue_bar_y - Blue_bar_long_half && two.y < Blue_bar_y + Blue_bar_long_half) {
			if (two.x_speed > 0) {
				two.x = 692 - two.x;
				two.x_speed = -two.x_speed - 0.2;
			}
			else {
				two.x = 732 - two.x;
				two.x_speed = -two.x_speed + 0.2;
			}
			two.y_speed += (two.y - Blue_bar_y) * 0.025;
		}
		if (one.x > 770 && one.x < 790 && one.y > Red_bar_y - Red_bar_long_half && one.y < Red_bar_y + Red_bar_long_half) {
			if (one.x_speed > 0) {
				one.x = 1540 - one.x;
				one.x_speed = -one.x_speed - 0.2;
			}
			else {
				one.x = 1580 - one.x;
				one.x_speed = -one.x_speed + 0.2;
			}
			one.y_speed += (one.y - Red_bar_y) * 0.025;
		}
		if (two.x > 770 && two.x < 790 && two.y > Red_bar_y - Red_bar_long_half && two.y < Red_bar_y + Red_bar_long_half) {
			if (two.x_speed > 0) {
				two.x = 1540 - two.x;
				two.x_speed = -two.x_speed - 0.2;
			}
			else {
				two.x = 1580 - two.x;
				two.x_speed = -two.x_speed + 0.2;
			}
			two.y_speed += (two.y - Red_bar_y) * 0.025;
		}

		//�{�[����y�X�s�[�h�̏��
		if (one.y_speed > 5) one.y_speed = 5;
		if (one.y_speed < -5) one.y_speed = -5;
		if (two.y_speed > 5) two.y_speed = 5;
		if (two.y_speed < -5) two.y_speed = -5;

		//�{�[���̒��˕Ԃ�(�u���b�N)
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 7; j++) {
				//�{�[���̒��˕Ԃ�(�ԃu���b�N)
				//ball one
				if (Red_block[i][j] != 0 && one.x > 40 * j - 2 && one.x < 40 * j + 42 && one.y>80 * i - 2 && one.y < 80 * i + 82) {
					if (one.x_speed > 0) {
						if (one.y_speed > 0) {
							//�E�������ɐi��ł���
							//��̕ӂŔ���
							if ((one.x - one.x_speed * (one.y - (80 * i - 2)) / one.y_speed) > 40 * j - 2) {
								one.y = (80 * i - 2) * 2 - one.y;
								one.y_speed = -one.y_speed;
								one.x_speed = 2;
							}
							else {
								one.x = (40 * j - 2) * 2 - one.x;
								one.x_speed = -2;
							}
						}
						else {
							//�E��(�E)�����ɐi��ł���
							//���̕ӂŔ���
							if ((one.x - -one.x_speed * ((80 * i + 82) - one.y) / one.y_speed) > 40 * j - 2) {
								one.y = (80 * i + 82) * 2 - one.y;
								one.y_speed = -one.y_speed;
								one.x_speed = 2;
							}
							else {
								one.x = (40 * j - 2) * 2 - one.x;
								one.x_speed = -2;
							}
						}
					}
					else {
						if (one.y_speed > 0) {
							//���������ɐi��ł���
							//��̕ӂŔ���
							if ((one.x - one.x_speed * (one.y - (80 * i - 2)) / one.y_speed) < 40 * j + 42) {
								one.y = (80 * i - 2) * 2 - one.y;
								one.y_speed = -one.y_speed;
								one.x_speed = -2;
							}
							else {
								one.x = (40 * j + 42) * 2 - one.x;
								one.x_speed = 2;
							}
						}
						else {
							//����(��)�����ɐi��ł���
							//���̕ӂŔ���
							if ((one.x + one.x_speed * ((80 * i + 82) - one.y) / one.y_speed) < 40 * j + 42) {
								one.y = (80 * i + 82) * 2 - one.y;
								one.y_speed = -one.y_speed;
								one.x_speed = -2;
							}
							else {
								one.x = (40 * j + 42) * 2 - one.x;
								one.x_speed = 2;
							}
						}
					}
					Red_block[i][j] = 0;
				}
				//ball two
				if (Red_block[i][j] != 0 && two.x > 40 * j - 2 && two.x < 40 * j + 42 && two.y>80 * i - 2 && two.y < 80 * i + 82) {
					if (two.x_speed > 0) {
						if (two.y_speed > 0) {
							//�E�������ɐi��ł���
							//��̕ӂŔ���
							if ((two.x - two.x_speed * (two.y - (80 * i - 2)) / two.y_speed) > 40 * j - 2) {
								two.y = (80 * i - 2) * 2 - two.y;
								two.y_speed = -two.y_speed;
								two.x_speed = 2;
							}
							else {
								two.x = (40 * j - 2) * 2 - two.x;
								two.x_speed = -2;
							}
						}
						else {
							//�E��(�E)�����ɐi��ł���
							//���̕ӂŔ���
							if ((two.x - -two.x_speed * ((80 * i + 82) - two.y) / two.y_speed) > 40 * j - 2) {
								two.y = (80 * i + 82) * 2 - two.y;
								two.y_speed = -two.y_speed;
								two.x_speed = 2;
							}
							else {
								two.x = (40 * j - 2) * 2 - two.x;
								two.x_speed = -2;
							}
						}
					}
					else {
						if (two.y_speed > 0) {
							//���������ɐi��ł���
							//��̕ӂŔ���
							if ((two.x - two.x_speed * (two.y - (80 * i - 2)) / two.y_speed) < 40 * j + 42) {
								two.y = (80 * i - 2) * 2 - two.y;
								two.y_speed = -two.y_speed;
								two.x_speed = -2;
							}
							else {
								two.x = (40 * j + 42) * 2 - two.x;
								two.x_speed = 2;
							}
						}
						else {
							//����(��)�����ɐi��ł���
							//���̕ӂŔ���
							if ((two.x + two.x_speed * ((80 * i + 82) - two.y) / two.y_speed) < 40 * j + 42) {
								two.y = (80 * i + 82) * 2 - two.y;
								two.y_speed = -two.y_speed;
								two.x_speed = -2;
							}
							else {
								two.x = (40 * j + 42) * 2 - two.x;
								two.x_speed = 2;
							}
						}
					}
					Red_block[i][j] = 0;
				}
				//�{�[���̒��˕Ԃ�(�u���b�N)
				//ball one
				if (Blue_block[i][j] != 0 && one.x > 40 * j + Blue_block_left_x - 2 && one.x < 40 * j + Blue_block_left_x + 42 && one.y > 80 * i - 2 && one.y < 80 * i + 82) {
					if (one.x_speed > 0) {
						if (one.y_speed > 0) {
							//�E�������ɐi��ł���
							//��̕ӂŔ���
							if ((one.x - one.x_speed * (one.y - (80 * i - 2)) / one.y_speed) > 40 * j - 2 + Blue_block_left_x) {
								one.y = (80 * i - 2) * 2 - one.y;
								one.y_speed = -one.y_speed;
								one.x_speed = 2;
							}
							else {
								one.x = (40 * j - 2 + Blue_block_left_x) * 2 - one.x;
								one.x_speed = -2;
							}
						}
						else {
							//�E��(�E)�����ɐi��ł���
							//���̕ӂŔ���
							if ((one.x - -one.x_speed * ((80 * i + 82) - one.y) / one.y_speed) > 40 * j - 2 + Blue_block_left_x) {
								one.y = (80 * i + 82) * 2 - one.y;
								one.y_speed = -one.y_speed;
								one.x_speed = 2;
							}
							else {
								one.x = (40 * j - 2 + Blue_block_left_x) * 2 - one.x;
								one.x_speed = -2;
							}
						}
					}
					else {
						if (one.y_speed > 0) {
							//���������ɐi��ł���
							//��̕ӂŔ���
							if ((one.x - one.x_speed * (one.y - (80 * i - 2)) / one.y_speed) < 40 * j + 42 + Blue_block_left_x) {
								one.y = (80 * i - 2) * 2 - one.y;
								one.y_speed = -one.y_speed;
								one.x_speed = -2;
							}
							else {
								one.x = (40 * j + 42 + Blue_block_left_x) * 2 - one.x;
								one.x_speed = 2;
							}
						}
						else {
							//����(��)�����ɐi��ł���
							//���̕ӂŔ���
							if ((one.x + one.x_speed * ((80 * i + 82) - one.y) / one.y_speed) < 40 * j + 42 + Blue_block_left_x) {
								one.y = (80 * i + 82) * 2 - one.y;
								one.y_speed = -one.y_speed;
								one.x_speed = -2;
							}
							else {
								one.x = (40 * j + 42 + Blue_block_left_x) * 2 - one.x;
								one.x_speed = 2;
							}
						}
					}
					Blue_block[i][j] = 0;
				}
				//ball two
				if (Blue_block[i][j] != 0 && two.x > 40 * j + 854 && two.x < 40 * j + 898 && two.y>80 * i - 2 && two.y < 80 * i + 82) {
					if (two.x_speed > 0) {
						if (two.y_speed > 0) {
							//�E�������ɐi��ł���
							//��̕ӂŔ���
							if ((two.x - two.x_speed * (two.y - (80 * i - 2)) / two.y_speed) > 40 * j - 2 + Blue_block_left_x) {
								two.y = (80 * i - 2) * 2 - two.y;
								two.y_speed = -two.y_speed;
								two.x_speed = 2;
							}
							else {
								two.x = (40 * j - 2 + Blue_block_left_x) * 2 - two.x;
								two.x_speed = -2;
							}
						}
						else {
							//�E��(�E)�����ɐi��ł���
							//���̕ӂŔ���
							if ((two.x - -two.x_speed * ((80 * i + 82) - two.y) / two.y_speed) > 40 * j - 2 + Blue_block_left_x) {
								two.y = (80 * i + 82) * 2 - two.y;
								two.y_speed = -two.y_speed;
								two.x_speed = 2;
							}
							else {
								two.x = (40 * j - 2 + Blue_block_left_x) * 2 - two.x;
								two.x_speed = -2;
							}
						}
					}
					else {
						if (two.y_speed > 0) {
							//���������ɐi��ł���
							//��̕ӂŔ���
							if ((two.x - two.x_speed * (two.y - (80 * i - 2)) / two.y_speed) < 40 * j + 42 + Blue_block_left_x) {
								two.y = (80 * i - 2) * 2 - two.y;
								two.y_speed = -two.y_speed;
								two.x_speed = -2;
							}
							else {
								two.x = (40 * j + 42 + Blue_block_left_x) * 2 - two.x;
								two.x_speed = 2;
							}
						}
						else {
							//����(��)�����ɐi��ł���
							//���̕ӂŔ���
							if ((two.x + two.x_speed * ((80 * i + 82) - two.y) / two.y_speed) < 40 * j + 42 + Blue_block_left_x) {
								two.y = (80 * i + 82) * 2 - two.y;
								two.y_speed = -two.y_speed;
								two.x_speed = -2;
							}
							else {
								two.x = (40 * j + 42 + Blue_block_left_x) * 2 - two.x;
								two.x_speed = 2;
							}
						}
					}
					Blue_block[i][j] = 0;
				}
			}
		}

		//�^�[�{���[�h
		//if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) {
		//	if (Turbo_flag == 0) Turbo_flag = 1;
		//	else Turbo_flag = 0;
		//}
		/*else */Turbo_flag = 0;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//�`��

		//��ʂ�����
		ClearDrawScreen();
		DrawBox(0, 0, 1136, 640, 0x000000, TRUE);
		//�c���
		//DrawFormatString(550, 0, GetColor(255, 63, 0), "�ԁF%d", Red_block_count);
		//DrawFormatString(550, 0, GetColor(0, 127, 255), "\n�F%d", Blue_block_count);
		char tmp[] = "�ǂ����������Ă�H";
		DrawString(1136 / 2 - GetDrawStringWidth(tmp, strlen(tmp)) / 2, 4, tmp, 0xffffff);
		DrawBox(1136 / 2 - 100, 24, 1136 / 2 + 100, 56, 0xff0000, TRUE);
		DrawBox(1136 / 2 - 100, 24, 1136 / 2 - 100 + 200 * Red_block_count / (Red_block_count + Blue_block_count), 56, 0x007fff, TRUE);
		DrawBox(1136 / 2 - 100, 24, 1136 / 2 + 100, 56, 0xffffff, FALSE);
		DrawFormatString(1136 / 2 + 104, 32, 0x007fff, "�c�� %2d ��", Blue_block_count);
		DrawFormatString(1136 / 2 - 104 - GetDrawStringWidth("�c�� 10 ��", strlen("�c�� 10 ��")), 32, 0xff0000, "�c�� %2d ��", Red_block_count);
		//�u���b�N�̕`��
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 7; j++) {
				if (Red_block[i][j] == 1) DrawBox(40 * j + 4, 80 * i + 4, 40 * j + 36, 80 * i + 76, GetColor(237, 28, 36), TRUE);
				if (Red_block[i][j] == 2) DrawBox(40 * j + 4, 80 * i + 4, 40 * j + 36, 80 * i + 76, GetColor(255, 127, 39), TRUE);
				if (Blue_block[i][j] == 1) DrawBox(40 * j + 860, 80 * i + 4, 40 * j + 892, 80 * i + 76, GetColor(0, 162, 232), TRUE);
				if (Blue_block[i][j] == 2) DrawBox(40 * j + 860, 80 * i + 4, 40 * j + 892, 80 * i + 76, GetColor(153, 217, 234), TRUE);
			}
		}
		//�o�[�̕`��
		DrawBox(776, Red_bar_y - Red_bar_long_half, 784, Red_bar_y + Red_bar_long_half, GetColor(255, 0, 0), TRUE);
		DrawBox(352, Blue_bar_y - Blue_bar_long_half, 360, Blue_bar_y + Blue_bar_long_half, GetColor(0, 127, 255), TRUE);
		//�{�[���̕`��
		DrawCircleAA(one.x, one.y, 6, 16, GetColor(255, 255, 255), TRUE);
		DrawCircleAA(two.x, two.y, 6, 16, GetColor(255, 255, 255), TRUE);
		//�^�[�{�`��
		if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) DrawString(0, 0, "�^�[�{���[�h", GetColor(0, 255, 0));
		//����ʂ�\��ʂɂ���
		if (Turbo_flag == 0) ScreenFlip();

	}

	//����

	//��ʂ�����
	ClearDrawScreen();
	DrawBox(0, 0, 1136, 640, 0x000000, TRUE);
	//���ʕ\��
	int Title2 = CreateFontToHandle(NULL, 64, -1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	if (win == RED) DrawStringToHandle(372, 290, "�Ԃ̏����I�I", GetColor(255, 0, 0), Title2, GetColor(0, 255, 255));
	else DrawStringToHandle(372, 290, "�̏����I�I", GetColor(0, 0, 255), Title2, GetColor(255, 255, 0));
	//����ʂ�\��ʂɂ���
	ScreenFlip();
	//�X�y�[�X�̓��͑҂�
	while (CheckHitKey(KEY_INPUT_SPACE) == 0) {
		ProcessMessage_and_End();
		SetMousePoint(1136 / 2, 640 / 2);
	}

	return win;
}