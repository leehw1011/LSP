#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){

	int num = atoi(argv[1]);
	char *ch = argv[2];
	char *line = "\n";
	char *fname = argv[3];
	int fd;

	if(argc<3){
		printf("error1\n");
		exit(1);
	}

	if((fd = open(fname,O_WRONLY|O_CREAT|O_EXCL,200))<0){
		printf("error2\n");
		exit(1);
	}

	//양수인 경우
	if(num>0){
		for(int i=0;i<num;i++){
			for(int j=0;j<=i;j++){
				write(fd,ch,1);
			}
			write(fd,line,1);
		}
	}

	//음수인 경우
	else{
		num = 0-num;
		for(int i=0;i<num;i++){
			for(int j=0;j<num-i;j++){
				write(fd,ch,1);
			}
			write(fd,line,1);
		}
	}

	close(fd);

}
