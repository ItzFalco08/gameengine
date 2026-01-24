#include "WinMsg.hpp"
#include <windows.h>

namespace WinMsg {
    void Error(const std::string& title, const std::string& message) {
        MessageBoxA(
            nullptr,
            message.c_str(),
            title.c_str(),
            MB_ICONERROR | MB_OK
        );
    }

    void Info(const std::string& title, const std::string& message) {
        MessageBoxA(
            nullptr,
            message.c_str(),
            title.c_str(),
            MB_ICONINFORMATION | MB_OK
        );
    }

    bool YesNo(const std::string& title, const std::string& message) {
        return MessageBoxA(
            nullptr,
            message.c_str(),
            title.c_str(),
            MB_YESNO | MB_ICONWARNING
        ) == IDYES;
    }

    void Warning(const std::string& title, const std::string& message) {
        MessageBoxA(
            nullptr,
            message.c_str(),
            title.c_str(),
            MB_ICONWARNING | MB_OK
        );
    }
}
