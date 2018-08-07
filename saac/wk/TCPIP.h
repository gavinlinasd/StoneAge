int TCPbind(char *bindAddress,int bindPort);
int TCPaccept(int sd);
int TCPacceptIP(int sd,unsigned long *ip);
int TCPacceptNB(int sd);
int TCPconnect(char *bindAddress,int bindPort);
int TCPconnectNB(char *bindAddress,int bindPort);


