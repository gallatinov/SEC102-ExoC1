#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include <strsafe.h>

// On d�finit la taille de nos chunks et da longueur de l'IP.
#define IP_LENGTH 100
#define CHUNK_SIZE 10000

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")

//Fonction d'usage donnant l'utilisation du binaire.
void Usage(Void)
{
	printf("Usage:sock.exe <host> <port> <url> <file name>\n");
}


// Fonction bool��nne qui cr�� un fichier et qui �crit dedans.
BOOL Save(BYTE *lpBuffer, DWORD cbBuffer, TCHAR *szFileName)
{
	BOOL bResult = TRUE;
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	//On g�re les erreurs du handle.
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Ouverture du fichier impossible : %d\n", GetLastError());
		bResult = FALSE;
		goto end;
	}

	DWORD cbWritten;
	if (!WriteFile(hFile, lpBuffer, cbBuffer, &cbWritten, NULL))
	{
		printf("Code d'erreur : %d\n", GetLastError());
		bResult = FALSE;
	}

	CloseHandle(hFile);

end: return bResult;
}

int main(int argc, char **argv)
{
	WSADATA WSAData;
	SOCKET sock;
	SOCKADDR_IN sin;
	char buffer[2048];
	TCHAR *szRequestFormat = "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", *szRequest = NULL;
	DWORD cbSent, cbReceived, cbTotalReceived = 0, cbAvailable = CHUNK_SIZE, i = 1;
	DWORD Port = StrToInt(argv[2]);
	BYTE *pBuffer;
	DWORD cbBufferSize;
	TCHAR aIp[IP_LENGTH], aResponseBuffer[CHUNK_SIZE];
	BYTE *lpResponse = NULL, *lpResponseRealloc = NULL;


	if (argc < 5)
	{
		Usage(argv[0]);
		return 0;
	}

	// initialisation du DLL winsock
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	//configuration du socket.
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Port);
	
	//Connection du socket si le code retourner est "-n" alors on affiche l'erreur sinon la connection tcp a �t� r�ussi.
	if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("Impossible de se connecer � l'IP %s sur le port %s\n", argv[1], argv[2]);
		return 0;
	}
	printf("Connect� � l'IP %s sur le port %s\n", argv[1], argv[2]);



	// Allocation et Envoi de la requete GET
	szRequest = malloc((strlen(szRequestFormat) + strlen(argv[3]) + strlen(argv[1])) * sizeof(TCHAR));
	printf(szRequest, szRequestFormat, argv[3], argv[1]);
	printf("Envoi HTTP message : \n\n%s\n", szRequest);
	cbSent = send(sock, szRequest, strlen(szRequest), 0);

	// V�rification du retour de la requete , voir si la socket a �t� d�connect�e.
	if (cbSent == SOCKET_ERROR)
	{
		printf("Probl�me pour envoyer la requ�te � : %s, code d'erreur : %d\n", argv[3], WSAGetLastError());
	}
	else
	{
		printf("Envoy�s %d octets\n", cbSent);
	}

	//Sur l'ensemble de la m�moire pr�vu on alloue une partie 
	lpResponse = malloc(CHUNK_SIZE * sizeof(TCHAR));
	// Initialisation d'une r�ponse vide
	StringCchCopy(lpResponse, 1, "");
	do 
	{ 
		// Chaque r�ponse re�u par la socket est entr� dans un buffeur
		cbReceived = recv(sock, aResponseBuffer, CHUNK_SIZE, 0);
		if (cbReceived > 0)
		{
			// On increment la taille total d'octet re�u et on decr�mente la taille totale disponible.
			cbTotalReceived += cbReceived;
			cbAvailable -= cbReceived;

			printf("Re�u %d octets\n", cbReceived);

			// On ajoute de la m�moire disponible si besoin.
			if (cbAvailable < CHUNK_SIZE && cbReceived == CHUNK_SIZE)
			{
				lpResponseRealloc = realloc(lpResponse, CHUNK_SIZE * ++i);
				if (lpResponseRealloc != NULL)
				{
					lpResponse = lpResponseRealloc;
					cbAvailable += CHUNK_SIZE;
				}
				else
				{
					printf("Probl�me d'allocation m�moire...\n");
					
				}
			}
			//On dump le buffer dans un tableau
			StringCchCat(lpResponse, cbTotalReceived, aResponseBuffer);
		}
		else if (cbReceived == 0)
		{
			printf("Connexion ferm�\n");
		}
		else
		{
			printf("Erreur de r�ception : %d\n", WSAGetLastError());
		}
	} while (cbReceived == CHUNK_SIZE);

	lpResponse[cbTotalReceived] = '\0';


	if (lpResponse != NULL)
	{
		printf("Ecriture des donn�es re�us dans %s\n", argv[4]);
		if (Save(lpResponse, cbTotalReceived, argv[4]))
		{
			printf("Ecriture finit de %s\n", argv[4]);
		}
		free(lpResponse);
	}


	WSACleanup();
	closesocket(sock);
	return 0;
}

