#include "MsgBox.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <iostream>
#endif

MsgBox::MsgBox(const std::string& title, const std::string& msg)
    : mTitle(title),
      mMessage(msg)
{
}

void MsgBox::Show() const
{
#ifdef _WIN32
    MessageBox(0, mMessage.c_str(), mTitle.c_str(), 0);
#else
    std::cout << mTitle << ": " << mMessage << std::endl;
#endif
}

