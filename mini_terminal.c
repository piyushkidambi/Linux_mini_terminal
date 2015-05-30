#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
int histt();
void pidredir(char* aaaa,int co);
void pidnumm();
char per[1024];
char all[1000][100];
int pidall[100];
int allp=1;
int num=0;
char *comm[100];
int flagforhist=0;
void prmt(){
	char hostname[1024],pp[100],cwd[1024];
	char *username=getenv("USER");
	//printf("%s\n",username);
	hostname[1023]='\0';
	gethostname(hostname,1023);
	//printf("%s\n",hostname);
	getcwd(cwd,1024);
	//printf("%s\n",cwd);
	strcpy(pp,"<");
	strcat(pp,username);
	strcat(pp,"@");
	strcat(pp,hostname);
	strcat(pp,":");
	int len=strlen(per);
	char *check=strstr(cwd,per);
	if(check==NULL){
		strcat(pp,cwd);
		strcat(pp,">");
	}
	else{
		check=check+len;
		char cop[100];
		strcpy(cop,check);
		char a[100];
		strcpy(a,"~");
		strcat(a,cop);
		strcat(pp,a);
		strcat(pp,">");
	}
	printf("%s",pp);
}
void sig_handler(int signum){
	if(signum == 2 || signum == 20){
		printf("\nEnter \"quit\" to terminate the program\n");
	}
	signal(SIGINT, sig_handler);
	signal(SIGTSTP, sig_handler);
	return;
}
void child_handler()
{
	int stat1, i;
	pid_t pc = waitpid(-1,&stat1,WNOHANG);
	for(i=1;i<allp;i++)
	{
		if(pidall[i]==pc)
		{
			if(WIFEXITED(stat1))
			{
				printf("\n%s %ld exited normally",all[i],(long)pc);
			}
			else if(WIFSIGNALED(stat1))
			{
				printf("\n%s %ld killed",all[i],(long)pc);
			}
#ifdef WCOREDUMP
			else if(WCOREDUMP(stat1))
			{
				printf("\n%s %ld (core dumped)",all[i],(long)pc);
			}
			fflush(stdout);
			printf("");
#endif
		}
	}
}
void sig1_handler(int signum){
	prmt();
	sig1_handler(signum);
}
void sigfun(int sig){
	printf("\n");
	prmt();
	fflush(stdout);
}
int h=1;
int flag;
int flag1=0;
int c=0;
char his[1000][100];
char inp[100],input[100];
char *argv[100];
int main(){
	(void)signal(SIGINT,  sig_handler);
	(void)signal(SIGTSTP, sig_handler);
	(void)signal(SIGCHLD, child_handler);
	(void)signal(SIGINT , sigfun);
	(void)signal(SIGSEGV,sig1_handler);
	getcwd(per,1024);
	setenv("PWD","~",1);
	prmt();
	flag=0;
	flagforhist=0;
	while(1){
		if(flag==0){
			gets(inp);
			strcpy(input,inp);
		}
		else{
			flag=0;
			flagforhist=0;
		}
		if(strcmp(inp,"quit")==0){
			break;
		}
		char *a;
		c=0;
		a=strtok(inp," ");
		flag1=0;
		while(a!=NULL){
			argv[c]=a;
			if(strcmp(argv[c],">")==0 || strcmp(argv[c],"<")==0 || strcmp(argv[c],"|")==0){
				flag1++;
			}
			a=strtok(NULL," ");
			c++;
		}
		argv[c]=NULL;
		if(argv[0]!=NULL){
			strcpy(his[h],input);
			char aaaa[2][100];
			strcpy(aaaa[0],".out0");
			strcpy(aaaa[1],".out1");
			int hidden=0;
			h++;
			if(flag1 > 0){
				if(strstr(inp,"!hist")!=NULL){
					histt();
					if(flagforhist==1){
						continue;
					}
				}
				char f1[100],f2[100];/*,*comm[100];*/
				strcpy(f1,"");
				strcpy(f2,"");
				int i,co=0;
				for(i=0;i<c;i++){
					if(strcmp(argv[i],"<")==0){
						strcpy(f1,argv[i+1]);
						i++;
					}
					else if(strcmp(argv[i],"|")==0){
						comm[co]=NULL;
						pid_t pid;
						int status;
						pid=fork();
						if(pid == 0){
							int fd1,fd2;
							if(strcmp(f1,"")!=0){
								fd1=open(f1,O_RDONLY,0666);
								if(fd1<0){
									fprintf(stderr,"File open Failed\n");
								}
								dup2(fd1,0);
							}
							if(strcmp(f2,"")==0){
								strcpy(f2,aaaa[hidden]);
							}
							fd2=open(f2, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
							if(fd2<0){
								fprintf(stderr,"File open Failed\n");
							}
							dup2(fd2,1);
							if(strstr(comm[0],"hist")!=NULL){
								flagforhist=0;
								histt();
							}
							else if(strcmp(comm[0],"pid")==0){
								pidnumm(co);
							}
							else{
								execvp(*comm,comm);
								fprintf(stderr,"1 No Such Command.\n");
							}
							exit(0);
						}
						else{
							if(flagforhist==1){
								break;
							}
							strcpy(all[allp],argv[0]);
							pidall[allp]=pid;
							allp++;
							wait(&status);
							strcpy(f1,aaaa[hidden]);
							hidden=(hidden+1)%2;
							strcpy(f2,"");
							int j;
							for(j=0;j<co;j++){
								comm[j]=NULL;
							}
							co=0;
							fflush(stdout);
						}
					}
					else if(strcmp(argv[i],">")==0){
						strcpy(f2,argv[i+1]);
						i++;
					}
					else{
						comm[co]=argv[i];
						co++;
					}
				}
				if(i==c){
					comm[co]=NULL;
					pid_t pid;
					int status;
					pid=fork();
					if(pid == 0){
						int fd1,fd2;
						if(strcmp(f1,"")!=0){
							fd1=open(f1,O_RDONLY,0666);
							if(fd1<0){
								fprintf(stderr,"File open Failed\n");
							}
							dup2(fd1,0);
						}
						if(strcmp(f2,"")!=0){
							fd2=open(f2, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
							if(fd2<0){
								fprintf(stderr,"File open Failed\n");
							}
							dup2(fd2,1);
						}
						if(strstr(comm[0],"hist")!=NULL){
							flagforhist=0;
							histt();
						}
						else if(strcmp(comm[0],"pid")==0){
							pidnumm(co);
						}
						else{
							execvp(*comm,comm);
							fprintf(stderr,"2 No Such Command.\n");
						}
						exit(0);
					}
					else{
						wait(&status);
						strcpy(all[allp],argv[0]);
						pidall[allp]=pid;
						allp++;
						fflush(stdout);
					}
				}
			}
			else if(strcmp(argv[0],"cd")==0){                         //for cd
				if(c==1){
					if(chdir(per)==-1){
						perror("cd");
					}
					else{
						chdir(per);
					}
				}
				else{
					if(chdir(argv[1])==-1){
						perror("cd");
					}
					else{
						//	chdir(argv[1]);
					}
				}
			}
			else if(strcmp(argv[0],"pid")==0){                   //for pid
				pidnumm(c);
			}
			else if(c>1 && strcmp(argv[c-1],"&")==0){              //for background processes
				pid_t pid;
				int status;
				pid=fork();
				if(pid == 0){
					execvp(*argv,argv);
					//perror(argv[0]);
					printf("command==%s\n",argv[0]);
					fprintf(stderr,"3 No Such Command.\n");
					exit(0);
				}
				else{
					printf("command %s pid %d\n",inp,pid);
					strcpy(all[allp],argv[0]);
					pidall[allp]=pid;
					allp++;
					//wait(&status);
				}
			}
			else if(strstr(argv[0],"hist")!=NULL){                    //for hist
				flag=0;
				flag=histt();
				if(flag==1){
					strcpy(inp,his[num]);
					strcpy(input,inp);
					continue;
				}

			}
			else{                                                          //for foreground processes
				pid_t pid;
				int status;
				pid=fork();
				if(pid == 0){
					execvp(*argv,argv);
					fprintf(stderr,"4 No Such Command.\n");
					printf("command==%s\n",argv[0]);
					//perror(argv[0]);
					exit(0);
				}
				else{
					strcpy(all[allp],argv[0]);
					pidall[allp]=pid;
					allp++;
					wait(&status);
					fflush(stdout);
				}
			}
		}
		else{
			flag=0;
			prmt();
			continue;
		}
		prmt();
	}
	return 0;
}
int histt(){
	int i;
	if(strlen(argv[0])==4){
		for(i=1;i<h-1;i++){
			printf("%d. %s\n",i,his[i]);
		}
		return 0;
	}
	else if(argv[0][0]=='!'){
		char *aa;
		aa=strstr(argv[0],"hist");
		aa=aa+4;
		num=atoi(aa);
		//printf("%d %d \n",num,h);
		if(num < h-1){
			char *bb;
			if(flag1>0){
				bb=strstr(input,argv[0]);
				int len=strlen(argv[0]);
				bb=bb+len;
				strcpy(inp,his[num]);
				strcpy(input,inp);
				flag=1;
				flagforhist=1;
				strcat(inp,bb);
				strcpy(input,inp);
			}
			else{
				flag=1;
				strcpy(inp,his[num]);
				strcpy(input,inp);
			}
			return 1;
		}
		else{
			printf("Error: Invalid index for hist!!\n");
			flag=0;
			flagforhist=0;
			return 0;
		}
	}
	else{
		char *aa;
		aa=strstr(argv[0],"hist");
		aa=aa+4;
		int num=atoi(aa);
		if(num>h-1){
			for(i=1;i<h-1;i++){
				printf("%d. %s\n",i,his[i]);
			}

		}
		else{
			for(i=h-num-1;i<h-1;i++){
				printf("%d. %s\n",i,his[i]);
			}
		}
		return 0;
	}
}
void pidnumm(int y){
	if(y==1){
		pid_t p;
		p=getpid();
		printf("command name: ./a.out process id: %d\n",p);
	}
	else{
		if(strcmp(argv[1],"current")==0){                   //pid current
			printf("List of currently executing processes spawned from this shell:\n");
			int i;
			for(i=1;i<allp;i++){
				if (kill(pidall[i], 0) == 0){
					printf("command name: %s process id: %d\n",all[i], pidall[i]);
					// process is running or a zombie 

				}
			}

		}
		if(strcmp(argv[1],"all")==0){                     //pid all
			printf("List of all processes spawned from this shell:\n");
			int i;
			for(i=1;i<allp;i++){
				printf("command name: %s process id: %d\n",all[i], pidall[i]);
			}
		}
	}
}


