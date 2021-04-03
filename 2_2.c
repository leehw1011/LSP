#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[]){

	if(argc<1){
		printf("error1\n");
		exit(1);
	}

	char buf[1];
	char *fname1 = argv[1];
	char *fname2 = argv[1];
	int fd1, fd2;
	
	if((fd1 = open(fname1,O_RDONLY))<0){
		printf("file 1 open error\n");
		exit(1);
	}

	//파일2 이름
	for(int i=0;i<(int)strlen(fname2);i++){
		if(fname2[i]>='A'&&fname2[i]<='Z'){
			fname2[i] = fname2[i]+32;
		}
		else if(fname2[i]>='a'&&fname2[i]<='z'){
			fname2[i] = fname2[i]-32;
		}
		else{
			;
		}	
	}

	if((fd2 = open(fname2,O_WRONLY|O_CREAT|O_EXCL,200))<0){
		printf("file 2 open error\n");
	}

	while((read(fd1,buf,1))>0){
		if(*buf>='A'&&*buf<='Z'){
			*buf = *buf+32;
		}
		else if(*buf>='a'&&*buf<='z'){
			*buf = *buf-32;
		}
		else{;}

		write(fd2,buf,1);
	}

	close(fd1);
	close(fd2);
}
