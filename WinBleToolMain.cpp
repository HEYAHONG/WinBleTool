/***************************************************************
 * Name:      WinBleToolMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    HYH (2229388563@qq.com)
 * Created:   2022-09-01
 * Copyright: HYH (hyhsystem.cn)
 * License:
 **************************************************************/

#include <Utility.h>
#include <BleDevice.h>
#include <BleException.h>
#include <BleGattServices.h>
#include <BleDeviceEnumerator.h>

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "WinBleToolMain.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include "wx/log.h"
#include <sstream>

extern const char * bluetooth_ico_xpm[];

WinBleToolDialog::WinBleToolDialog(wxWindow * dlg)
    : GUIDialog(dlg)
{
    SetIcon(bluetooth_ico_xpm);

    wxLog::EnableLogging(true);
    {
        //设置日志窗口
        wxLogTextCtrl *logger=new wxLogTextCtrl(m_textCtrl1);
        wxLog::SetActiveTarget(logger);
    }

    //刷新设备列表
    RefreshBLEDeviceList();
}

WinBleToolDialog::~WinBleToolDialog()
{
}

void WinBleToolDialog::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void WinBleToolDialog::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void WinBleToolDialog::OnAbout(wxCommandEvent &event)
{
    GuiDialogAbout dlg(this);
    dlg.ShowModal();
}

void WinBleToolDialog::RefreshBLEDeviceList()
{
    auto cb=[&](void)
    {
        wxLogMessage(_T("正在枚举BLE设备..."));
        try
        {

            BleDeviceEnumerator::enumerate();

            for (BleDeviceInfo const *i : BleDeviceEnumerator::getBleDevices())
            {
                try
                {
                    wxLogMessage(_T("\r\n名称: %s\r\nHardwareId: %s\r\nInstanceId: %s"),i->getName(),i->getHardwareId(),i->getInstanceId());
                    wxLogMessage(_T("正在枚举BLE设备服务..."));
                    BleDevice bleDevice = BleDevice(i->getInstanceId());
                    bleDevice.enumerateBleServices();
                    {
                        std::stringstream out;
                        out << "BleGattServices count: " << bleDevice.getBleGattServices().size() << endl;

                        for (unique_ptr<BleGattService> const& s : bleDevice.getBleGattServices())
                        {
                            auto tohex=[](uint32_t num) -> std::string
                            {
                                char buff[32]={0};
                                sprintf(buff,"%x",num);
                                return buff;
                            };
                            out << "Service - Guid: ["
                                << Utility::guidToString(s->getServiceUuid().Value.LongUuid) << "]"
                                << " Short Id: [" << tohex(s->getServiceUuid().Value.ShortUuid) << "]" << endl;

                            s->enumerateBleCharacteristics();

                            for (unique_ptr<BleGattCharacteristic> const& c : s->getBleCharacteristics())
                            {
                                out << "\tCharacteristic - Guid: ["
                                    << Utility::guidToString(c->getCharacteristicUuid().Value.LongUuid) << "]"
                                    << " Short Id: [" << tohex(c->getCharacteristicUuid().Value.ShortUuid) << "]" << endl;

                                out << "\t\tIsBroadcastable: " << +c->getIsBroadcastable() << endl
                                    << "\t\tIsIndicatable: " << +c->getIsIndicatable() << endl
                                    << "\t\tIsNotifiable: " << +c->getIsNotifiable() << endl
                                    << "\t\tIsReadable: " << +c->getIsReadable() << endl
                                    << "\t\tIsSignedWritable: " << +c->getIsSignedWritable() << endl
                                    << "\t\tIsWritable: " << +c->getIsWritable() << endl
                                    << "\t\tIsWritableWithoutResponse:" << +c->getIsWritableWithoutResponse() << endl;
                            }
                        }

                        wxLogMessage("\r\n%s",out.str());

                        {
                            auto tohex=[](uint32_t num) -> std::string
                            {
                                char buff[32]={0};
                                sprintf(buff,"%x",num);
                                return buff;
                            };
                            wxLogMessage(_T("正在查找设备信息服务..."));
                            BleDevice::BleGattServices const &services = bleDevice.getBleGattServices();
                            {
                                std::stringstream out;
                                auto it = find_if(begin(services), end(services), [&](unique_ptr<BleGattService> const& s)
                                {
                                    return s->getServiceUuid().Value.ShortUuid == GATT_UUID_DEVICE_INFO_SVC;
                                });

                                if (it != services.end())
                                {
                                    auto service = it->get();

                                    out << "Found Device Information Service: [" << tohex(service->getServiceUuid().Value.ShortUuid) << "]" << endl;

                                    BleGattService::BleGattCharacteristics const& characteristics = service->getBleCharacteristics();

                                    auto readCharacteristicValueAndDisplay=[&](BleGattService::BleGattCharacteristics const &characteristics, USHORT uuid)
                                    {
                                        auto cit = find_if(begin(characteristics), end(characteristics), [&](unique_ptr<BleGattCharacteristic> const &c)
                                        {
                                            return c->getCharacteristicUuid().Value.ShortUuid == uuid;
                                        });

                                        if (cit != characteristics.end())
                                        {
                                            auto characteristic = cit->get();

                                            BleGattCharacteristicValue value = characteristic->getValue();

                                            out << "\tUUID:" << tohex(uuid) <<"\tValue :" << value.getData() << endl;
                                        }
                                    };

                                    readCharacteristicValueAndDisplay(characteristics, GATT_UUID_MANU_NAME);

                                    readCharacteristicValueAndDisplay(characteristics, GATT_UUID_SW_VERSION_STR);

                                    readCharacteristicValueAndDisplay(characteristics, GATT_UUID_MODEL_NUMBER_STR);

                                    wxLogMessage("\r\n%s",out.str());
                                }
                            }
                            wxLogMessage(_T("\t查找设备信息服务完成..."));
                        }

                        wxLogMessage(_T("枚举BLE设备服务完成..."));
                    }

                }
                catch(BleException const &e)
                {
                    wxLogError(e.what());
                }

            }
        }
        catch (BleException const &e)
        {
            wxLogError(e.what());
            wxLogError(_T("枚举出错..."));
        }
    };
    AddUpdateUIFunciton(cb);
}

void WinBleToolDialog::OnMenuRefresh( wxCommandEvent& event )
{
    RefreshBLEDeviceList();
}
void  WinBleToolDialog::OnUpdateUITimer( wxTimerEvent& event )
{
    ProcessUpdateUIFunction();
}
