#include "grapher.h"

#include <iostream>

int main(void)
{
    try
    {
        Grapher g("settings.dat");
        g.run();
    }
    catch(std::exception &ex)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error occurs!", ex.what(), NULL);
    }
    return 0;
}
