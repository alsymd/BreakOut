#include "rect_shape.hpp"
namespace als
{
  bool rect_shape::collide(const rect_shape &rhs)const
  {
    auto this_v_top = y;
    auto this_v_bottom = y+h;
    auto rhs_v_top = rhs.y;
    auto rhs_v_bottom = rhs.y + rhs.h;
    auto v_intersect = ! (this_v_top > rhs_v_bottom || this_v_bottom < rhs_v_top);
    auto this_h_left = x;
    auto this_h_right = x+w;
    auto rhs_h_left = rhs.x;
    auto rhs_h_right = rhs.x + rhs.w;
    auto h_intersect = ! (this_h_left > rhs_h_right || this_h_right < rhs_h_left);
    return v_intersect && h_intersect;
  }
}
