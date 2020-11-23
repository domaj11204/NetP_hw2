#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
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

	FD_ZERO(&rset);
	FD_SET(0,&rset);
	FD_SET(sockfd,&rset);
	char tmp[100];
	for(;;){
		logined:printf("等待指令中.......0顯示、1邀請、2登出\n");
		FD_ZERO(&rset);
		FD_SET(0,&rset);
		FD_SET(sockfd,&rset);
		//printf("wainting I/O\n");
		int result = select(sockfd+1,&rset,NULL,NULL,NULL);
		//printf("result:%d\n",result);
		if(FD_ISSET(sockfd,&rset)){
			Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
			if(strncmp(recvline,"=invitefrom=",12) == 0){
				int invite_id = atoi(recvline+12);
				printf("\033[2J\n\033[1;1HID: %d 邀請你進行遊戲(Y/N)\n",invite_id);
				char tmpc;
				scanf(" %c",&tmpc);
				if(tmpc == 'Y'){
					sprintf(tmp,"=acceptinvite=%d",invite_id);
					printf("\033[2J\n\033[1;1H已接受，等待伺服器中.......");
				}
				else{
					printf("bye\n");	
				}
				Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
			}
			if(strncmp(recvline,"your_turn",9) == 0){
				playing = 1;
				char Table[200];
				sprintf(Table,"%s",recvline+9);
				printf("%s",Table);
				printf("要下哪呢～：");
				int xy;
				scanf("%d",&xy);
				sprintf(tmp,"=set=%d",xy);
				Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
			}
			if(strncmp(recvline,"=wait=",6) == 0){
				playing = 1;
				char Table[200];
				sprintf(Table,"%s",recvline+4);
				printf("%s",Table);
				printf("等待對方中～");
			}
			if(strncmp(recvline,"=win=",5) == 0){
				char Table[200];
				sprintf(Table,"%s",recvline+5);
				printf("你贏啦！！");
			}
			if(strncmp(recvline,"=lose=",6) == 0){
				char Table[200];
				sprintf(Table,"%s",recvline+6);
				printf("%s你輸啦！！輸入enter以繼續",Table);
				char tmpc;
				scanf("%c",&tmpc);
				goto logined;
			}
			if(strncmp(recvline,"=new_show=",10) == 0){
				printf("\033[2J\n\033[1;1H登入成功\n線上列表：\nID  |  Name  |       IP        | room\n");
				printf("%s",recvline+10);
				memset(recvline,0,MAXLINE);
			}
		}
		if(FD_ISSET(0,&rset)){
			printf("I/O!!\n");
			int fun_no;
			scanf("%d",&fun_no);
			if(fun_no == 0){
				char tmp[10]="=show=";
				printf("\033[2J\n\033[1;1H登入成功\n線上列表：\nID  |  Name  |       IP        | room\n");
				Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
				Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
				printf("%s",recvline);
				memset(recvline,0,MAXLINE);
			}
			if(fun_no == 1){
				printf("邀請：");
				int invite_id;
				scanf("%d",&invite_id);
				char tmp[20];
				sprintf(tmp,"=invite=%d",invite_id);
				Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
				Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
				if(strcmp(recvline,"=invite_ok=") == 0){
					printf("連線成功\n");
				}
				else
				{
					printf("連線錯誤\n");
				}
				//---------------------Play Game------------------------
				//char tmp[10];
				sprintf(tmp,"=play=");
				Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
			}
			if(fun_no == 2){
				sprintf(tmp,"=logout=");
				Sendto(sockfd,tmp,strlen(tmp),0,(SA*) &servaddr, sizeof(servaddr));
				printf("\033[2J\n\033[1;1H已登出...\n");
				goto unlogin;
			}
		}
	}
	//dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));

	exit(0);
}
