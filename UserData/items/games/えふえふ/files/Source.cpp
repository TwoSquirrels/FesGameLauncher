#include "DxLib.h"

#define PI    3.1415926535897932384626433832795f

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	int a, d;
	int PlayerX, PlayerY, PlayerGraph, PlayerDirect, PlayerHP, PlayerStun, PlayerSP;
	int Player2X, Player2Y, Player2Graph, Player2Direct, Player2HP, Player2Stun, Player2SP;
	int EnemyX, EnemyY, EnemyGraph, EnemyDirect, EnemyStun, EnemyHP;
	int FieldGraph, Field2Graph, Field3Graph;
	int Yellow, White, Red, Green;
	int DeathCount, KillCount;
	int PHPX, PHPY, EHPX, EHPY, P2HPX, P2HPY, HPGGraph, HPBGraph;
	int Button, ClickX, ClickY, LogType, DrawFlag, DrawX, DrawY;
	int Button2, ClickX2, ClickY2, LogType2, DrawFlag2, DrawX2, DrawY2;
	int TitleGraph, IntroGraph, Intro2Graph, StageGraph;
	int ArrowX[10], ArrowY[10], ArrowFlag[10], ArrowDirect[10], ArrowRGraph, ArrowLGraph, ArrowUGraph, ArrowDGraph;
	int CoolTime;
	int CureX, CureY, CureGraph, CureFlag, CureLFlag;
	int SwordGraph;
	int SwordMove, SwordMoveFlag;
	int BowGraph;
	int SwordLight, SwordLightGraph;
	int Stage;
	int JumpPoint, JumpFlag, JumpFlag2;
	int DamageStun, DamageStun2;
	int Time, Time2, Foot, Foot2;
	int PSPX, PSPY, P2SPX, P2SPY, SPGGraph, SPBGraph;
	int SPSwordFlag, SPSwordGraph, SPTime;
	int SPArrowFlag, SPArrowGraph, SPTime2, SPArrowX, SPArrowY;
	int ArrowFallFlag, ArrowFallX, ArrowFallY, ArrowFallGraph, ArrowWait;
	int FontHandle;
	int ArtPoint, ArtPoint2, VictoryPoint, VictoryPoint2;
	int ResultGraph[7];
	int TotalScore, TotalScore2;
	int ThreeGraph, TwoGraph, OneGraph;
	int P1Graph, P2Graph;

	double large;

	

	PlayerGraph = LoadGraph("Player.bmp");
	Player2Graph = LoadGraph("Player2.bmp");
	EnemyGraph = LoadGraph("Enemy.bmp");
	FieldGraph = LoadGraph("field map.bmp");
	HPGGraph = LoadGraph("HPゲージ.bmp");
	HPBGraph = LoadGraph("HPバーs.bmp");
	TitleGraph = LoadGraph("タイトル画面.bmp");
	ArrowRGraph = LoadGraph("右矢.bmp");
	ArrowLGraph = LoadGraph("左矢.bmp");
	ArrowUGraph = LoadGraph("上矢.bmp");
	ArrowDGraph = LoadGraph("下矢.bmp");
	CureGraph = LoadGraph("回復薬.bmp");
	IntroGraph = LoadGraph("1P説明画面.bmp");
	Intro2Graph = LoadGraph("2P説明画面.bmp");
	SwordGraph = LoadGraph("剣.bmp");
	BowGraph = LoadGraph("弓.bmp");
	SwordLightGraph = LoadGraph("剣光.bmp");
	StageGraph = LoadGraph("ステージ選択.bmp");
	Field2Graph = LoadGraph("field map2.bmp");
	Field3Graph = LoadGraph("field map3.bmp");
	SPGGraph = LoadGraph("SPゲージ.bmp");
	SPBGraph = LoadGraph("SPバー.bmp");
	SPSwordGraph = LoadGraph("必殺技.bmp");
	SPArrowGraph = LoadGraph("必殺技2.bmp");
	ArrowFallGraph = LoadGraph("矢の束.bmp");
	FontHandle = CreateFontToHandle("UD ﾃﾞｼﾞﾀﾙ 教科書体 NP-B",28 ,3);
	ResultGraph[0] = LoadGraph("Result1.bmp");
	ResultGraph[1] = LoadGraph("Result2.bmp");
	ResultGraph[2] = LoadGraph("Result3.bmp");
	ResultGraph[3] = LoadGraph("Result4.bmp");
	ResultGraph[4] = LoadGraph("Result5.bmp");
	ResultGraph[5] = LoadGraph("Result6.bmp");
	ResultGraph[6] = LoadGraph("Result7.bmp");
	ThreeGraph = LoadGraph("3.bmp");
	TwoGraph = LoadGraph("2.bmp");
	OneGraph = LoadGraph("1.bmp");
	P1Graph = LoadGraph("1P.bmp");
	P2Graph = LoadGraph("2P.bmp");


	SetDrawScreen(DX_SCREEN_BACK);
	SetMouseDispFlag(TRUE);

	Yellow = GetColor(255, 255, 0);
	White = GetColor(0, 0, 0);
	Red = GetColor(255, 0, 0);
	Green = GetColor(0, 100, 0);

	PlayerX = 305;
	PlayerY = 315;
	Player2X = 305;
	Player2Y = 115;
	EnemyX = 295;
	EnemyY = 115;
	EnemyStun = 0;
	PlayerStun = 0;
	Player2Stun = 0;
	DeathCount = 0;
	KillCount = 0;
	PlayerDirect = 1;
	Player2Direct = 1;
	PlayerHP = 40;
	PlayerSP = 0;
	Player2HP = 40;
	Player2SP = 0;
	EnemyHP = 40;
	DrawX = 0;
	DrawY = 0;
	DrawX2 = 0;
	DrawY2 = 0;
	CoolTime = 20;
	d = 0;
	CureFlag = 0;
	CureLFlag = 0;
	SwordMove = 0;
	SwordMoveFlag = 0;
	SwordLight = 0;
	JumpFlag = 1;
	JumpFlag2 = 1;
	DamageStun = 0;
	DamageStun2 = 0;
	Time = 0;
	Time2 = 0;
	SPSwordFlag = 0;
	SPArrowFlag = 0;
	SPTime = 0;
	SPTime2 = 0;
	ArrowFallFlag = 0;
	ArrowWait = 0;
	ArtPoint = 0;
	ArtPoint2 = 0;
	VictoryPoint = 0;
	VictoryPoint2 = 0;
	PHPX = PlayerX - 6;
	PHPY = PlayerY - 22;
	PSPX = PlayerX - 6;
	PSPY = PlayerY - 26;
	P2HPX = Player2X - 6;
	P2HPY = Player2Y - 22;
	P2SPX = Player2X - 6;
	P2SPY = Player2Y - 26;
	large = 1.0f;


	for (a = 0; a < 10; a++)
	{
		ArrowFlag[a] = 0;
	}

	DrawGraph(0, 0, TitleGraph, TRUE);

	ScreenFlip();

	while (CheckHitKey(KEY_INPUT_ESCAPE) == 0 && ProcessMessage() == 0)
	{
		// マウスのボタンが押されたり離されたりしたかどうかの情報を取得する
		if (GetMouseInputLog2(&Button, &ClickX, &ClickY, &LogType, TRUE) == 0)
		{
			// 左ボタンが押されたり離されたりしていたら描画するかどうかのフラグを立てて、座標も保存する
			if ((Button & MOUSE_INPUT_LEFT) != 0)
			{
				DrawFlag = TRUE;
				DrawX = ClickX;
				DrawY = ClickY;
			}
		}

		if (DrawX > 50 && DrawX < 265 && DrawY > 200 && DrawY < 315)
		{
			goto soro;
		}
		if (DrawX > 365 && DrawX < 560 && DrawY > 200 && DrawY < 315)
		{
			goto hutari;
		}
	}

soro:;

	ClearDrawScreen();

	DrawGraph(0, 0, StageGraph, TRUE);

	ScreenFlip();

	WaitTimer(1000);


	while (CheckHitKey(KEY_INPUT_ESCAPE) == 0 && ProcessMessage() == 0)
	{
		// マウスのボタンが押されたり離されたりしたかどうかの情報を取得する
		if (GetMouseInput() & MOUSE_INPUT_LEFT)
		{
			DrawFlag2 = TRUE;
			GetMousePoint(&ClickX2, &ClickY2);
			DrawX2 = ClickX2;
			DrawY2 = ClickY2;
		}

		if (DrawX2 > 80 && DrawX2 < 560 && DrawY2 > 20 && DrawY2 < 130)
		{
			Stage = 1;
			goto start2;
		}
		if (DrawX2 > 80 && DrawX2 < 560 && DrawY2 > 160 && DrawY2 < 270)
		{
			Stage = 2;
			goto start2;
		}
		if (DrawX2 > 80 && DrawX2 < 560 && DrawY2 > 300 && DrawY2 < 410)
		{
			Stage = 3;
			goto start2;
		}
	}

start2:;


	while (CheckHitKey(KEY_INPUT_RETURN) == 0 && ProcessMessage() == 0)
	{
		ClearDrawScreen();

		DrawGraph(0, 0, IntroGraph, TRUE);

		ScreenFlip();
	}

	ClearDrawScreen();

	DrawFormatString(300, 230, Yellow, "Game Start");

	ScreenFlip();

	WaitTimer(1000);


	while (large < 100.0f)
	{

		ClearDrawScreen();


		if (Stage == 1)
		{
			DrawGraph(0, 0, FieldGraph, TRUE);
		}
		if (Stage == 2)
		{
			DrawGraph(0, 0, Field2Graph, TRUE);
		}
		if (Stage == 3)
		{
			DrawGraph(0, 0, Field3Graph, TRUE);
		}


		DrawGraph(PlayerX, PlayerY, PlayerGraph, TRUE);

		DrawGraph(EnemyX, EnemyY, EnemyGraph, TRUE);

		DrawGraph(PHPX, PHPY, HPGGraph, TRUE);



		DrawExtendGraph(PHPX + 1, PHPY + 1, PHPX + 1 + PlayerHP, PHPY + 11, HPBGraph, TRUE);






		DrawRotaGraph(320, 240, large / 10.0f, 0, ThreeGraph, TRUE, FALSE);

		large = large + 2.0f;

		ScreenFlip();
	}

	large = 1.0f;

	while (large < 100.0f)
	{

		ClearDrawScreen();


		if (Stage == 1)
		{
			DrawGraph(0, 0, FieldGraph, TRUE);
		}
		if (Stage == 2)
		{
			DrawGraph(0, 0, Field2Graph, TRUE);
		}
		if (Stage == 3)
		{
			DrawGraph(0, 0, Field3Graph, TRUE);
		}


		DrawGraph(PlayerX, PlayerY, PlayerGraph, TRUE);

		DrawGraph(EnemyX, EnemyY, EnemyGraph, TRUE);

		DrawGraph(PHPX, PHPY, HPGGraph, TRUE);



		DrawExtendGraph(PHPX + 1, PHPY + 1, PHPX + 1 + PlayerHP, PHPY + 11, HPBGraph, TRUE);


		DrawRotaGraph(320, 240, large / 10.0f, 0, TwoGraph, TRUE, FALSE);

		large = large + 2.0f;

		ScreenFlip();
	}

	large = 1.0f;

	while (large < 100.0f)
	{

		ClearDrawScreen();


		if (Stage == 1)
		{
			DrawGraph(0, 0, FieldGraph, TRUE);
		}
		if (Stage == 2)
		{
			DrawGraph(0, 0, Field2Graph, TRUE);
		}
		if (Stage == 3)
		{
			DrawGraph(0, 0, Field3Graph, TRUE);
		}


		DrawGraph(PlayerX, PlayerY, PlayerGraph, TRUE);

		DrawGraph(EnemyX, EnemyY, EnemyGraph, TRUE);

		DrawGraph(PHPX, PHPY, HPGGraph, TRUE);



		DrawExtendGraph(PHPX + 1, PHPY + 1, PHPX + 1 + PlayerHP, PHPY + 11, HPBGraph, TRUE);


		DrawRotaGraph(320, 240, large / 10.0f, 0, OneGraph, TRUE, FALSE);

		large = large + 2.0f;

		ScreenFlip();
	}

	large = 1.0f;


	while (CheckHitKey(KEY_INPUT_ESCAPE) == 0 && ProcessMessage() == 0)
	{

		PHPX = PlayerX - 6;
		PHPY = PlayerY - 22;
		EHPX = EnemyX + 4;
		EHPY = EnemyY - 22;

		ClearDrawScreen();

		if (CureFlag != 1)
		{
			CureFlag = GetRand(500);
			CureLFlag = 1;
		}
		if (CureFlag == 1 && CureLFlag == 1)
		{
			CureX = GetRand(621);
			CureY = GetRand(451);
			CureLFlag = 0;
		}
		if (CureFlag == 1)
		{
			if (CureX + 20 > PlayerX && CureX < PlayerX + 30 && CureY + 30 > PlayerY && CureY < PlayerY + 50)
			{
				CureFlag = 0;
				if (PlayerHP > 20)
				{
					PlayerHP = 40;
				}
				else
				{
					PlayerHP += 20;
				}
			}
		}

		if (CheckHitKey(KEY_INPUT_RIGHT) == 1)
		{
			PlayerX += 5;
			PlayerDirect = 1;
		}
		if (CheckHitKey(KEY_INPUT_LEFT) == 1)
		{
			PlayerX -= 5;
			PlayerDirect = 2;
		}
		if (CheckHitKey(KEY_INPUT_DOWN) == 1)
		{
			PlayerY += 5;
			PlayerDirect = 3;
		}
		if (CheckHitKey(KEY_INPUT_UP) == 1)
		{
			PlayerY -= 5;
			PlayerDirect = 4;
		}

		if (Stage == 2)
		{
			if (PlayerX > 34 && PlayerX < 576)
			{
				if (PlayerY > 14 && PlayerY < 128)
				{
					PlayerY = 128;
				}
				if (PlayerY > 366 && PlayerY < 416)
				{
					PlayerY = 366;
				}
			}
			if (PlayerX > 34 && PlayerX < 416)
			{
				if (PlayerY > 174 && PlayerY < 288)
				{
					PlayerY = 288;
				}
			}
			if (PlayerY > 174 && PlayerY < 416)
			{
				if (PlayerX > 34 && PlayerX < 96)
				{
					PlayerX = 96;
				}
			}
			if (PlayerY > 14 && PlayerY < 416)
			{
				if (PlayerX > 514 && PlayerX < 576)
				{
					PlayerX = 514;
				}
			}
			if (EnemyX > 14 && EnemyX < 576)
			{
				if (EnemyY > 14 && EnemyY < 128)
				{
					EnemyY = 128;
				}
				if (EnemyY > 366 && EnemyY < 416)
				{
					EnemyY = 366;
				}
			}
			if (EnemyX > 14 && EnemyX < 416)
			{
				if (EnemyY > 174 && EnemyY < 288)
				{
					EnemyY = 288;
				}
			}
			if (EnemyY > 174 && EnemyY < 416)
			{
				if (EnemyX > 14 && EnemyX < 96)
				{
					EnemyX = 96;
				}
			}
			if (EnemyY > 14 && EnemyY < 416)
			{
				if (EnemyX > 494 && EnemyX < 576)
				{
					EnemyX = 494;
				}
			}
		}

		if (Stage == 3)
		{
			if (((PlayerX > 34 && PlayerX < 128 && PlayerY > 14 && PlayerY < 128) || (PlayerX > 482 && PlayerX < 576 && PlayerY > 14 && PlayerY < 128) || (PlayerX > 34 && PlayerX < 128 && PlayerY > 302 && PlayerY < 416) || (PlayerX > 482 && PlayerX < 576 && PlayerY > 302 && PlayerY < 416) || (PlayerX > 258 && PlayerX < 352 && PlayerY > 158 && PlayerY < 272)) && JumpFlag == 1)
			{
				JumpPoint = GetRand(4);

				if (JumpPoint == 0)
				{
					PlayerX = 305;
					PlayerY = 208;
				}
				else if (JumpPoint == 1)
				{
					PlayerX = 81;
					PlayerY = 64;
				}
				else if (JumpPoint == 2)
				{
					PlayerX = 529;
					PlayerY = 64;
				}
				else if (JumpPoint == 3)
				{
					PlayerX = 81;
					PlayerY = 352;
				}
				else if (JumpPoint == 4)
				{
					PlayerX = 529;
					PlayerY = 352;
				}
				JumpFlag = 50;
			}
			if (((EnemyX > 14 && EnemyX < 128 && EnemyY > 14 && EnemyY < 128) || (EnemyX > 462 && EnemyX < 576 && EnemyY > 14 && EnemyY < 128) || (EnemyX > 14 && EnemyX < 128 && EnemyY > 302 && EnemyY < 416) || (EnemyX > 462 && EnemyX < 576 && EnemyY > 302 && EnemyY < 416) || (EnemyX > 238 && EnemyX < 352 && EnemyY > 158 && EnemyY < 272)) && JumpFlag2 == 1)
			{
				JumpPoint = GetRand(4);

				if (JumpPoint == 0)
				{
					EnemyX = 295;
					EnemyY = 208;
				}
				else if (JumpPoint == 1)
				{
					EnemyX = 71;
					EnemyY = 64;
				}
				else if (JumpPoint == 2)
				{
					EnemyX = 519;
					EnemyY = 64;
				}
				else if (JumpPoint == 3)
				{
					EnemyX = 71;
					EnemyY = 352;
				}
				else if (JumpPoint == 4)
				{
					EnemyX = 519;
					EnemyY = 352;
				}
				JumpFlag2 = 50;
			}
		}

		if (JumpFlag > 1)
		{
			JumpFlag--;
		}
		if (JumpFlag2 > 1)
		{
			JumpFlag2--;
		}

		if (EnemyStun == 0)
		{
			if (EnemyX > PlayerX + 30)
			{
				EnemyX -= 4;
			}
			if (EnemyX + 50 < PlayerX)
			{
				EnemyX += 4;
			}
			if (EnemyX < PlayerX + 30 && PlayerX < EnemyX + 50)
			{
				if (EnemyY > PlayerY + 50)
				{
					EnemyY -= 4;
				}
				if (EnemyY + 50 < PlayerY)
				{
					EnemyY += 4;
				}
			}
		}

		if (EnemyStun > 0)
		{
			EnemyStun--;
		}

		EnemyDirect = GetRand(3) + 1;
		if (EnemyDirect == 1)
		{
			EnemyX += 3;
		}
		if (EnemyDirect == 2)
		{
			EnemyX -= 3;
		}
		if (EnemyDirect == 3)
		{
			EnemyY += 3;
		}
		if (EnemyDirect == 4)
		{
			EnemyY -= 3;
		}

		if (PlayerX < 0)
		{
			PlayerX = 0;
		}
		if (PlayerX > 610)
		{
			PlayerX = 610;
		}
		if (PlayerY < 0)
		{
			PlayerY = 0;
		}
		if (PlayerY > 430)
		{
			PlayerY = 430;
		}

		if (EnemyX < 0)
		{
			EnemyX = 0;
		}
		if (EnemyX > 590)
		{
			EnemyX = 590;
		}
		if (EnemyY < 0)
		{
			EnemyY = 0;
		}
		if (EnemyY > 430)
		{
			EnemyY = 430;
		}

		if (CheckHitKey(KEY_INPUT_Z) == 1 && ( SwordMove > 7 || SwordMoveFlag == 0))
		{
			if (PlayerDirect == 1)
			{
				if (EnemyY < PlayerY + 50 && PlayerY < EnemyY + 50 && PlayerX + 50 > EnemyX && PlayerX + 30 < EnemyX)
				{
					EnemyX += 50;
					EnemyStun = 50;
					KillCount++;
					EnemyHP--;
				}
			}
			if (PlayerDirect == 2)
			{
				if (EnemyY < PlayerY + 50 && PlayerY < EnemyY + 50 && EnemyX + 70 > PlayerX && EnemyX + 50 < PlayerX)
				{
					EnemyX -= 50;
					EnemyStun = 50;
					KillCount++;
					EnemyHP--;
				}
			}
			if (PlayerDirect == 3)
			{
				if (EnemyX < PlayerX + 30 && PlayerX < EnemyX + 50 && PlayerY + 70 > EnemyY && PlayerY + 50 < EnemyY)
				{
					EnemyY += 50;
					EnemyStun = 50;
					KillCount++;
					EnemyHP--;
				}
			}
			if (PlayerDirect == 4)
			{
				if (EnemyX < PlayerX + 30 && PlayerX < EnemyX + 50 && EnemyY + 70 > PlayerY && EnemyY + 50 < PlayerY)
				{
					EnemyY -= 50;
					EnemyStun = 50;
					KillCount++;
					EnemyHP--;
				}
			}
			if (SwordMoveFlag == 0)
			{
				SwordMove = 0;
				SwordLight = 0;
			}
			SwordMoveFlag = 1;

		}

		if (SwordMoveFlag == 1)
		{
			SwordMove++;
			SwordLight += 5;
		}
		if (SwordMove > 9)
		{
			SwordMoveFlag = 0;
			SwordMove = 0;
			SwordLight = 0;
		}

		if (EnemyX < PlayerX + 30 && PlayerX < EnemyX + 50 && EnemyY < PlayerY + 50 && PlayerY < EnemyY + 50)
		{
			if (EnemyDirect == 1)
			{
				PlayerX += 50;
				EnemyStun = 30;
			}
			if (EnemyDirect == 2)
			{
				PlayerX -= 50;
				EnemyStun = 30;
			}
			if (EnemyDirect == 3)
			{
				PlayerY += 50;
				EnemyStun = 30;
			}
			if (EnemyDirect == 4)
			{
				PlayerY -= 50;
				EnemyStun = 30;
			}

			if (DamageStun == 0)
			{
				DeathCount++;
				PlayerHP--;
				DamageStun = 20;
			}
		}

		if (DamageStun > 0)
		{
			DamageStun--;
		}

		if (PlayerHP == 0)
		{
			goto gameover;
		}
		if (EnemyHP == 0)
		{
			goto gameclear;
		}

		if (Stage == 1)
		{
			DrawGraph(0, 0, FieldGraph, TRUE);
		}
		if (Stage == 2)
		{
			DrawGraph(0, 0, Field2Graph, TRUE);
		}
		if (Stage == 3)
		{
			DrawGraph(0, 0, Field3Graph, TRUE);
		}

		if (PlayerDirect == 3)
		{
			for (a = 0; a < SwordLight; a++)
			{
				DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 1 / 4 + PI * a / 100, SwordLightGraph, TRUE, FALSE);
			}
			DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 1 / 2 + PI * SwordMove / 20, SwordGraph, TRUE, FALSE);
		}

		if (DamageStun == 0 || DamageStun == 2 || DamageStun == 4 || DamageStun == 8 || DamageStun == 10 || DamageStun == 12 || DamageStun == 14 || DamageStun == 16 || DamageStun == 18 || DamageStun == 20)
		{
			DrawGraph(PlayerX, PlayerY, PlayerGraph, TRUE);
		}

		DrawGraph(EnemyX, EnemyY, EnemyGraph, TRUE);
		DrawFormatString(0, 0, Green, "%d", KillCount);
		DrawFormatString(0, 30, Red, "%d", DeathCount);

		if (CureFlag == 1)
		{
			DrawGraph(CureX, CureY, CureGraph, TRUE);
		}

		if (PlayerDirect == 1)
		{
			for (a = 0; a < SwordLight; a++)
			{
				DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 7 / 4 + PI * a / 100, SwordLightGraph, TRUE, FALSE);
			}
			DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 2 + PI * SwordMove / 20, SwordGraph, TRUE, FALSE);
		}
		if (PlayerDirect == 2)
		{
			for (a = 0; a < SwordLight; a++)
			{
				DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 3 / 4 + PI * a / 100, SwordLightGraph, TRUE, FALSE);
			}
			DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 1 + PI * SwordMove / 20, SwordGraph, TRUE, FALSE);
		}
		if (PlayerDirect == 4)
		{
			for (a = 0; a < SwordLight; a++)
			{
				DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 5 / 4 + PI * a / 100, SwordLightGraph, TRUE, FALSE);
			}
			DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 3 / 2 + PI * SwordMove / 20, SwordGraph, TRUE, FALSE);
		}

		DrawGraph(PHPX, PHPY, HPGGraph, TRUE);
		DrawExtendGraph(PHPX + 1, PHPY + 1, PHPX + 1 + PlayerHP, PHPY + 11, HPBGraph, TRUE);
		DrawGraph(EHPX, EHPY, HPGGraph, TRUE);
		DrawExtendGraph(EHPX + 1, EHPY + 1, EHPX + 1 + EnemyHP, EHPY + 11, HPBGraph, TRUE);

		ScreenFlip();
	}

	goto end;

hutari:;

	ClearDrawScreen();

	DrawGraph(0, 0, StageGraph, TRUE);

	ScreenFlip();

	WaitTimer(1000);


	while (CheckHitKey(KEY_INPUT_ESCAPE) == 0 && ProcessMessage() == 0)
	{
		// マウスのボタンが押されたり離されたりしたかどうかの情報を取得する
		if (GetMouseInput() & MOUSE_INPUT_LEFT)
		{
			DrawFlag2 = TRUE;
			GetMousePoint(&ClickX2, &ClickY2);
			DrawX2 = ClickX2;
			DrawY2 = ClickY2;
		}

		if (DrawX2 > 80 && DrawX2 < 560 && DrawY2 > 20 && DrawY2 < 130)
		{
			Stage = 1;
			goto start;
		}
		if (DrawX2 > 80 && DrawX2 < 560 && DrawY2 > 160 && DrawY2 < 270)
		{
			Stage = 2;
			goto start;
		}
		if (DrawX2 > 80 && DrawX2 < 560 && DrawY2 > 300 && DrawY2 < 410)
		{
			Stage = 3;
			goto start;
		}
	}

start:;



	while (CheckHitKey(KEY_INPUT_RETURN) == 0 && ProcessMessage() == 0)
	{
		ClearDrawScreen();

		DrawGraph(0, 0, Intro2Graph, TRUE);

		ScreenFlip();
	}

	ClearDrawScreen();

	DrawFormatString(300, 230, Yellow, "Game Start");

	ScreenFlip();

	WaitTimer(1000);

	while (large < 100.0f)
	{

		ClearDrawScreen();

		if (Stage == 1)
		{
			DrawGraph(0, 0, FieldGraph, TRUE);
		}
		if (Stage == 2)
		{
			DrawGraph(0, 0, Field2Graph, TRUE);
		}
		if (Stage == 3)
		{
			DrawGraph(0, 0, Field3Graph, TRUE);
		}

		DrawGraph(PlayerX, PlayerY, PlayerGraph, TRUE);

		DrawGraph(Player2X, Player2Y, Player2Graph, TRUE);

		DrawGraph(PHPX, PHPY, HPGGraph, TRUE);



		SPBGraph = LoadGraph("SPバー.bmp");

		DrawExtendGraph(PHPX + 1, PHPY + 1, PHPX + 1 + PlayerHP, PHPY + 11, HPBGraph, TRUE);
		DrawGraph(PSPX, PSPY, SPGGraph, TRUE);
		DrawExtendGraph(PSPX + 1, PSPY, PSPX + 1 + PlayerSP * 4, PSPY + 2, SPBGraph, TRUE);
		DrawGraph(P2HPX, P2HPY, HPGGraph, TRUE);



		SPBGraph = LoadGraph("SPバー.bmp");

		DrawExtendGraph(P2HPX + 1, P2HPY + 1, P2HPX + 1 + Player2HP, P2HPY + 11, HPBGraph, TRUE);
		DrawGraph(P2SPX, P2SPY, SPGGraph, TRUE);
		DrawExtendGraph(P2SPX + 1, P2SPY, P2SPX + 1 + Player2SP * 4, P2SPY + 2, SPBGraph, TRUE);

		DrawGraph(PlayerX - 5, PlayerY + 60, P1Graph, TRUE);
		DrawGraph(Player2X - 5, Player2Y + 60, P2Graph, TRUE);

		DrawRotaGraph(320, 240, large / 10.0f, 0, ThreeGraph, TRUE, FALSE);

		large = large + 2.0f;

		ScreenFlip();
	}

	large = 1.0f;

	while (large < 100.0f)
	{

		ClearDrawScreen();

		if (Stage == 1)
		{
			DrawGraph(0, 0, FieldGraph, TRUE);
		}
		if (Stage == 2)
		{
			DrawGraph(0, 0, Field2Graph, TRUE);
		}
		if (Stage == 3)
		{
			DrawGraph(0, 0, Field3Graph, TRUE);
		}

		DrawGraph(PlayerX, PlayerY, PlayerGraph, TRUE);

		DrawGraph(Player2X, Player2Y, Player2Graph, TRUE);

		DrawGraph(PHPX, PHPY, HPGGraph, TRUE);

		SPBGraph = LoadGraph("SPバー.bmp");

		DrawExtendGraph(PHPX + 1, PHPY + 1, PHPX + 1 + PlayerHP, PHPY + 11, HPBGraph, TRUE);
		DrawGraph(PSPX, PSPY, SPGGraph, TRUE);
		DrawExtendGraph(PSPX + 1, PSPY, PSPX + 1 + PlayerSP * 4, PSPY + 2, SPBGraph, TRUE);
		DrawGraph(P2HPX, P2HPY, HPGGraph, TRUE);

		SPBGraph = LoadGraph("SPバー.bmp");

		DrawExtendGraph(P2HPX + 1, P2HPY + 1, P2HPX + 1 + Player2HP, P2HPY + 11, HPBGraph, TRUE);
		DrawGraph(P2SPX, P2SPY, SPGGraph, TRUE);
		DrawExtendGraph(P2SPX + 1, P2SPY, P2SPX + 1 + Player2SP * 4, P2SPY + 2, SPBGraph, TRUE);

		DrawGraph(PlayerX - 5, PlayerY + 60, P1Graph, TRUE);
		DrawGraph(Player2X - 5, Player2Y + 60, P2Graph, TRUE);

		DrawRotaGraph(320, 240, large / 10.0f, 0, TwoGraph, TRUE, FALSE);

		large = large + 2.0f;

		ScreenFlip();
	}

	large = 1.0f;

	while (large < 100.0f)
	{

		ClearDrawScreen();

		if (Stage == 1)
		{
			DrawGraph(0, 0, FieldGraph, TRUE);
		}
		if (Stage == 2)
		{
			DrawGraph(0, 0, Field2Graph, TRUE);
		}
		if (Stage == 3)
		{
			DrawGraph(0, 0, Field3Graph, TRUE);
		}

		DrawGraph(PlayerX, PlayerY, PlayerGraph, TRUE);

		DrawGraph(Player2X, Player2Y, Player2Graph, TRUE);

		DrawGraph(PHPX, PHPY, HPGGraph, TRUE);

		SPBGraph = LoadGraph("SPバー.bmp");

		DrawExtendGraph(PHPX + 1, PHPY + 1, PHPX + 1 + PlayerHP, PHPY + 11, HPBGraph, TRUE);
		DrawGraph(PSPX, PSPY, SPGGraph, TRUE);
		DrawExtendGraph(PSPX + 1, PSPY, PSPX + 1 + PlayerSP * 4, PSPY + 2, SPBGraph, TRUE);
		DrawGraph(P2HPX, P2HPY, HPGGraph, TRUE);

		SPBGraph = LoadGraph("SPバー.bmp");

		DrawExtendGraph(P2HPX + 1, P2HPY + 1, P2HPX + 1 + Player2HP, P2HPY + 11, HPBGraph, TRUE);
		DrawGraph(P2SPX, P2SPY, SPGGraph, TRUE);
		DrawExtendGraph(P2SPX + 1, P2SPY, P2SPX + 1 + Player2SP * 4, P2SPY + 2, SPBGraph, TRUE);

		DrawGraph(PlayerX - 5, PlayerY + 60, P1Graph, TRUE);
		DrawGraph(Player2X - 5, Player2Y + 60, P2Graph, TRUE);

		DrawRotaGraph(320, 240, large / 10.0f, 0, OneGraph, TRUE, FALSE);

		large = large + 2.0f;

		ScreenFlip();
	}

	large = 1.0f;

	while (CheckHitKey(KEY_INPUT_ESCAPE) == 0 && ProcessMessage() == 0)
	{
		PHPX = PlayerX - 6;
		PHPY = PlayerY - 22;
		PSPX = PlayerX - 6;
		PSPY = PlayerY - 26;
		P2HPX = Player2X - 6;
		P2HPY = Player2Y - 22;
		P2SPX = Player2X - 6;
		P2SPY = Player2Y - 26;

		ClearDrawScreen();

		if (CureFlag != 1)
		{
			CureFlag = GetRand(500);
			CureLFlag = 1;
		}
		if (CureFlag == 1 && CureLFlag == 1)
		{
			CureX = GetRand(621);
			CureY = GetRand(451);
			CureLFlag = 0;
		}
		if (CureFlag == 1)
		{
			if (CureX + 20 > PlayerX && CureX < PlayerX + 30 && CureY + 30 > PlayerY && CureY < PlayerY + 50)
			{
				CureFlag = 0;
				if (PlayerHP > 20)
				{
					PlayerHP = 40;
				}
				else
				{
					PlayerHP += 20;
				}
			}
			if (CureX + 20 > Player2X && CureX < Player2X + 30 && CureY + 30 > Player2Y && CureY < Player2Y + 50)
			{
				CureFlag = 0;
				if (Player2HP > 20)
				{
					Player2HP = 40;
				}
				else
				{
					Player2HP += 20;
				}
			}
		}

		if (PlayerStun == 0)
		{
			if (CheckHitKey(KEY_INPUT_RIGHT) == 1)
			{
				PlayerX += 5;
				if (SPSwordFlag == 1) PlayerX += 2;
				PlayerDirect = 1;
				Time++;
			}
			if (CheckHitKey(KEY_INPUT_LEFT) == 1)
			{
				PlayerX -= 5;
				if (SPSwordFlag == 1) PlayerX -= 2;
				PlayerDirect = 2;
				Time++;
			}
			if (CheckHitKey(KEY_INPUT_DOWN) == 1)
			{
				PlayerY += 5;
				if (SPSwordFlag == 1) PlayerY += 2;
				PlayerDirect = 3;
				Time++;
			}
			if (CheckHitKey(KEY_INPUT_UP) == 1)
			{
				PlayerY -= 5;
				if (SPSwordFlag == 1) PlayerY -= 2;
				PlayerDirect = 4;
				Time++;
			}
		}

		if (PlayerStun > 0)
		{
			PlayerStun--;
		}

		if (Player2Stun == 0)
		{
			if (CheckHitKey(KEY_INPUT_D) == 1)
			{
				Player2X += 5;
				Player2Direct = 1;
				Time2++;
			}
			if (CheckHitKey(KEY_INPUT_A) == 1)
			{
				Player2X -= 5;
				Player2Direct = 2;
				Time2++;
			}
			if (CheckHitKey(KEY_INPUT_S) == 1)
			{
				Player2Y += 5;
				Player2Direct = 3;
				Time2++;
			}
			if (CheckHitKey(KEY_INPUT_W) == 1)
			{
				Player2Y -= 5;
				Player2Direct = 4;
				Time2++;
			}
		}

		if (Player2Stun > 0)
		{
			Player2Stun--;
		}

		if (PlayerX < 0)
		{
			PlayerX = 0;
		}
		if (PlayerX > 610)
		{
			PlayerX = 610;
		}
		if (PlayerY < 0)
		{
			PlayerY = 0;
		}
		if (PlayerY > 430)
		{
			PlayerY = 430;
		}
		if (Player2X < 0)
		{
			Player2X = 0;
		}
		if (Player2X > 610)
		{
			Player2X = 610;
		}
		if (Player2Y < 0)
		{
			Player2Y = 0;
		}
		if (Player2Y > 430)
		{
			Player2Y = 430;
		}

		if (Stage == 2)
		{
			if (PlayerX > 34 && PlayerX < 576)
			{
				if (PlayerY > 14 && PlayerY < 128)
				{
					PlayerY = 128;
				}
				if (PlayerY > 366 && PlayerY < 416)
				{
					PlayerY = 366;
				}
			}
			if (PlayerX > 34 && PlayerX < 416)
			{
				if (PlayerY > 174 && PlayerY < 288)
				{
					PlayerY = 288;
				}
			}
			if (PlayerY > 174 && PlayerY < 416)
			{
				if (PlayerX > 34 && PlayerX < 96)
				{
					PlayerX = 96;
				}
			}
			if (PlayerY > 14 && PlayerY < 416)
			{
				if (PlayerX > 514 && PlayerX < 576)
				{
					PlayerX = 514;
				}
			}
			if (Player2X > 34 && Player2X < 576)
			{
				if (Player2Y > 14 && Player2Y < 128)
				{
					Player2Y = 128;
				}
				if (Player2Y > 366 && Player2Y < 416)
				{
					Player2Y = 366;
				}
			}
			if (Player2X > 34 && Player2X < 416)
			{
				if (Player2Y > 174 && Player2Y < 288)
				{
					Player2Y = 288;
				}
			}
			if (Player2Y > 174 && Player2Y < 416)
			{
				if (Player2X > 34 && Player2X < 96)
				{
					Player2X = 96;
				}
			}
			if (Player2Y > 14 && Player2Y < 416)
			{
				if (Player2X > 514 && Player2X < 576)
				{
					Player2X = 514;
				}
			}
		}

		if (Stage == 3)
		{
			if (((PlayerX > 34 && PlayerX < 128 && PlayerY > 14 && PlayerY < 128 ) || ( PlayerX >482 && PlayerX < 576 && PlayerY > 14 && PlayerY < 128 ) || ( PlayerX > 34 && PlayerX < 128 && PlayerY > 302 && PlayerY < 416 ) || ( PlayerX > 482 && PlayerX < 576 && PlayerY > 302 && PlayerY < 416 ) || ( PlayerX > 258 && PlayerX < 352 && PlayerY > 158 && PlayerY < 272)) && JumpFlag == 1)
			{
				JumpPoint = GetRand(4);

				if (JumpPoint == 0)
				{
					PlayerX = 305;
					PlayerY = 208;
				}
				else if (JumpPoint == 1)
				{
					PlayerX = 81;
					PlayerY = 64;
				}
				else if (JumpPoint == 2)
				{
					PlayerX = 529;
					PlayerY = 64;
				}
				else if (JumpPoint == 3)
				{
					PlayerX = 81;
					PlayerY = 352;
				}
				else if (JumpPoint == 4)
				{
					PlayerX = 529;
					PlayerY = 352;
				}
				JumpFlag = 50;
			}
			if (((Player2X > 34 && Player2X < 128 && Player2Y > 14 && Player2Y < 128) || (Player2X > 482 && Player2X < 576 && Player2Y > 14 && Player2Y < 128) || (Player2X > 34 && Player2X < 128 && Player2Y > 302 && Player2Y < 416) || (Player2X > 482 && Player2X < 576 && Player2Y > 302 && Player2Y < 416) || (Player2X > 258 && Player2X < 352 && Player2Y > 158 && Player2Y < 272)) && JumpFlag2 == 1)
			{
				JumpPoint = GetRand(4);

				if (JumpPoint == 0)
				{
					Player2X = 305;
					Player2Y = 208;
				}
				else if (JumpPoint == 1)
				{
					Player2X = 81;
					Player2Y = 64;
				}
				else if (JumpPoint == 2)
				{
					Player2X = 529;
					Player2Y = 64;
				}
				else if (JumpPoint == 3)
				{
					Player2X = 81;
					Player2Y = 352;
				}
				else if (JumpPoint == 4)
				{
					Player2X = 529;
					Player2Y = 352;
				}
				JumpFlag2 = 50;
			}
		}

		if (JumpFlag > 1)
		{
			JumpFlag--;
		}
		if (JumpFlag2 > 1)
		{
			JumpFlag2--;
		}
		//PlayerX++;
		//Player2X--;
		//PlayerHP = 40;
		//Player2HP = 40;

		for (a = 0; a < 10; a++)
		{
			if (ArrowFlag[a] == 1)
			{
				if (ArrowDirect[a] == 1)
				{
					ArrowX[a] += 10;
					if (ArrowX[a] > 640)
					{
						ArrowFlag[a] = 0;
					}
				}
				if (ArrowDirect[a] == 2)
				{
					ArrowX[a] -= 10;
					if (ArrowX[a] < -30)
					{
						ArrowFlag[a] = 0;
					}
				}
				if (ArrowDirect[a] == 3)
				{
					ArrowY[a] += 10;
					if (ArrowY[a] > 480)
					{
						ArrowFlag[a] = 0;
					}
				}
				if (ArrowDirect[a] == 4)
				{
					ArrowY[a] -= 10;
					if (ArrowY[a] < -30)
					{
						ArrowFlag[a] = 0;
					}
				}
			}
		}

		/*if (CheckHitKey(KEY_INPUT_Q) == 1)
		{
			Player2HP = 0;
		}
		if (CheckHitKey(KEY_INPUT_P) == 1)
		{
			PlayerHP = 0;
		}*/

		if (CheckHitKey(KEY_INPUT_SPACE) == 1 && SwordMoveFlag == 0)
		{
			if (PlayerDirect == 1)
			{
				if (Player2Y < PlayerY + 50 && PlayerY < Player2Y + 50 && PlayerX + 60 > Player2X && PlayerX + 30 <= Player2X  && DamageStun == 0 && DamageStun2 == 0)
				{
					Player2X += 50;
					KillCount += 2;
					Player2HP -= 2;
					Player2SP++;
					Player2Stun = 5;
					DamageStun2 = 10;
					ArtPoint += 5;
					ArtPoint2-= 2;
				}
				/*
				for (a = 0; a < 10; a++)
				{
					if (ArrowFlag[a] == 1)
					{
						if (ArrowDirect[a] == 2)
						{
							if()
						}
					}
				}
				*/
			}
			if (PlayerDirect == 2)
			{
				if (Player2Y < PlayerY + 50 && PlayerY < Player2Y + 50 && Player2X + 60 > PlayerX && Player2X + 30 <= PlayerX  && DamageStun == 0 && DamageStun2 == 0)
				{
					Player2X -= 50;
					KillCount += 2;
					Player2HP -= 2;
					Player2SP++;
					Player2Stun = 5;
					DamageStun2 = 10;
					ArtPoint += 5;
					ArtPoint-= 2;
				}
				/*
				for (a = 0; a < 10; a++)
				{
					if (ArrowFlag[a] == 1)
					{
						if (ArrowDirect[a] == 1)
						{
							if (ArrowX[a] + 30 > PlayerX - 30 && ArrowX[a] + 30 < PlayerX && ArrowY[a] < PlayerY + 50 && ArrowY[a] + 6 > PlayerY)
							{

							}
						}
					}
				}
				*/
			}
			if (PlayerDirect == 3)
			{
				if (Player2X < PlayerX + 30 && PlayerX < Player2X + 30 && PlayerY + 80 > Player2Y && PlayerY + 50 <= Player2Y  && DamageStun == 0 && DamageStun2 == 0)
				{
					Player2Y += 50;
					KillCount += 2;
					Player2HP -= 2;
					Player2SP++;
					Player2Stun = 5;
					DamageStun2 = 10;
					ArtPoint += 5;
					ArtPoint-= 2;
				}
				/*
				for (a = 0; a < 10; a++)
				{
					if (ArrowFlag[a] == 1)
					{
						if (ArrowDirect[a] == 1)
						{

						}
					}
				}
				*/
			}
			if (PlayerDirect == 4)
			{
				if (Player2X < PlayerX + 30 && PlayerX < Player2X + 30 && Player2Y + 80 > PlayerY && Player2Y + 50 <= PlayerY  && DamageStun == 0 && DamageStun2 == 0)
				{
					Player2Y -= 50;
					KillCount += 2;
					Player2HP -= 2;
					Player2SP++;
					Player2Stun = 5;
					DamageStun2 = 10;
					ArtPoint += 5;
					ArtPoint-= 2;
				}
				/*
				for (a = 0; a < 10; a++)
				{
					if (ArrowFlag[a] == 1)
					{
						if (ArrowDirect[a] == 1)
						{

						}
					}
				}*/
			}
			SwordMove = 0;
			SwordMoveFlag = 1;
		}

		if (DamageStun > 0)
		{
			DamageStun--;
		}
		if (DamageStun2 > 0)
		{
			DamageStun2--;
		}


		if (SwordMoveFlag == 1)
		{
			SwordMove++;
			SwordLight += 5;
		}
		if (SwordMove == 10)
		{
			SwordMoveFlag = 0;
			SwordMove = 0;
			SwordLight = 0;
		}


		for (a = 0; a < 10; a++)
		{
			if (ArrowFlag[a] == 1 && DamageStun == 0 && DamageStun2 == 0)
			{
				if (ArrowDirect[a] == 1)
				{
					if (ArrowX[a] + 30 > PlayerX && ArrowX[a] < PlayerX + 30 && ArrowY[a] < PlayerY + 50 && ArrowY[a] + 6 > PlayerY)
					{
						ArrowFlag[a] = 0;
						PlayerX += 50;
						PlayerStun = 20;
						DeathCount += 2;
						PlayerHP -= 2;
						PlayerSP++;
						DamageStun = 10;
						ArtPoint-= 2;
						ArtPoint2 += 5;
					}
				}
				if (ArrowDirect[a] == 2)
				{
					if (ArrowX[a] < PlayerX + 30 && ArrowX[a] + 30 > PlayerX && ArrowY[a] < PlayerY + 50 && ArrowY[a] + 6 > PlayerY)
					{
						ArrowFlag[a] = 0;
						PlayerX -= 50;
						PlayerStun = 20;
						DeathCount += 2;
						PlayerHP -= 2;
						PlayerSP++;
						DamageStun = 10;
						ArtPoint-= 2;
						ArtPoint2 += 5;
					}
				}
				if (ArrowDirect[a] == 3)
				{
					if (ArrowY[a] + 30 > PlayerY && ArrowY[a] < PlayerY + 50 && ArrowX[a] < PlayerX + 30 && ArrowX[a] + 6 > PlayerX)
					{
						ArrowFlag[a] = 0;
						PlayerY += 50;
						PlayerStun = 20;
						DeathCount += 2;
						PlayerHP -= 2;
						PlayerSP++;
						DamageStun = 10;
						ArtPoint-= 2;
						ArtPoint2 += 5;
					}
				}
				if (ArrowDirect[a] == 4)
				{
					if (ArrowY[a] < PlayerY + 50 && ArrowY[a] + 30 > PlayerY && ArrowX[a] < PlayerX + 30 && ArrowX[a] + 6 > PlayerX)
					{
						ArrowFlag[a] = 0;
						PlayerY -= 50;
						PlayerStun = 20;
						DeathCount += 2;
						PlayerHP -= 2;
						PlayerSP++;
						DamageStun = 10;
						ArtPoint-= 2;
						ArtPoint2 += 5;
					}
				}
			}
		}

		if (PlayerSP > 10) PlayerSP = 10;
		if (Player2SP > 10) Player2SP = 10;

		if (PlayerSP == 10)
		{
			if (CheckHitKey(KEY_INPUT_N) == 1)
			{
				SPSwordFlag = 1;
				SPTime = 1000;
				PlayerSP = 0;
				ArtPoint -= 10;
			}
		}
		if (Player2SP == 10)
		{
			if (CheckHitKey(KEY_INPUT_LSHIFT) == 1)
			{
				SPArrowFlag = 1;
				SPTime2 = 1000;
				Player2SP = 0;
				SPArrowX = PlayerX - 35;
				SPArrowY = PlayerY - 25;
				ArtPoint2 -= 10;
			}
		}

		if (SPTime > 0) SPTime--;
		if (SPTime2 > 0) SPTime2--;

		if (SPTime == 0) SPSwordFlag = 0;
		if (SPTime2 == 0) SPArrowFlag = 0;

		if(SPSwordFlag == 1 && PlayerX - 30 < Player2X + 30 && PlayerX + 60 > Player2X && PlayerY - 30 < Player2Y + 50 && PlayerY + 80 > Player2Y && DamageStun2 == 0)
		{
			if (Player2Direct == 1) Player2X -= 50;
			if (Player2Direct == 2) Player2X += 50;
			if (Player2Direct == 3) Player2Y -= 50;
			if (Player2Direct == 4) Player2Y += 50;

			Player2HP -= 2;
			KillCount += 2;
			DamageStun2 = 10;
			if (Player2HP <= 0) ArtPoint += 20;
		}
		if (SPArrowFlag == 1 && SPArrowX < PlayerX + 30 && SPArrowX + 100 > PlayerX && SPArrowY < PlayerY + 50 && SPArrowY + 100 > PlayerY && DamageStun == 0 && SPTime2 < 900 && ArrowFallFlag == 0 && ArrowWait == 0)
		{
			ArrowFallFlag = 1;
			ArrowFallX = SPArrowX;
			ArrowFallY = -50;
		}

		if (ArrowFallFlag == 1)
		{
			ArrowFallY += 30;
		}
		if (ArrowFallFlag == 1)
		{
			if (ArrowFallY > SPArrowY)
			{
				ArrowFallFlag = 0;
				PlayerHP -= 20;
				DeathCount += 20;
				DamageStun = 10;
				ArrowWait = 100;
				if (PlayerHP <= 0) ArtPoint2 += 20;
			}
		}
		if (ArrowWait > 0) ArrowWait--;

		if (CheckHitKey(KEY_INPUT_TAB) == 1 && CoolTime > 40)
		{
			while (ArrowFlag[d] == 1)
			{
				d++;
				if (d > 8)
				{
					d = 0;
				}
			}
			if (ArrowFlag[d] == 0)
			{
				ArrowFlag[d] = 1;
				CoolTime = 0;
				if (Player2Direct == 1)
				{
					ArrowX[d] = Player2X + 30;
					ArrowY[d] = Player2Y + 22;
					ArrowDirect[d] = 1;
				}
				if (Player2Direct == 2)
				{
					ArrowX[d] = Player2X - 30;
					ArrowY[d] = Player2Y + 22;
					ArrowDirect[d] = 2;
				}
				if (Player2Direct == 3)
				{
					ArrowX[d] = Player2X + 12;
					ArrowY[d] = Player2Y + 50;
					ArrowDirect[d] = 3;
				}
				if (Player2Direct == 4)
				{
					ArrowX[d] = Player2X + 12;
					ArrowY[d] = Player2Y - 30;
					ArrowDirect[d] = 4;
				}
			}
		}

		if (PlayerX < Player2X + 30 && Player2X < PlayerX + 30 && PlayerY < Player2Y + 50 && Player2Y < PlayerY + 50)
		{
			DeathCount++;
			KillCount++;
			PlayerHP--;
			Player2HP--;
			DamageStun = 10;
			DamageStun2 = 10;
			if (PlayerDirect == 1)
			{
				PlayerX -= 50;
				Player2X += 50;
			}
			if (PlayerDirect == 2)
			{
				PlayerX += 50;
				Player2X -= 50;
			}
			if (PlayerDirect == 3)
			{
				PlayerY -= 50;
				Player2Y += 50;
			}
			if (PlayerDirect == 4)
			{
				PlayerY += 50;
				Player2Y -= 50;
			}
		}

		CoolTime++;

		if (PlayerHP <= 0)
		{
			if (Player2HP <= 0)
			{
				goto draw;
			}
			goto twoPwin;
		}
		if (Player2HP <= 0)
		{
			goto onePwin;
		}

		if (Stage == 1)
		{
			DrawGraph(0, 0, FieldGraph, TRUE);
		}
		if (Stage == 2)
		{
			DrawGraph(0, 0, Field2Graph, TRUE);
		}
		if (Stage == 3)
		{
			DrawGraph(0, 0, Field3Graph, TRUE);
		}

		if (CureFlag == 1)
		{
			DrawGraph(CureX, CureY, CureGraph, TRUE);
		}

		if (PlayerDirect == 1)
		{
			for (a = 0; a < SwordLight; a++)
			{
				DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 7 / 4 + PI * a / 100, SwordLightGraph, TRUE, FALSE);
			}
			DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 2 + PI * SwordMove / 20, SwordGraph, TRUE, FALSE);
		}
		if (PlayerDirect == 2)
		{
			for (a = 0; a < SwordLight; a++)
			{
				DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 3 / 4 + PI * a / 100, SwordLightGraph, TRUE, FALSE);
			}
			DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 1 + PI * SwordMove / 20, SwordGraph, TRUE, FALSE);
		}
		if (PlayerDirect == 4)
		{
			for (a = 0; a < SwordLight; a++)
			{
				DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 5 / 4 + PI * a / 100, SwordLightGraph, TRUE, FALSE);
			}
			DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 3 / 2 + PI * SwordMove / 20, SwordGraph, TRUE, FALSE);
		}


	/*	Foot = Time % 6;
		Foot2 = Time2 % 6;

		if (PlayerDirect == 1)
		{
			if (0 <= Foot && Foot <= 2)
			{
				PlayerGraph = LoadGraph("PlayerTurn.bmp");
			}
			if (3 <= Foot && Foot <= 5)
			{
				PlayerGraph = LoadGraph("PlayerTurn2nd.bmp");
			}
		}
		if (PlayerDirect == 2)
		{
			if (0 <= Foot && Foot <= 2)
			{
				PlayerGraph = LoadGraph("Player.bmp");
			}
			if (3 <= Foot && Foot <= 5)
			{
				PlayerGraph = LoadGraph("Player2nd.bmp");
			}
		}
		if (Player2Direct == 1)
		{
			if (0 <= Foot2 && Foot2 <= 2)
			{
				Player2Graph = LoadGraph("Player2Turn.bmp");
			}
			if (3 <= Foot2 && Foot2 <= 5)
			{
				Player2Graph = LoadGraph("Player2Turn2nd.bmp");
			}
		}
		if (Player2Direct == 2)
		{
			if (0 <= Foot2 && Foot2 <= 2)
			{
				Player2Graph = LoadGraph("Player2.bmp");
			}
			if (3 <= Foot2 && Foot2 <= 5)
			{
				Player2Graph = LoadGraph("Player22nd.bmp");
			}
		}
		*/

		if (DamageStun == 0 || DamageStun == 2 || DamageStun == 4 || DamageStun == 8 || DamageStun == 10)
		{
			DrawGraph(PlayerX, PlayerY, PlayerGraph, TRUE);
		}
		if (DamageStun2 == 0 || DamageStun2 == 2 || DamageStun2 == 4 || DamageStun2 == 8 || DamageStun2 == 10)
		{
			DrawGraph(Player2X, Player2Y, Player2Graph, TRUE);
		}

		if (PlayerDirect == 3)
		{
			for (a = 0; a < SwordLight; a++)
			{
				DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 1 / 4 + PI * a / 100, SwordLightGraph, TRUE, FALSE);
			}
			DrawRotaGraph(PlayerX + 15, PlayerY + 25, 1.0f, PI * 1 / 2 + PI * SwordMove / 20, SwordGraph, TRUE, FALSE);
		}

		DrawFormatString(0, 0, White, "%d", KillCount);
		DrawFormatString(0, 30, Red, "%d", DeathCount);
		DrawGraph(PHPX, PHPY, HPGGraph, TRUE);

		for (a = 0; a < 10; a++)
		{
			if (ArrowFlag[a] == 1)
			{
				if (ArrowDirect[a] == 1)
				{
					DrawGraph(ArrowX[a], ArrowY[a], ArrowRGraph, TRUE);
				}
				if (ArrowDirect[a] == 2)
				{
					DrawGraph(ArrowX[a], ArrowY[a], ArrowLGraph, TRUE);
				}
				if (ArrowDirect[a] == 3)
				{
					DrawGraph(ArrowX[a], ArrowY[a], ArrowDGraph, TRUE);
				}
				if (ArrowDirect[a] == 4)
				{
					DrawGraph(ArrowX[a], ArrowY[a], ArrowUGraph, TRUE);
				}
			}
		}

		if (Player2Direct == 1)
		{
			DrawRotaGraph(Player2X + 20, Player2Y + 25, 1.0f, 0, BowGraph, TRUE, FALSE);
		}
		if (Player2Direct == 2)
		{
			DrawRotaGraph(Player2X + 10, Player2Y + 25, 1.0f, PI, BowGraph, TRUE, FALSE);
		}
		if (Player2Direct == 3)
		{
			DrawRotaGraph(Player2X + 15, Player2Y + 35, 1.0f, PI / 2, BowGraph, TRUE, FALSE);
		}
		if (Player2Direct == 4)
		{
			DrawRotaGraph(Player2X + 15, Player2Y + 15, 1.0f, PI * 3 / 2, BowGraph, TRUE, FALSE);
		}

		if (SPSwordFlag == 1)
		{
			DrawGraph(PlayerX - 30, PlayerY - 30, SPSwordGraph, TRUE);
		}

		if (SPArrowFlag == 1)
		{
			DrawGraph(SPArrowX, SPArrowY, SPArrowGraph, TRUE);
		}
		if (ArrowFallFlag == 1)
		{
			DrawGraph(ArrowFallX, ArrowFallY, ArrowFallGraph, TRUE);
		}

		if (PlayerSP == 10) SPBGraph = LoadGraph("SPバーM.bmp");
		if (PlayerSP != 10) SPBGraph = LoadGraph("SPバー.bmp");

		DrawExtendGraph(PHPX + 1, PHPY + 1, PHPX + 1 + PlayerHP, PHPY + 11, HPBGraph, TRUE);
		DrawGraph(PSPX, PSPY, SPGGraph, TRUE);
		DrawExtendGraph(PSPX + 1, PSPY, PSPX + 1 + PlayerSP * 4, PSPY + 2, SPBGraph, TRUE);
		DrawGraph(P2HPX, P2HPY, HPGGraph, TRUE);

		if (Player2SP == 10) SPBGraph = LoadGraph("SPバーM.bmp");
		if (Player2SP != 10) SPBGraph = LoadGraph("SPバー.bmp");

		DrawExtendGraph(P2HPX + 1, P2HPY + 1, P2HPX + 1 + Player2HP, P2HPY + 11, HPBGraph, TRUE);
		DrawGraph(P2SPX, P2SPY, SPGGraph, TRUE);
		DrawExtendGraph(P2SPX + 1, P2SPY, P2SPX + 1 + Player2SP * 4, P2SPY + 2, SPBGraph, TRUE);
		ScreenFlip();
	}


gameover:;

gameclear:;

	if (EnemyHP == 0)
	{
		ClearDrawScreen();

		DrawFormatString(280, 230, Yellow, "GAME CLEAR!!");

		WaitTimer(500);

		DrawFormatString(260, 280, Red, "Congraturation!!");

		ScreenFlip();

		goto end;
	}

	ClearDrawScreen();

	DrawFormatString(280, 230, Yellow, "GAME OVER");

	WaitTimer(1000);

	ScreenFlip();

	goto end;

	if (PlayerHP <= 0)
	{
		if (Player2HP <= 0)
		{
		draw:;
			//ClearDrawScreen();

			//DrawFormatString(280, 230, Yellow, "DRAW");
			//DrawFormatString(285, 270, Red, "1P     2P");
			//DrawFormatString(170, 300, Red, "ダメージ量 : %d     %d", KillCount, DeathCount);

			//ScreenFlip();

			//WaitTimer(1000);

			goto result;
		}
	twoPwin:;
		//ClearDrawScreen();

		//DrawFormatString(280, 230, Yellow, "2P WIN");
		//DrawFormatString(285, 270, Red, "1P     2P");
		//DrawFormatString(170, 300, Red, "ダメージ量 : %d     %d", KillCount, DeathCount);

		//ScreenFlip();

		//WaitTimer(1000);

		VictoryPoint2 += 50;

		goto result;
	}
	if (Player2HP <= 0)
	{
	onePwin:;
		//ClearDrawScreen();

		//DrawFormatString(280, 230, Yellow, "1P WIN");
		//DrawFormatString(285, 270, Red, "1P     2P");
		//DrawFormatString(170, 300, Red, "ダメージ量 : %d     %d", KillCount, DeathCount);

		//ScreenFlip();

		//WaitTimer(1000);

		VictoryPoint += 50;

		goto result;
	}

result:;
	ClearDrawScreen();
	DrawGraph(0, 0, ResultGraph[0], TRUE);

	ScreenFlip();
	WaitTimer(1000);

	DrawGraph(0, 0, ResultGraph[1], TRUE);
	DrawFormatStringToHandle(170, 190, Yellow, FontHandle, "%d",KillCount);
	DrawFormatStringToHandle(410, 190, Yellow, FontHandle, "%d",DeathCount);

	ScreenFlip();
	WaitTimer(1000);
	ClearDrawScreen();

	DrawGraph(0, 0, ResultGraph[2], TRUE);

	DrawFormatStringToHandle(170, 190, Yellow, FontHandle, "%d", KillCount);
	DrawFormatStringToHandle(410, 190, Yellow, FontHandle, "%d", DeathCount);
	DrawFormatStringToHandle(170, 260, Yellow, FontHandle, "%d", ArtPoint);
	DrawFormatStringToHandle(410, 260, Yellow, FontHandle, "%d", ArtPoint2);

	ScreenFlip();
	WaitTimer(1000);
	ClearDrawScreen();

	DrawGraph(0, 0, ResultGraph[3], TRUE);

	DrawFormatStringToHandle(170, 190, Yellow, FontHandle, "%d", KillCount);
	DrawFormatStringToHandle(410, 190, Yellow, FontHandle, "%d", DeathCount);
	DrawFormatStringToHandle(170, 260, Yellow, FontHandle, "%d", ArtPoint);
	DrawFormatStringToHandle(410, 260, Yellow, FontHandle, "%d", ArtPoint2);
	DrawFormatStringToHandle(170, 340, Yellow, FontHandle, "%d", VictoryPoint);
	DrawFormatStringToHandle(410, 340, Yellow, FontHandle, "%d", VictoryPoint2);

	ScreenFlip();
	WaitTimer(1000);
	ClearDrawScreen();

	DrawGraph(0, 0, ResultGraph[4], TRUE);

	DrawFormatStringToHandle(170, 190, Yellow, FontHandle, "%d", KillCount);
	DrawFormatStringToHandle(410, 190, Yellow, FontHandle, "%d", DeathCount);
	DrawFormatStringToHandle(170, 260, Yellow, FontHandle, "%d", ArtPoint);
	DrawFormatStringToHandle(410, 260, Yellow, FontHandle, "%d", ArtPoint2);
	DrawFormatStringToHandle(170, 340, Yellow, FontHandle, "%d", VictoryPoint);
	DrawFormatStringToHandle(410, 340, Yellow, FontHandle, "%d", VictoryPoint2);

	ScreenFlip();
	WaitKey();
	ClearDrawScreen();

	DrawGraph(0, 0, ResultGraph[5], TRUE);

	TotalScore = KillCount + ArtPoint + VictoryPoint;
	TotalScore2 = DeathCount + ArtPoint2 + VictoryPoint2;

	DrawFormatStringToHandle(170, 200, Yellow, FontHandle, "%d", TotalScore);
	DrawFormatStringToHandle(410, 200, Yellow, FontHandle, "%d", TotalScore2);

	ScreenFlip();
	WaitTimer(500);
	ClearDrawScreen();

	DrawGraph(0, 0, ResultGraph[6], TRUE);

	DrawFormatStringToHandle(170, 200, Yellow, FontHandle, "%d", TotalScore);
	DrawFormatStringToHandle(410, 200, Yellow, FontHandle, "%d", TotalScore2);
	if(TotalScore > TotalScore2) 	DrawFormatStringToHandle(230, 320, Yellow, FontHandle, "1Pの勝利！");
	if(TotalScore < TotalScore2) 	DrawFormatStringToHandle(230, 320, Yellow, FontHandle, "2Pの勝利！");
	if(TotalScore == TotalScore2) 	DrawFormatStringToHandle(230, 320, Yellow, FontHandle, "引き分け！");

	ScreenFlip();
	WaitTimer(500);

end:;

	WaitKey();				// キー入力待ち

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}