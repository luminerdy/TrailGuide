// Synthetic host policy tests, not hardware gesture data.
#include <cassert>
#include <set>
#include <map>
#include <array>
#include <cstdio>
#include "../trailguide_v3/model.h"
static uint32_t random_word(void *p) { auto &s=*static_cast<uint32_t*>(p); s=s*1664525u+1013904223u; return s; }
int main() {
 uint32_t seed=7; v3app::Model m(random_word,&seed);
 assert(m.view==v3app::View::Home && m.index==-1);
 static_assert(trailguide::cardCount==12,"V3 selection must reach the actual model");
 for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try}) {
  int last=-1;
  for(int cycle=0;cycle<100;cycle++) {std::set<int> ids;
   for(unsigned n=0;n<trailguide::cardCount;n++) {m.draw(mode); assert(m.index!=last); ids.insert(m.index);last=m.index;}
   assert(ids.size()==trailguide::cardCount);
  }
 }
 auto saved=m.index; m.guide(); m.back(); assert(m.index==saved);m.home();assert(m.index==saved);
 uint32_t a=42,b=42; v3app::Model x(random_word,&a),y(random_word,&b);
 for(int n=0;n<100;n++){x.draw(v3app::Mode::Ask);y.draw(v3app::Mode::Ask);assert(x.index==y.index);}
 // Empty/single pools consume no RNG and never modulo zero or spin.
 unsigned calls=0;auto counted=[](void *p)->uint32_t{++*static_cast<unsigned*>(p);return 0;};
 v3app::BasicModel<0> empty(counted,&calls);v3app::BasicModel<1> one(counted,&calls);
 for(int n=0;n<100;n++){empty.draw(v3app::Mode::Ask);assert(empty.index==-1 && empty.view==v3app::View::Home);one.draw(v3app::Mode::Try);assert(one.index==0);}
 assert(!calls);
 // Partial Try draws cannot consume/reset the existing Ask bag.
 a=99;b=99;v3app::Model left(random_word,&a),right(random_word,&b);
 left.draw(v3app::Mode::Ask);right.draw(v3app::Mode::Ask);
 for(int n=0;n<7;n++)left.draw(v3app::Mode::Try);
 for(unsigned n=1;n<trailguide::cardCount;n++){left.draw(v3app::Mode::Ask);right.draw(v3app::Mode::Ask);assert(left.index==right.index);}
 // Enumerate each initial Fisher-Yates choice for a three-card bag.
 std::map<std::array<int,3>,unsigned> permutations;
 for(unsigned j3=0;j3<3;j3++)for(unsigned j2=0;j2<2;j2++){
  struct Words {uint32_t words[2];unsigned at=0;} words{{3+j3,2+j2}};
  auto supplied=[](void *p)->uint32_t{auto &w=*static_cast<Words*>(p);assert(w.at<2);return w.words[w.at++];};
  v3app::BasicModel<3> small(supplied,&words);std::array<int,3> order{};
  for(auto &v:order){small.draw(v3app::Mode::Ask);v=small.index;}++permutations[order];assert(words.at==2);
 }
 assert(permutations.size()==6);for(auto entry:permutations)assert(entry.second==1);
 a=123;v3app::BasicModel<40> maximum(random_word,&a);std::set<int> all;
 for(int n=0;n<40;n++){maximum.draw(v3app::Mode::Ask);all.insert(maximum.index);}assert(all.size()==40);
 puts("PASS model: no boot draw, two independent fair 12-card bags, 100 boundaries/mode, N=0/1/3/40, six permutations, injected RNG, navigation");
}
