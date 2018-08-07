#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "TCPIP.h"
#include "version.h"

int getAddress(char *host, struct in_addr *iaddr)
{
	char *p = host;
        int ishost = 0;

	if (strcmp(host,"*")==0) {
		iaddr->s_addr=htonl(INADDR_ANY);
		return 1;
	}
        while (*p) {
	        if (!(isdigit(*p) || ((*p) == '.'))) {
		        ishost = 1;
		        break;
	        }
        	p++;
        }
        if (ishost) {
	        struct hostent *h;
	        h = gethostbyname(host);
	        if (!h) { return 0; }
	        memcpy((void *) &iaddr->s_addr,(void *) h->h_addr,4);
        	return 1;
        } else {
        	iaddr->s_addr = inet_addr(host);
        	return 1;
        }
}

int TCPbind(char *bindAddress,int bindPort)
{
	struct in_addr iaddr;
	struct sockaddr_in saddr;
	struct linger lg;
	int sd;
	int nodelay,reuse;

	if (!getAddress(bindAddress, &iaddr))
		errexit2("Host %s could not be resolved\n",bindAddress);
	sd = socket(PF_INET, SOCK_STREAM, 0);
	if (sd < 0)
		errexit("Couldn't create server socket!\n");
	
	saddr.sin_family = AF_INET;
        memcpy(&saddr.sin_addr, &iaddr, sizeof(iaddr));
        saddr.sin_port = htons(bindPort);

	if (setsockopt(sd,IPPROTO_TCP,TCP_NODELAY,&nodelay,sizeof(nodelay))<0)
		error("TCP_NODELAY failed\n");
	if (setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0)
		error("SO_REUSEADDR failed\n");
	lg.l_onoff=1; lg.l_linger=5;
	if (setsockopt(sd,SOL_SOCKET,SO_LINGER,&lg,sizeof(lg))<0)
		error("SO_LINGER failed\n");

        if (bind(sd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
        	errexit3("Couldn't bind to address %s port %d\n",
	        bindAddress, bindPort);
        if (listen(sd, 5) < 0)
	        errexit3("Couldn't listen to address %s port %d\n",
	        bindAddress, bindPort);
	return(sd);
}

int TCPaccept0(int sd,int block,unsigned long *ip)
{
	struct sockaddr_in saddr;
	int ad,len,j;

	if (block==0) {
		j=0;
		setsockopt(sd, SOL_SOCKET, SO_LINGER, &j, sizeof(j));
		fcntl(sd, F_SETFL, O_NONBLOCK);
	}
	len=sizeof(saddr);
	if ((ad = accept(sd, (struct sockaddr *)&saddr, (int *)&len)) < 0) {
		error("Couldn't accept\n");
		return -1;
	} else if (ip) memcpy(ip,&saddr.sin_addr, sizeof(unsigned long));
	return(ad);
}

int TCPaccept(int sd) { return TCPaccept0(sd,1,0); }
int TCPacceptIP(int sd,unsigned long *ip) { return TCPaccept0(sd,1,ip); }
int TCPacceptNB(int sd) { return TCPaccept0(sd,0,0); }

int TCPconnect0(char *bindAddress,int bindPort,int block)
{
	struct in_addr iaddr;
	struct sockaddr_in saddr;
	int sd,j;
	int nodelay;

	if (!getAddress(bindAddress, &iaddr)) {
		error2("Host %s could not be resolved\n",bindAddress);
		return -1;
	}
	sd = socket(PF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		error("Couldn't create server socket!\n");
		return -1;
	}
	saddr.sin_family = AF_INET;
        memcpy(&saddr.sin_addr, &iaddr, sizeof(iaddr));
        saddr.sin_port = htons(bindPort);

	if (setsockopt(sd,IPPROTO_TCP,TCP_NODELAY,&nodelay,sizeof(nodelay))<0)
		error("TCP_NODELAY failed\n");
		
	if (block==0) {
		j=0;
		setsockopt(sd, SOL_SOCKET, SO_LINGER, &j, sizeof(j));
		fcntl(sd, F_SETFL, O_NONBLOCK);
	}

    if (connect(sd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        	error3("无法连接地址 %s 端口 %d\n",
	        bindAddress, bindPort);
	        return -1;
	}
	return(sd);
}

int TCPconnect(char *bindAddress,int bindPort)
{ 
	TCPconnect0(bindAddress,bindPort,1); 
}

int TCPconnectNB(char *bindAddress,int bindPort)
{
	TCPconnect0(bindAddress,bindPort,0);
}



