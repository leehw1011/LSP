#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

//오름차순 비교 함수
int compare(const void *a, const void *b){
	int num1 = *(int *)a;
	int num2 = *(int *)b;

	if(num1<num2) return -1;
	if(num1>num2) return 1;
	return 0;
}

int main(){
	
	int input;
	int num;
	int num_list[100];
	char buf[500];
	char *space = " ";
	int fd1, fd2;
	printf("두 자리의 자연수를 입력하세요 : ");
	scanf("%d",&input);

	if((fd1 = open("f1.txt",O_WRONLY|O_CREAT|O_EXCL,200))<0){
		printf("file 1 open error/1\n");
		exit(1);
	}

	for(int i=0;i<input;i++){
		num = rand()%100;
		sprintf(buf,"%d",num);	//정수를 문자열 형태로
		write(fd1,buf,strlen(buf));
		write(fd1,space,1);
	}
	if(fchmod(fd1,S_IRWXU)<0){
		printf("chmod error");
	}
	close(fd1);
	//f1.txt를 읽기 전용으로 다시 열기
	if((fd1 = open("f1.txt",O_RDONLY))<0){
		printf("file 1 open error/2\n");
		exit(1);
	}
	
	read(fd1,buf,500);
	char *ptr = strtok(buf," ");
	int i=0;
	while(ptr!=NULL){
		num_list[i] = atoi(ptr);
		ptr = strtok(NULL," ");		
		i++;
	}
	
	//num_list의 요소의 개수는 처음 입력받았던 input과 같다.
	qsort(num_list,input,sizeof(int),compare);

	if((fd2 = open("f2.txt",O_WRONLY|O_CREAT|O_EXCL,200))<0){
		printf("file 2 open error");
		exit(1);
	}

	for(int i=0;i<input;i++){
		num = num_list[i];
		sprintf(buf,"%d",num);	//정수를 문자열 형태로
		write(fd2,buf,strlen(buf));
		write(fd2,space,1);
	}

	close(fd1);
	close(fd2);
}
