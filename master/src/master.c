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
#include <semaphore.h>

char * ports[10];
int port_available[10];
sem_t *sem_workers;
/* load ports from the file */
/* TO DO */
/* two semaphore mutex */
pthread_mutex_t mutex_tr = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t mutex_acq_port = PTHREAD_MUTEX_INITIALIZER; 

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

typedef struct for_dep {
              int position;
              int rep_num;
          } for_dep;
/* end of data structure */
int establish_connection(char * prt){
    struct sockaddr_in dest; /* Server name */
      struct addrinfo *result;
      int sock, size = 1;
      char* ip;
      ip= "localhost";
      char* port;
      port =prt;
      

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

      if (getaddrinfo("localhost", port, &hints, &result) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
      }

      memset((void *)&dest,0, sizeof(dest));
      memcpy((void*)&((struct sockaddr_in*)result->ai_addr)->sin_addr,(void*)&dest.sin_addr,sizeof(dest));
      dest.sin_family = AF_INET;
      dest.sin_port = htons(atoi(port));
      

        printf("reconnected\n");
      /* Establish connection */
      
      if (connect(sock, (struct sockaddr *) &dest, sizeof(dest)) == -1) {
        perror("connect");
        exit(1);
      }
      return sock;

}
int send_data(int sock, char* request) {
      printf("sending %s\n", request);

      if (send(sock, request, strlen(request)+1,0) == -1) { 
        perror("write fname");
        exit(2);
      }  
      
      return sock;
}

int close_socket(int sock) {
        /* Close connection */
      shutdown(sock,2); 
      close(sock);
      return sock;
  
}
/* establish connection and close connection part */




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
      content = malloc(5024);
        if ((size = read(fd, content, 5024)) < 0) {
          perror("read");
          exit(1);
        }
        if (send(sock, content, size+1,0) == -1) { 
          perror("write content");
          exit(2);
        }
        printf("content3\n");
      return sock;

}
/* end of connection part*/

/* Add loading port numbers from the files later */
int load_workers(){
       char * ports_temp[10] = {"8890","8891","8892","8893","8895", "8896","8897", "8898", "8899", "8900"};
       int k;
       for(k=0;k<10;k++){
         port_available[k]= 1;
         ports[k]= ports_temp[k];
       }
       return 0;
}

void * worker(void * send){
  for_dep s = *(for_dep *)send;
  printf("Yayy worker time to do your job now. %d\n", s.position);
  printf("Yayy worker time to do your job now. %d\n", s.rep_num);
  
  /*  wait till a worker is available. acquire port number */
  sem_wait(sem_workers);
  char* port_to_use;
  int port_at;
  int k;
  pthread_mutex_lock(& mutex_acq_port);
          for(k=0;k<10;k++){
            if(port_available[k]==1){
              port_at=k;
              break;
            }
          }
          port_available[port_at]=0;
  pthread_mutex_unlock(& mutex_acq_port);
  port_to_use= ports[port_at];
  /* establish connection with the worker using the port number */



  

  /* seen make file and code */



  /* wait for the response from the worker */ 

  /*  update all the varaibles associated with the request*/
  sem_post(sem_workers);

  /* exit the thread */
  pthread_exit((void *) 0);

}

int deploy_on_worker(int sock, char* job_ticket, int position){
    printf("Deployed \n");
    /* Use the position to extract right element  */
    int k;
    /* Acquire number of repliacas availle*/
    struct for_dep *send;
    for(k=0; k<requests[position].num_rep; k++){
          /* call worker, in thread, that will put one replica on one worder */
          send =(struct for_dep *) malloc(sizeof(struct for_dep));
          pthread_t tid;
          send->position= position;
          send->rep_num= k;

          if(pthread_create(&tid,NULL, worker, (void *)send)!=0){
              printf("pthread_create \n");
              exit(1);}
              printf("We are in worker thread. \n");
          }

      
    
    

    /*  wait until every worker is done with their work using p_thread*/

  return 0; 

}


void * deploy(void * sck) {
  int sock = *(int *)sck;
  printf("Communication socket  %d\n", sock);

  char job_ticket[24];
  

  char* content;
  char r[24];
  int size = 1;  

    /*** Acquire job ticket ***/
  if (recv( sock, job_ticket, sizeof(char)*24, 0) < 0) {
    perror("read");
    exit(1);
  }

  printf("Acquiring job ticket  %s\n", job_ticket);

  if (recv(sock, r, sizeof(char)*24,0) < 0) {
            perror("read");
            exit(1);
        }
  printf("Total replicas %s\n", r);



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
        int position;
        pthread_mutex_lock(& mutex_tr);
        position= total_requests;
        total_requests= total_requests+1;
        pthread_mutex_unlock(& mutex_tr);
  /* update requests array */
        requests[position]= temp;

  
  content = malloc(5024);

    if ((size = recv(sock, content, 5024,0)) < 0) {
      perror("read");
      exit(1);
    }
  printf("make file  %s\n", content);
  requests[position].make_file= content;

  
  content = malloc(5024);

    if ((size = recv(sock, content, 5024,0)) < 0) {
      perror("read");
      exit(1);
    }

  printf("code  %s\n", content);
  requests[position].data= content;

  /* Ask workers to start work */
  sock =deploy_on_worker(sock, job_ticket, position);
            /* Close connection */
	  
  printf("Deployment Done. \n");
  shutdown(sock,2);
  close(sock);
  pthread_exit((void *) 0);
}

void * result(void * sck) {
  int sock = *(int *)sck;
  printf("Communication socket  %d\n", sock);

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

void * status(void * sck) {
  int sock = *(int *)sck;
  printf("Communication socket  %d\n", sock);

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
  	/* create mutex with # of workers as an input. */
	sem_workers= sem_open("mysem_c", O_CREAT | O_RDWR, 0666, 10);
  int a= load_workers();
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
	   }
    if(strcmp(request_type,"status") == 0){
      if(pthread_create(&tid,NULL, status, (void *)pt_scom)!=0){
        printf("pthread_create \n");
        exit(1);}
        printf("We are in the thread. \n");
	 }
    if(strcmp(request_type,"result") == 0){
      if(pthread_create(&tid,NULL, result, (void *)pt_scom)!=0){
        printf("pthread_create \n");
        exit(1);}
        printf("We are in the thread. \n");
	  }
    
  }

  close(sc);

  return 0;
}
