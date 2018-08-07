#include <sys/time.h>
#include <stdio.h>
#include "error.h"
#include "version.h"
//char *logfile=ERRLOG;

#define SHOW_TIME

char *chartime()
{
	static char buf[80];
	time_t t;
	t=time(0);
//	strcpy(buf,ctime(&t));
	buf[strlen(buf)-1]=' ';
	return(buf);
}

void errexit(char *message)
{
#ifdef SHOW_TIME
	fprintf(stderr,"%s",chartime());
#endif
	fprintf(stderr,"%s\n",message);
	exit(1);
}

void errexit2(char *message1, char *message2)
{
#ifdef SHOW_TIME
	fprintf(stderr,"%s",chartime());
#endif
	fprintf(stderr,message1,message2);
	exit(1);
}

void errexit3(char *message1, char *message2, int message3)
{
#ifdef SHOW_TIME
	fprintf(stderr,"%s",chartime());
#endif
	fprintf(stderr,message1,message2,message3);
	exit(1);
}

void error(char *message)
{
#ifdef SHOW_TIME
	fprintf(stderr,"%s",chartime());
#endif
	fprintf(stderr,"%s",message);
}

void error2(char *message1, char *message2)
{
#ifdef SHOW_TIME
	fprintf(stderr,"%s",chartime());
#endif
	fprintf(stderr,message1,message2);
}

void error3(char *message1, char *message2, int message3)
{
#ifdef SHOW_TIME
	fprintf(stderr,"%s",chartime());
#endif
	fprintf(stderr,message1,message2,message3);
}


