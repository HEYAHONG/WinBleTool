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
#include "BLEwxTreeItemData.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include "wx/log.h"
#include <sstream>
#include "misc.h"

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
            {
                //清空原有数据
                m_treeCtrl1->DeleteAllItems();
                m_treeCtrl1->AddRoot(_T("BLE"));

            }
            for (BleDeviceInfo const *i : BleDeviceEnumerator::getBleDevices())
            {

                wxLogMessage(_T("\r\n名称: %s\r\nHardwareId: %s\r\nInstanceId: %s"),i->getName(),i->getHardwareId(),i->getInstanceId());
                wxLogMessage(_T("正在枚举BLE设备服务..."));

                //添加至TreeCtrl
                wxTreeItemId treedevice=m_treeCtrl1->AppendItem(m_treeCtrl1->GetRootItem(),wxString(_T("设备:"))+i->getName()+" ("+i->getInstanceId()+")");
                {
                    BLEDevicewxTreeItemData * data=new BLEDevicewxTreeItemData();
                    data->setInstanceId(i->getInstanceId());
                    m_treeCtrl1->SetItemData(treedevice,data);
                }

                BleDevice bleDevice = BleDevice(i->getInstanceId());
                bleDevice.enumerateBleServices();
                {
                    std::stringstream out;
                    out << "BleGattServices count: " << bleDevice.getBleGattServices().size() << endl;
                    for (unique_ptr<BleGattService> const& s : bleDevice.getBleGattServices())
                    {
                        try
                        {
                            //显示服务信息
                            auto tohex=[](uint32_t num) -> std::string
                            {
                                char buff[32]={0};
                                sprintf(buff,"%x",num);
                                return buff;
                            };
                            out << "Service - Guid: ["
                                << Utility::guidToString(s->getServiceUuid().Value.LongUuid) << "]"
                                << " Short Id: [" << tohex(s->getServiceUuid().Value.ShortUuid) << "]" << endl;

                            //添加服务
                            wxString UUID;
                            if(s->getServiceUuid().IsShortUuid)
                            {
                                UUID=tohex(s->getServiceUuid().Value.ShortUuid);
                                std::string desc=GetBLEUuidDescByShortUuid(s->getServiceUuid().Value.ShortUuid);
                                if(!desc.empty())
                                {
                                    UUID+=std::string(" [")+desc+"] ";
                                }
                            }
                            else
                            {
                                UUID=Utility::guidToString(s->getServiceUuid().Value.LongUuid);
                            }
                            wxTreeItemId treeservice=m_treeCtrl1->AppendItem(treedevice,wxString(_T("服务:"))+ UUID);
                            {
                                BLEServicewxTreeItemData *data=new BLEServicewxTreeItemData();
                                data->setInstanceId(i->getInstanceId());
                                data->setServiceUUID(s->getServiceUuid());
                                m_treeCtrl1->SetItemData(treeservice,data);
                            }

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

                                wxString flags;
                                {
                                    if(c->getIsBroadcastable())
                                    {
                                        flags+=_T(" Broadcastable");
                                    }
                                    if(c->getIsIndicatable())
                                    {
                                        flags+=_T(" Indicatable");
                                    }
                                    if(c->getIsNotifiable())
                                    {
                                        flags+=_T(" Notifiable");
                                    }
                                    if(c->getIsReadable())
                                    {
                                        flags+=_T(" Readable");
                                    }
                                    if(c->getIsSignedWritable())
                                    {
                                        flags+=_T(" SignedWritable");
                                    }
                                    if(c->getIsWritable())
                                    {
                                        flags+=_T(" Writable");
                                    }
                                    if(c->getIsWritableWithoutResponse())
                                    {
                                        flags+=_T(" WritableWithoutResponse");
                                    }
                                }

                                //添加特征值
                                wxString UUID;
                                if(c->getCharacteristicUuid().IsShortUuid)
                                {
                                    UUID=tohex(c->getCharacteristicUuid().Value.ShortUuid);
                                    std::string desc=GetBLEUuidDescByShortUuid(c->getCharacteristicUuid().Value.ShortUuid);
                                    if(!desc.empty())
                                    {
                                        UUID+=std::string(" [")+desc+"] ";
                                    }
                                }
                                else
                                {
                                    UUID=Utility::guidToString(c->getCharacteristicUuid().Value.LongUuid);
                                }
                                wxTreeItemId treechar=m_treeCtrl1->AppendItem(treeservice,wxString(_T("特征:"))+UUID+" ("+flags+" )");
                                {
                                    BLECharwxTreeItemData *data=new BLECharwxTreeItemData();
                                    data->setInstanceId(i->getInstanceId());
                                    data->setServiceUUID(s->getServiceUuid());
                                    data->setCharUUID(c->getCharacteristicUuid());
                                    {
                                        data->flag.notify=c->getIsNotifiable();
                                        data->flag.read=c->getIsReadable();
                                        data->flag.signedwrite=c->getIsSignedWritable();
                                        data->flag.write=c->getIsWritable();
                                        data->flag.writewithoutresponse=c->getIsWritableWithoutResponse();
                                    }
                                    m_treeCtrl1->SetItemData(treechar,data);
                                }

                                try
                                {
                                    c->enumerateBleDescriptors();
                                    for(unique_ptr<BleGattDescriptor> const &d:c->getBleDescriptors())
                                    {
                                        out << "\t\tDescriptor - Guid: ["
                                            << Utility::guidToString(d->getDescriptorUuid().Value.LongUuid) << "]"
                                            << " Short Id: [" << tohex(d->getDescriptorUuid().Value.ShortUuid) << "]" << endl;

                                        wxString UUID;
                                        if(d->getDescriptorUuid().IsShortUuid)
                                        {
                                            UUID=tohex(d->getDescriptorUuid().Value.ShortUuid);
                                            std::string desc=GetBLEUuidDescByShortUuid(d->getDescriptorUuid().Value.ShortUuid);
                                            if(!desc.empty())
                                            {
                                                UUID+=std::string(" [")+desc+"] ";
                                            }
                                        }
                                        else
                                        {
                                            UUID=Utility::guidToString(d->getDescriptorUuid().Value.LongUuid);
                                        }
                                        wxTreeItemId treedesc=m_treeCtrl1->AppendItem(treechar,wxString(_T("描述符:"))+UUID);
                                        {
                                            BLEDescwxTreeItemData *data=new BLEDescwxTreeItemData();
                                            data->setInstanceId(i->getInstanceId());
                                            data->setServiceUUID(s->getServiceUuid());
                                            data->setCharUUID(c->getCharacteristicUuid());
                                            data->setDescUUID(d->getDescriptorUuid());
                                            m_treeCtrl1->SetItemData(treedesc,data);
                                        }


                                    }
                                }
                                catch (BleException const &e)
                                {
                                    wxLogError(e.what());
                                    wxLogError(_T("访问特征描述出错..."));
                                }

                            }
                        }
                        catch (BleException const &e)
                        {
                            wxLogError(e.what());
                            wxLogError(_T("访问设备服务出错..."));
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


                                try
                                {
                                    readCharacteristicValueAndDisplay(characteristics, GATT_UUID_MANU_NAME);

                                    readCharacteristicValueAndDisplay(characteristics, GATT_UUID_SW_VERSION_STR);

                                    readCharacteristicValueAndDisplay(characteristics, GATT_UUID_MODEL_NUMBER_STR);
                                }
                                catch (BleException const &e)
                                {
                                    wxLogError(e.what());
                                    wxLogError(_T("读取设备信息出错..."));
                                }


                                wxLogMessage("\r\n%s",out.str());
                            }
                        }
                        wxLogMessage(_T("\t查找设备信息服务完成..."));
                    }
                }
                wxLogMessage(_T("枚举BLE设备服务完成..."));

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

void WinBleToolDialog::OnTreeItemRightClick( wxTreeEvent& event )
{
    wxTreeItemData * data=dynamic_cast<wxTreeItemData *>(event.GetClientObject());
    if(data!=NULL)
    {
        //数据不为空
        {
            BLEDevicewxTreeItemData * device=dynamic_cast<BLEDevicewxTreeItemData *>(data);
            if(device!=NULL)
            {
                //设备项
                m_treeCtrl1->SelectItem(device->GetId());
                m_treeCtrl1->Expand(device->GetId());
            }
        }

        {
            BLEServicewxTreeItemData *service=dynamic_cast<BLEServicewxTreeItemData *>(data);
            if(service!=NULL)
            {
                //服务项
                m_treeCtrl1->SelectItem(service->GetId());
                m_treeCtrl1->Expand(service->GetId());
            }
        }

        {
            BLECharwxTreeItemData *Char=dynamic_cast<BLECharwxTreeItemData *>(data);
            if(Char!=NULL)
            {
                //特征项
                m_treeCtrl1->SelectItem(Char->GetId());
                m_treeCtrl1->Expand(Char->GetId());
                {
                    //弹出菜单
                    wxMenu menu;
                    if(Char->flag.read)
                    {
                        auto OnMenuReadChar=[=]( wxCommandEvent& event )
                        {
                            wxLogMessage(wxString(_T("正在读取设备..."))+Char->getInstanceId());
                            try
                            {
                                BleDevice bleDevice = BleDevice(Char->getInstanceId());
                                bleDevice.enumerateBleServices();
                                for (unique_ptr<BleGattService> const& s : bleDevice.getBleGattServices())
                                {
                                    BTH_LE_UUID uuid1=s->getServiceUuid();
                                    BTH_LE_UUID uuid2=Char->getServiceUUID();
                                    if(memcmp(&uuid1,&uuid2,sizeof( BTH_LE_UUID))==0)
                                    {
                                        //已找到服务
                                        {
                                            wxString UUID;
                                            if(s->getServiceUuid().IsShortUuid)
                                            {
                                                auto tohex=[](uint32_t num) -> std::string
                                                {
                                                    char buff[32]={0};
                                                    sprintf(buff,"%x",num);
                                                    return buff;
                                                };
                                                UUID=tohex(s->getServiceUuid().Value.ShortUuid);
                                                std::string desc=GetBLEUuidDescByShortUuid(s->getServiceUuid().Value.ShortUuid);
                                                if(!desc.empty())
                                                {
                                                    UUID+=std::string(" [")+desc+"] ";
                                                }
                                            }
                                            else
                                            {
                                                UUID=Utility::guidToString(s->getServiceUuid().Value.LongUuid);
                                            }
                                            wxLogMessage(wxString(_T("\t已找到服务:"))+UUID);
                                        }

                                        s->enumerateBleCharacteristics();

                                        for (unique_ptr<BleGattCharacteristic> const& c : s->getBleCharacteristics())
                                        {
                                            BTH_LE_UUID uuid3=c->getCharacteristicUuid();
                                            BTH_LE_UUID uuid4=Char->getCharUUID();
                                            if(memcmp(&uuid3,&uuid4,sizeof(BTH_LE_UUID))==0)
                                            {
                                                {
                                                    wxString UUID;
                                                    if(c->getCharacteristicUuid().IsShortUuid)
                                                    {
                                                        auto tohex=[](uint32_t num) -> std::string
                                                        {
                                                            char buff[32]={0};
                                                            sprintf(buff,"%x",num);
                                                            return buff;
                                                        };
                                                        UUID=tohex(c->getCharacteristicUuid().Value.ShortUuid);
                                                        std::string desc=GetBLEUuidDescByShortUuid(c->getCharacteristicUuid().Value.ShortUuid);
                                                        if(!desc.empty())
                                                        {
                                                            UUID+=std::string(" [")+desc+"] ";
                                                        }
                                                    }
                                                    else
                                                    {
                                                        UUID=Utility::guidToString(c->getCharacteristicUuid().Value.LongUuid);
                                                    }
                                                    wxLogMessage(wxString(_T("\t已找到特征:"))+UUID);
                                                }

                                                {
                                                    //读取数据
                                                    BleGattCharacteristicValue value=c->getValue();
                                                    auto to_hex=[](const unsigned char *data,unsigned long len)->wxString
                                                    {
                                                        wxString ret;
                                                        for(size_t i=0;i<len;i++)
                                                        {
                                                            char buff[10]={0};
                                                            sprintf(buff,"%02X ",data[i]);
                                                            ret+=buff;
                                                        }
                                                        return ret;
                                                    };
                                                    wxLogMessage(_T("读取数据（%lu字节）:\r\nASCII:\r\n%s\r\nHEX:\r\n%s"),value.getDataSize(),wxString::FromUTF8((const char *)value.getData()),to_hex(value.getData(),value.getDataSize()));
                                                }
                                                break;
                                            }

                                        }

                                        break;
                                    }
                                }
                                wxLogMessage(_T("读取数据已完成..."));
                            }
                            catch (BleException const &e)
                            {
                                wxLogError(e.what());
                                wxLogError(_T("读取出错..."));
                            }

                        };
                        wxMenuItem *item=menu.Append(1000,_T("读取"));
                        menu.Bind(wxEVT_COMMAND_MENU_SELECTED,OnMenuReadChar);
                    }
                    PopupMenu(&menu);
                }
            }
        }

        {
            BLEDescwxTreeItemData *Desc=dynamic_cast<BLEDescwxTreeItemData *>(data);
            if(Desc!=NULL)
            {
                //描述符项
                m_treeCtrl1->SelectItem(Desc->GetId());
            }
        }

    }
}

