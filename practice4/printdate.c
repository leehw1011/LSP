#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]){	
	int timelimit=-1;
	pid_t pid;
	char *arg[] = {"date",NULL};

	if(argc>1){
		timelimit = atoi(argv[1]);
	}

	while(1){
		if(timelimit==0){
			raise(SIGKILL);
		}

		sleep(1);
		if((pid=fork())==0){	// date 명령어 실행
			execv("/bin/date",arg);
		}

		if(timelimit<=5&&timelimit>0){
			printf("종료 %d초 전\n",timelimit);
		}

		timelimit--;
	}
}
