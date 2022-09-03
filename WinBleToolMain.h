/***************************************************************
 * Name:      WinBleToolMain.h
 * Purpose:   Defines Application Frame
 * Author:    HYH (2229388563@qq.com)
 * Created:   2022-09-01
 * Copyright: HYH (hyhsystem.cn)
 * License:
 **************************************************************/

#ifndef WINBLETOOLMAIN_H
#define WINBLETOOLMAIN_H



#include "WinBleToolApp.h"



#include "GUIDialog.h"
#include "windows.h"
#include "mutex"
#include "functional"
#include "queue"
#include <memory>
#include <map>
#include <BleDevice.h>

class WinBleToolDialog: public GUIDialog
{
public:
    WinBleToolDialog(wxWindow * dlg);
    ~WinBleToolDialog();
private:
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnQuit(wxCommandEvent& event);
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnMenuRefresh( wxCommandEvent& event );
    virtual void OnUpdateUITimer( wxTimerEvent& event );
    virtual void OnTreeItemRightClick( wxTreeEvent& event );

    /*
        许多更新UI的操作最好在定时器/主线程内调用,故需要更新UI的队列
    */
    struct
    {
        std::mutex lock;
        std::queue<std::function<void(void)>> Queue;
    } UpdateUI;

    /*
        向更新UI队列中添加带有UI操作的函数
    */
    void AddUpdateUIFunciton(std::function<void(void)> func)
    {
        std::lock_guard<std::mutex> lock(UpdateUI.lock);
        UpdateUI.Queue.push(func);
    };
    /*
        处理更新UI队列中的函数
    */
    void ProcessUpdateUIFunction()
    {
        std::lock_guard<std::mutex> lock(UpdateUI.lock);
        while(UpdateUI.Queue.size()>0)
        {
            std::function<void(void)> cb=UpdateUI.Queue.front();
            if(cb!=NULL)
            {
                cb();
            }
            UpdateUI.Queue.pop();
        };
    }


    /*
        刷新设备列表
    */
    void RefreshBLEDeviceList();

    /*
        设备列表(只在主线程及定时器内访问)
    */
    std::map<std::wstring,std::shared_ptr<BleDevice>> BLEDeviceList;

};
#endif // WINBLETOOLMAIN_H
