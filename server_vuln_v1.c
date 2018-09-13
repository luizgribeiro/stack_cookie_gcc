#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>


void copyVuln(char *msg, int size){
	char input[32];

	memcpy(input, msg, size);
}

void sendReply(int sockfd){
	int n; 
	n = write(sockfd, "I got your message\n", 19);
	if (n < 0) perror("ERROR writing to socket\n");
}


int main(int argc, char *argv[]){

	int sockfd, newsockfd, portno, clilen, len;
	char buffer[256] ;
	struct sockaddr_in serv_addr, cli_addr;
	pid_t pid;

	if (argc < 2 ){
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}else{
		portno = atoi(argv[1]);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) perror ("ERROR, opening socket\n");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));


	serv_addr.sin_family = AF_INET;

	serv_addr.sin_port = htons(portno);

	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) perror("ERROR on binding\n");


	listen(sockfd, 5);

	clilen = sizeof(cli_addr);
  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) perror("ERROR on accept");
    pid = fork();
    if (pid < 0) perror("ERROR on fork");
    if (pid == 0)  {
//      close(sockfd);
      memset(&buffer, 0, sizeof(buffer));
      len = recv(newsockfd, buffer, sizeof(buffer), 0);
	  
      copyVuln(buffer, len);
      sendReply(newsockfd);
      exit(0);
    }
        else close(newsockfd);
  }  close(sockfd);
  return 0;}
