#pragma once
#include <string>

namespace WinMsg {
    extern void Error(const std::string& title, const std::string& message);
    extern void Info(const std::string& title, const std::string& message);
    extern void Warning(const std::string& title, const std::string& message);
    extern bool YesNo(const std::string& title, const std::string& message);
}