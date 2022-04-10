#pragma once

class MyTaskBarIcon;

// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
    virtual int OnExit() wxOVERRIDE;
private:
    MyTaskBarIcon* m_taskBarIcon;
private:
};
