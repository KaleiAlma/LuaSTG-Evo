#pragma once
#include <vector>
#include <string>
#include <string_view>

namespace Platform
{
    class CommandLineArguments
    {
    private:
        std::vector<std::string> m_args;
    public:
        bool Update(int argc, char *argv[]);
        bool GetArguments(std::vector<std::string_view>& list);
        bool IsOptionExist(std::string_view option);
    public:
        CommandLineArguments();
        ~CommandLineArguments();
    public:
        static CommandLineArguments& Get();
    };
}
