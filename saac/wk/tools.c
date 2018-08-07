#include "error.h"
#include "tools.h"
#include "version.h"

#define BUF_SIZE 4096
//#define P(A) { printf("%d\n",A); }

char Xtoken; // Original leading charactor of strxtok
char Ytoken; // Original tail charactor of strxtok

int bakayaro(char *name)
{
	int i,sum;
        if (!name) return 0;
        for (i=0,sum=0;name[i];i++) sum+=name[i]*(i+1);
//        error3("Aho:%s%d\n","",sum);
        return sum;
}

char *iptoa(unsigned long ip)
{
	static char buffer[80];
	unsigned char *p;
	p=(unsigned char *)&ip;
        sprintf(buffer,"%u.%u.%u.%u",p[0],p[1],p[2],p[3]);
	return(buffer);
}

char *strxcpy(char *src)
{
	static char dst[BUF_SIZE];
	int i;
	
	if (src) {
		for (i=0;i<=strlen(src);i++) 
			dst[i]=(src[i]=='\n')?'\t':src[i];
		return(dst);
	} else {
		return(0);
	}
}

// 
char *strxtok(char *src,int _limit)
{
	static char buf[BUF_SIZE];
	static char *ptr;
	static int i,j,limit;

	if (src) {
		limit=_limit<BUF_SIZE?_limit:BUF_SIZE-1;
		memcpy(buf,src,limit);
		buf[limit]=0; i=0; j=0;
	}
	Xtoken=0; Ytoken=0;
	if (i<limit) ptr=&buf[j]; else return(0);
	Xtoken=buf[i];
	while (i<limit) {
		// '\'
		if (buf[i]=='\\') {
			// '\\' => '\' or '\;' => ';' or '\&' => '&'
			if (buf[i+1]=='\\' || buf[i+1]==';' || buf[i+1]=='&') { 
				i++; buf[j++]=buf[i++]; 
			}
			// '\' -> Ignore '\'
			else { i++; }
		} else if (buf[i]==';') { // Bug from AP
			i++; buf[j++]=0;
			Ytoken=';';
			return(ptr);
		} else {
			buf[j++]=buf[i++];
		}
	}
	buf[j]=0;
	Ytoken=0;
	return(ptr);
}

// Add up \ for \, ; and &
char *xchange(char *src)
{
	static char buffer[BUF_SIZE];
	int i,j,limit;
	
	if (!src) return(0);
	limit=strlen(src);
	if (limit>=BUF_SIZE/2) limit=BUF_SIZE/2-1;
	for (j=0,i=0;i<limit;i++) {
		if ((src[i]=='\\') || (src[i]==';') || (src[i]=='&')) 
			buffer[j++]='\\';
		buffer[j++]=src[i];
	}
	buffer[j]=0;
	return(buffer);
}

/*main()
{
	char buffer[BUF_SIZE];
	char *ptr;
	int i,len;
	
	sprintf(buffer,"%s;%s;\n",xprintf("123\\456"),xprintf("789;1000"));
	printf("%s",buffer);
}*/

void stockInit(Stock_Struct *s)
{
	s->head=s->tail=0;
	s->count=0;
}

void stockAdd(Stock_Struct *s,char *data)
{
	Stock *new;
	
	if (!s) return;
	if (!data) data="";
	// Make new stock
	new=(Stock *)calloc(1,sizeof(Stock));
	new->data=strcpy((char *)calloc( 1,strlen(data)+1),data);
	new->flag=0;
	// Link new stock
	if (!s->head) { // First Item, add to tail
		s->head=s->tail=new;
	} else { // Add to tail
		s->tail->link=new;
		s->tail=new;
	}
	s->count++;
}

char *stockOut(Stock_Struct *s)
{
	static char buf[BUF_SIZE];
	Stock *t;

	if (!s || !s->head) return(0);
	t=s->head;
	s->head=t->link;
	strcpy(buf,t->data);
	free(t->data);
	free(t);
	s->count--;
	if (!s->head) s->tail=0;
	return(buf);
}

void stockDel(Stock_Struct *s,int count)
{
	Stock *t;

	if (!s || !s->head) return;

	while ((t=s->head) && count) {
		s->head=t->link;
		free(t->data);
		free(t);
		count--;
		s->count--;
	}
	if (!s->head) s->tail=0;
}

int stockCount(Stock_Struct *s)
{
	return(s->count);
}

void stockClear(Stock_Struct *s)
{
	Stock *t,*u;
	if (!s) return;
	u=s->head;
	while (t=u) {
		u=u->link;
		free(t->data);
		free(t);
	}
	s->head=0; s->tail=0;
	s->count=0;
}

void stockStore(Stock_Struct *s,char *buf,int limit)
{
	char *ptr;
//	show(s);
	if (!s || !buf) return;
	ptr=strxtok(buf,limit);
//	if (ptr && s->tail) P(s->tail->flag);
	if (ptr && s->tail && 
	   (s->tail->flag&STOCK_NONTERM)) { // Last message is non-terminated
		strcat((char *)realloc(s->tail->data,
			strlen(s->tail->data)+strlen(ptr)+1),ptr);
		if (Ytoken==';') { s->tail->flag^=STOCK_NONTERM; s->count++; } 
		ptr=strxtok(0,0);
	}
	while (ptr) {
		stockAdd(s,ptr);
		if (Xtoken=='&') s->tail->flag|=STOCK_START;	// The start
		if (Ytoken!=';') { 	// Non-terminated
			s->tail->flag|=STOCK_NONTERM;
			s->count--;	// This is not a valid item
		}
		ptr=strxtok(0,0);
	}
}

void rankAdd(Stock_Struct *ss,int rank,Rank_Struct *rs)
{
	int i;
	
	if (!ss) { error("rankAdd(): Null Stock?\n"); return; }
	if (!rs) { error("rankAdd(): Null Storage?\n"); return; }
	if (rank<=0) {error("rankAdd(): Invalid rank\n"); return; }
	for (i=0;i<10;i++) { rs->entry[i].name[0]=0; rs->entry[i].score=0; }
	rs->rank=rank;
	for (i=0;(i<10)&&(ss->head);i++) {
		strcpy(rs->entry[i].name,stockOut(ss));
		if (ss->head) rs->entry[i].score=atoi(stockOut(ss));
		else rs->entry[i].score=0;
	}
}

int rankSelect(int rank)
{
	return(((rank-1)/10)*10+1);
}

int charXCount(char *buf)
{
	int i,count;
	if (!buf) { error("charCount(): Null buffer?\n"); return(0); }
	i=0; count=0;
	while (buf[i]) {
		if (buf[i]=='\\') {
			if ((buf[i+1]=='\\') || (buf[i+1]=='&') || 
			    (buf[i+1]==';')) {
			    	i++;
			}
		} else if (buf[i]==';') count++;
		i++;
	}
	return(count);
}

// Construct the 8-byte formatted string
char *construct(char *src)
{
	static char dst[BUF_SIZE];
	const char *AND="&&&&&&&&";
	int n;
	if (!src) { dst[0]=0; return(dst); }
	else {
		sprintf(dst,"&;%d;%s",charXCount(src)+1,src);
		n=strlen(dst)%8;
 		if (!n) return(dst);
 		strcat(dst,AND+n);
	}
	return(dst);
}

void show(Stock_Struct *s)
{
	Stock *trace;
	if (!s) return;
	for (trace=s->head;trace;trace=trace->link) 
		error3("<%s:%d>",trace->data,trace->flag);
	error("\n");
}

/*main()
{
	Stock_Struct s;
	char buf[100];
	char buf1[4096];
	int i;

	stockInit(&s);
	while (1) {
		gets(buf);
//		for (i=0;buf[i];i++) printf("(%c)",buf[i]);
//		stockStore(&s,buf,strlen(buf));
		printf("%s\n",construct(buf));
//		show(&s);
	}
}*/








