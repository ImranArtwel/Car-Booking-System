#include "inet.h"


struct msgbuf //messga queue
{
    long mtype;
    char mtext[BUFSIZE]; 
};
struct msgbuf sbuf;
struct msgbuf recvBuf;

static int sedanRev=0;
static int execRev=0;
int sockfd, new_sockfd,len_size;
struct sockaddr_in my_addr;
struct sockaddr_in their_addr;
int sin_size,fd,nbytes;
struct sigaction sa;
int yes=1;
char cliChoice[BUFSIZE];
char sndBuf[BUFSIZE];
char recBuf[BUFSIZE];
char cart[BUFSIZE];
char cliAdd[BUFSIZE];
void sigchld_handler(int sig);

void connection()
{
    if((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Server: socket() error\n");
        exit(1);
    }
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1) {
        perror("\nsetsockopt() error!!!!\n");
        exit(1);
    }

    bzero((char *) &my_addr,sizeof(my_addr));//write 0 bytes to struct
    my_addr.sin_family = AF_INET;// IP family
    my_addr.sin_addr.s_addr = INADDR_ANY;//server takes up dynamic ip
    my_addr.sin_port = htons(MYPORT);//server port coverting host byte order
    bzero(&(my_addr.sin_zero),8);

    if(bind (sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr)) < 0) { //binding socket to server
        perror("Server: bin() error\n");
        exit(1);
    }
    if(listen(sockfd,BACKLOG) == -1) {
        perror("\nlisten() error");
        exit(1);
    }
    printf("\nwaiting for client  connection!!!!\n");
}
void addToCart(char *car)
{
    FILE *fp,*fp2;
    char recBuf[BUFSIZE];

    char *carid,*carMake,*carModel,*carYear,*carPrice,*carStatus;
    int carFound=0;

    fp=fopen(CATALOG,"r"); //open catalog file for reading
    if(fp==NULL)
    {
        printf("Error in opening file..!!\n");
        exit(1);
    }
    bzero(recBuf,sizeof(recBuf));
    while(fgets(recBuf,sizeof(recBuf),fp)!=NULL) // read from file and put in recBuf
    {

        carid = strtok(recBuf,","); // get car id
        if(!strncmp(carid,car,2)) // comparing car id from database with one from user
        {
            carFound = 1;
            carMake = strtok(NULL,",");
            carModel=strtok(NULL,",");
            carYear=strtok(NULL,",");
            carPrice=strtok(NULL,",");
            carStatus=strtok(NULL,",");
            if(!strncmp(carStatus,"out",3)) // make sure client doesnt book a car that is not available
            {

                if ((send(new_sockfd,"car not available for booking\n",100, 0))==-1) {
                    printf("sendto() error\n");
                    exit(1);
                }
                carFound = -1;
                break;
            }


            sprintf(cart,"%s,%s,%s,%s,%s\n",carid,carMake,carModel,carYear,carPrice); // data in cart will follow this format
            fprintf(fp,"%s\n",cart); //write to cart

        }

    }
    fclose(fp);

    if(!carFound) {
        if((send(new_sockfd,"Sorry..Car not found",50,0)) == -1) {
            printf("send error\n");
            exit(1);
        }
    }
    else if(carFound == 1)
    {
        if((send(new_sockfd,"Car added to cart",50,0)) == -1) {
            printf("send error\n");
            exit(1);
        }
        fp2 = fopen(CART,"a");
        fputs(cart,fp2);
        fclose(fp2);
    }
}

void confirmCart(int msgid,long mtype)
{

    bzero(cliAdd,sizeof(cliAdd));
    if(msgrcv(msgid,&recvBuf,BUFSIZE,mtype,0) < 0) // get msg type and msqid from msg queue
    {
        printf("msg receive error\n");
        exit(1);
    }
    strcpy(cliAdd,recvBuf.mtext); // copy message to cliAdd[]
    char *cmd[100];                /*  ssh to the client */
    cmd[0] = "ssh ";
    cmd[1] = cliAdd;
    cmd[2] = NULL;

    execv("/usr/bin/ssh",cmd);
}
int main(int argc,char **argv) {


    int msqid;
    int msgflg = IPC_CREAT | 0666; // message queue flags
    key_t key = 1234;
    sbuf.mtype =1; // set message type

    if((msqid=msgget(key,msgflg)) < 0)
        printf("error creating msg queue\n");
    else
        connection();// create TCP connection
    sa.sa_handler =sigchld_handler; // setup signal handler??
    sigemptyset(&sa.sa_mask); // empty signal set??
    sa.sa_flags = SA_RESTART; //Restart functions if interrupted by handler
    if(sigaction(SIGCHLD, &sa,NULL) == -1) { // usng signals in SIGCHLD??
        printf("\nsigaction error\n");
        exit(1);
    }
    for(;;) {
        sin_size = sizeof(struct sockaddr_in);
        if((new_sockfd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size)) == -1) {
            printf("\naccept() error\n");
            exit(1);
        }
        printf("server: connection from %s \n",inet_ntoa(their_addr.sin_addr));
        strcpy(sbuf.mtext,inet_ntoa(their_addr.sin_addr)); // copy the client address to buffer
        if(msgsnd(msqid,&sbuf,strlen(sbuf.mtext)+1,IPC_NOWAIT) < 0) // send message to message queue
            printf("error sending message\n");


        if(!fork()) { //making child process??
            close(sockfd); //close listening socket

            do {
                bzero(cliChoice,sizeof(cliChoice));
                if((recv(new_sockfd,cliChoice,BUFSIZE,0)) == -1) {
                    printf("receive error\n");
                    exit(1);
                }

                int ch = atoi(cliChoice);
                switch(ch)
                {
                case 1:
                    bzero(sndBuf,sizeof(sndBuf));
                    fd = open(CATALOG,O_RDONLY);
                    nbytes = read(fd,sndBuf,sizeof(sndBuf));
                    sndBuf[nbytes] ='\0'; // '/0' is a string terminatior. to show where the string ends
                    if((send(new_sockfd,sndBuf,nbytes,0)) == -1) {
                        printf("send error\n");
                        exit(1);
                    }
                    close(fd);
                    break;

                case 2:
                    bzero(recBuf,sizeof(recBuf));
                    if((send(new_sockfd,"Enter car id:\t",20,0)) == -1) {
                        printf("send error\n");
                        exit(1);
                    }
                    if((recv(new_sockfd,recBuf,BUFSIZE,0)) == -1) {
                        printf("receive error\n");
                        exit(1);
                    }

                    addToCart(recBuf);
                    break;
                case 3:
                    fd = open (CART,O_RDONLY);
                    bzero(sndBuf,sizeof(sndBuf));
                    nbytes = read(fd,sndBuf,sizeof(sndBuf));
                    sndBuf[nbytes] ='\0';


                    if(nbytes <=2)
                    {
                        if((send(new_sockfd,"Your cart is empty\n",20,0)) == -1) {
                            printf("send error\n");
                            exit(1);
                        }
                        break;
                    }

                    if((send(new_sockfd,sndBuf,nbytes-1,0)) == -1) { //??
                        printf("send error\n");
                        exit(1);
                    }
                    close(fd);
                    bzero(recBuf,sizeof(recBuf));
                    if((recv(new_sockfd,recBuf,BUFSIZE,0)) == -1) { //??
                        printf("receive error\n");
                        exit(1);
                    }

                    if(!strncmp(recBuf,"1",1)) // check if client selected confirm
                    {
                        confirmCart(msqid,sbuf.mtype);
                    }
                    else if(!strncmp(recBuf,"2",1))// check if client selected cancel
                    {
                        FILE *fp;
                        fp = fopen(CART,"w"); /* clear the contents of file */
                        fprintf(fp,"%s"," ");
                        fclose(fp);
                    }
                    break;
                }

            } while(strncmp(cliChoice,"0",1));
            close(new_sockfd);
            printf("Connection to client closed\n");
            msgctl(msqid, IPC_RMID, NULL); //deleting message queue - mesage queue stores th eip of the intended client and message queue ID??
            exit(0);
        }

    }
    return 0;
}
void sigchld_handler(int sig) { // wait for the child process to finish??
    wait((int *)0);
}
 