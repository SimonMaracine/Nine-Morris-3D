#pragma once

#include "nine_morris_3d_engine/application/platform.hpp"

/*
    Define a pseudo main function with this declaration:

    int sm_application_main();

    Include this entry_point.hpp header file once into your main compilation unit.
*/
int sm_application_main(int argc, char** argv);

#if defined(SM_PLATFORM_LINUX)
    int main(int argc, char** argv) {
        return sm_application_main(argc, argv);
    }
#elif defined(SM_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>
    #include <shellapi.h>

    #include <cwchar>

    #if defined(SM_BUILD_DISTRIBUTION)
        INT WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, INT) {
            int argc {0};
            char** argv {nullptr};

            // Windows forces wide strings upon us :P
            // Must safely convert back into simple strings
            // If an error occurs, simply pass an empty array of arguments
            // Must use GetCommandLineW() in order to have the program name as the first argument :P
            LPWSTR* wargv {CommandLineToArgvW(GetCommandLineW(), &argc)};

            if (wargv == nullptr) {
                goto ret_error;
            }

            try {
                argv = new char*[argc];   
            } catch (const std::bad_alloc&) {
                goto ret_error;    
            }

            try {
                for (int i {0}; i < argc; i++) {
                    const auto length {std::wcslen(wargv[i])};

                    argv[i] = new char[length + 1];
                        
                    for (int c {0}; c < length; c++) {
                        const char character {static_cast<char>(wargv[i][c])};
                        argv[i][c] = character >= 0 && character <= 127 ? character : '?';
                    }
                    argv[i][length] = 0;
                }
            } catch (const std::bad_alloc&) {
                for (int i {0}; i < argc; i++) {
                    delete[] argv[i];
                }
                delete[] argv;

                goto ret_error;
            }

            return sm_application_main(argc, argv);

        ret_error:
            argc = 0;
            argv = nullptr;
            return sm_application_main(argc, argv);
        }
    #else
        int main(int argc, char** argv) {
            return sm_application_main(argc, argv);
        }
    #endif
#endif
