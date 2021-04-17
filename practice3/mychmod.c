#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	unsigned int per = 0;

	sscanf(argv[1],"%o",&per);
	chmod(argv[2],per);

	exit(0);
}
