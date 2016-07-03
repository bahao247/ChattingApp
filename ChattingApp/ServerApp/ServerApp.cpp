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

			//Create Socket Server and Client
			CSocket server;
			CSocket* client = new CSocket [nMaxConnect];

			//Init Socket with port = 12345
			if (server.Create(nPort,SOCK_STREAM,NULL) == 0)
			{
				cout << "Init Socket fail!!! \n";
				cout << server.GetLastError();
				return 0;
			} 
			else
			{
				cout << "Server init sucess!!! \n";
			}

			//Listen connect from client
			if (server.Listen(1) == FALSE)
			{
				cout << "Do not listen on port!!! \n";
				server.Close();
				return false;
			}

			cout << "Waiting for Client!!! \n";

			//Init var
			char *msg =  new char [nMaxMessenger];
			int len;
			char* temp;
 
 			//Begin chat
 			while (true)
			{
				if (nNowConnect == nMaxConnect)
				{
					cout << "Full connect Server!!! \n";
					goto Chatting;
				}

				for (int nClient = nNowConnect; nClient < nMaxConnect; nClient++)
				{
					//Accept connect from Client
					if (server.Accept(client[nClient]))
					{
						cout << "Client [" << nClient + 1 << "] connected!!! \n";
						
						//Inc nNowConnect
						nNowConnect++;

						//Send nNowConnect to Client
						client [nClient].Send(&nNowConnect, sizeof(int), 0);
						goto Chatting;
					
					//Label Chatting
					Chatting:
						//Recive message
						for (int nClientCount = 0; nClientCount < nNowConnect; nClientCount++)
						{
							if (client[nClientCount] != NULL)
							{
								//Receive message from Client
								client[nClientCount].Receive((char *) &len, sizeof(int), 0);

								//Init temp
								temp = new char[len + 1];

								client[nClientCount].Receive((char *) temp, len, 0);

								temp[len] = '\0';//Ending char

								//Display meassge
								cout << "Client[" << nClientCount + 1 <<"] says: " << temp << "\n";
							}
							else
							{
								client[nClientCount].ShutDown(2);
								break;
							}
						}//end for

						for (int i = 0; i < nNowConnect; i++)
						{
							cout << "Server say with Client [" << i + 1 << "]: ";
							cin.getline(msg, 100);
							len = strlen(msg);

							//Send a message to Client
							client [i].Send(&len, sizeof(int), 0);
							client [i].Send(msg, len, 0);
						}//end for
					}//end if
				}//end for
			}//end while

			//Delete temp object
			delete temp;
			for (int i = 0; i < nNowConnect; i++)
			{
				client[i].Close();
			}
			delete client;
			
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
