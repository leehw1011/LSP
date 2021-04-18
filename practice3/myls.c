#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char type(mode_t);
char* perm(mode_t);
void printStat(char*, char*, struct stat*);

void noneOpt(char*);
void ls_i(char*);
void ls_l(char*);
void ls_t(char*);

int main(int argc, char** argv){
	
	char *dir;
	int n;
	if(argc==1){
		dir = ".";
		noneOpt(dir);
	}

	//옵션이 주어진 경우
	else if(argv[1][0]=='-'){
		//옵션 검사
		while((n=getopt(argc,argv,"ilt"))!=-1){
			switch(n){
				case 'i':
					if(argc==2) {dir=".";}
					else dir = argv[2];
					ls_i(dir);
					break;
				case 'l':
					if(argc==2) {dir=".";}
					else dir = argv[2];
					ls_l(dir);
					break;
				case 't':
					if(argc==2) {dir=".";}
					else dir = argv[2];
					ls_t(dir);
					break;

			}
		}	
	}

	else{
		dir = argv[1];
		noneOpt(dir);
	}

	exit(0);
}

void noneOpt(char* dir){
	//printf("Option : none\n");
	//printf("dir : %s\n",dir);

	DIR *dp;
	struct stat st;
	struct dirent *d;
	int count=0;

	if(lstat(dir,&st)<0) {perror("lstat error\n");}

	//일반 파일인 경우
	if(S_ISREG(st.st_mode)) {printf("%s\n",dir);}

	//디렉토리 파일인 경우
	if(S_ISDIR(st.st_mode)){
		if((dp = opendir(dir))==NULL){perror(dir);}
		while((d=readdir(dp))!=NULL){
			printf("%-12s",d->d_name);
			count++;

			if(count!=0&&count%6==0){
				printf("\n");
			}
		}
		printf("\n");
		closedir(dp);
	}
}

void ls_i(char* dir){
	//printf("Option : -i\n");
	//printf("dir : %s\n",dir);

	DIR *dp;
        struct stat st;
        struct dirent *d;
        int count=0;
	if(lstat(dir,&st)<0) {perror("lstat error\n");}
	
	//일반 파일인 경우
	if(S_ISREG(st.st_mode)){
		printf("%d ",(int)st.st_ino);
		printf("%s\n",dir);
	}

	//디렉토리 파일인 경우
	else if(S_ISDIR(st.st_mode)){
		if((dp=opendir(dir))==NULL){perror(dir);}
		while((d=readdir(dp))!=NULL){
			printf("%d ",(int)d->d_ino);
			printf("%-12s",d->d_name);
			count++;

			if(count!=0&&count%3==0){
				printf("\n");
			}
		}
		closedir(dp);
	}
}
void ls_l(char* dir){
	//printf("Option : -l\n");
        //printf("dir : %s\n",dir);

	DIR *dp;
        struct stat st;
        struct dirent *d;
        char path[1024];

        if(lstat(dir,&st)<0) {perror("lstat error\n");}

        //일반 파일인 경우
        if(S_ISREG(st.st_mode)){
                printStat(path,dir,&st);
        }

        //디렉토리 파일인 경우
        else if(S_ISDIR(st.st_mode)){
                if((dp=opendir(dir))==NULL){perror(dir);}
                while((d=readdir(dp))!=NULL){
			sprintf(path,"%s/%s",dir,d->d_name);
			if(lstat(path,&st)<0){perror(path);}
			printStat(path,d->d_name,&st);
                }
                closedir(dp);
	}
}
void ls_t(char* dir){
	printf("Option : -t\n");
        printf("dir : %s\n",dir);
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
	static char perms[10]="---------";
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

