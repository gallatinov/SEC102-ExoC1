
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>

//Une fonction qui affiche les informations relatives aux modules.
int PrintModules(DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	DWORD cProcess;
	unsigned int i;

// Mis en place d'un handle.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return 1;

//Création d'un liste des modules
	printf("Chemin du process et de ses modules : \n");
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
// Pour connaitre le nombe de process on divise la taille du tableau par la taille d'un module.
		cProcess = cbNeeded / sizeof(HMODULE);
		for (i = 0; i < cProcess; i++)
		{
			TCHAR szModName[MAX_PATH];

//On récupère le chemin du module.
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{

				_tprintf(TEXT("\t%s -- Adresse memoire[0x%08X] -- Taille du module : %lu\n"), szModName, hMods[i],sizeof(hMods[i]));
			}
		}
	}
	printf("_____________________\n");

	CloseHandle(hProcess);

	return 0;
}

void PrintProcessNameAndID(DWORD processID)
{
// on initialise le nom du process sur inconnu si szProcessName ne retourne rien de mieux.
	TCHAR szProcessName[MAX_PATH] = TEXT("<non reconnu>");
// Mis en place d'un handle.
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

// on récupère le nom du processus.
	if (NULL != hProcess)
	{
		HMODULE hMods;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMods, sizeof(hMods),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMods, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	_tprintf(TEXT("Nom : %s  \n"), szProcessName);

	CloseHandle(hProcess);
}


int main(void)
{

	DWORD aProcesses[1024];
	DWORD cbNeeded;
	DWORD cProcess;
	unsigned int i;
// On récupère la liste des paramètres dun processus.
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return 1;

	cProcess = cbNeeded / sizeof(DWORD);
//pour chaque processus on lance nos 2 fonctions.
	for (i = 0; i < cProcess; i++)
	{
		PrintProcessNameAndID(aProcesses[i]);
		PrintModules(aProcesses[i]);
	}

	return 0;
}
