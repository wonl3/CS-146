#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

void error(char *msg)
{
	printf("%s\n", msg);
	exit(1);
}

void get_shell_cmd(char commands[][BUFSIZ], int cores)
{
	char cmd[BUFSIZ];
	for (int i = 0; i < cores; ++i)
	{
		if (fgets(cmd, BUFSIZ, stdin))
			strcpy(commands[i], cmd);
		else
			commands[i][0] = '\0';

//		printf("cmd = %s\n", cmd);
	}
}

pid_t exec_cmd(char *args, char *shell)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		error("Fork Failed.");
	else if (pid == 0)
		if (execl(shell, shell, "-c", args, (char *) NULL) < 0)
			error("Exec Failed.");
	return pid;
}

int handle_wait(pid_t p, int cores)
{
	int count = 0;
	pid_t status, w;

	if (p)
		do
		{
			w = waitpid(p, &status, WUNTRACED);
			if (w == -1 || WIFSIGNALED(status))
				++count;
		} while (!WIFEXITED(status) && !WIFSIGNALED(status)); 
	return count;
}

int main(int argc, char *argv[], char *envp[])
{
	int cores, argv_i, shell_found;
	char shell[BUFSIZ];
	for (argv_i = 0; argv_i < argc; ++argv_i)
	{
		if ((argv[argv_i][0]) == '-')
			switch (argv[argv_i][1])
			{
				case 's':
					strcpy(shell, argv[argv_i+1]);
					shell_found = 1;
					break;
				default:
					error("Invalid Option.");
			}
			
			if (shell_found) break;
	}

	if (!shell_found)
	{
		strcpy(shell, getenv("SHELL"));
		if (argc == 1 || (cores = atoi(argv[1])) == 0) 
			cores = get_nprocs();
	}
	else
		if ((argv_i + 2 >= argc) || (cores = atoi(argv[argv_i + 2])) == 0)
			cores = get_nprocs();
	
	if (cores > get_nprocs())
		error("# of cores specified greater than available cores.");
	
//	printf("cores: %d, shell: %s\n", cores, shell);

	char commands[cores][BUFSIZ];
	int end = 0, mark = 0, count = 0;
	pid_t process[cores];

	while (!end)
	{
		get_shell_cmd(commands, cores);
		for (int i = 0; i < cores; ++i)
		{
			if (commands[i][0])
				process[i] = exec_cmd(commands[i], shell);
			else
			{
				end = 1;
				break;
			}
			++mark;
		}	
	
		for (int i = 0; i < mark; ++i)
			count += handle_wait(process[i], cores); 
	}
		
	return count;
}

