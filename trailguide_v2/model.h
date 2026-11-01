#pragma once
#include <cstddef>
#include <cstdint>
namespace trailguide {
enum class View { Home, Card, Guide };
enum class Mode { Ask, Try };
struct Model {
  View view = View::Home;
  Mode mode = Mode::Ask;
  std::size_t index = 0;
  bool hasCard() const { return view != View::Home; }
  void start(Mode nextMode) { mode = nextMode; index = 0; view = View::Card; }
  static constexpr std::size_t count = 8;
  void next() { if (view == View::Card) index = (index + 1) % count; }
  void guide() { if (view == View::Card) view = View::Guide; }
  void back() { if (view == View::Guide) view = View::Card; else home(); }
  void home() { view = View::Home; }
};
}
