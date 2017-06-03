#include "inet.h"


void menu()
{

    printf("\nChoose one of the following options:\n");
    printf("1. View Catalog\n2. Booking\n3. View cart\n0. Logout\n");

}

int main(int argc, char *argv[]) { //argument count & argument vector, passes command line arguments to main

    int sockfd,numbytes;
    char buffer[BUFSIZE];
    char recBuf[BUFSIZE]; // string in which recieve message will go
    char sndBuf[BUFSIZE]; // string in which sending message will go
    char tmp[BUFSIZE];	  // temporarty buffer to hold booking info
    char menuChoice[BUFSIZE]; // holds the menu
    char *word; // used to store and retrieve car details??
    struct hostent *he; //getting address of the client??
    struct sockaddr_in their_addr; // connector's address information
    if(argc != 2) { //if client doesnt put 2 arguments, error.?
        fprintf(stderr,"\nhow to use: ./client remoteIP ");
        exit(1);
    }
    if((he = gethostbyname(argv[1])) == (void *)0) { // gets info of the host
        printf("\ngethostbyname error\n");
        exit(1);
    }
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) { //creating socket
        printf("\nsocket error\n");
        exit(1);
    }
	// initializing the socket structure
    bzero(&(their_addr),sizeof(their_addr)); //writes specified No. of Null bytes to specific destination - is their_addr server_addr?
    their_addr.sin_family = AF_INET; //Initialize the socket family/domain to AF_INET (TCP/IP)
    their_addr.sin_port = htons(PORT); // host to network byte conversion, short - what would happen if we use long??
    their_addr.sin_addr = *((struct in_addr *)he->h_addr); //used to get host by name (maybe get host)??
    bzero(&(their_addr.sin_zero),8); // zero the rest of the struct

    if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) { //Connecting to server
        printf("connect error\n");
        exit(1);
    }
    printf("*****************Welcome To TCP/IP Car Dealers********************\n");
    do
    {
        bzero(menuChoice,sizeof(menuChoice)); //empties buffer for menu choice
        menu();
        fgets(menuChoice,BUFSIZE,stdin); // gets the menu - stdin is used to enter keyboard??
        if((send(sockfd,menuChoice,BUFSIZE,0)) == -1) { // if theres an error in getting the menu display following message
            printf("send error\n");
            exit(1);
        }
        int ch = atoi(menuChoice); // c library function that converts string argument to integer
        switch(ch)
        {
        case 1: // to retrieve catalog
            system("clear");
            bzero(recBuf,sizeof(recBuf)); //empties recieve buffer according to the size of recieve buffer
            if(recv(sockfd,recBuf,sizeof(recBuf),0)==-1) {
                printf("receive error\n");
                exit(1);
            }
            word = NULL;
            word = strtok(recBuf,","); // retrieve first word
            while(word) // the loop gets the rest of the words until the end of the message
            {
                printf("***********************************************");


                printf("\nCar id		:%s",word);
                word = strtok(NULL,","); // whats the function of strtok??
                printf("\nCar Make	:%s",word);
                word = strtok(NULL,",");
                printf("\nCar Model	:%s",word);
                word = strtok(NULL,",");
                printf("\nYear		:%s",word);
                word = strtok(NULL,",");
                printf("\nPrice(RM)	:%s",word);
                word = strtok(NULL,",\n");
                printf("\nStatus		:%s\n",word);
                word = strtok(NULL,",\n");
            }
            break;

        case 2: // booking a car
            system("clear");
            bzero(recBuf,sizeof(recBuf)); // empty recbuf
            bzero(tmp,sizeof(tmp)); // empty temp buf 
            if(recv(sockfd,recBuf,sizeof(recBuf),0)==-1) { // if any error in recieving booking data, print error
                printf("receive error\n");
                exit(1);
            }
            write(1,recBuf,strlen(recBuf)); // writes booking info in recbuf using the size of recbuf
            fgets(tmp,BUFSIZE,stdin); // stores booking info in temporary buffer
            if((send(sockfd,tmp,BUFSIZE,0)) == -1) { // sending booking data to server
                printf("send error\n");
                exit(1);
            }
            bzero(recBuf,sizeof(recBuf));
            if(recv(sockfd,recBuf,sizeof(recBuf),0)==-1) { // recieveing booking data from server
                printf("receive error\n");
                exit(1);
            }
            write(1,recBuf,strlen(recBuf)); // writing booking data recieved from server
            break;
			
        case 3: // view cart
            system("clear");
            bzero(recBuf,sizeof(recBuf)); 
            if(recv(sockfd,recBuf,sizeof(recBuf),0)==-1) {
                printf("receive error\n");
                exit(1);
            }
		
            word = NULL;
            word = strtok(recBuf,",");
            while(word)
            {
                printf("***********************************************");


                printf("\nCar id		:%s",word);
                word = strtok(NULL,",");
                printf("\nCar Make	:%s",word);
                word = strtok(NULL,",");
                printf("\nCar Model	:%s",word);
                word = strtok(NULL,",");
                printf("\nYear		:%s",word);
                word = strtok(NULL,",\n");
                printf("\nPrice(RM)	:%s\n",word);
                word = strtok(NULL,",");

            }
            printf("\n");
            printf("Choose one option\n");
            printf("[1]. Confirm\n[2]. Cancel <this will erase your cart>\n");
            char ch[3];
            fgets(ch,3,stdin);
            if((send(sockfd,ch,sizeof(ch),0)) == -1) {
                printf("send error\n");
                exit(1);
            }

            break; 
        }

    } while(strncmp(menuChoice,"0",1)); // compares the menu choice with the entered choice - logout
    close(sockfd); // end socket connection
    return 0;

}

