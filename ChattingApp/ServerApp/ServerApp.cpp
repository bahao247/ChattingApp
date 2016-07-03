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
	int id = 0;
	int idArray[100];

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
			CSocket* client = new CSocket [10];

			//Init Socket with port = nPort
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
 
 			//Begin chat
 			while (true)
			{
				int nClientCount = 0;
				if (nNowConnect == nMaxConnect)
				{
					cout << "Server is full. Waiting, please!!!\n";
					goto Chatting;
				}

				for (id; nNowConnect < nMaxConnect;)
				{
					//Accept connect from Client
					if (server.Accept(client[id]))
					{
						cout << "Client [" << id + 1 << "] connected!!!\n";
						
						//Inc nNowConnect
						nNowConnect++;

						//Send id to Client
						client [id].Send(&id, sizeof(int), 0);
						
						//Inc id
						idArray[id] = 1;
						id++;

						goto Chatting;
					
					//Label Chatting
					Chatting:
						//Recive message
						for (nClientCount; nClientCount < id; nClientCount++)
						{
							if (client[nClientCount] != NULL && idArray[nClientCount] == 1)
							{
								len = 0;
								//Receive message from Client
								client[nClientCount].Receive((char *) &len, sizeof(int), 0);

								//Init temp
								char* temp = new char[len + 1];

								client[nClientCount].Receive((char *) temp, len, 0);

								temp[len] = '\0';//Ending char

								if (temp == NULL || len == 0)
								{
									idArray[nClientCount] = 0;
									nNowConnect--;
									cout << "Client[" << nClientCount + 1 <<"] offline!!!\a\n";
									
									//Send a message to Client
									temp = "I'm offline. Nice to see you!";

									//temp[len] = '\0';//Ending char
									
									for (int i = 0; i < id; i++)
									{
										if (i != nClientCount && idArray[i] == 1)
										{
											client [i].Send(&len, sizeof(int), 0);
											client [i].Send(temp, len, 0);

											//Send nClientCount to Client
											client [i].Send(&nClientCount, sizeof(int), 0);
										}

									}//end for

									continue;
								}

								for (int i = 0; i < id; i++)
								{
									if (i != nClientCount && idArray[i] == 1)
									{
										//Send a message to Client
										client [i].Send(&len, sizeof(int), 0);
										client [i].Send(temp, len, 0);
										
										//Send nClientCount to Client
										client [i].Send(&nClientCount, sizeof(int), 0);
									}
									
								}//end for

								//Display meassge
								cout << "Client[" << nClientCount + 1 <<"] says: " << temp << ".\a\n";

								//Delete temp object
								delete temp;
							}
						}//end for
					}//end if
				}//end for
			}//end while

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
