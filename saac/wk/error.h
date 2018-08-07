/*
  error.h

  Handle error messages

 Created by Nuke <nuke@mail.hwaei.com.tw>
*/

void errexit(char *message);
void errexit2(char *message1,char *message2);
void errexit3(char *message1,char *message2, int message3);
void error(char *message);
void error2(char *message1,char *message2);
void error3(char *message1,char *message2, int message3);

#define WGSLOG "wgs.log"

#define P(A) { debug3("%s%d ","",A); }

