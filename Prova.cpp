*****   SERVER   *****
*/

#include <iostream>
#include <winsock2.h>
#include <winsock.h>
#include <stdio.h>
#include <string>
#include <ws2tcpip.h>
#include <Windows.h>
#include <process.h>
#include <sysinfoapi.h>
#include <sstream>
#include <iomanip>
#include <atlstr.h>

using namespace std;
#pragma comment(lib,"WS2_32")
#pragma warning(disable:4996)
/*

Trasforma la variabile di tipo SYSTEMTIME in stringa

*/
string getDate(SYSTEMTIME st) {
	std::string strMessage;

	// Output a given SYSTEMTIME in the format "

	// 1. Using CString
	CString cstrMessage;


	//Definisco come formattare il formato della data
	cstrMessage.Format("%d-%02d-%02d %02d:%02d:%02d.%03d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);

	strMessage = cstrMessage;
	std::cout << "System time = " << strMessage << std::endl;
	return strMessage;
}
/*

Trasforma la stringa ricevuta dal client in una variabile di tipo SYSTEMTIME
*/
void strToSysTime(SYSTEMTIME &systime, char *dateTimeString)
{
	//SYSTEMTIME systime;

	memset(&systime, 0, sizeof(systime));
	//Suddivide la stringa, e assegna i valori a systime
	sscanf_s(dateTimeString, "%d-%02d-%02d %02d:%02d:%02d.%03d",
		&systime.wYear,
		&systime.wMonth,
		&systime.wDay,
		&systime.wHour,
		&systime.wMinute,
		&systime.wSecond,
		&systime.wMilliseconds);
	return ;
}
/*

Funzione per calcolare la differenza di tempo tra i due dispositivi

*/
__int64 difference(SYSTEMTIME t1, SYSTEMTIME t2) {
	FILETIME ft1, ft2;
	SystemTimeToFileTime(&t1, &ft1);
	SystemTimeToFileTime(&t2, &ft2);

	ULARGE_INTEGER ul1;
	ul1.LowPart = ft1.dwLowDateTime;
	ul1.HighPart = ft1.dwHighDateTime;

	ULARGE_INTEGER ul2;
	ul2.LowPart = ft2.dwLowDateTime;
	ul2.HighPart = ft2.dwHighDateTime;


	return (ul2.QuadPart -= ul1.QuadPart);
	
}
void run(void * socket) {
	int result = 0;

	SYSTEMTIME localTime;
	SYSTEMTIME clientTime;
	SOCKET AcceptSocket = (SOCKET)socket;
	GetSystemTime(&localTime);
	char* buffer = new char[sizeof(SYSTEMTIME)];
	result = recv(AcceptSocket, buffer, sizeof(string), 0); //Si attende per il messaggio del client
	strToSysTime(clientTime, buffer);
	getDate(clientTime);
	string local = getDate(localTime);
	if (difference(localTime, clientTime) >= 0) { //Controlla che la differenza non sia negativa
		cout << "Differenza: " << difference(localTime, clientTime) << endl;
	}
	else {
		cout << "Differenza: " << difference(clientTime, localTime) << endl;
	}
	send(AcceptSocket, local.c_str() , sizeof(string), 0); //Si invia al client l'orario del server
	
	cout << "Connessione terminata!" << endl;
	closesocket(AcceptSocket);
}
int main()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		cout << "Errore WSAStartup()" << endl;
	}

	//si crea una socket TCP che utilizza IPv4 come indirizzamento
	SOCKET m_socket;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket < 0) {
		cout << "Errore creazione socket: " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else cout << "Socket creata con successo!" << endl;

	//si associa una socket ad una determinata porta e indirizzo ip

	SOCKADDR_IN service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(27000);
	if (bind(m_socket, (SOCKADDR*)&service, sizeof(service)) ==
		SOCKET_ERROR) {
		cout << "Errore bind()" << endl;
		closesocket(m_socket);
		return 0;
	}

	//Il server si mette in ascolto dei client, fino a 10 client

	if (listen(m_socket, 10) == SOCKET_ERROR) {
		cout << "Errore listen()" << endl;
	}

	cout << "In attesa del Client..." << endl;
	SOCKET AcceptSocket;
	HANDLE * connessioni = new HANDLE[10];
	int sin_size = sizeof(struct sockaddr_in);
	int i = 0;
	//se è arrivata una richiesta il server l'accetta e crea una connessione con il client creando un nuovo trhead

	while ((AcceptSocket = accept(m_socket, (struct sockaddr *)&service, &sin_size)) != NULL) {
		cout << "Connessione effettuata" << endl;
		//int result = 0;

		sockaddr in;
		int ipLen;

		if (AcceptSocket == INVALID_SOCKET) {
			printf("Accettazione fallita: %d\n", WSAGetLastError());
			closesocket(AcceptSocket);
			continue;
		}

		else cout << "Client connesso.\n";

		//Ip e Numero Porta del Client
		char clientHost[NI_MAXHOST];
		char clientPort[NI_MAXSERV];

		//Recupero Ip e Numero Porta del Client
		int theErrorCode = getnameinfo((sockaddr*)&service, sizeof(service), clientHost, sizeof(clientHost), clientPort, sizeof(clientPort), NI_NUMERICHOST | NI_NUMERICSERV);

		if (theErrorCode != 0)
		{
			cout << "Error!!!" << endl;
			continue;
		}
		else
		{
			cout << "Indirizzo Ip del Client : " << clientHost << "\n";
			cout << "Porta del Client :  " << clientPort << "\n";
		}
		connessioni[i++] = (HANDLE)_beginthread(run, NULL, (void*)AcceptSocket);

		if (i == 9) exit;
	}
	WaitForMultipleObjects(10, connessioni, true, NULL); //Aspetta tutti i thread

	system("pause");
}
 /*
             *****   CLIENT   *****
*/


#include<iostream>
#include<winsock.h>
#include<stdio.h>
#include <string>
#include <sysinfoapi.h>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <atlstr.h>
using namespace std;
#pragma comment(lib,"WS2_32")
string getDate(SYSTEMTIME st) {
	std::string strMessage;

	CString cstrMessage;

	cstrMessage.Format("%d-%02d-%02d %02d:%02d:%02d.%03d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);

	strMessage = cstrMessage;
	std::cout << "System time = " << strMessage << std::endl;
	return strMessage;
}
SYSTEMTIME strToSysTime(char *dateTimeString)
{
	SYSTEMTIME systime;

	memset(&systime, 0, sizeof(SYSTEMTIME));
	sscanf_s(dateTimeString, "%d-%02d-%02d %02d:%02d:%02d.%03d",
		&systime.wYear,
		&systime.wMonth,
		&systime.wDay,
		&systime.wHour,
		&systime.wMinute,
		&systime.wSecond,
		&systime.wMilliseconds);
	return systime;
}

int main()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == SOCKET_ERROR) {
		cout << "Errore WSAStartup()" << endl;
	}

	//si crea una socket TCP che utilizza IPv4 come indirizzamento

	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		cout << "Errore creazione socket: " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN client;
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	client.sin_port = htons(27000);

	//con il metodo connect il client richiede una connessione al server

	if (connect(ConnectSocket, (SOCKADDR*)&client, sizeof(client)) == SOCKET_ERROR) {
		cout << "Errore di connessione" << endl;
		WSACleanup();
		system("Pause");
		return 0;
	}
	//Se il server accetta la connessione, il client può iniziare a mandare messaggi
	cout << "Client connesso al server" << endl;

	cout << "Indirizzo Ip del Server : " << inet_ntoa(client.sin_addr) << "\n";
	cout << "Porta del server : " << htons(27000) << "\n";

	int result = 0;
	
	SYSTEMTIME localTime;

	GetSystemTime(&localTime);
	string str = getDate(localTime);
	
	cout << str.c_str() << endl;
	result = send(ConnectSocket, str.c_str() , sizeof(string), 0); //Mando la data corrente
	if (result == 0) {
		cout << "Errore nell'invio della data corrente" << endl;
		return 1;
	}
	
	char* buffer = new char[sizeof(SYSTEMTIME)];
	result = recv(ConnectSocket, buffer, sizeof(string), 0);
	if (result == 0) {
		cout << "Errore nell'invio della data corrente" << endl;
		return 1;
	}
	SYSTEMTIME serverTime = strToSysTime(buffer);

	//Imposto la data ricevuta dal server
	SetSystemTime(&serverTime);
	
	closesocket(ConnectSocket);

	system("pause");

}
