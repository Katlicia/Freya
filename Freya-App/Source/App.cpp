#include "Core/Application.h"
#include <iostream>
#include <exception>

//int main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    try
    {
        FRE::Application app;
        app.Run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return EXIT_FAILURE;
    }
}