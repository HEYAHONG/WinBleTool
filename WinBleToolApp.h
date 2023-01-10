/***************************************************************
 * Name:      WinBleToolApp.h
 * Purpose:   Defines Application Class
 * Author:    HYH (2229388563@qq.com)
 * Created:   2022-09-01
 * Copyright: HYH (hyhsystem.cn)
 * License:
 **************************************************************/

#ifndef WINBLETOOLAPP_H
#define WINBLETOOLAPP_H
#include <winsock2.h>
#include <wx/app.h>

class WinBleToolApp : public wxApp
{
public:
    virtual bool OnInit();
};

#endif // WINBLETOOLAPP_H
