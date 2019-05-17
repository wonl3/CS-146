#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

const char USAGE[] = "USAGE: trash";
const char ERR_MSG[] = "If you ever see this message, this means my code fails in some situations.\n";

void remove_dir(char* path) {
    DIR* d;
    struct dirent *dir;
    struct stat stbuf;
    char file[4096] = {'\0'};
    
	strcpy(file, path);
    strcat(file, "/");

    d = opendir(file);
    if (d) {
        while ((dir = readdir(d)) != NULL) {

			// Check if self/parent directory
            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
                continue;

            strcat(file, dir->d_name);

			// Check type of file
            if (lstat(file, &stbuf) == -1)
			{
                fprintf(stderr, "%s\n", ERR_MSG);
				exit(1);
            }
			// If dir, recursion
			else if (S_ISDIR(stbuf.st_mode))
                remove_dir(file);
			else
			{ 
                if (unlink(file) == -1) 
				{
					fprintf(stderr, "%s\n", ERR_MSG);
					exit(1);
                }
            }
            strcpy(file, path);
            strcat(file, "/");
        }
        closedir(d);
        rmdir(file);
    }
}

int get_file_name(char *line, char *filename)
{
	for (int i = 0; *(line + i); ++i)
	{
		if ((*(line + i)) == ' ')
			return 1;
		*(filename + i) = *(line + i);
	}

	return -1;
}

int main(int argc, char *argv[])
{
	if (argc != 1)
	{
		fprintf(stderr, "Too many arguments.\n%s\n", USAGE);
		exit(1);
	}

	char *line = NULL;
	size_t len;
	char trash[4096];
	strcpy(trash, getenv("HOME"));
	strcat(trash, "/trash_log");

	FILE *file = fopen(trash, "r");

	while (getline(&line, &len, file) != -1)
	{
		int l = strlen(line);
		*(line + l - 1) = '\0';
//		printf("%d, %s\n", l, line);
		char path[4096] = {'\0'}, file_name[4096] = {'\0'};
		char *dir_name = strrchr(line, ' ');
		if (!dir_name) continue;
		strcat(path, dir_name+1);
		if (get_file_name(line, file_name) == -1)
		{
			fprintf(stderr, "%s\n", ERR_MSG);
			exit(1);
		}
		
		strcat(path, ".");
		strcat(path, file_name);
//		printf("hi: %s\n", path);
		struct stat path_stat;
		stat(path, &path_stat);
		if (S_ISREG(path_stat.st_mode))
			unlink(path);
		else
			remove_dir(path);
	}

	unlink(trash);
	
	return 0;
}
