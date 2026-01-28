#include "Game.h"

#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <SDL.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

namespace {

void emscriptenTick(void* arg)
{
    Game* game = static_cast<Game*>(arg);
    if (!game->tick()) {
        emscripten_cancel_main_loop();
    }
}

} // namespace
#endif

#ifdef __EMSCRIPTEN__
int main(int argc, char* argv[])
{
    static Game game;
    (void)argc;
    (void)argv;
    emscripten_set_main_loop_arg(emscriptenTick, &game, 0, true);
    return 0;
}
#else
#ifdef WINNOCONSOLE
#include <Windows.h>

int wWinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPWSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
    Game game;
    (void)argc;
    (void)argv;
    game.play();
    return 0;
}
#endif
