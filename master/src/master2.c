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

int ports[10];
int port_available[10];

/* load ports from the file */
/* TO DO */
/* two semaphore mutex */
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; 

/* data structures to store all the requests. */
typedef struct rep { 
    int deployed;
    char * status;
    char *result; 
} rep;

typedef struct req { 
    char * job_ticket;
    int num_rep;
    char * make_file;
    char * data;
    struct rep replicas[100]; 
} req;

int total_requests;

struct req requests[10000];
/* end of data structure */

/* send file to the server */
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
        printf("Results back to the client\n");
      }
      return sock;

}
int deploy_on_worker(int sock, char* job_ticket, int position){
    printf("Deployed \n");
  return 0; 

}


void * deploy(void * sck) {
  int sock = *(int *)sck;
  printf("Communication socket  %d\n", sock);

  char data_type[24];
  char job_ticket[24];
  

  char* content;
  char r[24];
  int fd, size = 1;  

    /*** Acquire job ticket ***/
  if (read( sock, job_ticket, sizeof(char)*24) < 0) {
    perror("read");
    exit(1);
  }
  printf("Acquiring job ticket  %s\n", job_ticket);
  /* make new base directory based on the ticket number TODO  
  char* base = concat(job_ticket,"/");
  printf("Job ticket %s\n", job_ticket); */

  /*** Acquire data type ***/
  if (read(sock, data_type, sizeof(char)*24) < 0) {
    perror("read");
    exit(1);
  }

  printf("Data_type %s\n", data_type);

  if(strcmp(data_type,"replica") == 0){
	      /*** Acquire # replicas ***/
        if (read(sock, r, sizeof(char)*24) < 0) {
            perror("read");
            exit(1);
        }
        printf("Total replicas %s\n", r);

        /* make a structure */
        printf("Total replicas2 %c\n", r[0]);
        struct req temp;
        
        temp.job_ticket= job_ticket;
        
        temp.num_rep= atoi(r);
        /* add job ticket, number of replicas, status and rersults. */
        int j;
        for(j=0; j<temp.num_rep; j++){
        temp.replicas[j].status= "NOT RUNNING";
        temp.replicas[j].result="NONE"; 
        temp.replicas[j].deployed=0; }
        /* update the vaiable total_requests in semaphores */
        int temp_request;
        temp_request= total_requests;
        total_requests= total_requests+1;
        /* update requests array */
        requests[temp_request]= temp;
        /*
        char* base = concat(job_ticket,"/");
        request[temp_request].make_file= concat(base,"make_file");
        request[temp_request].data= concat(base,"data.c");
        */
        printf("Total replicas3 %d\n", temp.num_rep);
          shutdown(sock,2);
          close(sock);
        pthread_exit((void *) 0);
        }


  
  
  if ((fd = open(data_type, O_WRONLY|O_TRUNC|O_CREAT, 0666)) < 0) {
    perror("open please");
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

    /* find where to store them the details. */
        int k;
        int position;
        for(k=0; k<total_requests; k++){
            if(strcmp(requests[k].job_ticket,job_ticket) == 0){
                position=k;
                break;
            }
        }
    
    if(strcmp(data_type,"make_file") == 0){ 
        requests[position].make_file= content;
        }
    if(strcmp(data_type,"data") == 0){ 
        requests[position].make_file= content;
          printf("Time to Deploy.\n");
            /* Ask workers to start work */
            sock =deploy_on_worker(sock, job_ticket, position);
            /* Close connection */
        }
	  
  printf("content2\n");
  shutdown(sock,2);
  close(sock);
  pthread_exit((void *) 0);
}

int result(int sock) {

  char job_ticket[24];
  char * content="The result: ";
  
  
  char r[24];
  int fd, size = 1;  


  /*** Acquire job ticket ***/
  if (read(sock, job_ticket, sizeof(char)*24) < 0) {
    perror("read");
    exit(1);
  }

  printf("Job Ticket in status%s\n", job_ticket);


    /* find the position */
    int k;
    int position= -1;
    for(k=0; k<total_requests; k++){
            if(strcmp(requests[k].job_ticket,job_ticket) == 0){
                printf("position%d\n", k);
                position=k;
                break;
            }
        }

    /* access the value */
    /*  open a file */
          if ((fd = open(job_ticket, O_WRONLY|O_TRUNC|O_CREAT, 0666)) < 0) {
                perror("open");
                exit(1);
            }
    if(position!=-1){
        int k;

        for(k=0;k<requests[position].num_rep;k++){
            /* write each result to a file */
                content= requests[position].replicas[k].result;
                if (write(fd, content, sizeof(content)) == -1) { 
                perror("write");
                exit(2);
                }       
        }   
    }
    else{
             if (write(fd, content, sizeof(content)*10) == -1) { 
                perror("write");
                exit(2);
                }}

        /* read from a file and send back to client */ 
        /* send back the results */
        sock = send_file(sock, job_ticket);   

	  
  printf("status\n");
  /* Close connection */
  shutdown(sock,2);
  close(sock);
    return 0;
}

int status(int sock) {

  char job_ticket[24];
  char * content="DONE";
  
  
  char r[24];
  int fd, size = 1;  


  /*** Acquire job ticket ***/
  if (read(sock, job_ticket, sizeof(char)*24) < 0) {
    perror("read");
    exit(1);
  }

  printf("Job Ticket in status%s\n", job_ticket);


    /* find the position */
    int k;
    int position= -1;
    for(k=0; k<total_requests; k++){
        if(strcmp(requests[k].job_ticket,job_ticket) == 0){
                printf("position%d\n", k);
                position=k;
                break;
            }
        }

    /* access the value */
    if(position!=-1){
        int k;
        for(k=0;k<requests[position].num_rep;k++){
            if(strcmp(requests[position].replicas[k].status,"COMPLETED") != 0){
                content= "NOT DONE";
                break;
            }      
        }
        }
        

 
    printf("Content%s\n", content);
    /* send back the result*/
    if (write(sock, content, strlen(content)+1) == -1) { 
            perror("write fname");
            exit(2);
        }

	  
  printf("status\n");
  /* Close connection */
  shutdown(sock,2);
  close(sock);
    return 0;
}



int main(int argc, char *argv[])
{
  total_requests=0;
  struct sockaddr_in sin;   /* Name of the connection socket */
  struct sockaddr_in exp;   /* Name of the client socket */
  int sc ;                  /* Connection socket */
  int scom;                 /* Communication socket */
  socklen_t fromlen = sizeof (exp);
  int i;
  char request_type[24];
  

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
    int *pt_scom;
    
	i=0;
  while (i ==0) {
    if ( (scom = accept(sc, (struct sockaddr *)&exp, &fromlen))== -1) {
      perror("accept");
      exit(2);
    }
        /*** request type ***/
    if (read(scom, request_type, sizeof(char)*24) < 0) {
        perror("read");
        exit(1);
    
    }

    pt_scom =(int *) malloc(sizeof(int));
	  *pt_scom =scom;
    pthread_t tid;
    printf("pt_scom333.  %d\n", *pt_scom);
    printf("Request type done: %s\n", request_type);
    if(strcmp(request_type,"deploy") == 0){

        if(pthread_create(&tid,NULL, deploy, (void *)pt_scom)!=0){
			printf("pthread_create \n");
			exit(1);}
        printf("We are in the thread. \n");
	  /* deploy(scom); */ }
    if(strcmp(request_type,"status") == 0){
	  status(scom);}
    if(strcmp(request_type,"result") == 0){
	  result(scom);}
    
  }

  close(sc);

  return 0;
}
