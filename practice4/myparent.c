#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void process(int pnum){

	while(1){
		sleep(5);
		printf("생성 순서 번호 : %d\n",pnum);
	}
}

int main(int argc, char *argv[]){
	
	int numofchild = atoi(argv[1]);
	pid_t pids[numofchild], pid;
	int status;

	for(int i=0;i<numofchild;i++){
		pids[i] = fork();
		if(pids[i]==0){
			process(i+1);
		}
	}

	sleep(5);i
	while(1){
		if(numofchild==0){
			printf("모든 프로세스 종료됨\n");
			raise(SIGKILL);
		}

		sleep(1);
		kill(pids[numofchild-1],SIGKILL);

		numofchild--;
	}
}
