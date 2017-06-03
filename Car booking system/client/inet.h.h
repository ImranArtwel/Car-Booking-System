#include<stdio.h> //  defines three variable types, several macros, and various functions for performing input and output.
#include <stdlib.h> //  defines four variable types, several macros, and various functions for performing general functions.
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h> // miscellaneous symbolic constants and types, and declares miscellaneous functions
#include <sys/stat.h>
#include <fcntl.h> //file control
#include <signal.h> // defines a variable type sig_atomic_t, two function calls, and several macros to handle different signals reported during a program's execution
#include <sys/msg.h> 
#include <string.h> // defines one variable type, one macro, and various functions for manipulating arrays of characters
#include <errno.h> // used for error messages. its set to 0 at prgram startup. is modifyable.
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h> // defines four variable types, two macro and various functions for manipulating date and time
#include <netdb.h> // make available the type in_port_t and the type in_addr_t & hostent

#define BACKLOG 5	
#define BUFSIZE 1024
#define MYPORT 3490 // server port
#define PORT 3490   // client port
#define CATALOG "catalog.txt"
#define CART "cart.txt"
