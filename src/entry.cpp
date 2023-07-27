#include <iostream>
#include <cstdlib>
#include "App/app.h"

int main() {
    App app;

    app.Init();

    try
    {
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    app.Exit();
    return EXIT_SUCCESS;
}