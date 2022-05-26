#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir
{
	char *name;
	struct Dir *parent;
	struct File *head_children_files;
	struct Dir *head_children_dirs;
	struct Dir *next;
} Dir;

typedef struct File
{
	char *name;
	struct Dir *parent;
	struct File *next;
} File;

void touch(Dir *parent, char *name)
{
	// initializeaza fisiserul nou
	File *newFile = malloc(sizeof(File));
	newFile->name = malloc(strlen(name) + 1);
	strcpy(newFile->name, name);
	newFile->next = NULL;
	newFile->parent = parent;
	// verifica daca exista alte fisiere in directorul parinte
	if (parent->head_children_files == NULL)
	{
		parent->head_children_files = newFile;
	}
	else
	{
		File *currentFile = parent->head_children_files;
		while (currentFile->next != NULL)
		{
			// verifica daca exista deja un fisier cu numele respectiv
			if (strcmp(name, currentFile->name) == 0)
			{
				printf("File already exists\n");
				return;
			}
			currentFile = currentFile->next;
		}
		if (strcmp(name, currentFile->name) == 0)
		{
			printf("File already exists\n");
			free(newFile->name);
			free(newFile);
			return;
		}
		// adauga fisierul la finalul listei de fisiere
		currentFile->next = newFile;
	}
}

void mkdir(Dir *parent, char *name)
{
	// initializeaza directorul nou
	Dir *newDir = malloc(sizeof(Dir));
	newDir->name = malloc(strlen(name) + 1);
	strcpy(newDir->name, name);
	newDir->next = NULL;
	newDir->head_children_dirs = NULL;
	newDir->head_children_files = NULL;
	newDir->parent = parent;
	// verifica daca exista alte directoare in directorul parinte
	if (parent->head_children_dirs == NULL)
	{
		parent->head_children_dirs = newDir;
	}
	else
	{
		Dir *currentDir = parent->head_children_dirs;
		while (currentDir->next != NULL)
		{
			// verifica daca exista deja un director cu numele respectiv
			if (strcmp(name, currentDir->name) == 0)
			{
				printf("Directory already exists\n");
				return;
			}
			currentDir = currentDir->next;
		}
		if (strcmp(name, currentDir->name) == 0)
		{
			printf("Directory already exists\n");
			free(newDir->name);
			free(newDir);
			return;
		}
		// adauga directorul nou la finalul listei de directoare
		currentDir->next = newDir;
	}
}

void ls(Dir *parent)
{
	Dir *currentDir = parent->head_children_dirs;
	// afiseaza toate directoarele prezente in directorul parinte
	while (currentDir != NULL)
	{
		printf("%s\n", currentDir->name);
		currentDir = currentDir->next;
	}
	File *currentFile = parent->head_children_files;
	// afiseaza toate fisiserele prezente in fisierul parinte
	while (currentFile != NULL)
	{
		printf("%s\n", currentFile->name);
		currentFile = currentFile->next;
	}
}

void rm(Dir *parent, char *name)
{
	// verifica daca exista fisiere in directorul parinte
	if (parent->head_children_files == NULL)
	{
		printf("Could not find the file\n");
		return;
	}
	File *tempFile;
	// sterge fisierul daca acesta se gaaseste la inceputul listei de fisiere
	if (strcmp(parent->head_children_files->name, name) == 0)
	{
		tempFile = parent->head_children_files;
		parent->head_children_files = parent->head_children_files->next;
		free(tempFile->name);
		free(tempFile);
		return;
	}
	else
	{
		File *currentFile = parent->head_children_files;
		while (currentFile->next != NULL)
		{
			// sterge fisierul daca acesta se afla in interiorul listei de fisiere
			if (strcmp(currentFile->next->name, name) == 0)
			{
				tempFile = currentFile->next;
				currentFile->next = currentFile->next->next;
				free(tempFile->name);
				free(tempFile);
				return;
			}
			else
			{
				currentFile = currentFile->next;
			}
		}
	}
	printf("Could not find the file\n");
}

// functie auxiliara care sterge toate fisierele dintr-un director
void rmFiles(Dir *parent)
{
	File *tempFile;
	while (parent->head_children_files != NULL)
	{
		tempFile = parent->head_children_files;
		parent->head_children_files = parent->head_children_files->next;
		free(tempFile->name);
		free(tempFile);
	}
}

// functie auxiliara recursiva care sterge toate directoarele si continutul acestora din interiorul unui director
void rmDirs(Dir *parent)
{
	Dir *tempDir;
	if (parent->head_children_dirs == NULL)
	{
		return;
	}
	while (parent->head_children_dirs != NULL)
	{
		rmFiles(parent->head_children_dirs);
		rmDirs(parent->head_children_dirs);
		tempDir = parent->head_children_dirs;
		parent->head_children_dirs = parent->head_children_dirs->next;
		free(tempDir->name);
		free(tempDir);
	}
}

void rmdir(Dir *parent, char *name)
{
	// verifica daca exista directoare in directorul parinte
	if (parent->head_children_dirs == NULL)
	{
		printf("Could not find the dir\n");
		return;
	}
	Dir *tempDir;
	// sterge directorul si continutul acestuia daca acesta se afla la inceputul listei de directoare
	if (strcmp(parent->head_children_dirs->name, name) == 0)
	{
		rmFiles(parent->head_children_dirs);
		rmDirs(parent->head_children_dirs);
		tempDir = parent->head_children_dirs;
		parent->head_children_dirs = parent->head_children_dirs->next;
		free(tempDir->name);
		free(tempDir);
		return;
	}
	else
	{
		Dir *currentDir = parent->head_children_dirs;
		while (currentDir->next != NULL)
		{
			// sterge directorul si continutul acestuia daca il gaseste in lista de directoare
			if (strcmp(currentDir->next->name, name) == 0)
			{
				rmFiles(currentDir->next);
				rmDirs(currentDir->next);
				tempDir = currentDir->next;
				currentDir->next = currentDir->next->next;
				free(tempDir->name);
				free(tempDir);
				return;
			}
			else
			{
				currentDir = currentDir->next;
			}
		}
	}
	printf("Could not find the dir\n");
}

void cd(Dir **target, char *name)
{
	// schimba directorul curent in directorul parinte
	if (strcmp(name, "..") == 0)
	{
		if ((*target)->parent == NULL)
		{
			return;
		}
		else
		{
			*target = (*target)->parent;
			return;
		}
	}
	else
	{
		// schimba directorul in primul director din fisierul parinte daca aceesta are numele cautat
		if (strcmp((*target)->head_children_dirs->name, name) == 0)
		{
			*target = (*target)->head_children_dirs;
			return;
		}
		else
		{
			Dir *currentDir = (*target)->head_children_dirs;
			while (currentDir->next != NULL)
			{
				// schimba directorul in directorul cu numele dat, daca acesta exista
				if (strcmp(currentDir->next->name, name) == 0)
				{
					*target = currentDir->next;
					return;
				}
				currentDir = currentDir->next;
			}
			printf("No directories found!\n");
		}
	}
}

char *pwd(Dir *target)
{
	char *path = NULL;
	if (target->parent == NULL)
	{
		// adauga numele primului director din cale in string
		path = realloc(path, strlen(target->name) + 2);
		strcpy(path, "/");
		strcat(path, target->name);
		return path;
	}
	else
	{
		// adauga in string fiecare directo din cale in mod recursiv
		path = pwd(target->parent);
		path = realloc(path, strlen(path) + strlen(target->name) + 2);
		strcat(path, "/");
		strcat(path, target->name);
		return path;
	}
}

void stop(Dir *target)
{
	// dezaloca directoarele si fisierele
	rmDirs(target);
	rmFiles(target);
	free(target->name);
	free(target);
	// inchide programul
	exit(EXIT_SUCCESS);
}

void tree(Dir *target, int level)
{
	// daca nu exista directoare afiseaza fisierele
	if (target->head_children_dirs == NULL)
	{
		if (target->head_children_files != NULL)
		{
			File *currentFile = target->head_children_files;
			while (currentFile != NULL)
			{
				for (int i = 0; i < level; i++)
				{
					printf("    ");
				}
				printf("%s\n", currentFile->name);
				currentFile = currentFile->next;
			}
			return;
		}
		else
			return;
	}
	else
	{
		// afiseaza recursiv toate directoarele si continutul acestora
		Dir *currentDir = target->head_children_dirs;
		while (currentDir != NULL)
		{
			for (int i = 0; i < level; i++)
			{
				printf("    ");
			}
			printf("%s\n", currentDir->name);
			tree(currentDir, level + 1);
			currentDir = currentDir->next;
		}
		File *currentFile = target->head_children_files;
		while (currentFile != NULL)
		{
			for (int i = 0; i < level; i++)
			{
				printf("    ");
			}
			printf("%s\n", currentFile->name);
			currentFile = currentFile->next;
		}
		return;
	}
}

void mv(Dir *parent, char *oldname, char *newname)
{
	if (parent->head_children_dirs == NULL && parent->head_children_files == NULL)
	{
		return;
	}
	File *currentFile = parent->head_children_files;
	while (currentFile != NULL)
	{
		// verifica daca exista deja un fisier cu numele pe care vrem sa il dam
		if (strcmp(currentFile->name, newname) == 0)
		{
			printf("File/Director already exists\n");
			return;
		}
		currentFile = currentFile->next;
	}
	Dir *currentDir = parent->head_children_dirs;
	while (currentDir != NULL)
	{
		// verifica daca exista deja un director cu numele pe care vrem sa il dam
		if (strcmp(currentDir->name, newname) == 0)
		{
			printf("File/Director already exists\n");
			return;
		}
		currentDir = currentDir->next;
	}
	File *renamedFile;
	// cauta numele printre numele fisierelor si daca il gaseste il inlocuieste cu numele nou si il muta la finalul listei
	if (parent->head_children_files != NULL)
	{
		if (strcmp(parent->head_children_files->name, oldname) == 0)
		{
			if (parent->head_children_files->next == NULL)
			{
				parent->head_children_files->name = realloc(parent->head_children_files->name, strlen(newname) + 1);
				strcpy(parent->head_children_files->name, newname);
				return;
			}
			renamedFile = parent->head_children_files;
			parent->head_children_files = parent->head_children_files->next;
			renamedFile->name = realloc(renamedFile->name, strlen(newname) + 1);
			strcpy(renamedFile->name, newname);
			renamedFile->next = NULL;
			currentFile = parent->head_children_files;
			while (currentFile->next != NULL)
			{
				currentFile = currentFile->next;
			}
			currentFile->next = renamedFile;
			return;
		}
		else
		{
			currentFile = parent->head_children_files;
			while (currentFile->next != NULL)
			{
				if (strcmp(currentFile->next->name, oldname) == 0)
				{
					renamedFile = currentFile->next;
					currentFile->next = currentFile->next->next;
					renamedFile->name = realloc(renamedFile->name, strlen(newname) + 1);
					strcpy(renamedFile->name, newname);
					while (currentFile->next != NULL)
					{
						currentFile = currentFile->next;
					}
					currentFile->next = renamedFile;
					return;
				}
				else
				{
					currentFile = currentFile->next;
				}
			}
		}
	}
	Dir *renamedDir;
	// cauta numele printre directoare si daca il gaseste il redenumeste si il muta la finalul listei
	if (parent->head_children_dirs != NULL)
	{
		if (strcmp(parent->head_children_dirs->name, oldname) == 0)
		{
			if (parent->head_children_dirs->next == NULL)
			{
				parent->head_children_dirs->name = realloc(parent->head_children_dirs->name, strlen(newname) + 1);
				strcpy(parent->head_children_dirs->name, newname);
				return;
			}
			renamedDir = parent->head_children_dirs;
			parent->head_children_dirs = parent->head_children_dirs->next;
			renamedDir->name = realloc(renamedDir->name, strlen(newname) + 1);
			strcpy(renamedDir->name, newname);
			renamedDir->next = NULL;
			currentDir = parent->head_children_dirs;
			while (currentDir->next != NULL)
			{
				currentDir = currentDir->next;
			}
			currentDir->next = renamedDir;
			return;
		}
		else
		{
			currentDir = parent->head_children_dirs;
			while (currentDir->next != NULL)
			{
				if (strcmp(currentDir->name, oldname) == 0)
				{
					renamedDir = currentDir->next;
					currentDir->next = currentDir->next->next;
					renamedDir->name = realloc(renamedDir->name, strlen(newname) + 1);
					strcpy(renamedDir->name, newname);
					while (currentDir->next != NULL)
					{
						currentDir = currentDir->next;
					}
					currentDir->next = renamedDir;
					return;
				}
				else
				{
					currentDir = currentDir->next;
				}
			}
		}
	}
	printf("File/Director not found\n");
}

int main()
{
	char *line = malloc(MAX_INPUT_LINE_SIZE);
	// defineste directorul home
	Dir *home = malloc(sizeof(Dir));
	home->name = malloc(strlen("home") + 1);
	home->parent = NULL;
	home->head_children_dirs = NULL;
	home->head_children_files = NULL;
	strcpy(home->name, "home");
	Dir *currentDir = home;
	char *command;
	do
	{
		// citeste fiecare linie si o imparte in comanda si nume daca exista
		// executa comenzile citite
		fgets(line, MAX_INPUT_LINE_SIZE, stdin);
		command = strtok(line, " \n");
		char *token = strtok(NULL, " \n");
		if (token != NULL)
		{
			char *name = malloc(strlen(token) + 1);
			strcpy(name, token);
			if (strcmp(command, "touch") == 0)
			{
				touch(currentDir, name);
			}
			if (strcmp(command, "mkdir") == 0)
			{
				mkdir(currentDir, name);
			}
			if (strcmp(command, "rm") == 0)
			{
				rm(currentDir, name);
			}
			if (strcmp(command, "rmdir") == 0)
			{
				rmdir(currentDir, name);
			}
			if (strcmp(command, "cd") == 0)
			{
				cd(&currentDir, name);
			}
			if (strcmp(command, "mv") == 0)
			{
				char *oldname = malloc(strlen(token) + 1);
				strcpy(oldname, token);
				token = strtok(NULL, " \n");
				char *newname = malloc(strlen(token) + 1);
				strcpy(newname, token);
				mv(currentDir, oldname, newname);
				free(oldname);
				free(newname);
			}
			free(name);
		}
		if (strcmp(command, "ls") == 0)
		{
			ls(currentDir);
		}
		if (strcmp(command, "tree") == 0)
		{
			tree(currentDir, 0);
		}
		if (strcmp(command, "pwd") == 0)
		{
			char *path = pwd(currentDir);
			puts(path);
			free(path);
		}
		if (strcmp(command, "stop") == 0)
		{
			free(line);
			free(token);
			stop(home);
		}
	} while (1);
	return 0;
}