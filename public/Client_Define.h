#pragma once
#define _SCL_SECURE_NO_WARNINGS

#include <process.h>

static const unsigned int g_iWinCX = 1280;
static const unsigned int g_iWinCY = 720;

enum CHARACTER {CH_YU,CH_YUKIKO,CH_KANJI,CH_NAOTO,CH_END};

enum SCENE { SCENE_LOADING, SCENE_LOGO, SCENE_GAMEPLAY,SCENE_BATTLE, SCENE_STATIC, SCENE_END };

enum DIR {DIR_FORWARD,DIR_BACKWARD,DIR_LEFT,DIR_RIGHT,DIR_END};

enum STATE { STATE_IDLE, STATE_RUN, STATE_ATTACK, STATE_END };

enum PLAYERANI { P_ATTACK, P_ATTACK2, P_CARD, P_DAMAGED, P_DEAD, P_DEATH, P_DEFENCE, P_DOWN, P_DOWNLOOP, P_IDLE, P_LOWHP, P_MISS, P_RUN, P_STANDUP, P_USINGCARD, P_USINGITEM, P_WIN,P_END };
enum MONSTERANI { M_AROUND, M_ATTACK, M_DAMAGED, M_IDLE, M_LOOK, M_RUN };

//전투상태 
enum BATTLE { BATTLE_START, BATTLE_PLAYER, BATTLE_ENEMY, BATTLE_WIN, BATTLE_LOSE, BATTLE_END };

extern HINSTANCE g_hInst;
extern HWND g_hWnd;



#include <iostream>
#include <string>
#include "Client_Struct.h"

namespace Client {}

using namespace Client;


#include <Vfw.h>
#pragma comment (lib, "vfw32.lib")