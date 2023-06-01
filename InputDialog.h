#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "GUIDialog.h"
#include <string>
#include <stdint.h>

class InputDialog: public GuiDialogInputData
{
public:
    InputDialog(wxWindow *parent);
    virtual ~InputDialog();
    std::string GetData();
    bool IsHEX();

protected:
    virtual void OnButtonOk( wxCommandEvent& event );

private:
    std::string data;
};

#endif // INPUTDIALOG_H
