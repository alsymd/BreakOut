#include "moving_entity.hpp"
namespace als
{
  void moving_entity::move()
  {
    collider_.x += x_velocity_;
    collider_.y += y_velocity_;
  }
  void moving_entity::apply_scm_callback(entity &rhs)
  {
    rhs.apply_scm_callback_ball(*this);
  }
  float moving_entity::x_velocity()const{return x_velocity_;}
  float moving_entity::y_velocity()const{return y_velocity_;}
  void moving_entity::set_x_velocity(float x_v){x_velocity_ = x_v;}
  void moving_entity::set_y_velocity(float y_v){y_velocity_ = y_v;}
}
