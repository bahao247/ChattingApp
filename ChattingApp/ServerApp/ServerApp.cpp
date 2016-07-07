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

//////////////////////////////////////////////////////////////////////////
//
//Function Region
//
//////////////////////////////////////////////////////////////////////////
//Read file config
void readConfig(char* file)
{
	ifstream fileConfig;

	fileConfig.open(file);

	string line;

	int i = 0;

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
}
//////////////////////////////////////////////////////////////////////////
//Init socket
bool initSocket(CSocket &server)
{
	//Init Socket with port, type, flag
	if (server.Create(nPort,SOCK_STREAM,_T("127.0.0.1")) == 0)
	{
		cout << "Init Socket fail!!! \n";
		cout << server.GetLastError();
		return 0;
	} 
	else
	{
		cout << "Server init sucess!!! \n";
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

//Listen Connect
bool listenConnect(CSocket &server)
{
	//Listen connect from client
	if (server.Listen(1) == FALSE)
	{
		cout << "Do not listen on port!!! \n";
		server.Close();
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////

//Accept Connect
bool acceptConnect(CSocket &server, CSocket* client, int &id, int idArray[])
{
	try
	{
		server.Accept(client[id]);

		cout << "Client [" << id + 1 << "] connected!!!\n";

		//Inc nNowConnect
		nNowConnect++;

		//Send id to Client
		client [id].Send(&id, sizeof(int), 0);

		//Inc id
		idArray[id] = 1;
		id++;

		return true;
	}
	catch (int e)
	{
		return true;
	}
}
//////////////////////////////////////////////////////////////////////////
//Check full roomchat
bool checkNowConnect()
{
	if (nNowConnect == nMaxConnect)
	{
		cout << "Server is full. Waiting, please!!!\n";
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
//Send Packet
void sendPacket(CSocket* client, int &nClientCount, int nid, int rid, int &len, char* temp)
{
	for (int i = 0; i < id; i++)
	{
		if ((i != nid && idArray[i] != 0) || rid == 111)
		{
			client [i].Send(&len, sizeof(int), 0);
			client [i].Send(temp, len, 0);

			//Send nClientCount to Client
			client [i].Send(&nid, sizeof(int), 0);
		}
	}

	//Display meassge
	cout << "Client[" << nClientCount + 1 <<"] says: " << temp << ".\a\n";
}
//////////////////////////////////////////////////////////////////////////
//Chatting Packet
void chattingConnect(CSocket* client, int &nClientCount)
{
	int len = 0;
	int rid = 111;
	int nid = 0;

	//Receive id and rid
	client[nClientCount].Receive((char *) &nid, sizeof(int), 0);
	client[nClientCount].Receive((char *) &rid, sizeof(int), 0);

	idArray[nid] = rid + 2;

	//Receive message from Client
	client[nClientCount].Receive((char *) &len, sizeof(int), 0);

	//Init temp
	char* temp = new char[len + 1];

	client[nClientCount].Receive((char *) temp, len, 0);

	temp[len] = '\0';//Ending char

	if (temp == NULL || len == 0)
	{
		idArray[nid] = 0;
		nNowConnect--;
		cout << "Client[" << nid + 1 <<"] offline!!!\a\n";

		//Send a message to Client
		temp = "I'm offline. Nice to see you!";

		for (int i = 0; i < id; i++)
		{
			if ((i != nid && idArray[i] != 0) || rid == 111)
			{
				client [i].Send(&len, sizeof(int), 0);
				client [i].Send(temp, len, 0);

				//Send nClientCount to Client
				client [i].Send(&nid, sizeof(int), 0);
			}
		}

		//Display meassge
		cout << "Client[" << nClientCount + 1 <<"] says: " << temp << ".\a\n";

				sendPacket(client, nClientCount, nid, rid, len, temp);
	}
	sendPacket(client, nClientCount, nid, rid, len, temp);

	delete temp;
}
//////////////////////////////////////////////////////////////////////////
//Destruc
void destroyConnect(CSocket* client, CSocket &server)
{
	for (int i = 0; i < nNowConnect; i++)
	{
		client[i].Close();
	}

	server.Close();
	delete client;
}
//////////////////////////////////////////////////////////////////////////
//Main
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	//PTR TuanNA [readConfig- [7/7/2016]]
	readConfig("config.ini");

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
			//PTR TuanNA [Init socket in Windows- [7/7/2016]]
			if (AfxSocketInit(NULL) == FALSE)
			{
				cout << "Init socket libraray fail!!! \n"; 
				return false;
			}

			//Create Socket Server and Client
			CSocket server;
			CSocket* client = new CSocket [10];

			//PTR TuanNA [Init Socket with port, type, flag And Listen connect- [7/7/2016]]
			if (initSocket(server) && listenConnect(server))
			{
				cout << "Waiting for Client!!! \n";
			}
			else
			{
				return false;
			}

			//PTR TuanNA [Begin chat while- [7/7/2016]]
			while (true)
			{
				int nClientCount = 0;
				//PTR TuanNA [Check client in roomchat,  [7/7/2016]]
				if (checkNowConnect())
				{
					for (id; nNowConnect < nMaxConnect;)
					{
						//PTR TuanNA [Accept connect from Client- [7/7/2016]]
						if (acceptConnect(server, client, id, idArray))
						{
							for (nClientCount; nClientCount < id; nClientCount++)
							{
								if (client[nClientCount] != NULL && idArray[nClientCount] != 0)
								{
									//PTR TuanNA [Receicve and Send Packet with Client- [7/7/2016]]
									chattingConnect(client, nClientCount);
								}
							}//end for
						}//end if


					}//end for
				}
			}//end while
			
			//PTR TuanNA [destroyConnect- [7/7/2016]]
			destroyConnect(client, server);
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
