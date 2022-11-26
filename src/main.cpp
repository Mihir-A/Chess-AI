#include "Game.h"

#ifdef WINNOCONSOLE
#include <Windows.h>

int wWinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPWSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
    Game game;
    game.play();
    return 0;
}
