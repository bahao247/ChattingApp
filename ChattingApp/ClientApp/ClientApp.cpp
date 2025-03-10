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
bool initSocket(CSocket client)
{
	//Connect server
	if (client.Connect(_T("127.0.0.1"), nPort) != 0)
	{
		cout << "Connect server sucess!!!" << endl;
		
		//Init var
		char *msg = new char [nMaxMessenger];
		int len;
		char *temp;

		int id, rid;

		client.Receive((char *) &id, sizeof(id), 0);

		cout << "Client [" << id + 1 << "] connect server sucess!!! \n";
	}

	return true;
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
void sendPacket(CSocket &client, int &id, int &rid, int &len, char* msg)
{
	//Send a message to Server
	client.Send(&id, sizeof(int), 0);

	client.Send(&rid, sizeof(int), 0);

	client.Send(&len, sizeof(int), 0);

	client.Send(msg, len, 0);
}
//////////////////////////////////////////////////////////////////////////
//Receive Packet
void receivePacket(CSocket &client, int &id, int &rid, int &len)
{
	//Receive a message to Server
	client.Receive((char*) &len, sizeof(int), 0);

	//Init temp
	char* temp = new char[len + 1];

	//Receive meassge
	client.Receive((char*) temp, len, 0);

	temp[len + 1] = '\0';//Ending char

	//Display meassge
	int nClientCount;
	client.Receive((char *) &nClientCount, sizeof(nClientCount), 0);

	cout << "Client[" << nClientCount + 1 << "] says: " << temp << "\n\a";
}
//////////////////////////////////////////////////////////////////////////
//Chatting Packet
void selectIDM(int rid = 111)
{
	//Chatting with ID
	cout << "Want to chat with ID? \n";
	cin >> rid;
}
//////////////////////////////////////////////////////////////////////////
//Chatting Packet
void chattingClient(int &id, int &rid, char* msg, int &len)
{
	//Client send message
	cout << "Client [" << id + 1 <<"] chatting with Client [" << rid + 1 <<"]:\a";
	cin.getline(msg,100);
	len = strlen(msg);
}
//////////////////////////////////////////////////////////////////////////
//Destruc
void destroyConnect(CSocket &client)
{
	//Delete temp object
	//delete temp;
	//delete msg;

	client.Close();
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
			if (client.Connect(_T("127.0.0.1"), nPort) != 0)
			{
				cout << "Connect server sucess!!!" << endl;
				
				int id, rid;

				client.Receive((char *) &id, sizeof(id), 0);

				cout << "Client [" << id + 1 << "] connect server sucess!!! \n";

				//Begin chat
				while (true)
				{
					//PTR TuanNA [ [7/7/2016]] SelectIDM
					selectIDM(rid);

					//Init var
					char *msg = new char [nMaxMessenger];
					int len;
					
					chattingClient(id, rid, msg, len);

					char *temp = new char[len +1];

					//Send a message to Server
					sendPacket(client, id, rid, len, temp);

					//Receive a message to Server
					receivePacket(client, id, rid, len);
				}
				destroyConnect(client);
				//////////////////////////////////////////////////////////////////////////
			}
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
