/* include udpservselect01 */
#include	"unp.h"
#include <signal.h>
#include "OOXX2.c"
#include <string.h>
#define MAXNUM 10

void show_all_user(char PASSWORD[][2][10]){
    for(int i = 0 ; i < MAXNUM ; i++){
        printf("%s,%s\n",PASSWORD[i][0],PASSWORD[i][1]);
    }
}

int find_by_ip(char ip[],char user_online[][3][20]){
    printf("==find_by_ip==\nip:%s\n",ip);
    for(int i = 0 ; i < MAXNUM ; i++){
        if(strcmp(ip,user_online[i][1]) == 0){
            return i;
        }
    }
    return -1;
}
void all_show(char user_online[][3][20],struct sockaddr all_cliaddr[],int udpfd){
    for(int i = 0 ; i < MAXNUM ; i++){
        if(strcmp(user_online[i][0],"enable") != 0 && user_online[i][2][0] =='-' ){
            char mesg_out[MAXLINE] = "=new_show=\0";
            int ID = i;
            for(int j = 0 ; j < MAXNUM ; j++){
                if(strcmp(user_online[j][0],"enable") == 0){
                    continue; 
                }
                if(j == ID){
                    char tmp[100];
                    sprintf(tmp,"\x1b[;31;1m%2d  | %6s | %s | %s\x1b[0m\n",j,user_online[j][0],user_online[j][1],user_online[j][2]);
                    strcat(mesg_out,tmp);
                }
                else
                {
                    char tmp[50];
                    sprintf(tmp,"%2d  | %6s | %s | %s\n",j,user_online[j][0],user_online[j][1],user_online[j][2]);
                    strcat(mesg_out,tmp);   
                }
            }
            printf("%s\n",mesg_out);
            Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &all_cliaddr[ID], sizeof(all_cliaddr[ID]));
        }
    }
}
int main(int argc, char **argv)
{
	int					listenfd, connfd, udpfd, nready, maxfdp1;
	char				mesg[MAXLINE],mesg_out[MAXLINE] = "enn";
	pid_t				childpid;
	fd_set				rset;
	ssize_t				n;
	socklen_t			len;
	const int			on = 1;
	struct sockaddr_in	cliaddr, servaddr;
	void				sig_chld(int);
    int all_table[200][3][3];
    int who[200][3];//0 = 換誰 1=1的ID  2 = 2的ID
    int num_user_online = 0;
    int num_played_table = 0;
    struct sockaddr_in all_cliaddr[MAXNUM];
    char PASSWORD[MAXNUM][2][10]=
    {
        {"aa","aaa"},
        {"bb","bbb"},
        {"cc","ccc"},
        {"dd","ddd"},
        {"ee","eee"},
        {"ff","fff"},
        {"gg","ggg"},
        {"hh","hhh"},
        {"ii","iii"}
    };
    char user_online[MAXNUM][3][20];//no,ip,room    
    for(int i = 0 ; i < MAXNUM ; i++){
        sprintf(user_online[i][0],"enable");
    }
		/* 4create UDP socket */
	udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(udpfd, (SA *) &servaddr, sizeof(servaddr));
/* end udpservselect01 */

/* include udpservselect02 */
	//Signal(SIGCHLD, sig_chld);	/* must call waitpid() */

	FD_ZERO(&rset);
	maxfdp1 = max(listenfd, udpfd) + 1;
    table_initialization(all_table[0]);
	for ( ; ; ) {
		//FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
        printf("udpfd:%d\n",udpfd);
		if ( (nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				err_sys("select error");
		}

		//if (FD_ISSET(listenfd, &rset)) {
		//	len = sizeof(cliaddr);
		//	connfd = Accept(listenfd, (SA *) &cliaddr, &len);
          //  printf("lol\n");
			//if ( (childpid = Fork()) == 0) {	/* child process */
			//	Close(listenfd);	/* close listening socket */
			//	str_echo(connfd);	/* process the request */
			//	exit(0);
			//}
			//Close(connfd);			/* parent closes connected socket */
		//}

		if (FD_ISSET(udpfd, &rset)) {
			len = sizeof(cliaddr);
            memset(mesg,0,MAXLINE);
			n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *) &cliaddr, &len);
            //-----------------------------------------------------------------------
            //long size_now_buf =read(sockfd,buf,MAXLINE);
            //printf("%s:%d------>serv     \n",ipstr,port);
            char IP_PORT[20];
            sprintf(IP_PORT,"%s:%d",inet_ntoa(cliaddr.sin_addr),htons(cliaddr.sin_port));
            printf("---------------\nIP:%s\n",IP_PORT);
            printf("mesg:%s\n",mesg);
            memset(mesg_out,'\0',MAXLINE);
            if(strncmp(mesg,"=login=",7) == 0){
                char* tmp = strtok(mesg,"_");
                char acc[10],pass[10];
                sprintf(acc,"%s",tmp+7);
                tmp = strtok(NULL,"_");
                sprintf(pass,"%s",tmp);
                printf("acc:%s\n",acc);
                printf("pass:%s\n",pass);
                int login = 0;
                for(int i = 0 ; i < MAXNUM ; i ++){
                    if(strcmp(acc,PASSWORD[i][0]) == 0){
                        if(strcmp(pass,PASSWORD[i][1]) == 0){
                            int ID;
                            for(int j = 0 ; j < MAXNUM ; j++){
                                if(strcmp(user_online[j][0],"enable") == 0){
                                    ID=j;
                                    break;
                                }
                            }
                            //int ID = num_user_online;
                            num_user_online++;
                            sprintf(user_online[ID][0],"%s",acc);
                            sprintf(user_online[ID][1],"%s:%d",inet_ntoa(cliaddr.sin_addr),htons(cliaddr.sin_port));
                            sprintf(user_online[ID][2],"%d",-1);
                            printf("new user:%s\n%s\n",user_online[ID][0],user_online[num_user_online-1][1]);
                            sprintf(mesg_out,"success");
                            all_cliaddr[ID] = cliaddr;
                            Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &cliaddr, sizeof(cliaddr));
                            login = 1;
                            all_show(user_online,all_cliaddr,udpfd);
                        }
                    }
                    
                }
                if(login == 0){
                        show_all_user(PASSWORD);
                        sprintf(mesg_out,"FAQ");
                        Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &cliaddr, sizeof(cliaddr));
                }
            }
            else if(strncmp(mesg,"=show=",6) == 0){
                int ID = find_by_ip(IP_PORT,user_online);
                printf("==show==\n");
                mesg_out[0] = '\0';
                for(int i = 0 ; i < MAXNUM ; i++){
                    if(strcmp(user_online[i][0],"enable") == 0){
                        continue; 
                    }
                    if(i == ID){
                        char tmp[50];
                        sprintf(tmp,"\x1b[;31;1m%2d  | %6s | %s | %s\x1b[0m\n",i,user_online[i][0],user_online[i][1],user_online[i][2]);
                        strcat(mesg_out,tmp);
                    }
                    else
                    {
                        char tmp[50];
                        sprintf(tmp,"%2d  | %6s | %6s | %s\n",i,user_online[i][0],user_online[i][1],user_online[i][2]);
                        strcat(mesg_out,tmp);   
                    }
                }
                printf("%s\n",mesg_out);
                Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &cliaddr, sizeof(cliaddr));
            }
            else if(strncmp(mesg,"=invite=",8) == 0){
                int invite_id = atoi(mesg+8);
                int now_id = find_by_ip(IP_PORT,user_online);
                char invite_mesg[MAXLINE];
                sprintf(invite_mesg,"=invitefrom=%d",now_id);
                Sendto(udpfd, invite_mesg, strlen(invite_mesg), 0, (SA *) &all_cliaddr[invite_id], sizeof(all_cliaddr[invite_id]));
            }
            else if(strncmp(mesg,"=acceptinvite=",14) == 0){
                int invite_id =find_by_ip(IP_PORT,user_online) ;
                int now_id = atoi(mesg+14);    
                int room = num_played_table;
                //printf("==invite==\n%d invite %d\n",now_id,invite_id);
                table_initialization(all_table[room]);
                sprintf(user_online[now_id][2],"%d",room);
                who[room][1] = now_id;
                sprintf(user_online[invite_id][2],"%d",room);
                who[room][2]  = invite_id;
                who[room][0] = 1;
                num_played_table ++;
                sprintf(mesg_out,"=invite_ok=");
                Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &all_cliaddr[now_id], sizeof(all_cliaddr[now_id]));
                char tmp[200];
                show_table(all_table[room],tmp);
                sprintf(mesg_out,"=wait=%s",tmp);
                Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &all_cliaddr[invite_id], sizeof(all_cliaddr[invite_id]));
                all_show(user_online,all_cliaddr,udpfd);
            }
            else if(strncmp(mesg,"=play=",6) == 0){
                int ID = find_by_ip(IP_PORT,user_online);
                int room = atoi(user_online[ID][2]);
                printf("ID %d wantto play, room is %d\n",ID,room);
                if(who[room][1] == ID){
                    who[room][0] = 1;
                }
                else
                {
                    who[room][0] = 2;
                }
               char tmp[MAXLINE];
                show_table(all_table[room],tmp);
                sprintf(mesg_out,"your_turn%s",tmp);
                Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &cliaddr, sizeof(cliaddr));
            }
            else if(strncmp(mesg,"=set=",5) == 0){
                int ID = find_by_ip(IP_PORT,user_online);
                int room = atoi(user_online[ID][2]);
                int xy = atoi(mesg+5);
                int x = xy/10, y = xy%10;
                char tmp[200];
                int next = (who[room][0]==1)?2:1;
                int now_id = who[who[room][0]];
                int next_id = who[room][next];
                int ans = OOXX(who[room][0],all_table[room],x,y,tmp);
                //show_table(all_table[0]);
                //printf("mesg_out:\n%s",mesg_out);
                printf("ans:%d\n",ans);
                who[room][0] = next;
                if(ans != 0){
                    int winner = who[room][ans];
                    int loser = (ans==1)?2:1;
                    sprintf(mesg_out,"=win=");
                    Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &cliaddr, sizeof(cliaddr));
                    sprintf(mesg_out,"=lose=");
                    Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &all_cliaddr[who[room][loser]], sizeof(all_cliaddr[who[room][loser]]));                
                }
                else
                {
                    sprintf(mesg_out,"=wait=%s",tmp);
                    Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &cliaddr, sizeof(cliaddr));
                    sprintf(mesg_out,"your_turn%s",tmp);
                    Sendto(udpfd, mesg_out, strlen(mesg_out), 0, (SA *) &all_cliaddr[next_id], sizeof(all_cliaddr[next_id])); 
                }
                
                       
            }
            else if(strncmp(mesg,"=logout=",8) == 0){
                int ID = find_by_ip(IP_PORT,user_online);
                sprintf(user_online[ID][0],"enable");
                all_show(user_online,all_cliaddr,udpfd);
            }
            
            else{
                int xy = atoi(mesg);
                int x = xy/10;
                int y = xy%10;
            }
        }
	}
}
/* end udpservselect02 */