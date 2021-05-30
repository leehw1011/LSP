#include <stdio.h>
#include <stdlib.h>

int** readInput(FILE* fp, int** matrix, int* m, int* n){
	// 입력된 파일의 행, 열 계산(m x n matrix)
	char ch;
	*m = 0, *n = 0;

	while((ch = fgetc(fp))!=EOF){
		if(ch=='0' || ch=='1') (*n)++;
		else if(ch=='\n') (*m)++;
	}

	*n /= *m;
	printf("행 : %d, 열: %d\n",*m,*n);

	// 2차원 배열 동적할당
	matrix = (int**) malloc(sizeof(int*) * (*m));
	for(int i=0;i<*m;i++){
		matrix[i] = (int*)malloc(sizeof(int)*(*n));
	}

	rewind(fp);
	int i=0, j=0;
	while((ch = fgetc(fp))!=EOF){
		if(ch=='0' || ch=='1'){
			*(*(matrix+j)+i) = atoi(&ch);
			i++;
		}
		if(i==7){
			j++;
			i=0;
		}
	}

	printf("\nreadInput 함수 내\n");
	for(int i=0;i<(*m);i++){
		for(int j=0;j<(*n);j++){
			printf("%d ",matrix[i][j]);
		}
		printf("\n");
	}
	//return;
	return matrix;
}

// matrix[x][y]의 살아있는 이웃 수를 구하는 함수
int getneighbor(int** matrix, int m, int n, int x, int y){
	int neighborCell=0;
	int dx[8] = {-1,0,1,-1,1,-1,0,1};
	int dy[8] = {-1,-1,-1,0,0,1,1,1};

	for(int i=0;i<8;i++){
		if(x+dx[i]<0||x+dx[i]>m||y+dy[i]<0||y+dy[i]>n) continue;
		if(matrix[x+dx[i]][y+dy[i]]==1) neighborCell++;
	}
	return neighborCell;
}

void seq(FILE* fp, int gennum){

	int **matrix;
	int **output;
	int neighborCell;	// 살아있는 이웃 세포의 수
	int m,n;	// matrix size
	// 시작 시간 체크
	//
	// 입력된 파일 읽기
	matrix = readInput(fp,matrix,&m,&n);

	/*printf("여긴 seq함수!\n");
	printf("m : %d, n : %d\n",m,n);
	for(int i=0;i<m;i++){
                for(int j=0;j<n;j++){
                        printf("%d ",matrix[i][j]);
                }
                printf("\n");
        }*/

	output = (int**) malloc(sizeof(int*) * m);
        for(int i=0;i<m;i++){
                output[i] = (int*)malloc(sizeof(int)*n);
        }

	printf("1\n");
	// gennum만큼 세대 진행, gen_n.matrix 파일 생성

	for(int gen=0;gen<gennum;gen++){
		//왜 n , m이지?
		for(int i=0;i<n;i++){
			for(int j=0;j<m;j++){
				// 해당 세포가 살아있는 경우
				if(matrix[i][j]==1){
					// 살아있는 이웃 세포의 수 검사
					neighborCell = getneighbor(matrix,m,n,i,j);
					//printf("neighborCell : %d\n",neighborCell);
					if(neighborCell<=2||neighborCell>=7){
						output[i][j]=0;
					}
					else output[i][j]=1;
				}
				// 해당 세포가 죽어있는 경우
				else{
					// 살아있는 이웃 세포의 수 검사
					neighborCell = getneighbor(matrix,m,n,i,j);
					//printf("neighborCell : %d\n",neighborCell);

					if(neighborCell==4){
						output[i][j]=1;
					}
					else output[i][j]=0;
				}
			}
		}

		printf("--- %d세대 ----\n",gen+1);
		for(int i=0;i<m;i++){
                	for(int j=0;j<n;j++){
                        	printf("%d ",output[i][j]);
                	}
                printf("\n");
		}
		printf("\n");

	}

	// output.matrix 파일 생성
	//
	// 종료 시간 체크
	//
	// 총 수행시간 출력
	return;
}

void process();
void thread();

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

				break;
			case 4:
				// thread 병렬처리 실행
				printf("thread 병렬처리 실행\n");
                                printf("생성할 스레드의 수 입력 : ");
                                scanf("%d",&childnum);
                                printf("세대 수 입력 : ");
                                scanf("%d",&gennum);

				break;
			default :
				printf("잘못된 입력입니다\n");
				break;
		}
	}
	exit(0);
}
