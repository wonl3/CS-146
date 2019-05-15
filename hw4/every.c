#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

const char USAGE[] = "USAGE: every [-N,M]  [<list-of-files>], where M <= N";

// Checks if the char * passed in has a contains a comma
// and if so, return the index. Else, return -1
int find_comma(char *str)
{
	for (int i = 0; *(str + i) != '\0'; ++i)
	{
		if (*(str + i) == ',')
			return i;
	}
	return -1;
}

void check_correct_syntax(char *option)
{
	// For checking that the system call follows the correct syntax
	char *correct_syntax_regex = "^-[1-9][0-9]*($|,[1-9][0-9]*$)";
	regex_t regex_compiled;

	if (regcomp(&regex_compiled, correct_syntax_regex, REG_EXTENDED))
	{
		fprintf(stderr, "Could not compile regex for syntax checking\n");
		exit(1);
	}

	if (regexec(&regex_compiled, option, 0, NULL, 0) == REG_NOMATCH)
	{
		fprintf(stderr, "Option not in correct syntax.\n%s\n", USAGE);
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	// Default values for N, M
	int n = -1, m = 1, i = 1; // Start is to denote where the args (excluding the option starts)
	
	// If given options, check correct syntax + find N, M
	if ((*argv[1]) == '-')
	{
		check_correct_syntax(argv[1]);
		
		char *every_regex = "^-([0-9]*).*$", *print_regex = "^.*,([0-9]*)$";
		regex_t every_compiled, print_compiled;
		size_t g = 2;
		regmatch_t groups[g];

		if (regcomp(&every_compiled, every_regex, REG_EXTENDED) 
			|| regcomp(&print_compiled, print_regex, REG_EXTENDED))
		{
			fprintf(stderr, "%s\n", USAGE);
			exit(1);
		} 
		
		if (regexec(&every_compiled, argv[1], g, groups, 0) == 0)
			n = atoi(argv[1] + groups[1].rm_so);

		if (regexec(&print_compiled, argv[1], g, groups, 0) == 0)
			m = atoi(argv[1] + groups[1].rm_so);
		
		i = 2;
	}

	for (i; i < argc; ++i)
	{
		FILE *file = fopen(argv[i], "r");
		char *line = NULL;
		size_t len;

		if (!file)
		{
			fprintf(stderr, "File: %s does not exist\n", argv[i]);
		}
		else
		{
			int line_num = 0;
			while (getline(&line, &len, file) != -1)
			{
				if (line_num % n < m)
					printf("%s\n", line);
				++line_num;
			}
			fclose(file);
		}
	}

	return 0;
}
