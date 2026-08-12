/* CLASS COMMAND CENTER - Data Structures Lab Final (pure C) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXC 10
#define MAXD 100

struct Course{char code[20],name[50],teacher[50];int used;};
struct Ann{int id,priority,status;char title[100],desc[300],course[50],date[20];struct Ann*next;};
struct QNode{int id;char desc[150];struct QNode*next;};
struct Dl{int id,day,month,year,difficulty,importance,score,status;char course[50],task[100];};
struct Act{int type,refId;struct Ann annB;struct Dl dlB;char desc[100];struct Act*next;};

struct Course courses[MAXC]; int courseN=0;
struct Ann*annHead=NULL; int nextAnnId=101;
struct QNode*qFront=NULL,*qRear=NULL; int nextTaskId=1;
struct Dl heap[MAXD]; int hs=0; int nextDlId=201;
struct Act*stackTop=NULL;

void saveAll(void);

int getInt(const char*p){
    char b[512]; int v;
    for(;;){
        printf("%s",p);
        if(!fgets(b,sizeof(b),stdin)){printf("\nEOF. Saving & exiting.\n");saveAll();exit(0);}
        if(sscanf(b,"%d",&v)==1) return v;
        printf("Invalid number.\n");
    }
}
int getIntR(const char*p,int lo,int hi){
    int v; do{v=getInt(p); if(v<lo||v>hi) printf("Must be %d-%d.\n",lo,hi);} while(v<lo||v>hi);
    return v;
}
void getStr(const char*p,char*buf,int sz,int allowEmpty){
    for(;;){
        printf("%s",p);
        if(!fgets(buf,sz,stdin)){printf("\nEOF. Saving & exiting.\n");saveAll();exit(0);}
        buf[strcspn(buf,"\n")]='\0';
        if(!allowEmpty && !buf[0]){printf("Cannot be empty.\n");continue;}
        return;
    }
}
int validDate(int d,int m,int y){return y>=2000&&y<=2100&&m>=1&&m<=12&&d>=1&&d<=31;}

/* ---------- COURSES ---------- */
void addCourse(void){
    if(courseN>=MAXC){printf("Course list full.\n");return;}
    struct Course c={0};
    getStr("Course code: ",c.code,sizeof(c.code),0);
    for(int i=0;i<MAXC;i++) if(courses[i].used&&!strcmp(courses[i].code,c.code)){printf("Duplicate code.\n");return;}
    getStr("Course name: ",c.name,sizeof(c.name),0);
    getStr("Teacher: ",c.teacher,sizeof(c.teacher),0);
    c.used=1;
    for(int i=0;i<MAXC;i++) if(!courses[i].used){courses[i]=c;courseN++;printf("Course added.\n");return;}
}
void showCourses(void){
    printf("\n-- COURSES --\n");
    int f=0;
    for(int i=0;i<MAXC;i++) if(courses[i].used){f=1;printf("%s | %s | %s\n",courses[i].code,courses[i].name,courses[i].teacher);}
    if(!f) printf("None.\n");
}
void saveCourses(void){
    FILE*fp=fopen("courses.txt","w"); if(!fp)return;
    fprintf(fp,"%d\n",courseN);
    for(int i=0;i<MAXC;i++) if(courses[i].used) fprintf(fp,"%s\n%s\n%s\n",courses[i].code,courses[i].name,courses[i].teacher);
    fclose(fp);
}
void rdLine(char*d,int sz,FILE*fp){char b[512]; if(!fgets(b,sizeof(b),fp)){d[0]='\0';return;} b[strcspn(b,"\n")]='\0'; strncpy(d,b,sz-1); d[sz-1]='\0';}
void loadCourses(void){
    FILE*fp=fopen("courses.txt","r"); if(!fp)return;
    char b[512]; int n; if(!fgets(b,sizeof(b),fp)){fclose(fp);return;} sscanf(b,"%d",&n);
    courseN=0;
    for(int i=0;i<n&&i<MAXC;i++){
        struct Course c={0};
        rdLine(c.code,sizeof(c.code),fp); rdLine(c.name,sizeof(c.name),fp); rdLine(c.teacher,sizeof(c.teacher),fp);
        c.used=1; courses[i]=c; courseN++;
    }
    fclose(fp);
}

/* ---------- ANNOUNCEMENTS (linked list) ---------- */
struct Ann*newAnn(int id,const char*t,const char*d,const char*c,const char*dt,int pr,int st){
    struct Ann*n=malloc(sizeof(struct Ann)); if(!n)exit(1);
    n->id=id; strncpy(n->title,t,99); n->title[99]=0; strncpy(n->desc,d,299); n->desc[299]=0;
    strncpy(n->course,c,49); n->course[49]=0; strncpy(n->date,dt,19); n->date[19]=0;
    n->priority=pr; n->status=st; n->next=NULL; return n;
}
void appendAnn(struct Ann*n){
    n->next=NULL;
    if(!annHead){annHead=n;return;}
    struct Ann*c=annHead; while(c->next)c=c->next; c->next=n;
}
void pushAction(struct Act a){struct Act*n=malloc(sizeof(struct Act)); if(!n)exit(1); *n=a; n->next=stackTop; stackTop=n;}

void addAnn(void){
    char t[100],d[300],c[50],dt[20];
    getStr("Title: ",t,sizeof(t),0); getStr("Description: ",d,sizeof(d),1);
    getStr("Course: ",c,sizeof(c),0); getStr("Date (DD-MM-YYYY): ",dt,sizeof(dt),0);
    int pr=getIntR("Priority (1 Normal,2 Important,3 Urgent): ",1,3);
    int id=nextAnnId++;
    appendAnn(newAnn(id,t,d,c,dt,pr,1));
    struct Act a={0}; a.type=1; a.refId=id; snprintf(a.desc,sizeof(a.desc),"Added Announcement ID %d",id); pushAction(a);
    printf("Added with ID %d.\n",id);
}
void printAnn(struct Ann*n){
    const char*pl=n->priority==3?"URGENT":n->priority==2?"IMPORTANT":"NORMAL";
    printf("-- ID:%d [%s] %s | %s | %s\n   %s\n",n->id,pl,n->title,n->course,n->date,n->desc);
}
void showAnn(void){
    printf("\n-- ALL ANNOUNCEMENTS --\n");
    if(!annHead){printf("None.\n");return;}
    for(struct Ann*c=annHead;c;c=c->next) printAnn(c);
}
void showUrgentAnn(void){
    printf("\n-- URGENT ANNOUNCEMENTS --\n"); int f=0;
    for(struct Ann*c=annHead;c;c=c->next) if(c->priority==3){printAnn(c);f=1;}
    if(!f)printf("None.\n");
}
void showCourseAnn(void){
    char c[50]; getStr("Course: ",c,sizeof(c),0);
    printf("\n-- ANNOUNCEMENTS: %s --\n",c); int f=0;
    for(struct Ann*n=annHead;n;n=n->next) if(!strcmp(n->course,c)){printAnn(n);f=1;}
    if(!f)printf("None.\n");
}
struct Ann*findAnn(int id){for(struct Ann*c=annHead;c;c=c->next) if(c->id==id) return c; return NULL;}
void searchAnn(void){
    printf("Search by: 1.ID 2.Course 3.Title keyword\n");
    int ch=getInt("Choice: "); int f=0;
    if(ch==1){int id=getInt("ID: ");struct Ann*n=findAnn(id); if(n){printAnn(n);f=1;}}
    else if(ch==2){char c[50];getStr("Course: ",c,sizeof(c),0); for(struct Ann*n=annHead;n;n=n->next) if(!strcmp(n->course,c)){printAnn(n);f=1;}}
    else if(ch==3){char k[100];getStr("Keyword: ",k,sizeof(k),0); for(struct Ann*n=annHead;n;n=n->next) if(strstr(n->title,k)){printAnn(n);f=1;}}
    else {printf("Invalid.\n"); return;}
    if(!f)printf("Not found.\n");
}
void updateAnn(void){
    int id=getInt("ID to update: "); struct Ann*n=findAnn(id);
    if(!n){printf("Not found.\n");return;}
    struct Act a={0}; a.type=3; a.annB=*n; a.annB.next=NULL; snprintf(a.desc,sizeof(a.desc),"Updated Announcement ID %d",id);
    char t[100],d[300],c[50],dt[20];
    getStr("New title: ",t,sizeof(t),0); getStr("New description: ",d,sizeof(d),1);
    getStr("New course: ",c,sizeof(c),0); getStr("New date: ",dt,sizeof(dt),0);
    int pr=getIntR("New priority (1-3): ",1,3);
    strncpy(n->title,t,99);n->title[99]=0; strncpy(n->desc,d,299);n->desc[299]=0;
    strncpy(n->course,c,49);n->course[49]=0; strncpy(n->date,dt,19);n->date[19]=0; n->priority=pr;
    pushAction(a); printf("Updated.\n");
}
int delAnnById(int id){
    struct Ann*c=annHead,*p=NULL;
    while(c&&c->id!=id){p=c;c=c->next;}
    if(!c)return 0;
    if(!p)annHead=c->next; else p->next=c->next;
    free(c); return 1;
}
void deleteAnn(void){
    int id=getInt("ID to delete: "); struct Ann*n=findAnn(id);
    if(!n){printf("Not found.\n");return;}
    struct Act a={0}; a.type=2; a.annB=*n; a.annB.next=NULL; snprintf(a.desc,sizeof(a.desc),"Deleted Announcement ID %d",id);
    pushAction(a); delAnnById(id); printf("Deleted.\n");
}
void freeAnn(void){struct Ann*c=annHead; while(c){struct Ann*t=c;c=c->next;free(t);} annHead=NULL;}
void saveAnn(void){
    FILE*fp=fopen("announcements.txt","w"); if(!fp)return;
    int n=0; for(struct Ann*c=annHead;c;c=c->next)n++;
    fprintf(fp,"%d\n%d\n",n,nextAnnId);
    for(struct Ann*c=annHead;c;c=c->next) fprintf(fp,"%d\n%s\n%s\n%s\n%s\n%d\n%d\n",c->id,c->title,c->desc,c->course,c->date,c->priority,c->status);
    fclose(fp);
}
void loadAnn(void){
    FILE*fp=fopen("announcements.txt","r"); if(!fp)return;
    char b[512]; int n; if(!fgets(b,sizeof(b),fp)){fclose(fp);return;} sscanf(b,"%d",&n);
    if(!fgets(b,sizeof(b),fp)){fclose(fp);return;} sscanf(b,"%d",&nextAnnId);
    freeAnn();
    for(int i=0;i<n;i++){
        int id,pr,st; char t[100],d[300],c[50],dt[20];
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&id);
        rdLine(t,sizeof(t),fp); rdLine(d,sizeof(d),fp); rdLine(c,sizeof(c),fp); rdLine(dt,sizeof(dt),fp);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&pr);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&st);
        appendAnn(newAnn(id,t,d,c,dt,pr,st));
    }
    fclose(fp);
}

/* ---------- QUEUE (CR tasks) ---------- */
void enqueue(const char*d){
    struct QNode*n=malloc(sizeof(struct QNode)); if(!n)exit(1);
    n->id=nextTaskId++; strncpy(n->desc,d,149); n->desc[149]=0; n->next=NULL;
    if(!qRear)qFront=qRear=n; else {qRear->next=n; qRear=n;}
    printf("Task queued, ID %d.\n",n->id);
}
int qEmpty(void){return qFront==NULL;}
void dequeue(void){
    if(qEmpty()){printf("Queue empty.\n");return;}
    struct QNode*t=qFront; printf("Processed [%d] %s\n",t->id,t->desc);
    qFront=qFront->next; if(!qFront)qRear=NULL; free(t);
}
void peekQ(void){if(qEmpty())printf("Queue empty.\n"); else printf("Next [%d] %s\n",qFront->id,qFront->desc);}
void showQueue(void){
    printf("\n-- CR TASK QUEUE --\n"); if(qEmpty()){printf("Empty.\n");return;}
    int i=1; for(struct QNode*c=qFront;c;c=c->next) printf("%d. [%d] %s\n",i++,c->id,c->desc);
}
void freeQueue(void){struct QNode*c=qFront; while(c){struct QNode*t=c;c=c->next;free(t);} qFront=qRear=NULL;}
void queueMenu(void){
    int ch;
    do{
        printf("\n-- CR TASK QUEUE --\n1.Add 2.Dequeue 3.Peek 4.Show 5.Back\n");
        ch=getInt("Choice: ");
        if(ch==1){char d[150]; getStr("Task: ",d,sizeof(d),0); enqueue(d);}
        else if(ch==2) dequeue();
        else if(ch==3) peekQ();
        else if(ch==4) showQueue();
        else if(ch!=5) printf("Invalid.\n");
    } while(ch!=5);
}

/* ---------- MAX HEAP (deadlines) ---------- */
void swapDl(int i,int j){struct Dl t=heap[i]; heap[i]=heap[j]; heap[j]=t;}
void upHeap(int i){while(i>0){int p=(i-1)/2; if(heap[i].score>heap[p].score){swapDl(i,p);i=p;} else break;}}
void downHeap(int i){
    for(;;){
        int l=2*i+1,r=2*i+2,m=i;
        if(l<hs&&heap[l].score>heap[m].score)m=l;
        if(r<hs&&heap[r].score>heap[m].score)m=r;
        if(m==i)break; swapDl(i,m); i=m;
    }
}
int daysLeft(int d,int m,int y){
    time_t now=time(NULL); struct tm t={0}; t.tm_year=y-1900;t.tm_mon=m-1;t.tm_mday=d;t.tm_hour=23;t.tm_min=59;
    struct tm t0=*localtime(&now); t0.tm_hour=t0.tm_min=t0.tm_sec=0;
    return (int)(difftime(mktime(&t),mktime(&t0))/86400);
}
int calcScore(int d,int m,int y,int imp,int diff){
    int dl=daysLeft(d,m,y); if(dl<0)dl=0;
    int u = dl<=1?50:dl<=3?40:dl<=7?30:20;
    int im = imp==3?30:imp==2?20:10;
    int df = diff==3?20:diff==2?10:5;
    return u+im+df;
}
void heapInsert(struct Dl d){if(hs>=MAXD){printf("Deadline list full.\n");return;} heap[hs]=d; upHeap(hs); hs++;}
int heapTop(struct Dl*o){if(hs==0)return 0; *o=heap[0]; return 1;}
int heapPop(struct Dl*o){if(hs==0)return 0; *o=heap[0]; heap[0]=heap[hs-1]; hs--; if(hs>0)downHeap(0); return 1;}
int findDlIdx(int id){for(int i=0;i<hs;i++) if(heap[i].id==id) return i; return -1;}
int delDlAt(int idx,struct Dl*rm){
    if(idx<0||idx>=hs)return 0; *rm=heap[idx]; heap[idx]=heap[hs-1]; hs--;
    if(idx<hs){downHeap(idx); upHeap(idx);} return 1;
}
void addDeadline(void){
    struct Dl d={0};
    getStr("Course: ",d.course,sizeof(d.course),0); getStr("Task: ",d.task,sizeof(d.task),0);
    do{
        d.day=getInt("Due day: "); d.month=getInt("Due month: "); d.year=getInt("Due year: ");
        if(!validDate(d.day,d.month,d.year)) printf("Invalid date.\n");
    } while(!validDate(d.day,d.month,d.year));
    d.importance=getIntR("Importance (1 Low,2 Med,3 High): ",1,3);
    d.difficulty=getIntR("Difficulty (1 Low,2 Med,3 High): ",1,3);
    d.id=nextDlId++; d.status=1; d.score=calcScore(d.day,d.month,d.year,d.importance,d.difficulty);
    heapInsert(d);
    struct Act a={0}; a.type=4; a.refId=d.id; snprintf(a.desc,sizeof(a.desc),"Added Deadline ID %d",d.id); pushAction(a);
    printf("Added, ID %d, Score %d.\n",d.id,d.score);
}
const char*stLabel(int s){return s==2?"COMPLETED":s==3?"EXPIRED":"PENDING";}
void printDl(struct Dl*d){
    printf("-- ID:%d Score:%d [%s] %s | %s | Due %02d-%02d-%04d | Imp:%d Diff:%d\n",
        d->id,d->score,stLabel(d->status),d->task,d->course,d->day,d->month,d->year,d->importance,d->difficulty);
}
void showDeadlines(void){
    printf("\n-- ALL DEADLINES (by priority) --\n"); if(hs==0){printf("None.\n");return;}
    struct Dl tmp[MAXD]; for(int i=0;i<hs;i++) tmp[i]=heap[i];
    for(int i=0;i<hs-1;i++) for(int j=0;j<hs-1-i;j++) if(tmp[j].score<tmp[j+1].score){struct Dl t=tmp[j];tmp[j]=tmp[j+1];tmp[j+1]=t;}
    for(int i=0;i<hs;i++) printDl(&tmp[i]);
}
void searchDeadline(void){
    printf("Search by: 1.ID 2.Course 3.Task\n"); int ch=getInt("Choice: "); int f=0;
    if(ch==1){int id=getInt("ID: "); int i=findDlIdx(id); if(i!=-1){printDl(&heap[i]);f=1;}}
    else if(ch==2){char c[50];getStr("Course: ",c,sizeof(c),0); for(int i=0;i<hs;i++) if(!strcmp(heap[i].course,c)){printDl(&heap[i]);f=1;}}
    else if(ch==3){char k[100];getStr("Keyword: ",k,sizeof(k),0); for(int i=0;i<hs;i++) if(strstr(heap[i].task,k)){printDl(&heap[i]);f=1;}}
    else {printf("Invalid.\n"); return;}
    if(!f)printf("Not found.\n");
}
void markCompleted(void){
    int id=getInt("ID to mark completed: "); int i=findDlIdx(id);
    if(i==-1){printf("Not found.\n");return;}
    heap[i].status=2; printf("Marked completed.\n");
}
void deleteTopDeadline(void){
    struct Dl rm;
    if(!heapPop(&rm)){printf("None to delete.\n");return;}
    struct Act a={0}; a.type=5; a.dlB=rm; snprintf(a.desc,sizeof(a.desc),"Deleted Deadline ID %d",rm.id); pushAction(a);
    printf("Deleted:\n"); printDl(&rm);
}
void detectCluster(void){
    printf("\n-- DEADLINE CLUSTER DETECTOR --\n");
    int idx[MAXD],n=0;
    for(int i=0;i<hs;i++) if(heap[i].status==1){int r=daysLeft(heap[i].day,heap[i].month,heap[i].year); if(r>=0&&r<=2) idx[n++]=i;}
    if(n>=2){
        printf("CLUSTER DETECTED! %d tasks due within 48 hours:\n",n);
        for(int i=0;i<n;i++) printf("%d. %s (%s)\n",i+1,heap[idx[i]].task,heap[idx[i]].course);
        printf("WARNING: HIGH ACADEMIC WORKLOAD\n");
    } else printf("No cluster detected.\n");
}
void saveDl(void){
    FILE*fp=fopen("deadlines.txt","w"); if(!fp)return;
    fprintf(fp,"%d\n%d\n",hs,nextDlId);
    for(int i=0;i<hs;i++){struct Dl*d=&heap[i];
        fprintf(fp,"%d\n%s\n%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",d->id,d->course,d->task,d->day,d->month,d->year,d->difficulty,d->importance,d->score,d->status);}
    fclose(fp);
}
void loadDl(void){
    FILE*fp=fopen("deadlines.txt","r"); if(!fp)return;
    char b[512]; int n; if(!fgets(b,sizeof(b),fp)){fclose(fp);return;} sscanf(b,"%d",&n);
    if(!fgets(b,sizeof(b),fp)){fclose(fp);return;} sscanf(b,"%d",&nextDlId);
    hs=0;
    for(int i=0;i<n&&i<MAXD;i++){
        struct Dl d={0};
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&d.id);
        rdLine(d.course,sizeof(d.course),fp); rdLine(d.task,sizeof(d.task),fp);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&d.day);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&d.month);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&d.year);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&d.difficulty);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&d.importance);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&d.score);
        if(!fgets(b,sizeof(b),fp))break; sscanf(b,"%d",&d.status);
        heapInsert(d);
    }
    fclose(fp);
}

/* ---------- STACK (undo) ---------- */
int stEmpty(void){return stackTop==NULL;}
int popAction(struct Act*o){if(stEmpty())return 0; struct Act*n=stackTop; *o=*n; stackTop=n->next; free(n); return 1;}
int peekAction(struct Act*o){if(stEmpty())return 0; *o=*stackTop; return 1;}
void freeStack(void){struct Act*c=stackTop; while(c){struct Act*t=c;c=c->next;free(t);} stackTop=NULL;}
void undoLast(void){
    struct Act pk;
    if(!peekAction(&pk)){printf("Nothing to undo.\n");return;}
    printf("\nLast Action:\n%s\nUndo? 1.Yes 2.No\n",pk.desc);
    if(getInt("Choice: ")!=1){printf("Cancelled.\n");return;}
    struct Act a; popAction(&a);
    if(a.type==1){ if(delAnnById(a.refId)) printf("Undo: removed announcement ID %d.\n",a.refId); else printf("Undo failed.\n"); }
    else if(a.type==2){ appendAnn(newAnn(a.annB.id,a.annB.title,a.annB.desc,a.annB.course,a.annB.date,a.annB.priority,a.annB.status)); printf("Undo: restored announcement ID %d.\n",a.annB.id); }
    else if(a.type==3){ struct Ann*n=findAnn(a.annB.id); if(n){struct Ann*sv=n->next; *n=a.annB; n->next=sv; printf("Undo: reverted announcement ID %d.\n",n->id);} else printf("Undo failed.\n"); }
    else if(a.type==4){ int i=findDlIdx(a.refId); struct Dl rm; if(i!=-1&&delDlAt(i,&rm)) printf("Undo: removed deadline ID %d.\n",a.refId); else printf("Undo failed.\n"); }
    else if(a.type==5){ heapInsert(a.dlB); printf("Undo: restored deadline ID %d.\n",a.dlB.id); }
}

/* ---------- BRIEF / REPORTS / SEARCH ---------- */
void todaysBrief(void){
    printf("\n-- TODAY'S BRIEF --\n");
    struct Ann*u=NULL; for(struct Ann*c=annHead;c;c=c->next) if(c->priority==3){u=c;break;}
    printf("[URGENT ANNOUNCEMENT]\n%s\n",u?u->title:"None.");
    struct Dl top;
    printf("[UPCOMING DEADLINE]\n");
    if(heapTop(&top)){
        int r=daysLeft(top.day,top.month,top.year);
        printf("%s (%s)\n",top.task,top.course);
        if(r<0) printf("Remaining: OVERDUE\n"); else printf("Remaining: %d day(s)\n",r);
    } else printf("None.\n");
    struct Ann*nr=NULL; for(struct Ann*c=annHead;c;c=c->next) if(c->priority==1){nr=c;break;}
    printf("[NORMAL NOTICE]\n%s\n",nr?nr->title:"None.");
}
void reports(void){
    int ta=0,ua=0,ia=0,na=0;
    for(struct Ann*c=annHead;c;c=c->next){ta++; if(c->priority==3)ua++; else if(c->priority==2)ia++; else na++;}
    int pd=0,cd=0,ed=0;
    for(int i=0;i<hs;i++){ if(heap[i].status==1)pd++; else if(heap[i].status==2)cd++; else ed++; }
    printf("\n-- REPORTS --\nCourses: %d\nAnnouncements: %d (Urgent:%d Important:%d Normal:%d)\nDeadlines: %d (Pending:%d Completed:%d Expired:%d)\n",
        courseN,ta,ua,ia,na,hs,pd,cd,ed);
}
void globalSearch(void){
    printf("1.Search Announcement 2.Search Deadline\n");
    int c=getInt("Choice: ");
    if(c==1)searchAnn(); else if(c==2)searchDeadline();
}
void saveAll(void){saveAnn();saveDl();saveCourses();printf("All data saved.\n");}
void loadAll(void){loadAnn();loadDl();loadCourses();}

/* ---------- MENUS ---------- */
void annMenu(void){
    int ch;
    do{
        printf("\n-- ANNOUNCEMENTS --\n1.Add 2.ViewAll 3.ViewUrgent 4.Search 5.Update 6.Delete 7.ByCourse 8.Back\n");
        ch=getInt("Choice: ");
        if(ch==1)addAnn(); else if(ch==2)showAnn(); else if(ch==3)showUrgentAnn();
        else if(ch==4)searchAnn(); else if(ch==5)updateAnn(); else if(ch==6)deleteAnn();
        else if(ch==7)showCourseAnn(); else if(ch!=8)printf("Invalid.\n");
    } while(ch!=8);
}
void dlMenu(void){
    int ch;
    do{
        printf("\n-- DEADLINES --\n1.Add 2.ViewAll 3.MostUrgent 4.Search 5.MarkDone 6.DeleteTop 7.Cluster 8.Back\n");
        ch=getInt("Choice: ");
        if(ch==1)addDeadline(); else if(ch==2)showDeadlines();
        else if(ch==3){struct Dl t; if(heapTop(&t)) printDl(&t); else printf("None.\n");}
        else if(ch==4)searchDeadline(); else if(ch==5)markCompleted();
        else if(ch==6)deleteTopDeadline(); else if(ch==7)detectCluster();
        else if(ch!=8)printf("Invalid.\n");
    } while(ch!=8);
}
void classInfoMenu(void){
    int ch;
    do{
        printf("\n-- CLASS INFO --\n1.AddCourse 2.ViewCourses 3.Back\n");
        ch=getInt("Choice: ");
        if(ch==1)addCourse(); else if(ch==2)showCourses(); else if(ch!=3)printf("Invalid.\n");
    } while(ch!=3);
}
void mainMenu(void){
    int ch;
    do{
        printf("\n===== CLASS COMMAND CENTER =====\n1.Announcements 2.Deadlines 3.TodaysBrief 4.TaskQueue 5.Undo 6.Search 7.ClassInfo 8.Reports 9.Save 0.Exit\n");
        ch=getInt("Choice: ");
        switch(ch){
            case 1: annMenu(); break;
            case 2: dlMenu(); break;
            case 3: todaysBrief(); break;
            case 4: queueMenu(); break;
            case 5: undoLast(); break;
            case 6: globalSearch(); break;
            case 7: classInfoMenu(); break;
            case 8: reports(); break;
            case 9: saveAll(); break;
            case 0: printf("Saving...\n"); saveAll(); printf("Goodbye!\n"); break;
            default: printf("Invalid.\n");
        }
    } while(ch!=0);
}

int main(void){
    printf("Loading data...\n"); loadAll(); printf("Ready.\n");
    mainMenu();
    freeAnn(); freeQueue(); freeStack();
    return 0;
}
