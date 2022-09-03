#include "InputDialog.h"
#include "wx/msgdlg.h"

InputDialog::InputDialog(wxWindow *parent):GuiDialogInputData(parent)
{
    //ctor
}

std::string InputDialog::GetData()
{
    return data;
}
bool InputDialog::IsHEX()
{
    return m_checkBox1->IsChecked();
}

void InputDialog::OnButtonOk( wxCommandEvent& event )
{
    wxString textdata=m_textCtrl2->GetValue();
    if(textdata.empty())
    {
        wxMessageBox(_T("请输入数据!"),_T("警告"));
        return;
    }
    if(m_checkBox1->IsChecked())
    {


        data="";
        //去除空格
        textdata.Replace(" ","");
        while(!textdata.empty())
        {
            wxString Hex;
            if(textdata.size()>2)
            {
                Hex=textdata.substr(0,2);
                textdata=textdata.substr(2);
            }
            else
            {
                Hex=textdata;
                textdata="";
            }

            {
                unsigned long val=0;
                if(Hex.ToULong(&val,16))
                {
                    uint8_t dat[1]= {0};
                    dat[0]=val;
                    data+=std::string((const char *)dat,sizeof(dat));
                }
                else
                {
                    data="";
                    wxMessageBox(_T("输入的数据不是HEX数据"),_T("警告"));
                    return;
                }


            }
        }
    }
    else
    {
        data=textdata.ToStdString();
    }
    EndModal(wxID_OK);
}

InputDialog::~InputDialog()
{
    //dtor
}
