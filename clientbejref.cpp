#include <iostream>
#include <typeinfo>
#include "mysockbej.h"


using namespace std;

class Client{
private : 
	   int a,flag,status,tcpFlag;
	  int yes,listenflag;
	  char s[INET6_ADDRSTRLEN],message[MAXDATASIZE];
	  int sockfd,newfd;
	  const char *local;
	  struct sigaction sa;
	  const char *port;
	  struct addrinfo *p,*serverInfo, hints;
public : 
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
  Client(char *myport,int tcpFlag)
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
  }//end Constructor Client
  
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
  int clientConnect(int tcpFlag)
  {
    if(tcpFlag == 1)
    {
      for(p = serverInfo; p != NULL; p = p->ai_next)
      {
	sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
	if (sockfd == -1)
	{
	  perror("client: socket");
	  continue;
	}
	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
	{
	  close(sockfd);
	  perror("client: connect ");
	  //cout << sockfd ;
	  continue;
	}
	cout << " Inside TCP clientConnect getting address and connected" <<endl;
	break;
      }// end for
      int sendBytes;
      //sendBytes=sendTCP();
      //return sendBytes;
    }//end tcpFlag == 1
    else if(tcpFlag == 2)
    {
      for(p = serverInfo; p != NULL; p = p->ai_next)
      {
	sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
	if (sockfd == -1)
	{
	  perror("client: socket");
	  continue;
	}
	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
	{
	  close(sockfd);
	  perror("client: connect");
	  continue;
	}
	cout << " Inside UDP clientConnect getting address and connected" <<endl;
	break;
      }// end for
      return 2;
    }// end else tcpFlag == 2
  }// end clientConnect
  /*int sendTCP()
  {
    int numbytes;
    numbytes=send(sockfd,message,sizeof(message), 0); 
    cout << " TCP Client send " << numbytes << endl;
    return numbytes;
  }
  void setMessage(char *ptr)
  {
    strcpy(message,ptr);
  }*/
  
  void *get_in_addr(struct sockaddr *sa)
  {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
   
};

int main()
{
  char tcp[MAXDATASIZE],udp[MAXDATASIZE];
  int tcpCheck, udpCheck;
  char message[] = "Sri Saibaba";
  cout << "Enter TCP port number";
  cin >> tcp;
  //cout << "Enter UDP port number";
  //cin >> udp; 
   
  Client tcpClient(tcp,1);
  Client udpClient(udp,2);
    
  tcpClient.testDNS(1);
  //udpClient.testDNS(2);
  
  //tcpClient.setMessage(message);
  
  if ((tcpClient.clientConnect(1)) < 0)
  cout << " TCP Client Connect not happening" << endl;
  //if ((udpClient.clientConnect(2)) < 0)
  //cout << " UDP Client Connect not happening" << endl;
  
  //tcpClient.display(1);
  //udpClient.display(2);
}
