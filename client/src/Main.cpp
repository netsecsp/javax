/*****************************************************************************
Copyright (c) netsecsp 2012-2032, All rights reserved.

Developer: Shengqian Yang, from China, E-mail: netsecsp@hotmail.com, last updated 12/23/2024
http://pingx.sf.net

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above
copyright notice, this list of conditions and the
following disclaimer.

* Redistributions in binary form must reproduce the
above copyright notice, this list of conditions
and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/
#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>
#include <string>

#define EVENTID 10086

#pragma comment(lib, "advapi32.lib") //for service api

////////////////////////////////////////////////////////////////////////////////////////////////////
#include <frame/AsynCore.h>
#include <frame/asm/IScriptHost.h>

#define AAPIDLL_USING 1
#ifdef  AAPIDLL_USING
#include <frame/asm/ITypedef_i.c>
#include <frame/asm/IAsynFrame_i.c>
#endif

#ifdef AAPIDLL_USING
#pragma comment(lib, "asyncore_dll.lib")
#ifdef _DEBUG
#ifdef _DLL
#pragma comment(lib, "asynsdk_mini-MDd.lib")
#else
#pragma comment(lib, "asynsdk_mini-MTd.lib")
#endif
#else //
#ifdef _DLL
#pragma comment(lib, "asynsdk_mini-MD.lib")
#else
#pragma comment(lib, "asynsdk_mini-MT.lib")
#endif
#endif
#else
#ifdef _DLL
#pragma comment(lib, "asynframe-MD_lib.lib")
#else
#pragma comment(lib, "asynframe-MT_lib.lib")
#endif
#endif
STDAPI_(extern HRESULT) Initialize( /*[in ]*/IAsynMessageEvents *param1, /*[in ]*/IUnknown *param2 );
STDAPI_(extern HRESULT) Destory();
STDAPI_(extern InstancesManager*) GetInstancesManager();
////////////////////////////////////////////////////////////////////////////////////////////////////
static asynsdk::CSetting configure;
////////////////////////////////////////////////////////////////////////////////////////////////////
LOGGER_IMPLEMENT(logger, "app", "CService");
////////////////////////////////////////////////////////////////////////////////////////////////////
class CService
{
public:
    CService(DWORD dwServiceType = SERVICE_WIN32_OWN_PROCESS)
    {
        memset(&m_status, 0, sizeof(m_status));
        m_status.dwServiceType = dwServiceType;
    }

public:
    static void WINAPI Entry(DWORD argc, LPSTR* argv)
    {// Service Entry
        CService mySvc;

        mySvc.m_handle = RegisterServiceCtrlHandlerEx(argv[0], CService::CtrlHandler,&mySvc);
        if( mySvc.m_handle == NULL )
        {
            printf("RegisterServiceCtrlHandler failed (%d)\n", (int)GetLastError());
            return;
        }

        mySvc.ReportStatus(SERVICE_START_PENDING, NO_ERROR, 2000); // Report initial status to the SCM
        std::string file = argc > 1? argv[1] : configure.getString("app.file");
        asynsdk::CStringSetter jre(configure.getString("java.jre"));
        asynsdk::CKeyvalSetter env(1);
        env.Set(STRING_from_string(";vmhost"), 1, STRING_from_string(argc > 2? argv[2] : configure.getString("java.arg")));
        if( argc > 1 )
        {
            for(DWORD i = 2; i < argc; ++ i)
            {
                if((++i) < argc )
                    env.Set(STRING_from_string("classpath"), 1, STRING_from_string(argv[i]));
                else
                    break;
            }
        }
        else
        {
            env.Set(STRING_from_string("classpath"), 1, STRING_from_string(configure.getString("app.path")));
        }

        if( file.empty()!=0 )
        {
            LOGGER_ERROR(logger, "need config class/jar file");
        }
        else
        {
            ::SetCurrentDirectory(configure.getString("app.path").c_str());
            asynsdk::CreateObject(GetInstancesManager(), "com.command.jvm/jvmproxy", jre.m_val.empty()? 0 :&jre, configure.getNumber("java.ver"), IID_IOsCommand, (IUnknown**)&mySvc.m_jvm.p);
            if( mySvc.m_jvm )
            {
                mySvc.ReportStatus(SERVICE_RUNNING, NO_ERROR); // Report running status when initialization is complete.
                mySvc.m_jvm->Execute(0, STRING_from_string(file), 0, 0, &env, 0);
            }
        }

        mySvc.Shutdown(); //must call Shutdown before ReportStatus stop

        mySvc.ReportStatus(SERVICE_STOPPED, NO_ERROR);
    }

    static DWORD WINAPI CtrlHandler(DWORD inControl, DWORD inEventType, LPVOID inEventData, LPVOID Context)
    {// Service Handle the requested control code. 
        switch(inControl)
        {
            case SERVICE_CONTROL_STOP:
            case SERVICE_CONTROL_SHUTDOWN:
                 LOGGER_INFO(logger, "do Shutdown, Control=" << inControl);
                 ((CService*)Context)->ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 2000);
                 ((CService*)Context)->Notify(EVENT_ID_Shutdown, 0); //Signal the service to stop.
                 break;

            case SERVICE_CONTROL_POWEREVENT:
                 LOGGER_INFO(logger, "do PowerEvt, Control=" << inControl << ", EventType=" << inEventType);
                 if( inEventType == PBT_APMSUSPEND       )
                 {
                     ((CService*)Context)->Notify(EVENT_ID_SysPower, 0);
                 }
                 if( inEventType == PBT_APMRESUMESUSPEND )
                 {
                     ((CService*)Context)->Notify(EVENT_ID_SysPower, 1);
                 }
                 break;

            default:
                 break;
        }
        return NO_ERROR;
    }

    void ReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint = 0)
    {
        static DWORD dwCheckPoint = 1;

        // Fill in the SERVICE_STATUS structure.
        m_status.dwCurrentState = dwCurrentState;
        m_status.dwWin32ExitCode = dwWin32ExitCode;
        m_status.dwWaitHint = dwWaitHint;

        if( dwCurrentState == SERVICE_START_PENDING)
        {
            m_status.dwControlsAccepted = 0;
        }
        else 
        {
            m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
        }

        if( dwCurrentState == SERVICE_RUNNING || dwCurrentState == SERVICE_STOPPED )
        {
            m_status.dwCheckPoint = 0;
        }
        else
        {
            m_status.dwCheckPoint = dwCheckPoint ++;
        }

        SetServiceStatus(m_handle, &m_status); // Report the status of the service to the SCM.
    }

    void Notify(uint64_t evtid, uint32_t value)
    {
        CObjPtr<IScriptHost> scripthost;
        if( scripthost.From(m_jvm) )
        {
            asynsdk::CStringSetter JavaEventName(1, configure.getString("java.api", "ActionEvent").c_str()); //call java.api: ActionEvent.onMessage
            scripthost->Invoke(0, &JavaEventName, AF_EVENT_NOTIFY, EN_SystemEvent, (evtid << 32)+ value, 0);
        }
    }

    void Shutdown()
    {
        m_jvm = NULL;
    }

protected:
    SERVICE_STATUS_HANDLE m_handle;
    SERVICE_STATUS        m_status;
    CObjPtr< IOsCommand > m_jvm;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
static void ShowCopyright()
{
    printf("Copyright (c) netsecsp 2012-2032, All rights reserved.\n");
    printf("Developer: Shengqian Yang, from China, E-mail: netsecsp@hotmail.com, last updated " STRING_UPDATETIME "\n");
    printf("http://javax.sf.net\n\n");
}

static void ShowUsage(const std::string &name)
{
    printf("  Usage: %s [-i displayname] [-u] [jar/classname -classpath [path]]\n", name.c_str());
    printf("Options:\n");
    printf("      -i doinstall service\n");
    printf("      -u uninstall service\n");
    printf("      -classpath Search path for class directory or zip/jar files\n");
    printf("example: %s -i \"Java Test Service\"\n", name.c_str());
    printf("         %s -u\n", name.c_str());
    printf("         %s \"com.demo.Testsvc\" -classpath .\n", name.c_str());
    printf("         %s \"com.demo.Testsvc main\" -classpath .\n", name.c_str());
    printf("         %s \"Testsvc.jar\" -classpath .\n", name.c_str());
    printf("         %s \"Testsvc.jar com.demo.Testsvc\" -classpath .\n", name.c_str());
}

static void SvcDoInstall(const char *svcname, const char *dspname)
{
    char temp[MAX_PATH];
    GetModuleFileName(NULL, temp, MAX_PATH);
    // In case the path contains a space, it must be quoted so that
    // it is correctly interpreted. For example,
    // "d:\my share\javax.exe" should be specified as ""d:\my share\javax.exe""
    char path[MAX_PATH];
    StringCbPrintf(path, MAX_PATH, "\"%s\"", temp);
 
    // Get a handle to the SCM database. 
    SC_HANDLE schManager = OpenSCManager(
                                NULL,                    // local computer
                                NULL,                    // ServicesActive database 
                                SC_MANAGER_ALL_ACCESS);  // full access rights 
    if( schManager == NULL )
    {
        printf("call OpenSCManager failed (%d)\n", (int)GetLastError());
        return;
    }
 
    do{
    // Create the service
    SC_HANDLE schService = CreateService(
                                schManager,                // SCM database 
                                svcname,                   // name of service 
                                dspname,                   // service name to display 
                                SERVICE_ALL_ACCESS,        // desired access 
                                SERVICE_WIN32_OWN_PROCESS, // service type 
                                SERVICE_DEMAND_START,      // start type 
                                SERVICE_ERROR_NORMAL,      // error control type 
                                path,                      // path to service's binary 
                                NULL,                      // no load ordering group 
                                NULL,                      // no tag identifier 
                                NULL,                      // no dependencies 
                                NULL,                      // LocalSystem account 
                                NULL);                     // no password 
    if( schService == NULL )
    {
        printf("call CreateService failed (%d)\n", (int)GetLastError());
        break;
    }
    else
    {
        printf("Service installed successfully\n");
    }

    CloseServiceHandle(schService);
    }while(0);

    CloseServiceHandle(schManager);
}
 
static void SvcUnInstall(const char *svcname)
{ 
    // Get a handle to the SCM database. 
    SC_HANDLE schManager = OpenSCManager(
                                NULL,                    // local computer
                                NULL,                    // ServicesActive database 
                                SC_MANAGER_ALL_ACCESS);  // full access rights 
    if( schManager == NULL )
    {
        printf("call OpenSCManager failed (%d)\n", (int)GetLastError());
        return;
    }
 
    do{
    // Get a handle to the service.
    SC_HANDLE schService = OpenService(
                                schManager,       // SCM database 
                                svcname,          // name of service 
                                DELETE);          // need delete access 
    if( schService == NULL )
    {
        printf("call OpenService failed (%d)\n", (int)GetLastError());
        break;
    }
 
    // Delete the service.
    if(!DeleteService(schService))
    {
        printf("call DeleteService failed (%d)\n", (int)GetLastError());
    }
    else
    {
        printf("Service deleted successfully\n");
    }

    CloseServiceHandle(schService);
    }while(0);

    CloseServiceHandle(schManager);
}

int main(int argc, char* argv[])
{// https://learn.microsoft.com/en-us/windows/win32/services/svc-cpp
    std::string path;
    std::string name; {//part path/name
    std::string file = argv[0];
    std::string::size_type p = file.find_last_of("/\\");
    if( p != std::string::npos )
    {
        path = file.substr(0, p ++);
        name = file.substr(p);
    }
    else
    {
        path = ".";
        name = file;
    }
    }

    asynsdk::CStringSetter fileconf(1, (path + "\\config.txt").c_str());
    Initialize(NULL, &fileconf);

    InstancesManager *lpInstancesManager = GetInstancesManager();
    #ifdef _LOG
    asynsdk::AsynLogger_Initialize(lpInstancesManager);
    #endif

    configure.from(lpInstancesManager); //get config api

    std::string svcname = configure.getString("app.name", configure.getString(";product.sequence"));
    configure.setString("app.path", path);

    do{
    if( argc < 2 )
    {
        // Add additional services for the process to this table.
        SERVICE_TABLE_ENTRY DispatchTable[] =
        {
            { (LPSTR)svcname.c_str(), (LPSERVICE_MAIN_FUNCTION)CService::Entry },
            { NULL, NULL }
        };
        
        // This call returns when the service has stopped. 
        // The process should simply terminate when the call returns.
        if(!StartServiceCtrlDispatcher(DispatchTable))
        {// 首先要注册服务，然后必须用sc命令或者windows服务器管理器去启动程序。手动启动或者vs调试都将会失败, 返回1063
            printf("call StartServiceCtrlDispatcher failed (%d)\n", (int)GetLastError());
        }

        break;
    }

    if( lstrcmpi(argv[1], "--help") == 0 ||
        lstrcmpi(argv[1], "-h") == 0 ||
        lstrcmpi(argv[1], "/?") == 0 )
    {
        ShowCopyright();
        ShowUsage(name);
        break;
    }

    if( lstrcmpi(argv[1], "-i") == 0 )
    {
        ShowCopyright();
        SvcDoInstall(svcname.c_str(), argc > 2? argv[2] : configure.getString("app.displayname", "javax").c_str());
        break;
    }
    if( lstrcmpi(argv[1], "-u") == 0 )
    {
        ShowCopyright();
        SvcUnInstall(svcname.c_str());
        break;
    }

    asynsdk::CStringSetter jre(configure.getString("java.jre"));
    asynsdk::CKeyvalSetter env(1);
    env.Set(STRING_from_string(";vmhost"), 1, STRING_from_string(configure.getString("java.arg")));
    std::string file = configure.getString("app.file");
    for(int i = 1; i < argc; ++i)
    {
        if( argv[i][0] == '-' )
        {
            if( lstrcmpi(argv[i] + 1, "classpath") == 0 )
            {
                if((++i) < argc )
                    env.Set(STRING_from_string("classpath"), 1, STRING_from_string(argv[i]));
                else
                    break;
            }
        }
        else
        {
            file = argv[i];
        }
    }
    if( file.empty()!=0 )
    {
        ShowCopyright();
        ShowUsage(name);
    }
    else
    {
        CObjPtr<IOsCommand> jvm;
        asynsdk::CreateObject(lpInstancesManager, "com.command.jvm/jvmproxy", jre.m_val.empty()? 0 :&jre, configure.getNumber("java.ver"), IID_IOsCommand, (IUnknown**)&jvm.p);
        if( jvm )
        {
            jvm->Execute(0, STRING_from_string(file), 0, 0, &env, 0);
        }
    }
    }while(0);

    Destory();
    return 0;
}
