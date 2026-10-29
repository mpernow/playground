#include <type_traits>
#include <utility>
#include <vector>

#include "PixelBuffer.hpp"
#include "demos.hpp"
#include "report.hpp"

namespace {

// --- Deduction and reference collapsing, checked at compile time ---
// None of this needs to run: if a static_assert is wrong, the build fails.

// Returns the deduced T itself, wrapped so decltype can inspect it.
template <typename T> std::type_identity<T> deduce_plain(T);
template <typename T> std::type_identity<T> deduce_fwd(T &&);

int lv = 0;

// A plain T parameter strips the argument down to its type...
static_assert(std::is_same_v<decltype(deduce_plain(lv))::type, int>);
// ...but T&& deduces from the argument's value category:
static_assert(std::is_same_v<decltype(deduce_fwd(lv))::type, int &>); // lvalue
static_assert(std::is_same_v<decltype(deduce_fwd(0))::type, int>);    // rvalue

// Collapsing: only && + && stays an rvalue reference.
using LRef = int &;
using RRef = int &&;
static_assert(std::is_same_v<LRef &, int &>);
static_assert(std::is_same_v<LRef &&, int &>);
static_assert(std::is_same_v<RRef &, int &>);
static_assert(std::is_same_v<RRef &&, int &&>);

// So substituting the deduced T back into T&& gives:
//   lvalue arg: T = int&  ->  T&& = int& &&  ->  int&
//   rvalue arg: T = int   ->  T&& = int&&
template <typename T> constexpr bool deduces_as_lvalue_ref(T &&) {
  return std::is_lvalue_reference_v<T &&>;
}
static_assert(deduces_as_lvalue_ref(lv));
static_assert(!deduces_as_lvalue_ref(0));

// const T&& is not a forwarding reference, just an rvalue reference to const
// T, so it rejects lvalues. Uncomment to see the error:
//   cannot bind rvalue reference of type 'const int&&' to lvalue of type 'int'
template <typename T> void const_rref(const T &&x) { (void)x; }
// void try_const_rref() { const_rref(lv); }

// --- Perfect forwarding at runtime ---

// buf is a named variable, so it is an lvalue expression whatever T is. The
// caller's value category is lost and push_back always copies.
template <typename T>
void archive_broken(T &&buf, std::vector<PixelBuffer> &dest) {
  dest.push_back(buf);
}

// std::forward<T> casts back to T&&: an rvalue if T = PixelBuffer, an lvalue
// if T = PixelBuffer&. Moves what the caller gave away, copies what it kept.
template <typename T> void archive(T &&buf, std::vector<PixelBuffer> &dest) {
  dest.push_back(std::forward<T>(buf));
}

// Reserved up front: otherwise the vector reallocating would move existing
// elements and inflate move_count.
std::vector<PixelBuffer> empty_store() {
  std::vector<PixelBuffer> store;
  store.reserve(1);
  return store;
}

} // namespace

void demo_forwarding() {
  section("perfect forwarding");
  QuietLogs quiet;
  report("archive_broken(rvalue arg):", [] {
    auto store = empty_store();
    archive_broken(PixelBuffer(1920, 1080), store);
  });
  report("archive(rvalue arg):", [] {
    auto store = empty_store();
    archive(PixelBuffer(1920, 1080), store);
  });
  report("archive(lvalue arg):", [] {
    auto store = empty_store();
    PixelBuffer p(1920, 1080);
    archive(p, store);
  });
}
