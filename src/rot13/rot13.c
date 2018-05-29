#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <tchar.h>
#include <windows.h>

// Fonction qui verifie si la chaine en argument est un fichier et si oui de quel type.
// On retourne le type de fichier.
int IsFile(CHAR *argum)
{
	int exttype;
	CHAR * point ;
	//On cherche un point dans le nom de fichier et la chaine de l'extension derriere
	point = strrchr(argum, '.');
	if ((point != NULL) && (strcmp(point, ".txt") == 0))
	{
			exttype = 1;
			return exttype;
	}

	else if ((point != NULL) && (strcmp(point, ".rot13") == 0))
	{
			exttype = 2;
			return exttype;
	}
	else
	{
		exttype = 0;
		return exttype;
	}
	return 0;
}


char * encode13string(CHAR stringencode[])
{
	TCHAR cipher;
	TCHAR *strot13;
	//  on allouye de la m�moire de l chaine en entr�e +1 pour le \0 qui m'a pos� de nombreux probl�mes!
	strot13 = malloc(strlen(stringencode)+1);
	DWORD  key = 13;

	// On boucle entre le caract�re 0 et le dernier caract�re de notre chaine. 
	for (int n = 0;  n < strlen(stringencode); n++) 
	{
		// On cherche les caract�res en a et z pui s entre A et Z de la table ascii.
		TCHAR currentLetter = stringencode[n];
		if (currentLetter >='a' && currentLetter <='z')
		{
			strot13[n] = 'a' + ((currentLetter - 'a') + key) % 26;
		}
		else if (currentLetter >= 'A' && currentLetter <= 'Z')
		{
			strot13[n] = 'A' + ((currentLetter - 'A') + key) % 26;
		}

		else
		{
			strot13[n] = currentLetter;
		}
	}
	strot13[strlen(stringencode)] = '\0';
	return strot13;
}


bool encode13file(CHAR *filesrc, DWORD type)
{
	BOOL bSuccess = TRUE;
	CHAR buff[1024], buff13[1024];
	// Ici on remplit le buff de \0 afin de ne garder que les caract�res lut dans le fichier.
	memset(buff, '\0', sizeof(buff));
	
	DWORD dwBytesRead, dwBytesWritten;
	HANDLE source, target;
	CHAR *filetrg ;

	// On cr�er un handle pour le fichier source
	source = CreateFile(filesrc, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	// V�rification des erreurs
	if (source == INVALID_HANDLE_VALUE) {
		printf("Pb fichier source.");
		exit(EXIT_FAILURE);
	}
	else printf("Le Fichier source est : %s\n", filesrc);

	if (type == 1)
	{
		//filename est la variable du nom de fichier renomm�. ici en ".rot13" On r�alloue la taille de cette variable en cons�quence.
		filetrg = malloc(strlen(filesrc) + 2);
		strcpy(filetrg, filesrc);
		filetrg[strlen(filesrc) + 2 ] = '\0';
		filetrg[strlen(filesrc) + 1] = '3';
		filetrg[strlen(filesrc) ] = '1';
		filetrg[strlen(filesrc) - 1] = 't';
		filetrg[strlen(filesrc) - 2] = 'o';
		filetrg[strlen(filesrc) - 3] = 'r';
		//Cr�ation du fichier cible
		target = CreateFile(filetrg, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	else if (type == 2)
	{
		//filename est la variable du nom de fichier renomm�. ici en ".txt" On r�alloue la taille de cette variable en cons�quence.
		filetrg = malloc(strlen(filesrc) - 2);
		strcpy(filetrg, filesrc);
		filetrg[strlen(filesrc) -2] = '\0';
		filetrg[strlen(filesrc) -3] = 't';
		filetrg[strlen(filesrc) -4] = 'x';
		filetrg[strlen(filesrc) -5] = 't';
		//Cr�ation du fichier cible
		target = CreateFile(filetrg, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	else
	{
		printf("Erreur lors de la cr�ation du fichier\n");
	}

		if (target == INVALID_HANDLE_VALUE) 
		{
			printf("Pb � la cr�ation du fichier cible.");
			exit(EXIT_FAILURE);
		}
		else printf("Fichier cible est : %s\n", filetrg);
		// On lie le fichier source et on charge le buff avec les donn�es lues.
		ReadFile(source, buff, 1023, &dwBytesRead, NULL);
		// On d�finit la quantit� de donn�e � �crire dans le fichier cible et on encode le contenu de "buff" dans "buff13"
		DWORD dwBytesToWrite = (DWORD)strlen(buff);
		strcpy(buff13, encode13string(buff));
		//On �crit buff13 dans le fichier cible.
		WriteFile(target, buff13, dwBytesToWrite, &dwBytesWritten, NULL);
		//On lib�re les handle.
		CloseHandle(source);
		CloseHandle(target);
}


int main(int argc, char** argv)
{
	
	if (argc == 2)
	{
		DWORD argtype;
		CHAR *string13;
		argtype = IsFile(argv[1]);
		if (argtype > 0)
		{
			printf("Encodage ou d�codage en ROT13 du fichier : %s\n", argv[1]);
			if (encode13file(argv[1],argtype))
			{
				printf("\n\n Traitement finit\n");
			}
			else
			{
				printf("Erreur\n");
			}
		}
		else
		{
			printf("Encodage ou d�codage en ROT13de la chaine : %s\n", argv[1]);
			string13 = encode13string(argv[1]);
			printf("\n\n Traitement finit :\n%s\n", string13);
		}
	}
	else
	{
		printf("Usage : %s <chaine ou fichier a encoder ou d�coder>", argv[0]);
		printf("Usage :fichier dans le dossier executable de 1024 caract�res max.");
	}

	printf("\n");
	return 0;
}