#include <iostream>
#include <cstdlib>
#include "Core/App.h"

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
    catch ( ... )
	{
		std::cerr << "unknown error\n";
		exit( -1 );
  	}

    app.Exit();
    return EXIT_SUCCESS;
}