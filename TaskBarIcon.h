#pragma once

#include "wx/taskbar.h"

class MyTaskBarIcon : public wxTaskBarIcon
{
public:
    MyTaskBarIcon(wxApp* pParent);

    virtual wxMenu* CreatePopupMenu() wxOVERRIDE;
    
    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuDevice(wxCommandEvent&);

    wxDECLARE_EVENT_TABLE();
private:
    wxApp* m_parent;
    wxArrayString m_deviceNames;
    wxString m_defaultDeviceName;

    void ShowDevicePropertiesDialog(int id);
    void ShowDevicePropertiesDialog();
    void ReadDefaultDevice();
    void WriteDefaultDevice();
};
