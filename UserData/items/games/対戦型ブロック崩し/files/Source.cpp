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

	//ログファイル作らない
	SetOutApplicationLogValidFlag(FALSE);
	//フルスクリーンFlag
	//switch (MessageBox(NULL, "フルスクリーンで表示しますか？", "対戦型ブロック崩し ver0.5", (MB_YESNOCANCEL | MB_ICONINFORMATION | MB_DEFBUTTON2))) {
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
	//背景色
	SetBackgroundColor(0, 255, 0);
	//名前変える
	SetMainWindowText("対戦型ブロック崩し ver0.5");
	//アイコン変える
	//SetWindowIconID(333);
	//解像度変える
	SetGraphMode(1136, 640, 16);
	//DxLibの初期化
	if (DxLib_Init() == -1)return -1;
	//裏画面セット
	SetDrawScreen(DX_SCREEN_BACK);
	//全画面もどき
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	int Width = rc.right - rc.left, Height = rc.bottom - rc.top;
	if (Width * 1.0 / Height >= 1136.0 / 640) {
		//左右に余白
		SetWindowSizeExtendRate(Height * 1.0 / 640);
	}
	else {
		//上下に余白
		SetWindowSizeExtendRate(Width * 1.0 / 1136);
	}
	SetMouseDispFlag(FALSE);
	SetWindowStyleMode(3);

	start();
	game(0, 0, 0, 0);

	DxLib_End();
	return 0;
}


//スタート画面
int start(void) {
	DrawBox(0, 0, 1136, 640, 0x000000, TRUE);
	//ブロックの描画
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
	//バーの描画
	DrawBox(776, 260, 784, 380, GetColor(255, 0, 0), TRUE);
	DrawBox(352, 260, 360, 380, GetColor(0, 127, 255), TRUE);
	//フォント読み込み
	int Title1 = CreateFontToHandle(NULL, 32, -1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	int Title2 = CreateFontToHandle(NULL, 64, -1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	//タイトルの描画
	DrawStringToHandle(512, 32, "対戦型", GetColor(255, 255, 0), Title1, GetColor(0, 0, 255));
	DrawStringToHandle(372, 64, "ブロック崩し", GetColor(0, 255, 0), Title2, GetColor(255, 0, 255));
	//説明文
	DrawString(360, 240, "\
ルール説明\n\
これは二人用のゲームです。\n\
左の人は青いバーを\n\
w, sキー（キーボードの左の方にある）で動かし、\n\
右の人は赤いバーを\n\
↑↓キー（右のほうにある）で動かします。\n\
なるべくボールを相手の方に打ち返して\n\
最終的にボールが画面の右の辺に入ったら赤の勝ちで、\n\
左に入ったら青の勝ちです。\n\
試合が進むにつれどんどんバーが短くなってきます。\n\
\n\
２人の準備が出来たらスペースキーを押してください。\n\
するとゲームがスタートします！\n\
では頑張って戦ってください！\n\
", 0xffffff);
	//裏画面を表画面にする
	ScreenFlip();
	//スペースキーの入力待ち
	while (CheckHitKey(KEY_INPUT_SPACE) == 0) {
		ProcessMessage_and_End();
		SetMousePoint(1136 / 2, 640 / 2);
	}
	return 0;
}


//ゲーム画面
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

		//処理

		//勝利判定
		if (one.x < 0 || two.x < 0) {
			win = RED;
			break;
		}
		if (one.x > 1136 || two.x > 1136) {
			win = BLUE;
			break;
		}

		//ブロックの残り個数を調べる
		Red_block_count = 0;
		Blue_block_count = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 7; j++) {
				if (Red_block[i][j] != 0) Red_block_count++;
				if (Blue_block[i][j] != 0) Blue_block_count++;
			}
		}

		//バーの長さ
		Red_bar_long_half = 60 * Red_block_count / Red_block_sum;
		Blue_bar_long_half = 60 * Blue_block_count / Blue_block_sum;

		//バーの移動
		if (CheckHitKey(KEY_INPUT_UP) == 1 && CheckHitKey(KEY_INPUT_DOWN) == 0) Red_bar_y -= 5;
		if (CheckHitKey(KEY_INPUT_UP) == 0 && CheckHitKey(KEY_INPUT_DOWN) == 1) Red_bar_y += 5;
		if (CheckHitKey(KEY_INPUT_W) == 1 && CheckHitKey(KEY_INPUT_S) == 0) Blue_bar_y -= 5;
		if (CheckHitKey(KEY_INPUT_W) == 0 && CheckHitKey(KEY_INPUT_S) == 1) Blue_bar_y += 5;

		//バーの上下限界
		if (Red_bar_y < Red_bar_long_half) Red_bar_y = Red_bar_long_half;
		if (Red_bar_y > 640 - Red_bar_long_half) Red_bar_y = 640 - Red_bar_long_half;
		if (Blue_bar_y < Blue_bar_long_half) Blue_bar_y = Blue_bar_long_half;
		if (Blue_bar_y > 640 - Blue_bar_long_half) Blue_bar_y = 640 - Blue_bar_long_half;

		//ボールの移動
		one.x += one.x_speed;
		one.y += one.y_speed;
		two.x += two.x_speed;
		two.y += two.y_speed;

		//ボールの跳ね返り(上下)
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

		//ボールの跳ね返り(バー)
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

		//ボールのyスピードの上限
		if (one.y_speed > 5) one.y_speed = 5;
		if (one.y_speed < -5) one.y_speed = -5;
		if (two.y_speed > 5) two.y_speed = 5;
		if (two.y_speed < -5) two.y_speed = -5;

		//ボールの跳ね返り(ブロック)
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 7; j++) {
				//ボールの跳ね返り(赤ブロック)
				//ball one
				if (Red_block[i][j] != 0 && one.x > 40 * j - 2 && one.x < 40 * j + 42 && one.y>80 * i - 2 && one.y < 80 * i + 82) {
					if (one.x_speed > 0) {
						if (one.y_speed > 0) {
							//右下向きに進んでいる
							//上の辺で判定
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
							//右上(右)向きに進んでいる
							//下の辺で判定
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
							//左下向きに進んでいる
							//上の辺で判定
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
							//左上(左)向きに進んでいる
							//下の辺で判定
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
							//右下向きに進んでいる
							//上の辺で判定
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
							//右上(右)向きに進んでいる
							//下の辺で判定
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
							//左下向きに進んでいる
							//上の辺で判定
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
							//左上(左)向きに進んでいる
							//下の辺で判定
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
				//ボールの跳ね返り(青ブロック)
				//ball one
				if (Blue_block[i][j] != 0 && one.x > 40 * j + Blue_block_left_x - 2 && one.x < 40 * j + Blue_block_left_x + 42 && one.y > 80 * i - 2 && one.y < 80 * i + 82) {
					if (one.x_speed > 0) {
						if (one.y_speed > 0) {
							//右下向きに進んでいる
							//上の辺で判定
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
							//右上(右)向きに進んでいる
							//下の辺で判定
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
							//左下向きに進んでいる
							//上の辺で判定
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
							//左上(左)向きに進んでいる
							//下の辺で判定
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
							//右下向きに進んでいる
							//上の辺で判定
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
							//右上(右)向きに進んでいる
							//下の辺で判定
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
							//左下向きに進んでいる
							//上の辺で判定
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
							//左上(左)向きに進んでいる
							//下の辺で判定
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

		//ターボモード
		//if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) {
		//	if (Turbo_flag == 0) Turbo_flag = 1;
		//	else Turbo_flag = 0;
		//}
		/*else */Turbo_flag = 0;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//描画

		//画面を消す
		ClearDrawScreen();
		DrawBox(0, 0, 1136, 640, 0x000000, TRUE);
		//残り個数
		//DrawFormatString(550, 0, GetColor(255, 63, 0), "赤：%d", Red_block_count);
		//DrawFormatString(550, 0, GetColor(0, 127, 255), "\n青：%d", Blue_block_count);
		char tmp[] = "どっちが勝ってる？";
		DrawString(1136 / 2 - GetDrawStringWidth(tmp, strlen(tmp)) / 2, 4, tmp, 0xffffff);
		DrawBox(1136 / 2 - 100, 24, 1136 / 2 + 100, 56, 0xff0000, TRUE);
		DrawBox(1136 / 2 - 100, 24, 1136 / 2 - 100 + 200 * Red_block_count / (Red_block_count + Blue_block_count), 56, 0x007fff, TRUE);
		DrawBox(1136 / 2 - 100, 24, 1136 / 2 + 100, 56, 0xffffff, FALSE);
		DrawFormatString(1136 / 2 + 104, 32, 0x007fff, "残り %2d 個", Blue_block_count);
		DrawFormatString(1136 / 2 - 104 - GetDrawStringWidth("残り 10 個", strlen("残り 10 個")), 32, 0xff0000, "残り %2d 個", Red_block_count);
		//ブロックの描画
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 7; j++) {
				if (Red_block[i][j] == 1) DrawBox(40 * j + 4, 80 * i + 4, 40 * j + 36, 80 * i + 76, GetColor(237, 28, 36), TRUE);
				if (Red_block[i][j] == 2) DrawBox(40 * j + 4, 80 * i + 4, 40 * j + 36, 80 * i + 76, GetColor(255, 127, 39), TRUE);
				if (Blue_block[i][j] == 1) DrawBox(40 * j + 860, 80 * i + 4, 40 * j + 892, 80 * i + 76, GetColor(0, 162, 232), TRUE);
				if (Blue_block[i][j] == 2) DrawBox(40 * j + 860, 80 * i + 4, 40 * j + 892, 80 * i + 76, GetColor(153, 217, 234), TRUE);
			}
		}
		//バーの描画
		DrawBox(776, Red_bar_y - Red_bar_long_half, 784, Red_bar_y + Red_bar_long_half, GetColor(255, 0, 0), TRUE);
		DrawBox(352, Blue_bar_y - Blue_bar_long_half, 360, Blue_bar_y + Blue_bar_long_half, GetColor(0, 127, 255), TRUE);
		//ボールの描画
		DrawCircleAA(one.x, one.y, 6, 16, GetColor(255, 255, 255), TRUE);
		DrawCircleAA(two.x, two.y, 6, 16, GetColor(255, 255, 255), TRUE);
		//ターボ描画
		if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) DrawString(0, 0, "ターボモード", GetColor(0, 255, 0));
		//裏画面を表画面にする
		if (Turbo_flag == 0) ScreenFlip();

	}

	//結果

	//画面を消す
	ClearDrawScreen();
	DrawBox(0, 0, 1136, 640, 0x000000, TRUE);
	//結果表示
	int Title2 = CreateFontToHandle(NULL, 64, -1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	if (win == RED) DrawStringToHandle(372, 290, "赤の勝ち！！", GetColor(255, 0, 0), Title2, GetColor(0, 255, 255));
	else DrawStringToHandle(372, 290, "青の勝ち！！", GetColor(0, 0, 255), Title2, GetColor(255, 255, 0));
	//裏画面を表画面にする
	ScreenFlip();
	//スペースの入力待ち
	while (CheckHitKey(KEY_INPUT_SPACE) == 0) {
		ProcessMessage_and_End();
		SetMousePoint(1136 / 2, 640 / 2);
	}

	return win;
}