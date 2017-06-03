#include<stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h>
#include <netdb.h>

#define BACKLOG 5
#define BUFSIZE 1024
#define MYPORT 3490 // server port
#define PORT 3490   // client port
#define CATALOG "catalog.txt"
#define CART "cart.txt"
