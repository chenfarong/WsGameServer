/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef _MSC_VER

//#undef UNICODE

#include "ServiceWin32.h"

//#include "Common.h"
//#include "Log.h"
#include <cstring>
#include <windows.h>
#include <winsvc.h>
#include <string>

#include <assert.h>
#include <strsafe.h>
#include <memory>


#if !defined(WINADVAPI)
#if !defined(_ADVAPI32_)
#define WINADVAPI DECLSPEC_IMPORT
#else
#define WINADVAPI
#endif
#endif

#include <stdio.h>

//#define XLOG_ERROR printf

extern int App_ServiceStatus;
int main_server();
//////////////////////////////////////////////////////////////////////////


class CThreadPool
{
public:

	template <typename T>
	static void QueueUserWorkItem(void (T::*function)(void),
		T *object, ULONG flags = WT_EXECUTELONGFUNCTION)
	{
		typedef std::pair<void (T::*)(), T *> CallbackType;
		std::auto_ptr<CallbackType> p(new CallbackType(function, object));

		if (::QueueUserWorkItem(ThreadProc<T>, p.get(), flags))
		{
			// The ThreadProc now has the responsibility of deleting the pair.
			p.release();
		}
		else
		{
			throw GetLastError();
		}
	}

private:

	template <typename T>
	static DWORD WINAPI ThreadProc(PVOID context)
	{
		typedef std::pair<void (T::*)(), T *> CallbackType;

		std::auto_ptr<CallbackType> p(static_cast<CallbackType *>(context));

		(p->second->*p->first)();
		return 0;
	}
};








//////////////////////////////////////////////////////////////////////////
//
//
//extern int main(int argc, char** argv);
//extern char serviceLongName[];
//extern char serviceName[];
//extern char serviceDescription[];
//
//extern int m_ServiceStatus;
//
//SERVICE_STATUS serviceStatus;
//
//SERVICE_STATUS_HANDLE serviceStatusHandle = nullptr;
//
//typedef WINADVAPI BOOL (WINAPI* CSD_T)(SC_HANDLE, DWORD, LPCVOID);
//
//void WINAPI ServiceMain(DWORD argc, char** argv);
//bool WinServiceRun();
//bool WinServiceInstall();
//bool WinServiceUninstall();
//
///** Window Service **/
//const int nBufferSize = 500;
//CRITICAL_SECTION		myCS;
//CHAR pLogFile[nBufferSize + 1];
//CHAR pExeFile[nBufferSize + 1];
//
//SERVICE_TABLE_ENTRY		lpServiceStartTable[] =
//{
//	{ serviceName, ServiceMain },
//	{ NULL, NULL }
//};
//
//
//VOID WriteLog(char* pFile, char* pMsg)
//{
//	// write error or other information into log file
//	::EnterCriticalSection(&myCS);
//	try
//	{
//		SYSTEMTIME oT;
//		::GetLocalTime(&oT);
//		//FILE* pLog = fopen(pFile, "a");
//		FILE* pLog = NULL;
//		fopen_s(&pLog, pFile, "a");
//		fprintf(pLog, "%02d/%02d/%04d, %02d:%02d:%02d\n    %s", oT.wMonth, oT.wDay, oT.wYear, oT.wHour, oT.wMinute, oT.wSecond, pMsg);
//		fclose(pLog);
//	}
//	catch (...) {}
//	::LeaveCriticalSection(&myCS);
//}
//
//VOID XLOG_ERROR(const char* fmt, ...)
//{
//	::EnterCriticalSection(&myCS);
//	try
//	{
//		va_list ap;
//		va_start(ap, fmt);
//
//		SYSTEMTIME oT;
//		::GetLocalTime(&oT);
//		//FILE* pLog = fopen(pFile, "a");
//		FILE* pLog = NULL;
//		fopen_s(&pLog, pLogFile, "a");
//		fprintf(pLog, "%02d/%02d/%04d, %02d:%02d:%02d ", oT.wMonth, oT.wDay, oT.wYear, oT.wHour, oT.wMinute, oT.wSecond);
//		vfprintf_s(pLog, fmt, ap);
//		fclose(pLog);
//
//		va_end(ap);
//	}
//	catch (...) {}
//	::LeaveCriticalSection(&myCS);
//}
//
//
//VOID ExecuteSubProcess()
//{
//	/*
//	if (_beginthread(ProcMonitorThread, 0, NULL) == -1)
//	{
//		long nError = GetLastError();
//		char pTemp[121];
//		sprintf(pTemp, "StartService failed, error code = %d\n", nError);
//		WriteLog(pLogFile, pTemp);
//	}
//	*/
//
//	if (!StartServiceCtrlDispatcher(lpServiceStartTable))
//	{
//		long nError = GetLastError();
//		char pTemp[121];
//		sprintf_s(pTemp,120, "StartServiceCtrlDispatcher failed, error code = %d\n", nError);
//		WriteLog(pLogFile, pTemp);
//	}
//	::DeleteCriticalSection(&myCS);
//}
//
//int ServiceMainProc(int argc,const char** argv)
//{
//	::InitializeCriticalSection(&myCS);
//	// initialize variables for .exe and .log file names
//	char pModuleFile[nBufferSize + 1];
//	DWORD dwSize = GetModuleFileName(NULL, pModuleFile, nBufferSize);
//	pModuleFile[dwSize] = 0;
//	if (dwSize > 4 && pModuleFile[dwSize - 4] == '.')
//	{
//		sprintf_s(pExeFile, nBufferSize, "%s", pModuleFile);
//		pModuleFile[dwSize - 4] = 0;
//		sprintf_s(pLogFile, nBufferSize, "%s.log", pModuleFile);
//	}
////	strcpy(pServiceName, "Sundar_Service");
//
//
//	for (int i = 0; i < argc; i++)
//	{
//
//		if (0 == strcmp(argv[i], "-s") && argc > i) {
//			const char* arg = argv[i + 1];
//			switch (::tolower(arg[0]))
//			{
//			case 'i':
//				if (WinServiceInstall())
//					printf("Installing service\n");
//				return 1;
//			case 'u':
//				if (WinServiceUninstall())
//					printf("Uninstalling service\n");
//				return 1;
//			//case 'r':
//			//	WinServiceRun();
//			//	break;
//			}
//		}
//	}
//	return 0;
//
//}
//
//
//bool WinServiceInstall()
//{
//    CSD_T ChangeService_Config2;
//    HMODULE advapi32;
//    SC_HANDLE serviceControlManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
//
//    if (!serviceControlManager)
//    {
//		XLOG_ERROR("SERVICE: No access to service control manager.");
//        return false;
//    }
//
//    char path[_MAX_PATH + 10];
//    if (!GetModuleFileName(nullptr, path, sizeof(path) / sizeof(path[0])))
//    {
//        CloseServiceHandle(serviceControlManager);
//		XLOG_ERROR("SERVICE: Can't get service binary filename.");
//        return false;
//    }
//
//   // std::strcat(path, " -s run");
//   // std::string strPath = path + std::string(" -s run");
//
//    SC_HANDLE service = CreateService(serviceControlManager,
//                                      serviceName,          // name of service
//                                      serviceLongName,      // service name to display
//                                      SERVICE_ALL_ACCESS,   // desired access
//                                      // service type
//                                      SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
//                                      SERVICE_AUTO_START,   // start type
//                                      SERVICE_ERROR_IGNORE, // error control type
//		path,                 // service's binary
//                                      nullptr,              // no load ordering group
//                                      nullptr,              // no tag identifier
//                                      nullptr,              // no dependencies
//                                      nullptr,              // LocalSystem account
//                                      nullptr);             // no password
//
//    if (!service)
//    {
//        CloseServiceHandle(serviceControlManager);
//		XLOG_ERROR("SERVICE: Can't register service for: %s", path);
//        return false;
//    }
//
//    advapi32 = GetModuleHandle("ADVAPI32.DLL");
//    if (!advapi32)
//    {
//		XLOG_ERROR("SERVICE: Can't access ADVAPI32.DLL");
//        CloseServiceHandle(service);
//        CloseServiceHandle(serviceControlManager);
//        return false;
//    }
//
//    ChangeService_Config2 = (CSD_T) GetProcAddress(advapi32, "ChangeServiceConfig2A");
//    if (!ChangeService_Config2)
//    {
//		XLOG_ERROR("SERVICE: Can't access ChangeServiceConfig2A from ADVAPI32.DLL");
//        CloseServiceHandle(service);
//        CloseServiceHandle(serviceControlManager);
//        return false;
//    }
//
//    SERVICE_DESCRIPTION sdBuf;
//    sdBuf.lpDescription = serviceDescription;
//    ChangeService_Config2(
//        service,                                // handle to service
//        SERVICE_CONFIG_DESCRIPTION,             // change: description
//        &sdBuf);                                // new data
//
//    SC_ACTION _action[1];
//    _action[0].Type = SC_ACTION_RESTART;
//    _action[0].Delay = 10000;
//    SERVICE_FAILURE_ACTIONS sfa;
//    ZeroMemory(&sfa, sizeof(SERVICE_FAILURE_ACTIONS));
//    sfa.lpsaActions = _action;
//    sfa.cActions = 1;
//    sfa.dwResetPeriod = INFINITE;
//    ChangeService_Config2(
//        service,                                // handle to service
//        SERVICE_CONFIG_FAILURE_ACTIONS,         // information level
//        &sfa);                                  // new data
//
//    CloseServiceHandle(service);
//    CloseServiceHandle(serviceControlManager);
//    return true;
//}
//
//bool WinServiceUninstall()
//{
//    SC_HANDLE serviceControlManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
//
//    if (!serviceControlManager)
//    {
//		XLOG_ERROR("SERVICE: No access to service control manager.");
//        return false;
//    }
//
//    SC_HANDLE service = OpenService(serviceControlManager,
//                                    serviceName, SERVICE_QUERY_STATUS | DELETE);
//
//    if (!service)
//    {
//        CloseServiceHandle(serviceControlManager);
//		XLOG_ERROR("SERVICE: Service not found: %s", serviceName);
//        return false;
//    }
//
//    SERVICE_STATUS serviceStatus2;
//    if (QueryServiceStatus(service, &serviceStatus2))
//    {
//        if (serviceStatus2.dwCurrentState == SERVICE_STOPPED)
//            DeleteService(service);
//    }
//
//    CloseServiceHandle(service);
//    CloseServiceHandle(serviceControlManager);
//    return true;
//}
//
//void WINAPI ServiceControlHandler(DWORD controlCode)
//{
//    switch (controlCode)
//    {
//        case SERVICE_CONTROL_INTERROGATE:
//            break;
//
//        case SERVICE_CONTROL_SHUTDOWN:
//        case SERVICE_CONTROL_STOP:
//			m_ServiceStatus = 0;
//
//			serviceStatus.dwWin32ExitCode = 0;
//			serviceStatus.dwCheckPoint = 0;
//			serviceStatus.dwWaitHint = 0;
//			serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
//            SetServiceStatus(serviceStatusHandle, &serviceStatus);
//
//           
//            return;
//
//        case SERVICE_CONTROL_PAUSE:
//            m_ServiceStatus = 2;
//            serviceStatus.dwCurrentState = SERVICE_PAUSED;
//            SetServiceStatus(serviceStatusHandle, &serviceStatus);
//            break;
//
//        case SERVICE_CONTROL_CONTINUE:
//            serviceStatus.dwCurrentState = SERVICE_RUNNING;
//            SetServiceStatus(serviceStatusHandle, &serviceStatus);
//            m_ServiceStatus = 1;
//            break;
//
//        default:
//            if (controlCode >= 128 && controlCode <= 255)
//                // user defined control code
//                break;
//            else
//                // unrecognized control code
//                break;
//    }
//
//	if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
//	{
//		XLOG_ERROR("SetServiceStatus failed, error code = %d\n", GetLastError());
//	}
//}
//
//void WINAPI ServiceMain(DWORD argc, char** argv)
//{
//
//	char pModuleFile[nBufferSize + 1];
//	DWORD dwSize = GetModuleFileName(NULL, pModuleFile, nBufferSize);
//	pModuleFile[dwSize] = 0;
//	if (dwSize > 4 && pModuleFile[dwSize - 4] == '.')
//	{
//		sprintf_s(pExeFile, nBufferSize, "%s", pModuleFile);
//		pModuleFile[dwSize - 4] = 0;
//		sprintf_s(pLogFile, nBufferSize, "%s.log", pModuleFile);
//	}
//
//
//    // initialise service status
//    serviceStatus.dwServiceType = SERVICE_WIN32;
//    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
//    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
//    serviceStatus.dwWin32ExitCode = NO_ERROR;
//    serviceStatus.dwServiceSpecificExitCode = NO_ERROR;
//    serviceStatus.dwCheckPoint = 0;
//    serviceStatus.dwWaitHint = 0;
//
//    serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, ServiceControlHandler);
//
//    if (serviceStatusHandle)
//    {
//        char path[_MAX_PATH + 1];
//        unsigned int i, last_slash = 0;
//
//        GetModuleFileName(nullptr, path, sizeof(path) / sizeof(path[0]));
//
//        for (i = 0; i < std::strlen(path); ++i)
//        {
//            if (path[i] == '\\') last_slash = i;
//        }
//
//        path[last_slash] = 0;
//
//        // service is starting
//        serviceStatus.dwCurrentState = SERVICE_START_PENDING;
//        SetServiceStatus(serviceStatusHandle, &serviceStatus);
//
//        // do initialisation here
//        SetCurrentDirectory(path);
//
//        // running
//        serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
//        serviceStatus.dwCurrentState = SERVICE_RUNNING;
//		serviceStatus.dwCheckPoint = 0;
//		serviceStatus.dwWaitHint = 0;
//
//        SetServiceStatus(serviceStatusHandle, &serviceStatus);
//
//        ////////////////////////
//        // service main cycle //
//        ////////////////////////
//        m_ServiceStatus = 1;
//        argc = 1;
//        main(argc , argv);
//
//        // service was stopped
//        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
//        SetServiceStatus(serviceStatusHandle, &serviceStatus);
//
//        // do cleanup here
//
//        // service is now stopped
//        serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
//        serviceStatus.dwCurrentState = SERVICE_STOPPED;
//        SetServiceStatus(serviceStatusHandle, &serviceStatus);
//    }
//
////	::DeleteCriticalSection(&myCS);
//}
//
//
//bool WinServiceRun()
//{
//    SERVICE_TABLE_ENTRY serviceTable[] =
//    {
//        { serviceName, ServiceMain },
//        { nullptr, nullptr }
//    };
//
//    if (!StartServiceCtrlDispatcher(serviceTable))
//    {
//		XLOG_ERROR("StartService Failed. Error [%u]", ::GetLastError());
//        return false;
//    }
//    return true;
//}
//
//
//BOOL RunService(char* pName);
//
///*
//bool WinServiceRun()
//{
//	if (!StartServiceCtrlDispatcher(lpServiceStartTable))
//	{
//		long nError = GetLastError();
//		char pTemp[121];
//		sprintf(pTemp, "StartServiceCtrlDispatcher failed, error code = %d\n", nError);
//		WriteLog(pLogFile, pTemp);
//	}
////	::DeleteCriticalSection(&myCS);
//
//	RunService(serviceName);
//	return true;
//}
//*/
//
//BOOL RunService(char* pName)
//{
//	// run service with given name
//	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
//	if (schSCManager == 0)
//	{
//		long nError = GetLastError();
//		char pTemp[121];
//		sprintf_s(pTemp,120, "OpenSCManager failed, error code = %d\n", nError);
//		WriteLog(pLogFile, pTemp);
//	}
//	else
//	{
//		// open the service
//		SC_HANDLE schService = OpenService(schSCManager, pName, SERVICE_ALL_ACCESS);
//		if (schService == 0)
//		{
//			long nError = GetLastError();
//			char pTemp[121];
//			sprintf_s(pTemp,120, "OpenService failed, error code = %d\n", nError);
//			WriteLog(pLogFile, pTemp);
//		}
//		else
//		{
//			// call StartService to run the service
//			if (StartService(schService, 0, (const char**)NULL))
//			{
//				CloseServiceHandle(schService);
//				CloseServiceHandle(schSCManager);
//				return TRUE;
//			}
//			else
//			{
//				long nError = GetLastError();
//				char pTemp[121];
//				sprintf_s(pTemp,120, "StartService failed, error code = %d\n", nError);
//				WriteLog(pLogFile, pTemp);
//			}
//			CloseServiceHandle(schService);
//		}
//		CloseServiceHandle(schSCManager);
//	}
//	return FALSE;
//}
//
//BOOL KillService(char* pName)
//{
//	// kill service with given name
//	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
//	if (schSCManager == 0)
//	{
//		long nError = GetLastError();
//		char pTemp[121];
//		sprintf_s(pTemp, 120, "OpenSCManager failed, error code = %d\n", nError);
//		WriteLog(pLogFile, pTemp);
//	}
//	else
//	{
//		// open the service
//		SC_HANDLE schService = OpenService(schSCManager, pName, SERVICE_ALL_ACCESS);
//		if (schService == 0)
//		{
//			long nError = GetLastError();
//			char pTemp[121];
//			sprintf_s(pTemp,120, "OpenService failed, error code = %d\n", nError);
//			WriteLog(pLogFile, pTemp);
//		}
//		else
//		{
//			// call ControlService to kill the given service
//			SERVICE_STATUS status;
//			if (ControlService(schService, SERVICE_CONTROL_STOP, &status))
//			{
//				CloseServiceHandle(schService);
//				CloseServiceHandle(schSCManager);
//				return TRUE;
//			}
//			else
//			{
//				long nError = GetLastError();
//				char pTemp[121];
//				sprintf_s(pTemp, 120, "ControlService failed, error code = %d\n", nError);
//				WriteLog(pLogFile, pTemp);
//			}
//			CloseServiceHandle(schService);
//		}
//		CloseServiceHandle(schSCManager);
//	}
//	return FALSE;
//}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class CServiceBase
{
public:

	// Register the executable for a service with the Service Control Manager 
	// (SCM). After you call Run(ServiceBase), the SCM issues a Start command, 
	// which results in a call to the OnStart method in the service. This 
	// method blocks until the service has stopped.
	static BOOL Run(CServiceBase &service);

	// Service object constructor. The optional parameters (fCanStop, 
	// fCanShutdown and fCanPauseContinue) allow you to specify whether the 
	// service can be stopped, paused and continued, or be notified when 
	// system shutdown occurs.
	CServiceBase(PWSTR pszServiceName,
		BOOL fCanStop = TRUE,
		BOOL fCanShutdown = TRUE,
		BOOL fCanPauseContinue = FALSE);

	// Service object destructor. 
	virtual ~CServiceBase(void);

	// Stop the service.
	void Stop();

protected:

	// When implemented in a derived class, executes when a Start command is 
	// sent to the service by the SCM or when the operating system starts 
	// (for a service that starts automatically). Specifies actions to take 
	// when the service starts.
	virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv);

	// When implemented in a derived class, executes when a Stop command is 
	// sent to the service by the SCM. Specifies actions to take when a 
	// service stops running.
	virtual void OnStop();

	// When implemented in a derived class, executes when a Pause command is 
	// sent to the service by the SCM. Specifies actions to take when a 
	// service pauses.
	virtual void OnPause();

	// When implemented in a derived class, OnContinue runs when a Continue 
	// command is sent to the service by the SCM. Specifies actions to take 
	// when a service resumes normal functioning after being paused.
	virtual void OnContinue();

	// When implemented in a derived class, executes when the system is 
	// shutting down. Specifies what should occur immediately prior to the 
	// system shutting down.
	virtual void OnShutdown();

	// Set the service status and report the status to the SCM.
	void SetServiceStatus(DWORD dwCurrentState,
		DWORD dwWin32ExitCode = NO_ERROR,
		DWORD dwWaitHint = 0);

	// Log a message to the Application event log.
	void WriteEventLogEntry(PWSTR pszMessage, WORD wType);

	// Log an error message to the Application event log.
	void WriteErrorLogEntry(PWSTR pszFunction,
		DWORD dwError = GetLastError());

private:

	// Entry point for the service. It registers the handler function for the 
	// service and starts the service.
	static void WINAPI ServiceMain(DWORD dwArgc, LPWSTR *lpszArgv);

	// The function is called by the SCM whenever a control code is sent to 
	// the service.
	static void WINAPI ServiceCtrlHandler(DWORD dwCtrl);

	// Start the service.
	void Start(DWORD dwArgc, PWSTR *pszArgv);

	// Pause the service.
	void Pause();

	// Resume the service after being paused.
	void Continue();

	// Execute when the system is shutting down.
	void Shutdown();

	// The singleton service instance.
	static CServiceBase *s_service;

	// The name of the service
	PWSTR m_name;

	// The status of the service
	SERVICE_STATUS m_status;

	// The service status handle
	SERVICE_STATUS_HANDLE m_statusHandle;
};



// Initialize the singleton service instance.
CServiceBase *CServiceBase::s_service = NULL;


//
//   FUNCTION: CServiceBase::Run(CServiceBase &)
//
//   PURPOSE: Register the executable for a service with the Service Control 
//   Manager (SCM). After you call Run(ServiceBase), the SCM issues a Start 
//   command, which results in a call to the OnStart method in the service. 
//   This method blocks until the service has stopped.
//
//   PARAMETERS:
//   * service - the reference to a CServiceBase object. It will become the 
//     singleton service instance of this service application.
//
//   RETURN VALUE: If the function succeeds, the return value is TRUE. If the 
//   function fails, the return value is FALSE. To get extended error 
//   information, call GetLastError.
//
BOOL CServiceBase::Run(CServiceBase &service)
{
	s_service = &service;

	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ service.m_name, ServiceMain },
		{ NULL, NULL }
	};

	// Connects the main thread of a service process to the service control 
	// manager, which causes the thread to be the service control dispatcher 
	// thread for the calling process. This call returns when the service has 
	// stopped. The process should simply terminate when the call returns.
	return StartServiceCtrlDispatcher(serviceTable);
}


//
//   FUNCTION: CServiceBase::ServiceMain(DWORD, PWSTR *)
//
//   PURPOSE: Entry point for the service. It registers the handler function 
//   for the service and starts the service.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
void WINAPI CServiceBase::ServiceMain(DWORD dwArgc, PWSTR *pszArgv)
{
	assert(s_service != NULL);

	// Register the handler function for the service
	s_service->m_statusHandle = RegisterServiceCtrlHandler(
		s_service->m_name, ServiceCtrlHandler);
	if (s_service->m_statusHandle == NULL)
	{
		throw GetLastError();
	}

	// Start the service.
	s_service->Start(dwArgc, pszArgv);
}


//
//   FUNCTION: CServiceBase::ServiceCtrlHandler(DWORD)
//
//   PURPOSE: The function is called by the SCM whenever a control code is 
//   sent to the service. 
//
//   PARAMETERS:
//   * dwCtrlCode - the control code. This parameter can be one of the 
//   following values: 
//
//     SERVICE_CONTROL_CONTINUE
//     SERVICE_CONTROL_INTERROGATE
//     SERVICE_CONTROL_NETBINDADD
//     SERVICE_CONTROL_NETBINDDISABLE
//     SERVICE_CONTROL_NETBINDREMOVE
//     SERVICE_CONTROL_PARAMCHANGE
//     SERVICE_CONTROL_PAUSE
//     SERVICE_CONTROL_SHUTDOWN
//     SERVICE_CONTROL_STOP
//
//   This parameter can also be a user-defined control code ranges from 128 
//   to 255.
//
void WINAPI CServiceBase::ServiceCtrlHandler(DWORD dwCtrl)
{
	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP: s_service->Stop(); break;
	case SERVICE_CONTROL_PAUSE: s_service->Pause(); break;
	case SERVICE_CONTROL_CONTINUE: s_service->Continue(); break;
	case SERVICE_CONTROL_SHUTDOWN: s_service->Shutdown(); break;
	case SERVICE_CONTROL_INTERROGATE: break;
	default: break;
	}
}

#pragma endregion


#pragma region Service Constructor and Destructor

//
//   FUNCTION: CServiceBase::CServiceBase(PWSTR, BOOL, BOOL, BOOL)
//
//   PURPOSE: The constructor of CServiceBase. It initializes a new instance 
//   of the CServiceBase class. The optional parameters (fCanStop, 
///  fCanShutdown and fCanPauseContinue) allow you to specify whether the 
//   service can be stopped, paused and continued, or be notified when system 
//   shutdown occurs.
//
//   PARAMETERS:
//   * pszServiceName - the name of the service
//   * fCanStop - the service can be stopped
//   * fCanShutdown - the service is notified when system shutdown occurs
//   * fCanPauseContinue - the service can be paused and continued
//
CServiceBase::CServiceBase(PWSTR pszServiceName,
	BOOL fCanStop,
	BOOL fCanShutdown,
	BOOL fCanPauseContinue)
{
	// Service name must be a valid string and cannot be NULL.
	m_name = (pszServiceName == NULL) ? L"" : pszServiceName;

	m_statusHandle = NULL;

	// The service runs in its own process.
	m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

	// The service is starting.
	m_status.dwCurrentState = SERVICE_START_PENDING;

	// The accepted commands of the service.
	DWORD dwControlsAccepted = 0;
	if (fCanStop)
		dwControlsAccepted |= SERVICE_ACCEPT_STOP;
	if (fCanShutdown)
		dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
	if (fCanPauseContinue)
		dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
	m_status.dwControlsAccepted = dwControlsAccepted;

	m_status.dwWin32ExitCode = NO_ERROR;
	m_status.dwServiceSpecificExitCode = 0;
	m_status.dwCheckPoint = 0;
	m_status.dwWaitHint = 0;
}


//
//   FUNCTION: CServiceBase::~CServiceBase()
//
//   PURPOSE: The virtual destructor of CServiceBase. 
//
CServiceBase::~CServiceBase(void)
{
}

#pragma endregion


#pragma region Service Start, Stop, Pause, Continue, and Shutdown

//
//   FUNCTION: CServiceBase::Start(DWORD, PWSTR *)
//
//   PURPOSE: The function starts the service. It calls the OnStart virtual 
//   function in which you can specify the actions to take when the service 
//   starts. If an error occurs during the startup, the error will be logged 
//   in the Application event log, and the service will be stopped.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
void CServiceBase::Start(DWORD dwArgc, PWSTR *pszArgv)
{
	try
	{
		// Tell SCM that the service is starting.
		SetServiceStatus(SERVICE_START_PENDING);

		// Perform service-specific initialization.
		OnStart(dwArgc, pszArgv);

		// Tell SCM that the service is started.
		SetServiceStatus(SERVICE_RUNNING);
	}
	catch (DWORD dwError)
	{
		// Log the error.
		WriteErrorLogEntry(L"Service Start", dwError);

		// Set the service status to be stopped.
		SetServiceStatus(SERVICE_STOPPED, dwError);
	}
	catch (...)
	{
		// Log the error.
		WriteEventLogEntry(L"Service failed to start.", EVENTLOG_ERROR_TYPE);

		// Set the service status to be stopped.
		SetServiceStatus(SERVICE_STOPPED);
	}
}


//
//   FUNCTION: CServiceBase::OnStart(DWORD, PWSTR *)
//
//   PURPOSE: When implemented in a derived class, executes when a Start 
//   command is sent to the service by the SCM or when the operating system 
//   starts (for a service that starts automatically). Specifies actions to 
//   take when the service starts. Be sure to periodically call 
//   CServiceBase::SetServiceStatus() with SERVICE_START_PENDING if the 
//   procedure is going to take long time. You may also consider spawning a 
//   new thread in OnStart to perform time-consuming initialization tasks.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
void CServiceBase::OnStart(DWORD dwArgc, PWSTR *pszArgv)
{
}


//
//   FUNCTION: CServiceBase::Stop()
//
//   PURPOSE: The function stops the service. It calls the OnStop virtual 
//   function in which you can specify the actions to take when the service 
//   stops. If an error occurs, the error will be logged in the Application 
//   event log, and the service will be restored to the original state.
//
void CServiceBase::Stop()
{
	DWORD dwOriginalState = m_status.dwCurrentState;
	try
	{
		// Tell SCM that the service is stopping.
		SetServiceStatus(SERVICE_STOP_PENDING);

		// Perform service-specific stop operations.
		OnStop();

		// Tell SCM that the service is stopped.
		SetServiceStatus(SERVICE_STOPPED);
	}
	catch (DWORD dwError)
	{
		// Log the error.
		WriteErrorLogEntry(L"Service Stop", dwError);

		// Set the orginal service status.
		SetServiceStatus(dwOriginalState);
	}
	catch (...)
	{
		// Log the error.
		WriteEventLogEntry(L"Service failed to stop.", EVENTLOG_ERROR_TYPE);

		// Set the orginal service status.
		SetServiceStatus(dwOriginalState);
	}
}


//
//   FUNCTION: CServiceBase::OnStop()
//
//   PURPOSE: When implemented in a derived class, executes when a Stop 
//   command is sent to the service by the SCM. Specifies actions to take 
//   when a service stops running. Be sure to periodically call 
//   CServiceBase::SetServiceStatus() with SERVICE_STOP_PENDING if the 
//   procedure is going to take long time. 
//
void CServiceBase::OnStop()
{
}


//
//   FUNCTION: CServiceBase::Pause()
//
//   PURPOSE: The function pauses the service if the service supports pause 
//   and continue. It calls the OnPause virtual function in which you can 
//   specify the actions to take when the service pauses. If an error occurs, 
//   the error will be logged in the Application event log, and the service 
//   will become running.
//
void CServiceBase::Pause()
{
	try
	{
		// Tell SCM that the service is pausing.
		SetServiceStatus(SERVICE_PAUSE_PENDING);

		// Perform service-specific pause operations.
		OnPause();

		// Tell SCM that the service is paused.
		SetServiceStatus(SERVICE_PAUSED);
	}
	catch (DWORD dwError)
	{
		// Log the error.
		WriteErrorLogEntry(L"Service Pause", dwError);

		// Tell SCM that the service is still running.
		SetServiceStatus(SERVICE_RUNNING);
	}
	catch (...)
	{
		// Log the error.
		WriteEventLogEntry(L"Service failed to pause.", EVENTLOG_ERROR_TYPE);

		// Tell SCM that the service is still running.
		SetServiceStatus(SERVICE_RUNNING);
	}
}


//
//   FUNCTION: CServiceBase::OnPause()
//
//   PURPOSE: When implemented in a derived class, executes when a Pause 
//   command is sent to the service by the SCM. Specifies actions to take 
//   when a service pauses.
//
void CServiceBase::OnPause()
{
}


//
//   FUNCTION: CServiceBase::Continue()
//
//   PURPOSE: The function resumes normal functioning after being paused if
//   the service supports pause and continue. It calls the OnContinue virtual 
//   function in which you can specify the actions to take when the service 
//   continues. If an error occurs, the error will be logged in the 
//   Application event log, and the service will still be paused.
//
void CServiceBase::Continue()
{
	try
	{
		// Tell SCM that the service is resuming.
		SetServiceStatus(SERVICE_CONTINUE_PENDING);

		// Perform service-specific continue operations.
		OnContinue();

		// Tell SCM that the service is running.
		SetServiceStatus(SERVICE_RUNNING);
	}
	catch (DWORD dwError)
	{
		// Log the error.
		WriteErrorLogEntry(L"Service Continue", dwError);

		// Tell SCM that the service is still paused.
		SetServiceStatus(SERVICE_PAUSED);
	}
	catch (...)
	{
		// Log the error.
		WriteEventLogEntry(L"Service failed to resume.", EVENTLOG_ERROR_TYPE);

		// Tell SCM that the service is still paused.
		SetServiceStatus(SERVICE_PAUSED);
	}
}


//
//   FUNCTION: CServiceBase::OnContinue()
//
//   PURPOSE: When implemented in a derived class, OnContinue runs when a 
//   Continue command is sent to the service by the SCM. Specifies actions to 
//   take when a service resumes normal functioning after being paused.
//
void CServiceBase::OnContinue()
{
}


//
//   FUNCTION: CServiceBase::Shutdown()
//
//   PURPOSE: The function executes when the system is shutting down. It 
//   calls the OnShutdown virtual function in which you can specify what 
//   should occur immediately prior to the system shutting down. If an error 
//   occurs, the error will be logged in the Application event log.
//
void CServiceBase::Shutdown()
{
	try
	{
		// Perform service-specific shutdown operations.
		OnShutdown();

		// Tell SCM that the service is stopped.
		SetServiceStatus(SERVICE_STOPPED);
	}
	catch (DWORD dwError)
	{
		// Log the error.
		WriteErrorLogEntry(L"Service Shutdown", dwError);
	}
	catch (...)
	{
		// Log the error.
		WriteEventLogEntry(L"Service failed to shut down.", EVENTLOG_ERROR_TYPE);
	}
}


//
//   FUNCTION: CServiceBase::OnShutdown()
//
//   PURPOSE: When implemented in a derived class, executes when the system 
//   is shutting down. Specifies what should occur immediately prior to the 
//   system shutting down.
//
void CServiceBase::OnShutdown()
{
}

#pragma endregion


#pragma region Helper Functions

//
//   FUNCTION: CServiceBase::SetServiceStatus(DWORD, DWORD, DWORD)
//
//   PURPOSE: The function sets the service status and reports the status to 
//   the SCM.
//
//   PARAMETERS:
//   * dwCurrentState - the state of the service
//   * dwWin32ExitCode - error code to report
//   * dwWaitHint - estimated time for pending operation, in milliseconds
//
void CServiceBase::SetServiceStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure of the service.

	m_status.dwCurrentState = dwCurrentState;
	m_status.dwWin32ExitCode = dwWin32ExitCode;
	m_status.dwWaitHint = dwWaitHint;

	m_status.dwCheckPoint =
		((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED)) ?
		0 : dwCheckPoint++;

	// Report the status of the service to the SCM.
	::SetServiceStatus(m_statusHandle, &m_status);
}


//
//   FUNCTION: CServiceBase::WriteEventLogEntry(PWSTR, WORD)
//
//   PURPOSE: Log a message to the Application event log.
//
//   PARAMETERS:
//   * pszMessage - string message to be logged.
//   * wType - the type of event to be logged. The parameter can be one of 
//     the following values.
//
//     EVENTLOG_SUCCESS
//     EVENTLOG_AUDIT_FAILURE
//     EVENTLOG_AUDIT_SUCCESS
//     EVENTLOG_ERROR_TYPE
//     EVENTLOG_INFORMATION_TYPE
//     EVENTLOG_WARNING_TYPE
//
void CServiceBase::WriteEventLogEntry(PWSTR pszMessage, WORD wType)
{
	HANDLE hEventSource = NULL;
	LPCWSTR lpszStrings[2] = { NULL, NULL };

	hEventSource = RegisterEventSource(NULL, m_name);
	if (hEventSource)
	{
		lpszStrings[0] = m_name;
		lpszStrings[1] = pszMessage;

		ReportEvent(hEventSource,  // Event log handle
			wType,                 // Event type
			0,                     // Event category
			0,                     // Event identifier
			NULL,                  // No security identifier
			2,                     // Size of lpszStrings array
			0,                     // No binary data
			lpszStrings,           // Array of strings
			NULL                   // No binary data
		);

		DeregisterEventSource(hEventSource);
	}
}


//
//   FUNCTION: CServiceBase::WriteErrorLogEntry(PWSTR, DWORD)
//
//   PURPOSE: Log an error message to the Application event log.
//
//   PARAMETERS:
//   * pszFunction - the function that gives the error
//   * dwError - the error code
//
void CServiceBase::WriteErrorLogEntry(PWSTR pszFunction, DWORD dwError)
{
	wchar_t szMessage[260];
	StringCchPrintf(szMessage, ARRAYSIZE(szMessage),
		L"%s failed w/err 0x%08lx", pszFunction, dwError);
	WriteEventLogEntry(szMessage, EVENTLOG_ERROR_TYPE);
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class CSampleService : public CServiceBase
{
public:

	CSampleService(PWSTR pszServiceName,
		BOOL fCanStop = TRUE,
		BOOL fCanShutdown = TRUE,
		BOOL fCanPauseContinue = FALSE);
	virtual ~CSampleService(void);

protected:

	virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv);
	virtual void OnStop();

	void ServiceWorkerThread(void);

private:

	BOOL m_fStopping;
	HANDLE m_hStoppedEvent;
};

CSampleService::CSampleService(PWSTR pszServiceName,
	BOOL fCanStop,
	BOOL fCanShutdown,
	BOOL fCanPauseContinue)
	: CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
{
	m_fStopping = FALSE;

	// Create a manual-reset event that is not signaled at first to indicate 
	// the stopped signal of the service.
	m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hStoppedEvent == NULL)
	{
		throw GetLastError();
	}
}


CSampleService::~CSampleService(void)
{
	if (m_hStoppedEvent)
	{
		CloseHandle(m_hStoppedEvent);
		m_hStoppedEvent = NULL;
	}
}


//
//   FUNCTION: CSampleService::OnStart(DWORD, LPWSTR *)
//
//   PURPOSE: The function is executed when a Start command is sent to the 
//   service by the SCM or when the operating system starts (for a service 
//   that starts automatically). It specifies actions to take when the 
//   service starts. In this code sample, OnStart logs a service-start 
//   message to the Application log, and queues the main service function for 
//   execution in a thread pool worker thread.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
//   NOTE: A service application is designed to be long running. Therefore, 
//   it usually polls or monitors something in the system. The monitoring is 
//   set up in the OnStart method. However, OnStart does not actually do the 
//   monitoring. The OnStart method must return to the operating system after 
//   the service's operation has begun. It must not loop forever or block. To 
//   set up a simple monitoring mechanism, one general solution is to create 
//   a timer in OnStart. The timer would then raise events in your code 
//   periodically, at which time your service could do its monitoring. The 
//   other solution is to spawn a new thread to perform the main service 
//   functions, which is demonstrated in this code sample.
//
void CSampleService::OnStart(DWORD dwArgc, LPWSTR *lpszArgv)
{
	// Log a service start message to the Application log.
	WriteEventLogEntry(L"CppWindowsService in OnStart",
		EVENTLOG_INFORMATION_TYPE);

	// Queue the main service function for execution in a worker thread.
	CThreadPool::QueueUserWorkItem(&CSampleService::ServiceWorkerThread, this);
}


//
//   FUNCTION: CSampleService::ServiceWorkerThread(void)
//
//   PURPOSE: The method performs the main function of the service. It runs 
//   on a thread pool worker thread.
//
void CSampleService::ServiceWorkerThread(void)
{
	// Periodically check if the service is stopping.
/*
	while (!m_fStopping)
	{
		// Perform main service function here...
		//FIXME //TODO 这里执行需要执行的内容

		::Sleep(2000);  // Simulate some lengthy operations.
	}
*/
	App_ServiceStatus = 1;
	main_server();

	// Signal the stopped event.
	SetEvent(m_hStoppedEvent);
}


//
//   FUNCTION: CSampleService::OnStop(void)
//
//   PURPOSE: The function is executed when a Stop command is sent to the 
//   service by SCM. It specifies actions to take when a service stops 
//   running. In this code sample, OnStop logs a service-stop message to the 
//   Application log, and waits for the finish of the main service function.
//
//   COMMENTS:
//   Be sure to periodically call ReportServiceStatus() with 
//   SERVICE_STOP_PENDING if the procedure is going to take long time. 
//
void CSampleService::OnStop()
{
	// Log a service stop message to the Application log.
	WriteEventLogEntry(L"CppWindowsService in OnStop",
		EVENTLOG_INFORMATION_TYPE);

	// Indicate that the service is stopping and wait for the finish of the 
	// main service function (ServiceWorkerThread).
	m_fStopping = TRUE;

	App_ServiceStatus = 0;

	if (WaitForSingleObject(m_hStoppedEvent, INFINITE) != WAIT_OBJECT_0)
	{
		throw GetLastError();
	}
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

//
//   FUNCTION: InstallService
//
//   PURPOSE: Install the current application as a service to the local 
//   service control manager database.
//
//   PARAMETERS:
//   * pszServiceName - the name of the service to be installed
//   * pszDisplayName - the display name of the service
//   * dwStartType - the service start option. This parameter can be one of 
//     the following values: SERVICE_AUTO_START, SERVICE_BOOT_START, 
//     SERVICE_DEMAND_START, SERVICE_DISABLED, SERVICE_SYSTEM_START.
//   * pszDependencies - a pointer to a double null-terminated array of null-
//     separated names of services or load ordering groups that the system 
//     must start before this service.
//   * pszAccount - the name of the account under which the service runs.
//   * pszPassword - the password to the account name.
//
//   NOTE: If the function fails to install the service, it prints the error 
//   in the standard output stream for users to diagnose the problem.
//
void InstallService(PWSTR pszServiceName,
	PWSTR pszDisplayName,
	DWORD dwStartType,
	PWSTR pszDependencies,
	PWSTR pszAccount,
	PWSTR pszPassword)
{
	wchar_t szPath[MAX_PATH];
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;

	if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0)
	{
		wprintf(L"GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	// Open the local default service control manager database
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT |
		SC_MANAGER_CREATE_SERVICE);
	if (schSCManager == NULL)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	// Install the service into SCM by calling CreateService
	schService = CreateService(
		schSCManager,                   // SCManager database
		pszServiceName,                 // Name of service
		pszDisplayName,                 // Name to display
		SERVICE_QUERY_STATUS,           // Desired access
		SERVICE_WIN32_OWN_PROCESS,      // Service type
		dwStartType,                    // Service start type
		SERVICE_ERROR_NORMAL,           // Error control type
		szPath,                         // Service's binary
		NULL,                           // No load ordering group
		NULL,                           // No tag identifier
		pszDependencies,                // Dependencies
		pszAccount,                     // Service running account
		pszPassword                     // Password of the account
	);
	if (schService == NULL)
	{
		wprintf(L"CreateService failed w/err 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	wprintf(L"%s is installed.\n", pszServiceName);

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
		schSCManager = NULL;
	}
	if (schService)
	{
		CloseServiceHandle(schService);
		schService = NULL;
	}
}


//
//   FUNCTION: UninstallService
//
//   PURPOSE: Stop and remove the service from the local service control 
//   manager database.
//
//   PARAMETERS: 
//   * pszServiceName - the name of the service to be removed.
//
//   NOTE: If the function fails to uninstall the service, it prints the 
//   error in the standard output stream for users to diagnose the problem.
//
void UninstallService(PWSTR pszServiceName)
{
	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService = NULL;
	SERVICE_STATUS ssSvcStatus = {};

	// Open the local default service control manager database
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (schSCManager == NULL)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	// Open the service with delete, stop, and query status permissions
	schService = OpenService(schSCManager, pszServiceName, SERVICE_STOP |
		SERVICE_QUERY_STATUS | DELETE);
	if (schService == NULL)
	{
		wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	// Try to stop the service
	if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
	{
		wprintf(L"Stopping %s.", pszServiceName);
		Sleep(1000);

		while (QueryServiceStatus(schService, &ssSvcStatus))
		{
			if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
			{
				wprintf(L".");
				Sleep(1000);
			}
			else break;
		}

		if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
		{
			wprintf(L"\n%s is stopped.\n", pszServiceName);
		}
		else
		{
			wprintf(L"\n%s failed to stop.\n", pszServiceName);
		}
	}

	// Now remove the service by calling DeleteService.
	if (!DeleteService(schService))
	{
		wprintf(L"DeleteService failed w/err 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	wprintf(L"%s is removed.\n", pszServiceName);

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
		schSCManager = NULL;
	}
	if (schService)
	{
		CloseServiceHandle(schService);
		schService = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: wmain(int, wchar_t *[])
//
//  PURPOSE: entrypoint for the application.
// 
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    wmain() either performs the command line task, or run the service.
//
int ServiceMainProc(int argc, const char** argv)
{
	if ((argc > 1) && ((*argv[1] == '-' || (*argv[1] == '/'))))
	{
		if (_stricmp("install", argv[1] + 1) == 0)
		{
			// Install the service when the command is 
			// "-install" or "/install".
			InstallService(
				SERVICE_NAME,               // Name of service
				SERVICE_DISPLAY_NAME,       // Name to display
				SERVICE_START_TYPE,         // Service start type
				SERVICE_DEPENDENCIES,       // Dependencies
				SERVICE_ACCOUNT,            // Service running account
				SERVICE_PASSWORD            // Password of the account
			);
			return 1;
		}
		else if (_stricmp("remove", argv[1] + 1) == 0)
		{
			// Uninstall the service when the command is 
			// "-remove" or "/remove".
			UninstallService(SERVICE_NAME);
			return 1;
		}
	}
	else
	{
		wprintf(L"Parameters:\n");
		wprintf(L" -install  to install the service.\n");
		wprintf(L" -remove   to remove the service.\n");

		CSampleService service(SERVICE_NAME);
		if (!CServiceBase::Run(service))
		{
			wprintf(L"Service failed to run w/err %ld\n", GetLastError());
		}
	}

	return 0;
}



#endif
