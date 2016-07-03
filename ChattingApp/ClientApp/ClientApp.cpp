// ClientApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ClientApp.h"
#include <afxsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	LPCTSTR nIP;
	UINT nPort;
	int nMaxConnect;
	int nMaxMessenger;
	int nNowConnect = 0;

	//readConfig(nIP, nPort, nMaxConnect, nMaxMessenger);
	string line;
	int i = 0;

	ifstream fileConfig;

	fileConfig.open("config.ini");

	if (fileConfig.is_open())
	{
		while (getline(fileConfig,line) )
		{
			if (i == 0)
			{
				CA2T tempIP(line.c_str());
				nIP = tempIP;
			}else if (i == 1)
			{
				nPort = stoi(line);
			} else if (i == 2)
			{
				nMaxConnect = stoi(line);
			} else if (i == 3)
			{
				nMaxMessenger = stoi(line);
			}

			i++;
		}
		fileConfig.close();
	}

	else 
	{
		cout << "Unable to open fileConfig";
	}
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
			// Init socket in Windows
			if (AfxSocketInit(NULL) == FALSE)
			{
				cout << "Init socket libraray fail!!! \n"; 
				return false;
			}

			//Create Socket Client
			CSocket client;

			//Init Socket
			client.Create();

			//Connect server
			if (client.Connect(nIP, nPort) != 0)
			{
				int id;

				client.Receive(&id, sizeof(id), 0);

				cout << "Client " << id + 1 << " Connect server sucess!!! \n";
			} 

			//Init var
			char* msg = new char [nMaxMessenger];
			int len = 0;

			//Begin chat
			while (true)
			{
				//Client send message
				cout << "Client says: ";
				gets(msg);
				len = strlen(msg);

				//Send a message to Server
				client.Send(&len, sizeof(int), 0);
				client.Send(msg, len, 0);

				client.Receive(&len, sizeof(int), 0);

				//Init temp
				char* temp = new char[len + 1];

				//Receive meassge
				client.Receive(temp, len, 0);

				temp[len] = 0;//Ending char

				//Display meassge
				cout << "Server says: " << temp << "\n";

				//Delete temp object
				delete temp;
				delete msg;
			}

			client.Close();

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
