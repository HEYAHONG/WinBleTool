#ifndef BLEWXTREEITEMDATA_H
#define BLEWXTREEITEMDATA_H

#include <wx/treectrl.h>
#include <BleDevice.h>
#include <BleGattServices.h>


class BLEDevicewxTreeItemData:public wxTreeItemData
{
protected:
    wstring _instanceId;
public:
    BLEDevicewxTreeItemData()
    {

    }
    virtual ~BLEDevicewxTreeItemData()
    {

    }
    wstring getInstanceId()
    {
        return _instanceId;
    }
    void setInstanceId(wstring id)
    {
        _instanceId=id;
    }
};

class BLEServicewxTreeItemData:public wxTreeItemData
{
protected:
    wstring _instanceId;
    BTH_LE_UUID service_uuid;
public:
    BLEServicewxTreeItemData()
    {

    }
    virtual ~BLEServicewxTreeItemData()
    {

    }
    wstring getInstanceId()
    {
        return _instanceId;
    }
    void setInstanceId(wstring id)
    {
        _instanceId=id;
    }
    BTH_LE_UUID getServiceUUID()
    {
        return service_uuid;
    }
    void setServiceUUID(BTH_LE_UUID uuid)
    {
        service_uuid=uuid;
    }
};

class BLECharwxTreeItemData:public wxTreeItemData
{
protected:
    wstring _instanceId;
    BTH_LE_UUID service_uuid;
    BTH_LE_UUID char_uuid;
public:
    BLECharwxTreeItemData()
    {

    }
    virtual ~BLECharwxTreeItemData()
    {

    }
    wstring getInstanceId()
    {
        return _instanceId;
    }
    void setInstanceId(wstring id)
    {
        _instanceId=id;
    }
    BTH_LE_UUID getServiceUUID()
    {
        return service_uuid;
    }
    void setServiceUUID(BTH_LE_UUID uuid)
    {
        service_uuid=uuid;
    }
    BTH_LE_UUID getCharUUID()
    {
        return char_uuid;
    }
    void setCharUUID(BTH_LE_UUID uuid)
    {
        char_uuid=uuid;
    }
};

#endif // BLEWXTREEITEMDATA_H
