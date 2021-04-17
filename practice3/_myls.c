#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

char type(mode_t);
char* perm(mode_t);
void printStat(char*, char*, struct stat*);

int main(int argc, char **argv)
{
	DIR *dp;
	char *dir;
	struct stat st;
	struct dirent *d;
	char path[1024];
	int n, count;

	//명령어만 입력된 경우
	if(argc==1) 
	{
		dir = ".";
		if((dp = opendir(dir))==NULL){perror(dir);}
		while((d=readdir(dp))!=NULL){
			stat(d->d_name,&st);	//현재 파일의 정보를 st에 저장
			if(S_ISREG(st.st_mode)|S_ISDIR(st.st_mode)){
				count++;
				printf("%-12s",d->d_name);
			}
			if(count%6==0){
				printf("\n");
			}
		}
		printf("\n");
		closedir(dp);
		exit(0);
	}

	//argc==2인 경우 -> 옵션이 주어졌거나, filename이 주어진 경우
	if(argc>=2)
	{
		//옵션이 주어진 경우
		if(argv[1][0]=='-'){
			if(argc==3){dir=argv[2];}
			else{dir=".";}

			//옵션 검사
			while((n=getopt(argc,argv,"ilt"))!=-1){
				switch(n){
					case 'i':
						//printf("dir : %s\n",dir);
						if((dp=opendir(dir))==NULL)
							perror(dir);
						count=0;
						while((d=readdir(dp))!=NULL)
						{
							count++;
							sprintf(path,"%s/%s",dir,d->d_name);
							if(lstat(path,&st)<0)
								perror(path);
							printf("%d ",(int)d->d_ino);
							printf("%-12s",d->d_name);
							if(count%3==0) printf("\n");
						}
						closedir(dp);
						break;
					case 'l':
						if((dp=opendir(dir))==NULL){
							perror(dir);
						}

						while((d=readdir(dp))!=NULL){
							sprintf(path,"%s/%s",dir,d->d_name);
							if(lstat(path,&st)<0)
								perror(path);
							printStat(path,d->d_name,&st);
						}
						closedir(dp);
						break;
					case 't':
						printf("option : t\n");
						break;
				}
			}
			exit(0);
		}
		//filename이 주어진 경우
		else{

			if(lstat(argv[1],&st)<0){
				perror("lstat error\n");
			}
			//일반 파일인 경우
			if(S_ISREG(st.st_mode)){
				printf("%s\n",argv[1]);
			}
			//디렉토리 파일인 경우
			if(S_ISDIR(st.st_mode)){
		
				dir = argv[1];
				if((dp = opendir(dir))==NULL){perror(dir);}
				while((d=readdir(dp))!=NULL){
					count++;
					printf("%-12s",d->d_name);
					if(count==6){
						printf("\n");
						count=0;
					}
				}
				printf("\n");
				closedir(dp);
				exit(0);
			}
		}
	}
}

//파일 상태 정보를 출력
void printStat(char *pathname, char *file, struct stat *st){
	//printf("%5d ",st->st_blocks);
	printf("%c%s ",type(st->st_mode),perm(st->st_mode));
	printf("%2d ",st->st_nlink);
	printf("%s %s ",getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
	printf("%6d ", st->st_size);
	printf("%.12s ",ctime(&st->st_mtime)+4);
	printf("%s\n",file);
}

//파일 타입을 리턴
char type(mode_t mode){
	if(S_ISREG(mode)) return ('-');
	if(S_ISDIR(mode)) return ('d');
	if(S_ISCHR(mode)) return ('c');
	if(S_ISBLK(mode)) return ('b');
	if(S_ISLNK(mode)) return ('l');
	if(S_ISFIFO(mode)) return ('p');
	if(S_ISSOCK(mode)) return ('s');
}

//파일 사용권한을 리턴
char *perm(mode_t mode){
	int i;
	static char perms[10]="----------";
	for(i=0;i<3;i++){
		if(mode & (S_IRUSR>>i*3))
			perms[i*3]='r';
		if(mode &(S_IWUSR>>i*3))
			perms[i*3+1] = 'w';
		if(mode & (S_IXUSR>>i*3))
			perms[i*3+2]='x';
	}
	return(perms);
}
