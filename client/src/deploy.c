
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define BUFM 1000
#define BUFC 3000


int establish_connection(){
    struct sockaddr_in dest; /* Server name */
      struct addrinfo *result;
      int sock, size = 1;
      char* ip;
      ip= "localhost";
      int port;
      port =8889;
      

      if ((sock = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        perror("socket");
        exit(1);
      }

      /* Find server */
      
      struct addrinfo hints = {};
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
      hints.ai_protocol = 0;

      if (getaddrinfo("localhost", "8889", &hints, &result) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
      }

      memset((void *)&dest,0, sizeof(dest));
      memcpy((void*)&((struct sockaddr_in*)result->ai_addr)->sin_addr,(void*)&dest.sin_addr,sizeof(dest));
      dest.sin_family = AF_INET;
      dest.sin_port = htons(port);
      

        printf("reconnected\n");
      /* Establish connection */
      
      if (connect(sock, (struct sockaddr *) &dest, sizeof(dest)) == -1) {
        perror("connect");
        exit(1);
      }
      return sock;

}

int send_request(int sock, char* request) {
      printf("sending %s\n", request);

      if (send(sock, request, strlen(request)+1, 0) == -1) { 
        perror("write fname");
        exit(2);
      }  
      
      return sock;}


int send_file(int sock, char* file_name) {
      int fd;
      int size=1;
      void* content;
      /* read file */
      if((fd = open(file_name, O_RDONLY, 0666)) == -1) {
        perror("open");
        exit(1);
      }
      
        /* Send file contents */
      content = malloc(1024);
      while(size > 0) {
        if ((size = read(fd, content, 1024)) < 0) {
          perror("read");
          exit(1);
        }
        if (write(sock, content, size) == -1) { 
          perror("write content");
          exit(2);
        }
        printf("content3\n");
      }
      return sock;

}

int close_socket(int sock) {
        /* Close connection */
      shutdown(sock,2); 
      close(sock);
      return sock;
  
}

int main(int argc, char *argv[]){
    /* read inputs */
	int replicas;	
	replicas= atoi(argv[1]);
    printf("%d\n", replicas);

    char *make_file;
    make_file= argv[2];
    printf("%s\n", make_file);

    char *code;
    code= argv[3];
    printf("%s\n", code);

    /* generate job ticket number that can be used to send information to the server. */
    char job_ticket[10];
    int num= rand() % 50000;
    sprintf(job_ticket, "%d", num); 

    /* send request type to the master server. */

    /* send data type. */

    /* send data to the maste server */
    char * data_type[3]= {"replica","make_file", "data"};
    int sock;
    int i;
    for(i=0; i<3;i++){
        sock = establish_connection();
        sock = send_request(sock, "deploy");
        sock = send_request(sock, job_ticket);
        sock = send_request(sock, data_type[i]);
        if(i==0){

            sock = send_request(sock, argv[i+1]);
        }
        else{
            sock = send_file(sock, argv[i+1]);}
        sock=  close_socket(sock);
    }

    printf("Job ticket is %s\n", job_ticket);
    /* return job ticket number */
	return 0;

}