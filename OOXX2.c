#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char mesg[4096];
char Q[4] = {" OX"};
char* mesg_test = "\033[2J\n\033[1;1H";//11
//char* mesg_test = "\0";//11
int show_table(int table[][3],char mesg_out[]){
    printf("mesg:%s\nlen=%d\n",mesg_test,strlen(mesg_test));
    printf("%s",mesg_test);
    strcpy(mesg,mesg_test);
    char m[3][3][4];
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 3 ; j++){
            m[i][j][0] = ' ';
            m[i][j][1]= Q[table[i][j]];
            m[i][j][2] = ' ';
            m[i][j][3] = '\0';
        }
    }
    //printf("-------------\n");
    strcat(mesg,"-------------\n");
    
    //printf("|%s|%s|%s|\n",m[0][0],m[0][1],m[0][2]);
    char temp[256];
    sprintf(temp,"|%s|%s|%s|\n",m[0][0],m[0][1],m[0][2]);
    strcat(mesg,temp);
    //printf("-------------\n");
    strcat(mesg,"-------------\n");
    //printf("|%s|%s|%s|\n",m[1][0],m[1][1],m[1][2]);
    sprintf(temp,"|%s|%s|%s|\n",m[1][0],m[1][1],m[1][2]);
    strcat(mesg,temp);
    //printf("-------------\n");
    strcat(mesg,"-------------\n");
    //printf("|%s|%s|%s|\n",m[2][0],m[2][1],m[2][2]);
    sprintf(temp,"|%s|%s|%s|\n",m[2][0],m[2][1],m[2][2]);
    strcat(mesg,temp);
    //printf("-------------\n");
    strcat(mesg,"-------------\n");
    printf("%s",mesg);
    memset(mesg_out,'\0',MAXLINE);
    strcpy(mesg_out,mesg);
    printf("mesg_out:\n%s",mesg_out);
}

int Q_win(int table[][3]){
    for(int i = 0 ; i < 3 ; i++){
        if(table[i][0] != 0 && table[i][0] == table[i][1] && table[i][1] == table[i][2]){
            return table[i][0];
        }
        if(table[0][i] != 0 && table[0][i] == table[1][i] && table[1][i] == table[2][i]){
            return table[0][i];
        }
    }
    if(table[0][0] != 0 && table[0][0] == table[1][1] && table[1][1] == table[2][2])
        return table[0][0];
    if(table[0][2] != 0 && table[0][2] == table[1][1] && table[1][1] == table[2][0])
        return table[0][0];
    return -1;
}

void table_initialization(int table[][3]){
    for(int i = 0 ; i < 3 ; i ++){
        for(int j = 0 ; j < 3 ; j++){
            table[i][j] = 0;
        }
    }
}
int OOXX(int chracter, int table[][3], int x, int y,char mesg_out[]){//return 0 = 無 否則return贏得
    //1 = O , 2 = X
    //int table[3][3];

    int i = 1;
    /*while(1){
        i = i*(-1);
        printf("\r%2d",i);
        //usleep(1);
    }*/
    //show(table);
    //for(int i = 0 ; i < 3 ; i++)
      /*  printf("   |   |   \n-----------\n");
        //printf("\033[47;31m hello world! \033[4m\n");
        //printf("\033[%d;%dHX", 1,1);
        printf("\033[3AX");
        printf("\033[H");*/
    //while (1)
    //{
        //int x,y,z;
        //scanf("%d%d%d",&x,&y,&z);
        int z = chracter;
        table[x][y] = z;
        show_table(table,mesg_out);
        int q_win = Q_win(table);
        if(q_win != -1){
            //printf("%c WIN\n",Q[q_win]);
            return q_win;
        }
        for(int i = 0 ; i < 3 ; i++){
            for(int j = 0 ; j < 3; j++){
                if(table[i][j] == 0)
                    return 0;
            }
        }
    //}
    return -1;
}