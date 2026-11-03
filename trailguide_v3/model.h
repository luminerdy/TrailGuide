#pragma once
#include <stdint.h>
#include "cards_generated.h"
namespace v3app {
enum class Mode { Ask, Try };
enum class View { Home, Card, Guide };
using RandomWord=uint32_t(*)(void*);
// Rejection avoids modulo bias. Caller supplies host deterministic or esp_random.
// Count comes from the generated validated selection, never a second editable pool.
template<unsigned N> class BasicModel {
 static_assert(N<=40,"Card bag exceeds canonical catalog maximum");
 struct Bag { uint8_t ids[N?N:1]={}; unsigned next=N; int last=-1; } bags[2];
 RandomWord rng; void *context;
 unsigned bounded(unsigned n) { if(n<2)return 0;const uint32_t low=uint32_t(-n)%n; uint32_t r; do {r=rng(context);}while(r<low);return r%n; }
 public:
 BasicModel(RandomWord r,void *c):rng(r),context(c){}
 View view=View::Home; Mode mode=Mode::Ask; int index=-1;
 void draw(Mode selected) {
  mode=selected;
  if constexpr(N==0){index=-1;view=View::Home;return;}
  auto &b=bags[unsigned(mode)];
  if(b.next==N) {
   // Uniform permutations conditioned on first != previous final card.
   do {for(unsigned i=0;i<N;i++)b.ids[i]=i;for(unsigned i=N;i>1;--i){unsigned j=bounded(i);auto t=b.ids[i-1];b.ids[i-1]=b.ids[j];b.ids[j]=t;}}while(N>1 && b.ids[0]==b.last);
   b.next=0;
  }
  index=b.ids[b.next++];b.last=index;view=View::Card;
 }
 void guide(){if(view==View::Card)view=View::Guide;}
 void back(){if(view==View::Guide)view=View::Card;}
 void home(){view=View::Home;}
};
using Model=BasicModel<trailguide::cardCount>;
}
