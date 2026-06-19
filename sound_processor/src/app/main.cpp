#include "app/application.h"

#include <iostream>

int main(int argc, char* argv[])
{
    static constexpr int EXIT_STD_EXCEPTION = 1;
    static constexpr int EXIT_UNKNOWN_EXCEPTION = 2;

    Application app;
    try
    {
        app.configure();
        return app.start(argc, argv);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_STD_EXCEPTION;
    }
    catch(...)
    {
        std::cerr << "Error: unknown exception.\n";
        return EXIT_UNKNOWN_EXCEPTION;
    }
}
