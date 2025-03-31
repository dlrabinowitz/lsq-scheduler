/* socket.c

   routines for opening, connecting, sending, and receiving commands
   with unix sockets
 
   DLR 2007 Mar 5
*/

#include "socket.h"

double get_ut();
extern int verbose;
extern int verbose1;

int send_command(char *command, char *reply, char *machine, 
			int port, int timeout_sec);
void init_socket_status(int s,socket_status *s_status);
int read_data(int s, char *buf, int n);
int write_data(int s, char *buf, int n);
int call_socket(char *hostname, u_short portnum);
int establish(u_short portnum);
int get_connection(int s);
int wait_pipe(int pipe, int timeout_sec);

/************************************************************/

int send_command(char *command, char *reply, char *machine, int port, int timeout_sec)
{
  int i,s;
  u_short p;
  pid_t result, pid;
  int command_pipe[2];
  double ut;

  for(i=0;i<MAXBUFSIZE;i++)reply[i]=0;

  if(strlen(command)>MAXBUFSIZE){
     fprintf(stderr,"send_command: command size too long : $s\n",
               command);
     return(-1);
  }

  if(verbose1){
     ut=get_ut();
     fprintf(stderr,
        "send_command: %9.6lf calling socket with machine %s port %d\n",
         ut,machine,port);
     fflush(stderr);
  }

  p = port;
  if ((s= call_socket(machine,p)) < 0) { 
        fprintf(stderr,"send_command: could not open socket with machine %s port %d\n",
             machine,port);
        fflush(stderr);
        perror("send_command: call_socket");
        return(-1);
  }

  if(verbose1){
  	    ut=get_ut();
            fprintf(stderr,"send_command: %9.6lf writing command : %s\n",
		ut,command);
            fflush(stderr);
  }

  if (write_data(s, (char *)command, strlen(command)+1)
            != strlen(command)+1) {
          fprintf(stderr,"send_command: can't write data to socket\n");
          return(-1);
  }
 
  if(verbose1){
  	  ut=get_ut();
          fprintf(stderr,"send_command: %9.6lf reading reply \n",ut);
          fflush(stderr);
  }

  if(read_data(s,reply,MAXBUFSIZE)<=0){
          fprintf(stderr,"send_command: can't read command reply\n");
          return(-1);
  }

  if(verbose1){
  	  ut=get_ut();
          fprintf(stderr,"send_command: %9.6lf reply is %s",ut,reply);
          fflush(stderr);
  }

  close(s);

  return(0);
}


/************************************************************/

int wait_pipe(int pipe, int timeout_sec)
{
     struct timeval timeout;
     fd_set read_set;
     int result;

     timeout.tv_sec=timeout_sec;
     timeout.tv_usec=0;

     /* add pipe to the read_set */
     FD_ZERO(&read_set);
     FD_SET(pipe,&read_set);

     /* wait for data on the pipe */
     result=select(pipe+1,&read_set,NULL,NULL,&timeout);
     if(result==0){
       fprintf(stderr,"wait_pipe: timeout waiting for command on command pipe\n");
       fflush(stderr);
       return(-1);
     }
     else if (result==-1){
       fprintf(stderr,"wait_pipe: error waiting for command on command pipe\n");
       fflush(stdout);
       return(-1);
     }
    
     return(0);
}

/************************************************************/


void init_socket_status(int s,socket_status *s_status)
{
	s_status->max_fd=s+1;

	/* initialize s_status so that select command waits for socket to
	   become readable */
 
	FD_SET(s,&(s_status->readfds));

	/* clear the write and exception lists */

	FD_ZERO(&(s_status->writefds));
	FD_ZERO(&(s_status->exceptfds));

	/* set wait timeout to zero so that select command will return
	   immediately if the socket is not readyu to be written to */

	(s_status->timeout).tv_sec=0;
	(s_status->timeout).tv_usec=0;

	return;
}

/************************************************************/
int read_data(int s,        /* connected socket */
	      char *buf,    /* pointer to the buffer */
	      int n)          /* number of characters (bytes) we want */
{ 
    int bcount, /* counts bytes read */
    br; /* bytes read this pass */

    bcount= 0;
    br= 0;
    while (bcount < n) { /* loop until full buffer */
 	if ((br= read(s,buf,n-bcount)) > 0) {
 	    bcount += br; /* increment byte counter */
 	    buf += br; /* move buffer ptr for next read */
 	}
 	if (br < 0) /* signal an error to the caller */
 	    return(-1);
        else if (*buf==0||br==0)
           return(bcount);
    }
    
    return(bcount);
 }

/************************************************************/
int write_data(int s,         /* connected socket */
	       char *buf,     /* pointer to the buffer */
	       int n)         /* number of characters (bytes) we want */
{ 
    int bcount, /* counts bytes read */
    br; /* bytes read this pass */

    bcount= 0;
    br= 0;
    while (bcount < n) { /* loop until full buffer */
 	if ((br= write(s,buf,n-bcount)) > 0) {
 	    bcount += br; /* increment byte counter */
 	    buf += br; /* move buffer ptr for next read */
 	}
 	if (br < 0) /* signal an error to the caller */
 	    return(-1);
    }
    
    return(bcount);
 }


/************************************************************/
 
int call_socket(char *hostname, u_short portnum)
 { 
     struct sockaddr_in sa;
     struct hostent *hp;
     int s;

     if ((hp= gethostbyname(hostname)) == NULL) { /* do we know the host's */
 	errno= ECONNREFUSED; /* address? */
 	return(-1); /* no */
     }

     bzero(&sa,sizeof(sa));
     bcopy(hp->h_addr,(char *)&sa.sin_addr,hp->h_length); /* set address */
     sa.sin_family= hp->h_addrtype;
     sa.sin_port= htons((u_short)portnum);
     if ((s= socket(hp->h_addrtype,SOCK_STREAM,0)) < 0) /* get socket */
 	return(-1);
 	
     if (connect(s,(const  struct sockaddr* )&sa,sizeof sa) < 0) { /* connect */
 	close(s);
 	return(-1);
     }
     
     return(s);
 }
/************************************************************/

int get_connection(int s)  /* connect to socket created with establish() */
{ 
    struct sockaddr_in isa; /* address of socket */
    int i; /* size of address */
    int t; /* socket of connection */

    i = sizeof(isa); /* find socket's address */
    
    getsockname(s,(struct sockaddr* )&isa,&i); /* for accept() */

    if ((t = accept(s,(struct sockaddr* )&isa,&i)) < 0) /* accept connection if there is one */
 	return(-1);
 	
    return(t);
}
 
/************************************************************/
int establish(u_short portnum)
{ 
    char myname[MAXHOSTNAME+1];
    int s;
    struct sockaddr_in sa;
    struct hostent *hp;

    bzero(&sa,sizeof(struct sockaddr_in)); /* clear our address */
    
    gethostname(myname,MAXHOSTNAME); /* who are we? */
    
    
    hp= gethostbyname(myname); /* get our address info */
    
    if (hp == NULL) /* we don't exist !? */
 	return(-1);
 	
    sa.sin_family= hp->h_addrtype; /* this is our host address */
    sa.sin_port= htons(portnum); /* this is our port number */
    
 
    if ((s= socket(AF_INET,SOCK_STREAM,0)) < 0){/* create socket */
       fprintf(stderr,"establish: can't open socket\n");
 	return(-1);
    }
    
    
    if (bind(s,(const  struct sockaddr* )&sa,sizeof(sa)) < 0) {
        fprintf(stderr,"establish: can't bind socket\n");
	close(s);
	return(-1); /* bind address to socket */
    }
    
    listen(s, 3); /* max # of queued connects */
    
    return(s);
 }

