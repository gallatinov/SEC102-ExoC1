#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>


/* \brief This is the main function
* \param _In_ argc : Number of parameters passed to the main function
* \param _In_ argv : List of all parameters of type char*
* \return ERROR_SUCCESS, or failure otherwise
* \remark
*/



bool ListDirectoryContents(const char *sDir)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	char sPath[1024];

	//On définit un mask, ici on veut tout voir
	sprintf(sPath, "%s\\*.*", sDir);
	//On vérifie que le Handle renvois un code valide
	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		printf("Erreur : [%s]\n", sDir);
		return false;
	}

	do
	{

		// On compare nos trouvailles à "." et ".." afin de ne pas les explorer
		if (strcmp(fdFile.cFileName, ".") != 0
			&& strcmp(fdFile.cFileName, "..") != 0)
		{
			// l'API win32 demande des chemins avec \\ entre chaque dossier.
			sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);
			printf("________\n");
			//On vérifie si c'est un fichier ou un dossier.
			if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
			{

				printf("Directory: %s\n", sPath);
				attributefile(fdFile, sPath);
				// Si l'objet est un dossier alors on réutilise la fonction pour l'explorer
				ListDirectoryContents(sPath);
			}
			else {
				printf("File: %s\n", sPath);
				attributefile(fdFile, sPath);
			}

		}
	} while (FindNextFile(hFind, &fdFile));
	// On nettoie le handler.
	FindClose(hFind);

	return true;
}

int attributefile(WIN32_FIND_DATA File)
{
	// On va lire la taille du fichier
	printf("->	Taille : %u octets\n", File.nFileSizeLow);
	// récupération de la timezone configuré dans le windows.
	TIME_ZONE_INFORMATION tzi;
	DWORD dwRet;
	dwRet = GetTimeZoneInformation(&tzi);
	//On définit le temps du système et le temps du système avec timezone configuré.
	SYSTEMTIME wintime, lwintime;
	// Récupération de la date et l'heure de création du fichier
	FileTimeToSystemTime(&File.ftCreationTime, &wintime);
	SystemTimeToTzSpecificLocalTime(&tzi, &wintime, &lwintime);
	printf("->	Date de creation: %02d-%02d-%d %02d:%02d\n", lwintime.wDay, lwintime.wMonth, lwintime.wYear, lwintime.wHour, lwintime.wMinute);
	// Récupération de la date et l'heure de dernier acces du fichier
	FileTimeToSystemTime(&File.ftLastAccessTime, &wintime);
	SystemTimeToTzSpecificLocalTime(&tzi, &wintime, &lwintime);
	printf("->	Dernier acces: %02d-%02d-%d %02d:%02d\n", lwintime.wDay, lwintime.wMonth, lwintime.wYear, lwintime.wHour, lwintime.wMinute);
	// Récupération de la date et l'heure de la dernière modification du fichier
	FileTimeToSystemTime(&File.ftLastWriteTime, &wintime);
	SystemTimeToTzSpecificLocalTime(&tzi, &wintime, &lwintime);
	printf("->	Derniere modification: %02d-%02d-%d %02d:%02d\n", lwintime.wDay, lwintime.wMonth, lwintime.wYear, lwintime.wHour, lwintime.wMinute);

	// On vérifie si le fichier est caché, système ou lecture seule.
	if (File.dwFileAttributes &FILE_ATTRIBUTE_HIDDEN)
	{
		printf("Fichier cache\n");
	}
	if (File.dwFileAttributes &FILE_ATTRIBUTE_READONLY)
	{
		printf("Fichier en lecture seule\n");
	}
	if (File.dwFileAttributes &FILE_ATTRIBUTE_SYSTEM)
	{
		printf("Fichier systeme\n");
	}
}


int main(int argc, char **argv)
{
	// Vérification basique du nombre d'argument.
	if (argc == 2)
	{

		ListDirectoryContents(argv[1]);
	}
	else
	{
		printf("Problème d'argument");
	}
}
