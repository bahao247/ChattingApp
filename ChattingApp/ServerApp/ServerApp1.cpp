// ServerApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ServerApp.h"
#include <afxsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	ServerApp sApp;
	sApp.readConfig("config.ini");
//////////////////////////////////////////////////////////////////////////
	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
//////////////////////////////////////////////////////////////////////////
 			// Init socket in Windows and Chatting
			if (sApp.initSocket() || sApp.listenClient())
			{
				return false;
			}
			else
			{
				sApp.Chatting();
			}
//////////////////////////////////////////////////////////////////////////
		}

	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
