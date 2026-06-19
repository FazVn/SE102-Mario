#include "Core/Game.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand)
{
    Game game(instance);
    if (!game.Initialize(showCommand))
    {
        return -1;
    }

    return game.Run();
}
