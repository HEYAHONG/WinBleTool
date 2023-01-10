/***************************************************************
 * Name:      WinBleToolMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    HYH (2229388563@qq.com)
 * Created:   2022-09-01
 * Copyright: HYH (hyhsystem.cn)
 * License:
 **************************************************************/

#include "winsock2.h"
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
#include "InputDialog.h"
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
    dlg.m_versionstr->SetLabel(_T(PROJECT_VERSION_STR));
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
                BLEDeviceList.clear();

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

                BLEDeviceList[i->getInstanceId()]=std::make_shared<BleDevice>(i->getInstanceId());
                BleDevice &bleDevice = (*BLEDeviceList[i->getInstanceId()]);
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
                        auto ReadChar=[&]( bool directRead)
                        {
                            wxLogMessage(wxString(_T("正在读取设备..."))+Char->getInstanceId());
                            try
                            {
                                if(BLEDeviceList.find(Char->getInstanceId())==BLEDeviceList.end())
                                {
                                    return;
                                }
                                auto PbleDevice=BLEDeviceList[Char->getInstanceId()];
                                BleDevice &bleDevice = (*PbleDevice);
                                //bleDevice.enumerateBleServices();
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

                                        //s->enumerateBleCharacteristics();

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
                                                    BleGattCharacteristicValue value=c->getValue(directRead);
                                                    auto to_hex=[](const unsigned char *data,unsigned long len)->wxString
                                                    {
                                                        wxString ret;
                                                        for(size_t i=0; i<len; i++)
                                                        {
                                                            char buff[10]= {0};
                                                            sprintf(buff,"%02X ",data[i]);
                                                            ret+=buff;
                                                        }
                                                        return ret;
                                                    };
                                                    wxLogMessage(_T("读取数据（%lu字节）:\r\nASCII:\r\n%s\r\nHEX:\r\n%s"),value.getDataSize(),wxString::FromUTF8((const char *)value.getData(),value.getDataSize()),to_hex(value.getData(),value.getDataSize()));
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


                        {
                            auto OnMenuReadChar=[&]( wxCommandEvent& event )
                            {
                                ReadChar(false);
                            };
                            wxMenuItem *item=menu.Append(1000,_T("读取"));
                            menu.Bind(wxEVT_COMMAND_MENU_SELECTED,OnMenuReadChar,item->GetId(),item->GetId());
                        }


                        {
                            auto OnMenuDirectReadChar=[&]( wxCommandEvent& event )
                            {
                                ReadChar(true);
                            };
                            wxMenuItem *item=menu.Append(1001,_T("读取(从设备中读取)"));
                            menu.Bind(wxEVT_COMMAND_MENU_SELECTED,OnMenuDirectReadChar,item->GetId(),item->GetId());
                        }

                        menu.AppendSeparator();

                    }

                    if(Char->flag.write || Char->flag.writewithoutresponse)
                    {
                        //写入

                        auto WriteChar=[&](std::string data,bool writewithoutresponse)
                        {
                            wxLogMessage(wxString(_T("正在写入设备..."))+Char->getInstanceId());
                            try
                            {
                                if(BLEDeviceList.find(Char->getInstanceId())==BLEDeviceList.end())
                                {
                                    return;
                                }
                                auto PbleDevice=BLEDeviceList[Char->getInstanceId()];
                                BleDevice &bleDevice = (*PbleDevice);
                                //bleDevice.enumerateBleServices();
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

                                        //s->enumerateBleCharacteristics();

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
                                                    //写入数据
                                                    c->setValue((uint8_t *)data.c_str(),data.length(),writewithoutresponse);
                                                }
                                                break;
                                            }

                                        }

                                        break;
                                    }
                                }
                                wxLogMessage(_T("写入数据已完成..."));
                            }
                            catch (BleException const &e)
                            {
                                wxLogError(e.what());
                                wxLogError(_T("写入出错..."));
                            }

                        };


                        if(Char->flag.write)
                        {
                            auto OnMenuWriteChar=[&]( wxCommandEvent& event )
                            {
                                InputDialog dlg(this);
                                if(dlg.ShowModal()==wxID_OK)
                                {
                                    {
                                        wxString data;
                                        if(dlg.IsHEX())
                                        {
                                            std::string hexdata=dlg.GetData();
                                            for(size_t i=0; i<hexdata.length(); i++)
                                            {
                                                char buff[10]= {0};
                                                sprintf(buff,"%02X ",hexdata.c_str()[i]);
                                                data+=buff;
                                            }
                                        }
                                        else
                                        {
                                            data=dlg.GetData();
                                        }

                                        wxLogMessage(_T("待写入数据:\r\n%s"),data);
                                    }

                                    WriteChar(dlg.GetData(),false);
                                }
                                else
                                {
                                    wxLogMessage(_T("取消写入"));
                                }
                            };
                            wxMenuItem *item=menu.Append(1002,_T("写入"));
                            menu.Bind(wxEVT_COMMAND_MENU_SELECTED,OnMenuWriteChar,item->GetId(),item->GetId());
                        }

                        if(Char->flag.writewithoutresponse)
                        {
                            auto OnMenuWriteWithoutResponseChar=[&]( wxCommandEvent& event )
                            {
                                InputDialog dlg(this);
                                if(dlg.ShowModal()==wxID_OK)
                                {
                                    {
                                        wxString data;
                                        if(dlg.IsHEX())
                                        {
                                            std::string hexdata=dlg.GetData();
                                            for(size_t i=0; i<hexdata.length(); i++)
                                            {
                                                char buff[10]= {0};
                                                sprintf(buff,"%02X ",hexdata.c_str()[i]);
                                                data+=buff;
                                            }
                                        }
                                        else
                                        {
                                            data=dlg.GetData();
                                        }

                                        wxLogMessage(_T("待快速写入数据:\r\n%s"),data);
                                    }

                                    WriteChar(dlg.GetData(),true);
                                }
                                else
                                {
                                    wxLogMessage(_T("取消快速写入"));
                                }
                            };
                            wxMenuItem *item=menu.Append(1003,_T("快速写入(不等待响应)"));
                            menu.Bind(wxEVT_COMMAND_MENU_SELECTED,OnMenuWriteWithoutResponseChar,item->GetId(),item->GetId());
                        }

                        menu.AppendSeparator();
                    }

                    if(Char->flag.notify)
                    {
                        //通知数据

                        auto OnNotify=[=](std::string data)
                        {
                            auto cb=[=]()
                            {
                                wxString ServiceUUID;
                                if(Char->getServiceUUID().IsShortUuid)
                                {
                                    auto tohex=[](uint32_t num) -> std::string
                                    {
                                        char buff[32]={0};
                                        sprintf(buff,"%x",num);
                                        return buff;
                                    };
                                    ServiceUUID=tohex(Char->getServiceUUID().Value.ShortUuid);
                                    std::string desc=GetBLEUuidDescByShortUuid(Char->getServiceUUID().Value.ShortUuid);
                                    if(!desc.empty())
                                    {
                                        ServiceUUID+=std::string(" [")+desc+"] ";
                                    }
                                }
                                else
                                {
                                    ServiceUUID=Utility::guidToString(Char->getServiceUUID().Value.LongUuid);
                                }

                                wxString CharUUID;
                                if(Char->getCharUUID().IsShortUuid)
                                {
                                    auto tohex=[](uint32_t num) -> std::string
                                    {
                                        char buff[32]={0};
                                        sprintf(buff,"%x",num);
                                        return buff;
                                    };
                                    ServiceUUID=tohex(Char->getCharUUID().Value.ShortUuid);
                                    std::string desc=GetBLEUuidDescByShortUuid(Char->getCharUUID().Value.ShortUuid);
                                    if(!desc.empty())
                                    {
                                        CharUUID+=std::string(" [")+desc+"] ";
                                    }
                                }
                                else
                                {
                                    CharUUID=Utility::guidToString(Char->getCharUUID().Value.LongUuid);
                                }

                                auto to_hex=[](const unsigned char *data,unsigned long len)->wxString
                                {
                                    wxString ret;
                                    for(size_t i=0; i<len; i++)
                                    {
                                        char buff[10]= {0};
                                        sprintf(buff,"%02X ",data[i]);
                                        ret+=buff;
                                    }
                                    return ret;
                                };

                                wxLogMessage(_T("设备：%s,服务:%s,特征:%s:\r\nASCII:%s\r\n%s\r\nHEX:\r\n%s"),wxString(Char->getInstanceId()),ServiceUUID,CharUUID,wxString(data),to_hex((const unsigned char *)data.c_str(),data.length()));

                            };
                            AddUpdateUIFunciton(cb);
                        };

                        auto NotifyChar=[&](bool OnOff)
                        {
                            wxLogMessage(wxString(_T("正在查找设备..."))+Char->getInstanceId());
                            try
                            {
                                if(BLEDeviceList.find(Char->getInstanceId())==BLEDeviceList.end())
                                {
                                    return;
                                }
                                auto PbleDevice=BLEDeviceList[Char->getInstanceId()];
                                BleDevice &bleDevice = (*PbleDevice);
                                //bleDevice.enumerateBleServices();
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

                                        //s->enumerateBleCharacteristics();

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
                                                    if(OnOff)
                                                    {
                                                        auto cb=[=](BleGattNotificationData& data)
                                                        {
                                                            wxLogMessage(_T("收到通知数据..."));
                                                            if(data.getData()==NULL || data.getDataSize()==0)
                                                            {
                                                                return;
                                                            }

                                                            OnNotify(std::string((const char *)data.getData(),data.getDataSize()));

                                                        };

                                                        c->registerNotificationHandler(cb);
                                                        wxLogMessage(_T("正在打开通知..."));
                                                    }
                                                    else
                                                    {
                                                        c->unregisterNotificationHandler();
                                                        wxLogMessage(_T("正在关闭通知..."));
                                                    }

                                                }

                                                break;
                                            }

                                        }

                                        break;
                                    }
                                }
                                wxLogMessage(_T("设置通知已完成..."));
                            }
                            catch (BleException const &e)
                            {
                                wxLogError(e.what());
                                wxLogError(_T("设置通知出错..."));
                            }

                        };


                        {
                            auto OnEnableNotify=[&]( wxCommandEvent& event )
                            {
                                NotifyChar(true);
                            };
                            wxMenuItem *item=menu.Append(1004,_T("打开通知"));
                            menu.Bind(wxEVT_COMMAND_MENU_SELECTED,OnEnableNotify,item->GetId(),item->GetId());
                        }


                        {
                            auto OnDisableNotify=[&]( wxCommandEvent& event )
                            {
                                NotifyChar(false);
                            };
                            wxMenuItem *item=menu.Append(1005,_T("关闭通知"));
                            menu.Bind(wxEVT_COMMAND_MENU_SELECTED,OnDisableNotify,item->GetId(),item->GetId());
                        }

                        menu.AppendSeparator();
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

