#pragma once
#include <stdint.h>
#include <string.h>
#include "coaching_generated.h"
namespace trailguide_coach {
using RandomWord=uint32_t(*)(void*);
enum class Screen { Opening, Question, Experiment, Guide, Reminders, Closing, Picker, Options };
struct Context { Screen screen=Screen::Opening; int path=-1,index=-1,scroll=0; };
// Bounded shuffle; constant/random boundary values cannot cause rejection loops.
template<unsigned N> class Pool {
 unsigned ids[N?N:1]={},next=N;int last=-1;
 public:int draw(RandomWord random,void *context){if constexpr(!N)return -1;
  if(next==N){for(unsigned i=0;i<N;++i)ids[i]=i;for(unsigned i=N;i>1;--i){unsigned j=random(context)%i;unsigned t=ids[i-1];ids[i-1]=ids[j];ids[j]=t;}if(N>1&&int(ids[0])==last){unsigned t=ids[0];ids[0]=ids[1];ids[1]=t;}next=0;}
  last=int(ids[next++]);return last;
 }
};
constexpr bool equal(const char *a,const char *b){while(*a&&*a==*b){++a;++b;}return *a==*b;}
constexpr unsigned count(const char *type){unsigned n=0;for(auto &e:trailguide_coaching::entries)if(equal(e.type,type))++n;return n;}
// Text is generated once. State is RAM-only, with separate path/face scroll memory.
class CoachModel {
 RandomWord random;void *context;Pool<count("opener")> openers;Context stack[16]{};unsigned depth=0;
 Screen lastFace[6]={};
 public:Context current{};Context opening{},questions[6]{},experiments[6]{};unsigned draws=0;
 CoachModel(RandomWord r,void *c):random(r),context(c){for(int p=0;p<6;++p){lastFace[p]=Screen::Question;questions[p]={Screen::Question,p,first("question",p),0};experiments[p]={Screen::Experiment,p,first("experiment",p),0};}}
 static int first(const char *type,int path=-1){for(unsigned i=0;i<trailguide_coaching::entryCount;++i){auto &e=trailguide_coaching::entries[i];if(e.path==path&&!strcmp(e.type,type))return int(i);}return -1;}
 const trailguide_coaching::Entry *entry()const{return current.index<0?nullptr:&trailguide_coaching::entries[current.index];}
 void remember(int y){current.scroll=y;if(current.screen==Screen::Opening)opening=current;if(current.path>=0){if(current.screen==Screen::Question){questions[current.path]=current;lastFace[current.path]=Screen::Question;}if(current.screen==Screen::Experiment){experiments[current.path]=current;lastFace[current.path]=Screen::Experiment;}}}
 void push(Context c){remember(current.scroll);if(depth<16)stack[depth++]=current;current=c;}
 void back(){if(depth)current=stack[--depth];}
 void enter(){if(opening.index<0)draw();else{current=opening;depth=0;}}
 void draw(){int n=openers.draw(random,context);int i=-1;for(unsigned k=0;k<trailguide_coaching::entryCount;++k)if(equal(trailguide_coaching::entries[k].type,"opener")&&n--==0){i=int(k);break;}current={Screen::Opening,-1,i,0};opening=current;depth=0;++draws;}
 void path(int p){if(p<0||p>=6)return;if(current.screen==Screen::Picker&&current.path==p){back();return;}const Context c=lastFace[p]==Screen::Experiment?experiments[p]:questions[p];if(current.screen==Screen::Picker){current=c;depth=0;}else push(c);}
 void picker(){push({Screen::Picker,current.path,-1,0});}
 void select(int p){if(p<0||p>=6)return;current.path=p;}
 void nextStep(){release();}
 void toOpening(){remember(current.scroll);current=opening;depth=0;}
 void guide(){push({Screen::Guide,current.path,current.index,0});}
 void reminders(){push({Screen::Reminders,-1,first("reminder"),0});}
 void release(){if(current.screen!=Screen::Closing)push({Screen::Closing,-1,first("closing"),0});}
 void options(){push({Screen::Options,current.path,current.index,0});}
 void question(){if(current.path>=0){remember(current.scroll);current=questions[current.path];}}
 void experiment(){if(current.path>=0){remember(current.scroll);current=experiments[current.path];}}
 void next(){const auto *e=entry();if(!e)return;int firstIndex=-1,nextIndex=-1;for(unsigned i=0;i<trailguide_coaching::entryCount;++i){const auto &v=trailguide_coaching::entries[i];if(v.path==e->path&&!strcmp(v.type,e->type)){if(firstIndex<0)firstIndex=int(i);if(int(i)>current.index&&nextIndex<0)nextIndex=int(i);}}current.index=nextIndex<0?firstIndex:nextIndex;current.scroll=0;remember(0);}
};
}
