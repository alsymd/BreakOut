#ifndef ALS_RECT_SHAPE_HPP
#define ALS_RECT_SHAPE_HPP
namespace als
{
  class rect_shape
  {
  public:
    bool collide(const rect_shape &rhs)const;
    float x;
    float y;
    float w;
    float h;
  };
}
#endif
