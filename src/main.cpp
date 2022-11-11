#include "Game.h"

#ifdef WINNOCONSOLE
#include <Windows.h>
int wWinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPWSTR nCmdLine, int nCmdShow)
#else
int main()
#endif

{
    auto game = Game();
    game.play();

    return 0;
}
