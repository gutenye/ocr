#include "shared.h"

std::ostream& operator<<(std::ostream& os, const Frame& obj) {
  os << "Frame[" << obj.top << "," << obj.left << " " << obj.width << "x" << obj.height << "]";
  return os;
}