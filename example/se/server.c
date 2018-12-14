#define _XOPEN_SOURCE 700

/** File reception server via TCP **/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>


int portnb;

int process_request(int sock) {

  char filename[24];
  char* content;
  int fd, size = 1;  

  /*** Acquire filename ***/
  if (read(sock, filename, sizeof(char)*24) < 0) {
    perror("read");
    exit(1);
  }

  printf("Opening / Creating file %s\n", filename);

  if ((fd = open(filename, O_WRONLY|O_TRUNC|O_CREAT, 0666)) < 0) {
    perror("open");
    exit(1);
  }


	/*** Process file content ***/
  
  content = malloc(1024);
  while(size > 0) {
    if ((size = read(sock, content, 1024)) < 0) {
      perror("read");
      exit(1);
    }

    if (write(fd, content, size) == -1) { 
      perror("write");
      exit(2);
    }
  }
	  
  printf("content2\n");
  /* Close connection */
  shutdown(sock,2);
  close(sock);
    return 0;
}
	


int main(int argc, char *argv[])
{
  struct sockaddr_in sin;   /* Name of the connection socket */
  struct sockaddr_in exp;   /* Name of the client socket */
  int sc ;                  /* Connection socket */
  int scom;                 /* Communication socket */
  socklen_t fromlen = sizeof (exp);
  int i;
  

  if (argc != 2) {
    fprintf(stderr, "Usage : %s port\n", argv[0]);
    exit(1);
  }

 
  /* socket creation */
  if ((sc = socket(AF_INET,SOCK_STREAM,0)) < 0) {
    perror("socket");
    exit(1);
  }
    
  memset((void*)&sin, 0, sizeof(sin));
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(atoi(argv[1]));
  sin.sin_family = AF_INET;

  int reuse = 1;
  setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); 


  /* socket naming */
  if (bind(sc,(struct sockaddr *)&sin,sizeof(sin)) < 0) {
    perror("bind");
    exit(2);
  }
  
  listen(sc, 5);

  /* main loop */
	
  for (i = 0;i < 4; i++) {
    if ( (scom = accept(sc, (struct sockaddr *)&exp, &fromlen))== -1) {
      perror("accept");
      exit(2);
    }
    
	  process_request(scom);
  }

  close(sc);

  return 0;
}
