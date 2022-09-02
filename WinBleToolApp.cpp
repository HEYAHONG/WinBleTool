/***************************************************************
 * Name:      WinBleToolApp.cpp
 * Purpose:   Code for Application Class
 * Author:    HYH (2229388563@qq.com)
 * Created:   2022-09-01
 * Copyright: HYH (hyhsystem.cn)
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "WinBleToolApp.h"
#include "WinBleToolMain.h"

IMPLEMENT_APP(WinBleToolApp);

bool WinBleToolApp::OnInit()
{

    WinBleToolDialog* dlg = new WinBleToolDialog(0L);
    dlg->Show();
    return true;
}
