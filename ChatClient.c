#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <netdb.h>

void* sendthread(void *arg);
void* recvthread(void *arg);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in srvr_addr;
    struct hostent *srvr;
    pthread_t tid_send, tid_recv;

    /*check for the correct number of arguments*/
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s username\n", argv[0]);
        exit(0);
    }
    /*create the socket and check for errors*/
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }
    
    /*define the server to be connected to*/
    srvr_addr.sin_family = AF_INET;
    srvr_addr.sin_port = htons(atoi("2020")); /*'2020' is the predefined port to be used. This will be subject to change for other applications.*/
    srvr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvr = gethostbyname("tux3.cs.drexel.edu"); /*The server name will be different for different applications.*/
    memmove(&(srvr_addr.sin_addr), srvr->h_addr_list[0], srvr->h_length);
    
    /*connect to the server and check for errors*/
    if((connect(sock, (struct sockaddr *) &srvr_addr, sizeof(srvr_addr))) < 0)
    {
        perror("Failed to connect to the server");
        exit(1);
    }
    
    /*send username to the server*/
    send(sock, argv[1], sizeof(argv[1]), 0);
    
    /*create the threads for sending and receiving*/
    if(pthread_create(&tid_send, NULL, &sendthread, &sock) != 0)
    {
        perror("Failed to create 'send' thread");
        exit(1);
    }
    if(pthread_create(&tid_recv, NULL, &recvthread, &sock) != 0)
    {
        perror("Failed to create 'receive' thread");
        exit(1);
    }
    
    /*exit the threads*/
    pthread_exit(&tid_send);
    pthread_exit(&tid_recv);
    return 0;
}

void* sendthread(void *arg)
{
    char sendbuf[128];
    char *n;
    int sockid = *(int *) arg;

    while((n = fgets(sendbuf, sizeof(sendbuf), stdin)) != NULL)
    {
        send(sockid, sendbuf, sizeof(sendbuf), 0);
    }
    return arg;
}

void* recvthread(void *arg)
{
    char recvbuf[128];
    int n;
    int sockid = *(int *) arg;
    
    while((n = recv(sockid, recvbuf, sizeof(recvbuf), 0)) > 0)
    {
        printf("%s", recvbuf);
    }
    return arg;
}