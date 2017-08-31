#ifndef ALS_ENTITY_HPP
#define ALS_ENTITY_HPP
#include"rect_shape.hpp"
#include<memory>
#include<libguile.h>
namespace als
{
  class texture;
  class renderer;
  class moving_entity;
  class entity
  {
  public:
    entity();
    entity(const rect_shape &shape, const std::string &texture_path);
    virtual ~entity()=default;
    virtual void render(const renderer &rend)const;
    void handle_collide(entity &rhs);
    void register_ball_callback(SCM cb);
    void register_entity_callback(SCM cb);
    void set_x(float x);
    void set_y(float y);
    void set_texture(const std::string &texture_path);
    rect_shape &collider();
    const rect_shape &collider()const;
    virtual void apply_scm_callback(entity &rhs);
    virtual void apply_scm_callback_ball(moving_entity &rhs);
    virtual void apply_scm_callback_entity(entity &rhs);
  protected:
    std::shared_ptr<SCM> on_collision_with_ball = nullptr;
    std::shared_ptr<SCM> on_collision_with_entity = nullptr;
    rect_shape collider_;
    std::shared_ptr<texture>texture_;
  };
}
#endif
