#pragma once
#include <stdint.h>
namespace v3app {
enum class Mode { Ask, Try };
enum class View { Home, Card, Guide };
using RandomWord=uint32_t(*)(void*);
// Rejection avoids modulo bias. Caller supplies host deterministic or esp_random.
class Model {
 struct Bag { uint8_t ids[8]={}; unsigned next=8; int last=-1; } bags[2];
 RandomWord rng; void *context;
 unsigned bounded(unsigned n) { const uint32_t low=uint32_t(-n)%n; uint32_t r; do {r=rng(context);}while(r<low);return r%n; }
 public:
 Model(RandomWord r,void *c):rng(r),context(c){}
 View view=View::Home; Mode mode=Mode::Ask; int index=-1;
 void draw(Mode selected) {
  mode=selected;auto &b=bags[unsigned(mode)];
  if(b.next==8) {
   // Uniform permutations conditioned on first != previous final card.
   do {for(unsigned i=0;i<8;i++)b.ids[i]=i;for(unsigned i=7;i>0;i--){unsigned j=bounded(i+1);auto t=b.ids[i];b.ids[i]=b.ids[j];b.ids[j]=t;}}while(b.ids[0]==b.last);
   b.next=0;
  }
  index=b.ids[b.next++];b.last=index;view=View::Card;
 }
 void guide(){if(view==View::Card)view=View::Guide;}
 void back(){if(view==View::Guide)view=View::Card;}
 void home(){view=View::Home;}
};
}
