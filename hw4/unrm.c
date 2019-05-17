#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

const char USAGE[] = "USAGE: unrm [<list-of-files>]";
const char ERR_MSG[] = "If you ever see this message, this means my code fails in some situations.\n";

// Return line num if pattern is found in filename, -1 otherwise
int my_grep(char *pattern, char *filename, char *buffer)
{
	// Could not find a way to read a file line by line without using fopen and FILE*.
	FILE *file = fopen(filename, "r");
	char *line = NULL;
	size_t len;
	char *pat = strcat(pattern, " -");
	int line_num = 0;

	if (!file)
	{
		fprintf(stderr, "File %s does not exist\n%s\n", filename, ERR_MSG);
		return -1;
	}

	while (getline(&line, &len, file) != -1)
	{
		if (strstr(line, pat))
		{
			strcpy(buffer, line);
			fclose(file);
			free(line);
			return line_num;
		}
		++line_num;
	}
	
	fclose(file);
	free(line);

	return -1;
}

// Remove the file to be unrm'd from trash_log
int remove_from_trash(int line_num)
{
	char *temp_trash = "temp_trash";
	char trash[4096];
	strcpy(trash, getenv("HOME"));
	strcat(trash, "/trash_log");
	char *line = NULL;
	size_t len;
	int num = 0;
	char ch;
	FILE *file = fopen(trash, "r"), *temp = fopen(temp_trash, "w");

	if (!file)
	{
		fprintf(stderr, "File does not exist.\n%s\n", ERR_MSG);
		return -1;
	}

	while ((ch = getc(file)) != EOF)
	{
		if (ch == '\n')
			++num;

		if (num != line_num)
			putc(ch, temp);
	}

	fclose(file);
	fclose(temp);
	unlink(trash);
	rename(temp_trash, trash);
	return 1;	
}

int main(int argc, char *argv[])
{
	// Check if no arg
	if (argc == 1)
	{
		fprintf(stderr, "No arguments.\n%s\n", USAGE);
		exit(1);
	}

	// Check if ~/trash_log	exists
	char trash[4096];
	strcpy(trash, getenv("HOME"));
	strcat(trash, "/trash_log");
	int fd;
	if ((fd = open(trash, O_RDWR, 0666)) == -1)
	{
		fprintf(stderr, "Error: Trashbin does not exist. Please call 'srm' first before calling 'unrm' or 'trash'.\n");
		exit(1);
	}
	
	char check_file_content[2] = {'\0'};
	// Check if trash_log is empty
	if (read(fd, check_file_content, 1) == -1)
	{
		fprintf(stderr, "Error: Trashbin is empty.\n");
		exit(1);
	}
	close(fd);

	for (int i = 1; i < argc; ++i)
	{
		char buf[4096] = {'\0'};
		char *base_name = basename(argv[i]);
		int line_num = -1;
		char temp[4096];
		strcpy(temp, argv[i]);

		// Check if file is in trash_log
		if ((line_num = my_grep(temp, trash, buf)) == -1)
		{
			fprintf(stderr, "Error: File %s not found in trash_log\n", base_name);
			continue;
		}

//		printf("%s\n", buf);
		char *file_path = strrchr(buf, ' ');
		*(file_path + strlen(file_path) - 1) = '\0';
//		printf("filepath %s\n", file_path);

		// Check if the path is valid (a directory)
/*
		DIR * dir = opendir(file_path);
		if (dir) closedir(dir);
		else if (ENOENT == errno)
		{
			fprintf(stderr, "Could not find correct filepath.\nThe only possibility for this is if you srm\'d a directory that %s belonged to after you srm\'d %s\n. Unfortunately, this implementation does not support this case.\n", base_name, base_name);
			continue;
		}
		else
		{
			fprintf(stderr, "%s\n", ERR_MSG);
			exit(1);
		}
*/
		char hidden_name[4096] = {'\0'};
		strcat(hidden_name, ".");
		strcat(hidden_name, base_name);
//		printf("hidden: %s\n", hidden_name);
		
		char old_path[4096] = {'\0'};
		char new_path[4096] = {'\0'};

		strcat(strcat(old_path, file_path), hidden_name);
		strcat(strcat(new_path, file_path), base_name);

//		printf("old: %s\nnew: %s\n", old_path, new_path);
		if (remove_from_trash(line_num) == -1)
		{
			fprintf(stderr, "Could not edit trash_log.\n%s\n", ERR_MSG);
			exit(1);
		}

		rename(old_path+1, new_path+1);
	}
}
