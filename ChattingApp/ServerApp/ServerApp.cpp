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

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

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
			AfxSocketInit(NULL);

			//Create Socket Server and Client
			CSocket server;
			CSocket client;

			//Init Socket with port = 12345
			server.Create(12345);

			//Listen connect from client
			server.Listen();

			cout << "Waiting for Client!!! \n";

			//Accept connect from Client
			if (server.Accept(client))
			{
				cout << "Client connect!!! \n";
			}

			//Init var
			char msg[100];
			int len = 0;

			//Begin chat
			while (true)
			{
				//Recive message
				client.Receive(&len, sizeof(int), 0);
				
				//Init temp
				char* temp = new char[len + 1];

				//Receive meassge
				client.Receive(temp, len, 0);

				temp[len] = 0;//Ending char

				//Display meassge
				cout << "Client says: " << temp << "\n";

				//Send meassge for Client
				cout << "Server says: ";
				gets(msg);
				len = strlen(msg);

				//Send a message to Server
				client.Send(&len, sizeof(int), 0);
				client.Send(msg, len, 0);


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
