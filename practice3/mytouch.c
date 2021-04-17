#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>

int main(int argc, char **argv)
{
	int fd;
	if(utime(argv[1],NULL)==-1){
		fd=creat(argv[1],0644);
	}
	close(fd);
	exit(0);
}
