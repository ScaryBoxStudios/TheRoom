#include "Console.hpp"
#include <iostream>

void Console::Init()
{

}

void Console::Shutdown()
{
    mCommandMap.clear();
}

bool Console::HasCommand(const std::string& command)
{
    return mCommandMap.find(command) != std::end(mCommandMap);
}

void Console::RegisterCommand(const std::string& command, Console::CommandCb cb)
{
    mCommandMap[command] = cb;
}

void Console::OnKey(char k)
{
    if (k == '\r')
    {
        std::cout << std::endl << "Received command: " << mCommandBuffer << std::endl;
        // Search for command and if exists execute it
        auto f = mCommandMap.find(mCommandBuffer);
        if (f != std::end(mCommandMap)) {
            f->second();
            std::cout << "Executing command: " << mCommandBuffer << std::endl;
        }
        // Clear command buffer
        mCommandBuffer = "";
    }
    else if (k == '\b')
    {
        // Remove last key inputed if it exists
        mCommandBuffer = mCommandBuffer.substr(0, mCommandBuffer.size() - 1);
        std::cout << "\b " << k;
    }
    else
    {
        // Check if printable character and add it to command buffer
        // TODO: Check for printable character
        std::cout << k;
        mCommandBuffer += k;
    }
}
