#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

const char USAGE[] = "USAGE: srm [<list-of-files>]";
const char ERR_MSG[] = "If you ever see this message, this means my code fails in some situations.\n";

int main(int argc, char *argv[])
{
	// Check if no arg
	if (argc == 1)
	{
		fprintf(stderr, "No arguments.\n%s\n", USAGE);
		exit(1);
	}

	// Create trashlog if it does not exist
	char *trash = strcat(getenv("HOME"), "/trash_log");
	int fd = open(trash, O_RDWR | O_CREAT, 0666);
	close(fd);
//	free(trash);

	// Loop through the args
	for (int i = 1; i < argc; ++i)
	{
		// Check if . or ..
		if (!strcmp(argv[i], ".") || !strcmp(argv[i], ".."))
		{
			fprintf(stderr, "Attempted to srm current/parent directory.\n%s\n", USAGE);
			exit(1);
		}
		
		// Check if file exists
		// IF it doesn't, just move onto the next arg
		int test_fd;
		if ((test_fd = open(argv[i], O_RDONLY, 0666)) == -1)
		{
			fprintf(stderr, "File: %s does not exist.\n", argv[i]);
			continue;
		}
		close(test_fd);

		char file_path[4096] = {'\0'};

		if ((*argv[i]) == '/' || (*argv[i]) == '~')
			printf("Note: You entered the absolute filepath.\n");
		else
		{
			char *working_dir = NULL;
			printf("Note: You entered the relative filepath.\n");
			if (!(working_dir = getcwd(NULL, 4096)))
			{
				fprintf(stderr, "Could not get working directory.\n%s\n", ERR_MSG);
				exit(1);
			}
			strcat(file_path, working_dir);
			strcat(file_path, "/");
			free(working_dir);
		}
		
		char temp[4096];
		strcpy(temp, argv[i]);
		char *dir_name = dirname(temp);
		char *base_name = basename(argv[i]);	

		strcat(file_path, dir_name);
		strcat(file_path, "/");

		int fd;
		if ((fd = open(trash, O_WRONLY | O_APPEND, 0666)) == -1)
		{
			fprintf(stderr, "Could not open trashlog.\n%s\n", ERR_MSG);
			exit(1);
		} 

//		printf("Basename: %s\nDirname: %s\nFilepath: %s\n", base_name, dir_name, file_path);
		char buf[4096] = {'\0'};
		int stop = 0;

		strcat(buf, base_name);
		stop += strlen(base_name);

		DIR *dir = opendir(base_name);
		if (dir)
		{
			strcat(buf, " -d ");
			closedir(dir);
		}
		else
			strcat(buf, " -f ");
		stop += 4;
		
		strcat(buf, file_path);
		strcat(buf, "\n");
		stop += strlen(file_path) + 1;
		buf[stop] = '\0';

		if (write(fd, buf, stop) == -1)
		{
			fprintf(stderr, "Write error.\n%s\n", ERR_MSG);
			exit(1);
		}
		close(fd);

		char old_path[4096] = {'\0'};
		char new_path[4096] = {'\0'};

		strcat(strcat(old_path, file_path), base_name);
		strcat(strcat(strcat(new_path, file_path), "."), base_name);

	//	printf("old: %s\nnew: %s\n", old_path, new_path);
		rename(old_path, new_path);
	//	free(dir_name);
	//	free(base_name);
	}

	return 0;
}
