#include <cassert>
#include "../trailguide_v2/model.h"
int main() {
  trailguide::Model model;
  assert(model.view == trailguide::View::Home);
  assert(!model.hasCard());
  model.start(trailguide::Mode::Ask);
  assert(model.view == trailguide::View::Card);
  assert(model.mode == trailguide::Mode::Ask);
  assert(model.index < 8);
  const auto first = model.index;
  model.guide();
  assert(model.view == trailguide::View::Guide);
  model.back();
  assert(model.view == trailguide::View::Card && model.index == first);
  model.home();
  assert(model.view == trailguide::View::Home && !model.hasCard());
  model.next();
  assert(!model.hasCard());
  model.start(trailguide::Mode::Try);
  assert(model.mode == trailguide::Mode::Try);
  bool seen[8] = {};
  for (unsigned n = 0; n < 1000; ++n) {
    assert(model.index < 8);
    if (n < 8) { assert(!seen[model.index]); seen[model.index] = true; }
    const auto previous = model.index;
    model.next();
    assert(model.index != previous);
    assert(model.mode == trailguide::Mode::Try);
  }
  model.guide();
  const auto guided = model.index;
  model.next();
  assert(model.index == guided && model.view == trailguide::View::Guide);
}
