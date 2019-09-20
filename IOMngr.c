#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "IOMngr.h"

/* Author:    Zachary Baklund
   Created:   2/6/19
   Resources: https://syntaxdb.com/ref/c/structures
              https://www.learn-c.org/en/Linked_lists
              Drew
              Dr. Senger
*/

int sourceFD;
char * source;
size_t sourceByteCnt;
char * sourceLastChar;
char * nextChar;        // address of next char to return
int curLine;
int nextMessageNumber;
int nextEOFSpan;

struct Message {
  struct Span span;
  int seqNumber;
  char * message;
  struct Message * next;
};

struct Message * Messages;
struct Message * EOFMessages;

struct Message *
MakeMessage(struct Span span, int seq, const char * message) {
  struct Message * msg = malloc(sizeof(struct Message));
  msg->span = span;
  msg->seqNumber = seq;
  msg->message = strdup(message);
  msg->next = NULL;
  return msg;
}

void
FreeHeadMessage() {
  struct Message * temp = Messages;
  Messages = Messages->next;
  free(temp->message);
  free(temp);
}

void
FreeEOFMessage(){
  struct Message * eoftemp = EOFMessages;
  EOFMessages = EOFMessages->next;
  free(eoftemp->message);
  free(eoftemp);
}

bool
isLineBreak(char * c) {
  return *c == '\n' || *c == '\r' || c == sourceLastChar;
}

// ansi escape sequences for colors
char * colorSeqs[] = {"\033[91m","\033[92m","\033[93m","\033[94m","\033[95m","\033[96m"};

void
OutputMarkStart(struct Message * msg) {
  fprintf(stdout,"%s",colorSeqs[(msg->seqNumber % 6)]);
}

void
OutputMarkStop() {
  fprintf(stdout,"\033[0m");
}

void
LineHeader() {
  fprintf(stdout, "%6d: ", curLine);
  curLine++;
}

void
OutputMessagesBefore(struct Message * curMsg) {
  struct Message * temp = Messages;
  while(temp && temp != curMsg){
    fprintf(stdout, "        ");
    OutputMarkStart(temp);
    fprintf(stdout, "**");
    OutputMarkStop();
    fprintf(stdout, " %s\n", temp->message);
    temp = temp->next;
    FreeHeadMessage();
  }
}

void
OutputInterval(char * start, char * stop) {
  if (stop < start) return;
  fwrite(start,stop - start + 1,1,stdout);
}

void FlushMessages() {
  struct Message * temp = Messages;
  while(temp){
    fprintf(stdout, "        ");
    OutputMarkStart(temp);
    fprintf(stdout, "**");
    OutputMarkStop();
    fprintf(stdout, " %s\n", temp->message);
    temp = temp->next;
    FreeHeadMessage();
  }
  struct Message * eoftemp = EOFMessages;
  while(eoftemp){
    fprintf(stdout, "        ");
    OutputMarkStart(eoftemp);
    fprintf(stdout, "**");
    OutputMarkStop();
    fprintf(stdout, " %s\n", eoftemp->message);
    eoftemp = eoftemp->next;
    FreeEOFMessage();
  }
}

void
OutputSource() {
  char c;
  int cnt = 0;
  bool marked = false;
  curLine = 1;
  LineHeader();
  struct Message * curr = Messages;
  while (source[cnt] != EOF && &source[cnt] <= sourceLastChar) {
    if (source[cnt] == '\n' || source[cnt] == '\r') {
      if (marked) OutputMarkStop();
      putc(source[cnt], stdout);
      cnt++;
      OutputMessagesBefore(curr);
      if (&source[cnt] <= sourceLastChar){
        LineHeader();
      }
      if (marked) OutputMarkStart(curr);
    }else{
      if (curr && cnt == curr->span.first
        && curr->span.first == curr->span.last) {
          OutputMarkStart(curr);
          putc(source[cnt], stdout);
          OutputMarkStop();
          curr = curr->next;
          cnt++;
      }else if(curr && cnt == curr->span.first){
        OutputMarkStart(curr);
        putc(source[cnt], stdout);
        cnt++;
        marked = true;
      }else if(curr && cnt >= curr->span.last){
        putc(source[cnt], stdout);
        OutputMarkStop();
        curr = curr->next;
        cnt++;
        marked = false;
      }else {
        putc(source[cnt], stdout);
        cnt++;
      }
    }
  }
  FlushMessages();
}

bool
OpenSource(const char * aFilename) {
  sourceFD = open(aFilename,O_RDONLY);
  if (sourceFD < 0) return false;
  struct stat buf;
  if (fstat(sourceFD,&buf)) return false;
  sourceByteCnt = buf.st_size;
  source = mmap(NULL,sourceByteCnt,PROT_READ,MAP_PRIVATE,sourceFD,0);
  if (source == NULL) return false;
  sourceLastChar = source + sourceByteCnt - 1;
  nextChar = source;
  curLine = 1;
  nextMessageNumber = 0;
  nextEOFSpan = -1;
  return true;
}

void
CloseSource() {
  nextChar = source;
  OutputSource();

  munmap(source, sourceByteCnt);
}

int
GetSourceChar() {
  if(*nextChar == '\0' || *nextChar == EOF){
    return EOF;
  }
  if(isLineBreak(nextChar)){
    curLine++;
  }
  char sourceChar = *nextChar;
  nextChar++;
  return sourceChar;
}

bool
PostMessage(struct Span span, const char * aMessage) {
  if (nextMessageNumber == 0 && span.first >= 0){
    //inital message
    Messages = MakeMessage(span, nextMessageNumber, aMessage);
    nextMessageNumber++;
    return true;
  }
  if (span.first < 0){
    if(!EOFMessages){
      EOFMessages = MakeMessage(span, nextMessageNumber, aMessage);
    }else{
      struct Message * eofcurr = EOFMessages;
      while(eofcurr->next){
        eofcurr = eofcurr->next;
      }
      eofcurr->next = MakeMessage(span, nextMessageNumber, aMessage);
    }
    nextMessageNumber++;
    return true;
  }
  // get to end of linked list
  struct Message * curr = Messages;
  while(curr){
    if(span.first <= curr->span.first || span.last <= curr->span.last
        || span.first <= curr->span.last){
      //overlap checking
      nextMessageNumber++;
      return false;
    }
    curr = curr->next;
  }
  //make new message in linked list
  curr = Messages;
  while(curr->next){
    curr = curr->next;
  }
  curr->next = MakeMessage(span, nextMessageNumber, aMessage);
  nextMessageNumber++;
  return true;
}

int
GetLastPosition() {
  return nextChar - 1 - source;
}

struct Span
MakeSpan(int first, int last) {
  struct Span span = {first, last};
  return span;
}

struct Span
MakeSpanFromLength(int start, int length) {
  struct Span span = {start,start + length - 1};
  return span;
}

struct Span
MakeEOFSpan() {
  struct Span span = {nextEOFSpan,nextEOFSpan};
  nextEOFSpan--;
  return span;
}
