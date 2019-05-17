#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("hi\n");
		exit(0);
	}

	for (int i = 1; i < argc; ++i)
		printf("%s\n", argv[i]);
	
	return 0;
}
