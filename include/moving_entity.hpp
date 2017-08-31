#ifndef ALS_MOVING_ENTITY_HPP
#define ALS_MOVING_ENTITY_HPP
#include "entity.hpp"
namespace als
{
  class moving_entity : public entity
  {
  public:
    using entity::entity;
    using entity::set_texture;
    void move();
    void set_x_velocity(float x_v);
    void set_y_velocity(float y_v);
    float x_velocity()const;
    float y_velocity()const;
    void apply_scm_callback(entity &rhs);
  protected:
    float x_velocity_ = 0.0f;
    float y_velocity_ = 0.0f;
  };
}
#endif
