#include <utility>

#include "PixelBuffer.hpp"
#include "demos.hpp"
#include "report.hpp"

namespace {

// Guaranteed copy elision (C++17): PixelBuffer(1920, 1080) is a prvalue, so
// it is constructed directly in the caller's storage. There is no temporary
// to copy or move from - this is mandatory, not an optimisation.
PixelBuffer make_frame() { return PixelBuffer(1920, 1080); }

// NRVO: frame is a named local. The compiler is *allowed* to construct it in
// the caller's storage, but not required to.
PixelBuffer make_named_frame() {
  PixelBuffer frame(1920, 1080);
  return frame;
}

// Defeating NRVO on purpose: which local is returned depends on a runtime
// branch, so neither can be built in the caller's storage up front. Returning
// a local by name is still move-eligible, so the fallback is an implicit move,
// never a copy.
PixelBuffer make_one_of_two(bool pick_first) {
  PixelBuffer first(1920, 1080);
  PixelBuffer second(3840, 2160);
  if (pick_first) {
    return first;
  }
  return second;
}

// The classic mistake: std::move turns the return expression into an xvalue
// instead of a plain local name, which disqualifies NRVO. Trying to "help"
// costs a move that make_named_frame doesn't pay. GCC flags this with
// -Wpessimizing-move, silenced here only because the point is to show it.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpessimizing-move"
PixelBuffer make_moved_frame() {
  PixelBuffer frame(1920, 1080);
  return std::move(frame);
}
#pragma GCC diagnostic pop

// Variadic perfect forwarding into the constructor. The return expression is
// a prvalue, so this elides exactly like make_frame.
template <typename... Args> PixelBuffer make_buffer(Args &&...args) {
  return PixelBuffer(std::forward<Args>(args)...);
}

} // namespace

void demo_elision() {
  section("guaranteed elision vs NRVO");
  QuietLogs quiet;
  report("make_frame():", [] { PixelBuffer a = make_frame(); });
  report("make_named_frame():", [] { PixelBuffer a = make_named_frame(); });
  report("make_one_of_two():", [] { PixelBuffer a = make_one_of_two(true); });
  report("make_moved_frame():", [] { PixelBuffer a = make_moved_frame(); });
  report("make_buffer(1920, 1080):",
         [] { PixelBuffer a = make_buffer(1920, 1080); });
}
