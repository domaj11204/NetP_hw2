#include	"unp.h"
struct sockaddr_in	servaddr;
void sigalrm_fn(int sig)
{
	printf("alive!!\n");
    char alive_mesg[10] = "=alive=";
	Sendto(3,alive_mesg,strlen(alive_mesg),0,(SA*) &servaddr, sizeof(servaddr));
	alarm(20);
}

void want_show(){
	char tmp[10]="=show=";
	char recvline[MAXLINE];
	printf("\033[2J\n\033[1;1H登入成功\n線上列表：\nID  |  Name  |       IP        | room\n");
	Sendto(3,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
	Recvfrom(3, recvline, MAXLINE, 0, NULL, NULL);
	printf("%s",recvline);
	printf("等待指令中.......0顯示、1邀請、2登出\n");
	memset(recvline,0,MAXLINE);
}
int
main(int argc, char **argv)
{
	int					sockfd;
	
	fd_set rset;
	if (argc != 2)
		err_quit("usage: udpcli <IPaddress>");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	int login;
	//LOGIN
	unlogin:login = 0;
	char LOGIN[20],acc[9],pass[9],recvline[MAXLINE];
	int playing = 0;
	int state = -1;
	int invite_id;
	while (login != 1)
	{
		printf("\033[2J\n\033[1;1H請登入\n帳號：");
		scanf("%s",&acc);
		printf("密碼：");
		scanf("%s",&pass);
		sprintf(LOGIN,"=login=%s_%s",acc,pass);
		Sendto(sockfd,LOGIN,strlen(LOGIN),0,(SA*) &servaddr, sizeof(servaddr));
		Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
		if(strcmp(recvline,"success") == 0){
			login = 1;
		}
		else
		{
			printf("登入錯誤\n");
		}
		memset(recvline,0,MAXLINE);
	}
	printf("\033[2J\n\033[1;1H登入成功\n");
	char ch = ' ';
	signal(SIGALRM, sigalrm_fn);
    //alarm(20);
	FD_ZERO(&rset);
	FD_SET(0,&rset);
	FD_SET(sockfd,&rset);
	char tmp[100];
	printf("等待指令中.......0顯示、1邀請、2登出\n");
	for(;;){
		//fflush(stdin);
		logined:
		FD_ZERO(&rset);
		FD_SET(0,&rset);
		playing:FD_SET(sockfd,&rset);
		//printf("wainting I/O\n");
		int result = select(sockfd+1,&rset,NULL,NULL,NULL);
		//printf("result:%d\n",result);
		if(FD_ISSET(sockfd,&rset)){
			Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
			if(strncmp(recvline,"=invitefrom=",12) == 0){
				invite_id = atoi(recvline+12);
				printf("\033[2J\n\033[1;1HID: %d 邀請你進行遊戲(Y/N)\n",invite_id);
				state = 1;
			}
			if(strncmp(recvline,"your_turn",9) == 0){
				state = 2;
				char Table[200];
				sprintf(Table,"%s",recvline+9);
				printf("%s",Table);
				printf("(你是%c) 要下哪呢～：\n輸入\"x\"投降\n",ch);
				int xy;
				//fflush(stdin);
			}
			if(strncmp(recvline,"=wait=",6) == 0){
				playing = 1;
				char Table[200];
				sprintf(Table,"%s",recvline+6);
				printf("%s",Table);
				printf("輪到對方～\n輸入\"x\"投降\n");
				state = 5;
			}
			if(strncmp(recvline,"=win=",5) == 0){
				char Table[200];
				sprintf(Table,"%s",recvline+5);
				printf("你贏啦！！(Y)\n");
				state = 3;
			}
			if(strncmp(recvline,"=lose=",6) == 0){
				char Table[200];
				sprintf(Table,"%s\0",recvline+6);
				printf("%s你輸啦!!(Y)\n",Table);
				state = 3;
			}
			if(strncmp(recvline,"=tie=",5) == 0){
				char Table[200];
				sprintf(Table,"%s",recvline+5);
				printf("%s平手gg(Y)\n",Table);
				state = 3;
			}
			if(strncmp(recvline,"=new_show=",10) == 0){
				if(state == 3){
					continue;
				}
				printf("\033[2J\n\033[1;1H登入成功\n線上列表：\nID  |  Name  |       IP        | room\n");
				printf("%s",recvline+10);
				memset(recvline,0,MAXLINE);
				printf("等待指令中.......0顯示、1邀請、2登出\n");
			}
			if(strncmp(recvline,"=alive=",7) == 0){
				char alive_mesg[10] = "=alive=";
				Sendto(sockfd,alive_mesg,strlen(alive_mesg),0,(SA*) &servaddr, sizeof(servaddr));
				goto logined;
			}
			if(strncmp(recvline,"=end=",5) == 0){
				printf("\033[2J\n\033[1;1H伺服器關閉\n");
				printf("客戶端即將關閉........\n");
				return 0;
			}
			if(strncmp(recvline,"=refuse=",8) == 0){
				printf("對方拒絕啦\n");
				sleep(1);
				want_show();
			}
			if(strcmp(recvline,"=invite_ok=") == 0){
					printf("連線成功\n");
					ch = 'O';
					sprintf(tmp,"=play=");
					Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));	
			}
			if(strcmp(recvline,"=alive=") == 0)
			{
				char alive_mesg[10] = "=alive=";
				Sendto(sockfd,alive_mesg,strlen(alive_mesg),0,(SA*) &servaddr, sizeof(servaddr));
			}
			if(strcmp(recvline,"=invite_error=") == 0)
			{
				char alive_mesg[10] = "=alive=";
				want_show();
				printf("邀請錯誤\n");
			}
		}
		if(FD_ISSET(0,&rset)){
			//printf("I/O!!\n");
			if(state == 1){
				char tmpc;
				scanf(" %c",&tmpc);
				if(tmpc == 'Y'){
					sprintf(tmp,"=acceptinvite=%d",invite_id);
					printf("\033[2J\n\033[1;1H已接受，等待伺服器中.......\n");
					ch = 'X';
					Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
				}
				else{
					char refuse[10] = "=refuse=";
					sprintf(tmp,"%s%d",refuse,invite_id);
					Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
					printf("已拒絕\n");
					sleep(1);
					want_show();
				}
				state = -1;
			}
			else if(state == 2){
				int xy;
				scanf("%d",&xy);
				sprintf(tmp,"=set=%d",xy);
				Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
				state = -1;
			}
			else if(state == 3)
			{
				char tmpc='0';
				scanf(" %c",&tmpc);
				if(tmpc == 'Y'){
					char tmp[10] = "=free=";
					Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
					want_show();
					state = -1;
				}
			}
			else if( state == 4){
				int invite_id;
				scanf("%d",&invite_id);
				printf("等待對方回應........\n");
				char tmp[20];
				sprintf(tmp,"=invite=%d",invite_id);
				Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
				state = -1;
			}
			else{
				int fun_no;
				scanf("%d",&fun_no);
				if(fun_no == 0){
					want_show();
				}
				if(fun_no == 1){
					printf("邀請：\n");
					state = 4;
					//---------------------Play Game------------------------
					//char tmp[10];
				}
				if(fun_no == 2){
					sprintf(tmp,"=logout=");
					Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
					printf("\033[2J\n\033[1;1H已登出...\n");
					goto unlogin;
				}
			}
			
		}
	}
	//dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));

	exit(0);
}
