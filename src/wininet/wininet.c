#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <shlwapi.h>


// On appel les librairies nécessaires
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Shlwapi.lib")

//Fonction d'usage donnant l'utilisation du binaire.
void Usage(Void)
{
	printf("Usage:wininet.exe <host> <port> <url>\n");
	printf("Usage: !! l'url doit commencer par http: ou https: ou ftp: \n");
}



int main(int argc, char **argv)
{
	DWORD Port = StrToInt(argv[2]);

	if (argc < 4)
	{
		Usage(argv[0]);
		return 0;
	}

	HINTERNET hIOpen = InternetOpen("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (hIOpen == INVALID_HANDLE_VALUE)
	{
		printf("Pas de connection internet\n");
	}

	HINTERNET hIConnect = InternetConnect(hIOpen, _T(""), Port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
	HINTERNET hIRequest = InternetOpenUrl(hIOpen, argv[3], NULL, 0, 0, 0);
	BYTE bBuffer[2048];
	DWORD dwBufferSize = sizeof(bBuffer), dwStatusCode = 0;

	if (hIRequest)
	{
		if (HttpQueryInfo(hIRequest, HTTP_QUERY_STATUS_CODE, &bBuffer, &dwBufferSize, NULL))
		{
			dwStatusCode = StrToInt(bBuffer);
		}
		else
		{
			printf("Pas de réponse du serveur, error code : %d\n", GetLastError());
		}
	}
	else
	{
		printf("Requète non envoyée, error code : %d\n", GetLastError());
	}

	InternetCloseHandle(hIRequest);
	InternetCloseHandle(hIConnect);
	InternetCloseHandle(hIOpen);

	printf("Réponse du serveur : %d\n", dwStatusCode);
	return 0;
}

