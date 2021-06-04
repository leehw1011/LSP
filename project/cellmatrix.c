#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

typedef struct MultipleArg{
	int start;
	int end;
}args;

int** matrix;
int** output;
int m,n;	// matrix size

void readInput(FILE* fp){
	// 입력된 파일의 행, 열 계산(m x n matrix)

	rewind(fp);
	char ch;
	m = 0 ,n = 0;
	while((ch = fgetc(fp))!=EOF){
		if(ch=='0' || ch=='1') n++;
		else if(ch=='\n') m++;
	}

	m = m+1;
	n /= m;
	
	// 2차원 배열 동적할당
	matrix = (int**) malloc(sizeof(int*) * m);
	for(int i=0;i<m;i++){
		matrix[i] = (int*)malloc(sizeof(int)*n);
	}

	rewind(fp);
	int i=0, j=0;
	while((ch = fgetc(fp))!=EOF){
		if(ch=='0' || ch=='1'){
			*(*(matrix+j)+i) = atoi(&ch);
			i++;
		}
		if(ch=='\n'){
			j++;
			i=0;
		}
	}
	return;
}

// gen_n.matrix, output.matrix 파일을 만드는 함수
void mkfile(int gen){

	FILE* fp;
	char filename1[30] = "gen_";
	char filename2[10] = ".matrix";
	char num[30];
	if(gen==-1){ strcpy(filename1,"output.matrix");}
	else{
		sprintf(num,"%d",gen);
		strcat(filename1,num);
		strcat(filename1,filename2);
	}

	if((fp = fopen(filename1,"w+"))==NULL){
		printf("file open error\n");
	}

	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			fprintf(fp,"%d",matrix[i][j]);
			fprintf(fp," ");
		}
		if(i<m-1) fprintf(fp,"\n");
	}

	fclose(fp);
	return;
}

// matrix[x][y]의 살아있는 이웃 수를 구하는 함수
int getneighbor(int x, int y){
	int neighborCell=0;
	int dx[8] = {-1,-1,-1,0,0,1,1,1};
	int dy[8] = {-1,0,1,-1,1,-1,0,1};

	for(int i=0;i<8;i++){
		if(x+dx[i]<0||x+dx[i]>=m||y+dy[i]<0||y+dy[i]>=n) continue;
		if(matrix[x+dx[i]][y+dy[i]]==1) neighborCell++;
	}
	return neighborCell;
}

void run(int i, int j){

	int neighborCell;
	// 해당 세포가 살아있는 경우
	if(matrix[i][j]==1){
		// 살아있는 이웃 세포의 수 검사
		neighborCell = getneighbor(i,j);
		if(neighborCell<=2||neighborCell>=7){
			output[i][j] = 0;
		}
		else output[i][j] = 1;
	}
	// 해당 세포가 죽어있는 경우
	else{
		neighborCell = getneighbor(i,j);
		if(neighborCell==4){
			output[i][j]=1;
		}
		else output[i][j]=0;
	}
}

// 각 스레드가 실행할 함수 
void *run_thread(void *myargs){

	pthread_t id;
	args *data = (args *)myargs;

	int start = data->start;
	int end = data->end;

	printf("%ld ",pthread_self());
	for(int i=start;i<end;i++){
		for(int j=0;j<n;j++){
			run(i,j);
		}
	}

	pthread_exit(NULL);
	return NULL;
}

void seq(FILE* fp, int gennum){

	struct timeval start,end;
	double diff;

	// 시작 시간 체크
	gettimeofday(&start,NULL);
	// 입력된 파일 읽기
	readInput(fp);

	// gennum만큼 세대 진행, gen_n.matrix 파일 생성
	for(int gen=0;gen<gennum;gen++){
		output = (int**) malloc(sizeof(int*) * m);
        	for(int i=0;i<m;i++){
                	output[i] = (int*)malloc(sizeof(int)*n);
        	}

		// 세대 진행
		for(int i=0;i<m;i++){
			for(int j=0;j<n;j++){
				run(i,j);
			}
		}

		for(int i=0;i<m;i++){
			for(int j=0;j<n;j++){
				matrix[i][j]=output[i][j];
			}
		}

		// gen_n.matrix 파일 생성
		mkfile(gen+1);
	}

	// output.matrix 파일 생성 
	mkfile(-1);

	for(int i=0;i<m;i++){
		free(output[i]);
	}
	free(output);

	for(int i=0;i<m;i++){
                free(matrix[i]);
        }
        free(matrix);

	// 종료 시간 체크
	gettimeofday(&end,NULL);

	// 총 수행시간 출력
	diff = (end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	printf("수행 시간 : %lf (ms)\n\n",diff);
	return;
}

void process(FILE* fp, int childnum, int gennum){

	struct timeval start, end;
	double diff;
	pid_t pids[childnum], pid;
	int status;
	char buffer[m*n];
	int fd[2];
	if(pipe(fd)==-1){
		printf("pipe error\n");
		exit(1);
	}
	// 시작 시간 체크
	gettimeofday(&start,NULL);

	// 입력된 파일 읽기
	readInput(fp);

	// 한 프로세스가 몇 개의 행을 실행할지 구하기!
	int div = m/childnum;
	int remain = m%childnum;
	int sum=0;
	int per_process[childnum];
	int offset[childnum+1];

	for(int i=0;i<=childnum;i++){
		if(i<remain){per_process[i] = div+1;}
		else per_process[i] = div;
		offset[i] = sum;
		sum+=per_process[i];
	}
	
	// 자식 프로세스 생성(childnum만큼 반복하며 fork())
	for(int gen=0;gen<gennum;gen++){
		printf("\n-- %d세대 진행에 사용한 프로세스 ID --\n",gen+1);
		output = (int**) malloc(sizeof(int*) * m);
		for(int i=0;i<m;i++){
			output[i] = (int*)malloc(sizeof(int)*n);
		}
		for(int processnum=0;processnum<childnum;processnum++){
			pids[processnum] = fork();
			if(pids[processnum]<0){		// error
				printf("fork error\n");
				exit(1);
			}
			else if(pids[processnum]==0){	// child(processnum번째 자식?)
				int a=0;
				for(int i=offset[processnum];i<offset[processnum+1];i++){
					for(int j=0;j<n;j++){
						run(i,j);
						buffer[a] = output[i][j];
						a++;
					}
				}
				write(fd[1],buffer,m*n);
				printf("%d ",getpid());
				
				exit(0);	// child process 종료
			}
			else{				// parent
				read(fd[0],buffer,m*n);
				int a=0;
				for(int i=offset[processnum];i<offset[processnum+1];i++){
					for(int j=0;j<n;j++){
						output[i][j] = buffer[a];
						a++;
					}
				}
			}
		}
		while(wait((int *)0) != -1);

		// 각 세대마다 gen_n.matrix 파일 만들기
		for(int i=0;i<m;i++){
			for(int j=0;j<n;j++){
				matrix[i][j]=output[i][j];
			}
		}

		mkfile(gen+1);
	}

	// output.matrix 파일 생성
	mkfile(-1);

	for(int i=0;i<m;i++){
		free(output[i]);
        }
        free(output);

        for(int i=0;i<m;i++){
                free(matrix[i]);
        }
        free(matrix);
	
	// 종료 시간 체크
	gettimeofday(&end,NULL);
	
	// 총 수행시간 출력
	diff = (end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	printf("\n수행 시간 : %lf (ms)\n\n",diff);
	return;
}

void thread(FILE* fp, int total_thread, int gennum){
	
	args args_array[total_thread];
        struct timeval start, end;
        double diff;
        pthread_t tid[total_thread];
	int status;

	// 시작 시간 체크
        gettimeofday(&start,NULL);

        // 입력된 파일 읽기
	readInput(fp);

        // 한 스레드가 몇 개의 행을 실행할지 구하기!
        int div = m/total_thread;
        int remain = m%total_thread;
        int sum=0;
        int per_thread[total_thread];
        int offset[total_thread+1];

        for(int i=0;i<=total_thread;i++){
                if(i<remain){per_thread[i] = div+1;}
                else per_thread[i] = div;
                offset[i] = sum;
                sum+=per_thread[i];
        }

        // 스레드 생성(total_thread만큼 반복하며 pthread_create())
        for(int gen=0;gen<gennum;gen++){
		printf("\n-- %d 세대 진행에 사용한 thread ID --\n",gen+1);
                output = (int**) malloc(sizeof(int*) * m);
                for(int i=0;i<m;i++){
                        output[i] = (int*)malloc(sizeof(int)*n);
                }
                for(int threadnum=0;threadnum<total_thread;threadnum++){
			args_array[threadnum].start = offset[threadnum];
			args_array[threadnum].end = offset[threadnum+1];

		       	if(pthread_create(&tid[threadnum],NULL,run_thread,(void *)&args_array[threadnum])!=0){
				printf("pthread_create error\n");
				exit(1);
			}
                        
                }
		
		for(int threadnum=0;threadnum<total_thread;threadnum++){
			pthread_join(tid[threadnum],(void*)&status);
		}

                // 각 세대마다 gen_n.matrix 파일 만들기
                for(int i=0;i<m;i++){
			for(int j=0;j<n;j++){
				matrix[i][j] = output[i][j];
			}
		}
		mkfile(gen+1);
        }

        // output.matrix 파일 생성
	mkfile(-1);
        
	for(int i=0;i<m;i++){
                free(output[i]);
        }
        free(output);

        for(int i=0;i<m;i++){
                free(matrix[i]);
        }
        free(matrix);

	// 종료 시간 체크
	gettimeofday(&end,NULL);

	// 총 수행시간 출력
	diff = (end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	printf("\n수행 시간 : %lf (ms)\n\n",diff);
	return;
}

int main(int argc, char* argv[]){
	
	FILE* fp;
	int option;
	int gennum, childnum;	// 세대 수, 생성할 프로세스 or 스레드 개수

	if(argc<2){
		printf("error : 파일명을 입력하세요!\n");
		exit(1);
	}

	if((fp = fopen(argv[1],"r"))==NULL){
		printf("error : file open error\n");
		exit(1);
	}

	while(1){
		printf("[(1) 프로그램 종료 (2) 순차처리 (3) process 병렬처리 (4) Thread 병렬처리]\n원하는 동작을 선택하세요 : ");
		scanf("%d",&option);
		
		switch(option){
			case 1:
				// 프로그램 종료
				fclose(fp);
				exit(0);
			case 2:
				// 순차처리 실행
				printf("순차처리 실행\n");
				printf("세대 수 입력 : ");
				scanf("%d",&gennum);
				seq(fp,gennum);
				break;
			case 3:
				// process 병렬처리 실행
				printf("process 병렬처리 실행\n");
				printf("생성할 자식 프로세스의 수 입력 : ");
				scanf("%d",&childnum);
				printf("세대 수 입력 : ");
				scanf("%d",&gennum);
				process(fp,childnum,gennum);
				break;
	
			case 4:
				// thread 병렬처리 실행
				printf("thread 병렬처리 실행\n");
				printf("생성할 스레드의 수 입력 : ");
				scanf("%d",&childnum);
				printf("세대 수 입력 : ");
				scanf("%d",&gennum);
				thread(fp,childnum,gennum);
				break;

			defalut:
				printf("잘못된 입력입니다.\n");
		}
	}

	exit(0);
}
