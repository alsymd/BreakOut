#include "renderer.hpp"
#include "entity.hpp"
#include "moving_entity.hpp"
#include "texture.hpp"
namespace als
{
  entity::entity(const rect_shape &shape, const std::string &texture_path):
    collider_(shape), texture_(std::make_shared<texture>(texture_path))
  {
  }

  void entity::render(const renderer &rend)const
  {
    rend.render(*texture_, collider_);
  }

  void entity::handle_collide(entity &rhs)
  {
    if(collider_.collide(rhs.collider_))
      {
	apply_scm_callback(rhs);
	rhs.apply_scm_callback(*this);
      }
  }

  void entity::set_texture(const std::string &path)
  {
    texture_=std::make_shared<texture>(path);
  }

  void entity::apply_scm_callback_entity(entity &rhs)
  {
    if(on_collision_with_entity)
      scm_call_2(*on_collision_with_entity,
		 scm_from_pointer(reinterpret_cast<void*>(this), nullptr),
		 scm_from_pointer(reinterpret_cast<void*>(this), nullptr));
  }
  
  void entity::apply_scm_callback(entity &rhs)
  {
    rhs.apply_scm_callback_entity(*this);
  }


  void entity::apply_scm_callback_ball(moving_entity &rhs)
  {
    if(on_collision_with_ball)
      scm_call_2(*on_collision_with_ball,
		 scm_from_pointer(reinterpret_cast<void*>(this), nullptr),
		 scm_from_pointer(reinterpret_cast<void*>(&rhs), nullptr));
  }

  void entity::register_ball_callback(SCM cb)
  {
    static const auto destructor = [](SCM *procedure){scm_gc_unprotect_object(*procedure); delete procedure; };
    scm_gc_protect_object(cb);
    on_collision_with_ball = std::shared_ptr<SCM>(new SCM (cb),destructor);
  }

  void entity::register_entity_callback(SCM cb)
  {
    static const auto destructor = [](SCM *procedure){scm_gc_unprotect_object(*procedure);};
    scm_gc_protect_object(cb);    
    on_collision_with_entity = std::shared_ptr<SCM>(new SCM (cb),destructor);
  }

  entity::entity():texture_(std::make_shared<texture>())
  {
    
  }

  const rect_shape &entity::collider()const{return collider_;}

  rect_shape &entity::collider(){return collider_;}

  void entity::set_x(float x){collider_.x=x;}
  void entity::set_y(float y){collider_.y=y;}
}
