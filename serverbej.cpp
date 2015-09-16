//serverbej.cpp

#include "mysockbej.h"
using namespace std;

class Server
{
private:
	  int status,tcpFlag,yes,sockfd,newfd;;
	  char s[INET6_ADDRSTRLEN],newMsg[MAXDATASIZE];
	  const char *local,*port;
	  struct sigaction sa;
	  struct addrinfo *p,*serverInfo, hints;
	  struct sockaddr_storage their_addr;
	  socklen_t sin_size,addr_len;
	  
	  //Data Types for multiple socket listening
	  fd_set master;    // master file descriptor list
	  fd_set read_fds;  // temp file descriptor list for select()
	  int fdmax;        // maximum file descriptor number

  
public:
  void sigchld_handler(int s)
  {
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;
  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
  }  
  void display(int tcpFlag)
  {
    //bool display(struct addrinfo *pointer)
    if(tcpFlag == 1)
    {
      cout << "\n addrinfo strucutre has the following details TCP " ;
      cout << "\n ai_family" << serverInfo->ai_family;
      cout << "\n ai_socktype" << serverInfo->ai_socktype;
      cout << "\n ai_addr : " << (struct sockaddr *) (serverInfo->ai_addr)->sa_data;
      cout << "\n ai_addrlen: " << serverInfo->ai_addrlen;
    }//end tcpFlag == 1
    else if(tcpFlag == 2)
    {
      cout << "\n addrinfo strucutre has the following details UDP " ;
      cout << "\n ai_family" << serverInfo->ai_family;
      cout << "\n ai_socktype" << serverInfo->ai_socktype;
      cout << "\n ai_addr : " << (struct sockaddr *) (serverInfo->ai_addr)->sa_data;
      cout << "\n ai_addrlen:" << serverInfo->ai_addrlen << endl;
    }// end tcpFlag == 2
    
  }// end display
  Server(char *myport,int tcpFlag)
  {
    //Default for both TCP and UDP ports
    memset(&hints, 0, sizeof (hints));
    port=myport;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
      
    local="127.0.0.1";
    yes=1;
     if(tcpFlag == 1)
     {
      cout << " Input myportTCP \n" << myport;
      cout << " Assigned portTCP \n" << port;
      hints.ai_socktype = SOCK_STREAM;
      }//end tcpFlag
     else if(tcpFlag == 2)
     {
      cout << " Input myportUDP \n" << myport;
      cout << " Assigned portUDP \n" << port;
      hints.ai_socktype = SOCK_DGRAM;
     }
  }//end Constructor Server
  
  void testDNS(int tcpFlag)
  {
    if(tcpFlag == 1)
    {
      if ((status = getaddrinfo(local, port, &hints, &serverInfo)) != 0) {
      fprintf(stderr, "TCP DNS getaddrinfo error: %s\n", gai_strerror(status));
      exit(1);
      }
      display(1);
    }//end tcpFlag==1
    else if(tcpFlag == 2)
    {
      if ((status = getaddrinfo(local, port, &hints, &serverInfo)) != 0) {
      fprintf(stderr, "UDP DNS getaddrinfo error: %s\n", gai_strerror(status));
      exit(1);
       }
       display(2);
    }//end tcpFlag==2
  }//end DNS
  int bindSocket(int tcpFlag)
  {
    if(tcpFlag == 1)
    {
      cout << "bindTCP()" << endl;
      for(p = serverInfo; p != NULL; p = p->ai_next) 
      {
	if ((sockfd = socket(p->ai_family, p->ai_socktype,
	p->ai_protocol)) == -1) {
	perror("TCP server: socket");
	continue;
	}
	cout << "bindTCP() got correct first structure" << endl;
	//If the port is already in use, then re-use it
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
	  sizeof(yes)) == -1) {
	  perror("TCP setsockopt");
	exit(1);
	}
	cout << "bindTCP() got reused address" << endl;
	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	  close(sockfd);
	  perror("TCP server: bind");
	  continue;
	}
	cout << "bindTCP() got binded" << endl;
	//break;
      	freeaddrinfo(serverInfo); // all done with this structure
	if (p == NULL) {
	fprintf(stderr, "TCP server: failed to bind\n");
	exit(1);
	}
       }//end for
       return 1;
      }//end if tcpflag == 1
      else if(tcpFlag == 2)
      {
	cout << "bindUDP()" << endl;
	for(p = serverInfo; p != NULL; p = p->ai_next) {
	if ((sockfd = socket(p->ai_family, p->ai_socktype,
	p->ai_protocol)) == -1) {
	perror("UDP server: socket");
	continue;
	}
	cout << "bindUDP() got correct first structure" << endl;
	//If the port is already in use, then re-use it
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
	  sizeof(yes)) == -1) {
	  perror("UDP setsockopt");
	exit(1);
	}
	cout << "bindUDP() got reused address" << endl;
	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	  close(sockfd);
	  perror("UDP server: bind");
	  continue;
	}
	cout << "bindUDP() got binded" << endl;
	//break;
      	freeaddrinfo(serverInfo); // all done with this structure
	if (p == NULL) {
	fprintf(stderr, "UDP server: failed to bind\n");
	exit(1);
	}
	receiveUDP(sockfd);
	}// end for 
	return 2;
      }// end else if tcpFlag == 2
  } // end bind
  
  int listenSocket(int tcpFlag)
  {
      if(tcpFlag == 1)
      {
	cout << "listenTCPServer listening" << endl;
	if (listen(sockfd, BACKLOG) == -1) {
	perror("TCP listen");
	exit(1);
	}
	cout << "TCP server: waiting for connections...\n" ;
	while(1) {
	// main accept() loop
	sin_size = sizeof their_addr;
	newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
	perror("TCP accept");
	continue;
	}
	inet_ntop(their_addr.ss_family,
	      get_in_addr((struct sockaddr *)&their_addr),
	      s, sizeof s);
	printf("TCP server: got connection from %s\n", s);
	if (!fork())// this is the child process
	  {
	  close(sockfd); // child doesn't need the listener
	  receiveTCP(newfd);
	  }
	}//end While
	return 1; 
      }//end tcpFlag == 1
      /*
       Listen is not defined for UDP, just connect is sufficient. Connect tells OS that there 'may' be future connections coming 
       on this UDP port. Remember UDP is unreliable and used for broadcast.
       */
      else if(tcpFlag == 2)
      {
	
	cout << "UDP server: waiting for connections...\n" ;
	//cout << "listenUDPServer listening" << endl;
	/*if (listen(sockfd, BACKLOG) == -1) {
	perror("UDP listen");
	exit(1);
	}
	cout << "UDP server: waiting for connections...\n" ;
	receiveUDP(sockfd);
	while(1) {
	// main accept() loop
	sin_size = sizeof their_addr;
	newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
	perror("UDP accept");
	continue;
	}
	inet_ntop(their_addr.ss_family,
	      get_in_addr((struct sockaddr *)&their_addr),
	      s, sizeof s);
	printf("UDP server: got connection from %s\n", s);
	return 1;
	close(newfd); // parent doesn't need this
	}//end While*/
      }
  }//end listenSocket
  void receiveTCP(int newfd)
  {
	int numbytes;
	char buffer[MAXDATASIZE];
	if ((numbytes = recv(newfd,buffer,MAXDATASIZE-1, 0)) == -1) {
	perror("TCP recv");
	exit(1);
	}// endif
     	buffer[numbytes] = '\0';
	printf("server: received '%s'\n",buffer);
	close(newfd);
	exit(0);
   }//end receiveTCP
  void receiveUDP(int newfd)
  {
	int numbytes;
	char buffer[MAXDATASIZE];
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(newfd,buffer,MAXDATASIZE-1, 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
	perror("UDP recv");
	exit(1);
	}// endif
     	buffer[numbytes] = '\0';
	printf("UDP server: received '%s'\n",buffer);
	close(newfd);
	//exit(0);
  }//end receiveUDP
  
  void *get_in_addr(struct sockaddr *sa)
  {
      if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
  //Select Functions
  
};// end Server


int main(int c, char *argv[])
{
  char tcp[MAXDATASIZE],udp[MAXDATASIZE];
  int choice;
  
  cout << " Enter 1 for TCP Server and Enter 2 for UDP server" << endl;
  cin >> choice;
  
  switch(choice)
  {
    case 1: {
	    cout << "\nEnter TCP port number\n";
	    cin >> tcp;
	    Server tcpServer(tcp,1);
	    tcpServer.testDNS(1);
	    if(tcpServer.bindSocket(1) < 0)
	    cout << "\nTCP Server bind not successful\n";
	    tcpServer.listenSocket(1);
	    }//end 1 switch
	    break;
    case 2:{
	   cout << "Enter UDP port number";
	   cin >> udp;
	   Server udpServer(udp,2);
	   udpServer.testDNS(2);
	   if(udpServer.bindSocket(2) < 0)
	   cout << " UDP Server bind not successful" << endl;
	   //udpServer.listenSocket(2);
	  // udpServer.receiveUDP();
	  //udpServer.display(2);
	   break;
	  }//end 2 switch
    default : cout << "\n Please Enter the correct choice\n";
  
  }
}
