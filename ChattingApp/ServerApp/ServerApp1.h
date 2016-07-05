#pragma once

#include "resource.h"
#include <string.h>

using namespace std;

class ServerApp
{
	LPCTSTR nIP; //IP
	UINT nPort; //Port
	int nMaxConnect; //Max Connect
	int nMaxMessenger; //Max Messenger
	int nNowConnect; //Now Connect
	//Create Socket Server and Client
	CSocket server;
	CSocket* client;

public:
	//Contruc
	ServerApp()
	{
		//Create Socket Client
		client = new CSocket [nMaxConnect];
		nNowConnect = 0;
	}
	//Destruc
	~ ServerApp()
	{
		delete client;
	}

	void Chatting()
	{
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
	}

	bool listenClient()
	{
		//Listen connect from client
		if (server.Listen(1) == FALSE)
		{
			cout << "Do not listen on port!!! \n";
			server.Close();
			return false;
		}

		cout << "Waiting for Client!!! \n";
		return true;
	}

	bool initSocket()
	{
		// Init socket in Windows
		if (AfxSocketInit(NULL) == FALSE)
		{
			cout << "Init socket libraray fail!!! \n"; 
			return false;
		}

		//Init Socket with nPort
		if (server.Create(nPort,SOCK_STREAM,NULL) == 0)
		{
			cout << "Init Socket fail!!! \n";
			cout << server.GetLastError();
			return false;
		} 
		else
		{
			cout << "Server init sucess!!! \n";
			return true;
		}
	}

	ifstream readFile(char* file)
	{
		ifstream filename;
		
		filename.open(file);

		return filename;
	}

	ifstream writeFile(char* file)
	{
		ofstream filename;

		filename.open(file);

		return filename;
	}

	void readConfig(char* file)
	{
		ifstream fileConfig = readFile (file);
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

	void writeConfig(LPCTSTR nIP, UINT nPort, int nMaxConnect, int nMaxMessenger)
	{
		ofstream fileConfig = writeFile ("config.ini");
		string line;

		if (fileConfig.is_open())
		{
			fileConfig << nIP << "/n";
			fileConfig << nPort << "/n";
			fileConfig << nMaxConnect << "/n";
			fileConfig << nMaxMessenger << "/n";
			fileConfig.close();
		}
		else 
		{
			cout << "Unable to open fileConfig";
		}
	}
};