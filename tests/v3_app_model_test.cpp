// Synthetic host policy tests, not hardware gesture data.
#include <cassert>
#include <set>
#include <cstdio>
#include "../trailguide_v3/model.h"
static uint32_t random_word(void *p) { auto &s=*static_cast<uint32_t*>(p); s=s*1664525u+1013904223u; return s; }
int main() {
 uint32_t seed=7; v3app::Model m(random_word,&seed);
 assert(m.view==v3app::View::Home && m.index==-1);
 for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try}) {
  int last=-1;
  for(int cycle=0;cycle<100;cycle++) {std::set<int> ids;
   for(int n=0;n<8;n++) {m.draw(mode); assert(m.index!=last); ids.insert(m.index);last=m.index;}
   assert(ids.size()==8);
  }
 }
 auto saved=m.index; m.guide(); m.back(); assert(m.index==saved);m.home();assert(m.index==saved);
 uint32_t a=42,b=42; v3app::Model x(random_word,&a),y(random_word,&b);
 for(int n=0;n<100;n++){x.draw(v3app::Mode::Ask);y.draw(v3app::Mode::Ask);assert(x.index==y.index);}
 puts("PASS model: no boot draw, two fair 8-card bags, 100 boundaries/mode, injected RNG, navigation");
}
