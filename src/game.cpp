#include <algorithm>
#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <cstdlib>
#include <cassert>
#include "entity.hpp"
#include "moving_entity.hpp"
#include <iostream>
namespace als
{
  game::game(unsigned int window_width, unsigned int window_height, const std::string &window_title,
	     const std::string &bkg_path):
    window(sf::VideoMode(window_width,window_height),window_title,sf::Style::Close),
    background(
	       rect_shape
	       {
		 0.0f,
		   0.0f,
		   static_cast<float>(window_width),
		   static_cast<float>(window_height)
		   }
	       ,bkg_path),
    rend(&window)
  {
  }

  void game::win()
  {
    std::cout<<"You won!"<<std::endl;
    life_is_too_much = true;
  }
  
  void game::die()
  {
    if(life == 0u)
      game_over();
    else
      {
	assert(life!=0u);
	ball_board_sync = true;
	--life;
      }
  }
  
  void game::register_space_release_callback(SCM cb)
  {
    space_release_callback = cb;
  }

  void game::register_left_release_callback(SCM cb)
  {
    left_release_callback = cb;
  }

  void game::register_right_release_callback(SCM cb)
  {
    right_release_callback = cb;
  }

  void game::remove_brick(entity *const addr)
  {
    bricks.erase(std::remove_if(std::begin(bricks), std::end(bricks), [addr](const auto &b){return &b==addr;}), std::end(bricks));
    std::cout<<"Score: "<< ++score<<std::endl;
    if(bricks.empty())win();
  }
  
  void game::handle_input()
  {
	
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
      {
	scm_call_0(left_callback);
      }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
      {
	scm_call_0(right_callback);
      }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
      {
	if(ball_board_sync)
	  {
	    scm_call_0(space_callback);
	    ball_board_sync = false;
	  }
      }
    else
      {
	board.set_x_velocity(0);
	if(ball_board_sync)
	  ball.set_x_velocity(0);
      }
  }

  bool game::run()
  {
    sf::Event event;
    auto last_time_handled  = std::chrono::high_resolution_clock::now();
    auto lag = std::chrono::high_resolution_clock::duration(0);
    //Event handling
    while(!life_is_too_much)
      {
	while(window.pollEvent(event))
	  if(event.type == sf::Event::Closed)
	    life_is_too_much = true;
	handle_input();
	//Update
	auto curr_time = std::chrono::high_resolution_clock::now();
	auto elapsed = curr_time - last_time_handled;
	last_time_handled = curr_time;
	lag += elapsed;
	while(lag > update_interval)
	  {
	    ball.move();
	    board.move();
	    for(auto &brick : bricks)
	      ball.handle_collide(brick);
	    for(auto &obj : bound_or_something_else)
	      ball.handle_collide(obj);
	    for(auto &obj : bound_or_something_else)
	      board.handle_collide(obj);
	    ball.handle_collide(board);
	    lag-=update_interval;
	  }
	//Render
	render_all();
      }
    return EXIT_SUCCESS;
  }

  void game::render_all()
  {
    rend.clear();
    background.render(rend);
    board.render(rend);
    ball.render(rend);
    for(const auto &brick : bricks)
      brick.render(rend);
    for(const auto &obj : bound_or_something_else)
      obj.render(rend);
    window.display();
    
  }

  void game::register_space_callback(SCM cb)
  {
    space_callback = cb;
  }

  void game::register_left_callback(SCM cb)
  {
    left_callback = cb;
  }

  void game::register_right_callback(SCM cb)
  {
    right_callback = cb;
  }

  
  void game::push_brick(const entity &e){bricks.push_back(e);}
  void game::push_brick(entity &&e){bricks.push_back(std::move(e));}

  void game::push_bound(const entity &e){bound_or_something_else.push_back(e);}
  void game::push_bound(entity &&e){bound_or_something_else.push_back(std::move(e));}




  //Scheme api
  game *new_game = nullptr;


  static SCM remove_brick_scm(SCM brick_ref)
  {
    new_game->remove_brick(reinterpret_cast<entity*>(scm_to_pointer(brick_ref)));
    return SCM_UNSPECIFIED;
  }

  
  static SCM register_space(SCM cb)
  {
    new_game->register_space_callback(cb);
    return SCM_UNSPECIFIED;
  }

  static SCM register_left(SCM cb)
  {
    new_game->register_left_callback(cb);
    return SCM_UNSPECIFIED;
  }

  static SCM register_right(SCM cb)
  {
    new_game->register_right_callback(cb);
    return SCM_UNSPECIFIED;
  }

  
  SCM construct_game(SCM screen_width, SCM screen_height, SCM background, SCM window_title,
		     SCM ball_sprite, SCM border_sprite)
  {
    //Yuri saikoda!!!!
    new_game = new game(scm_to_uint(screen_width), scm_to_uint(screen_height),
			     scm_to_locale_string(window_title),
			     scm_to_locale_string(background));
    auto ball_addr = &new_game->ball;
    auto board_addr = &new_game->board;
    static const auto destructor = [](void *g){delete reinterpret_cast<game*>(g);};
    static const auto destructor_moving = [](void *g){delete reinterpret_cast<moving_entity*>(g);};
    new_game->board.set_texture(scm_to_locale_string(border_sprite));
    new_game->ball.set_texture(scm_to_locale_string(ball_sprite));
    auto ret = scm_c_make_vector(3,SCM_UNSPECIFIED);
    scm_c_vector_set_x(ret, 0, scm_from_pointer(new_game, destructor));
    scm_c_vector_set_x(ret, 1, scm_from_pointer(ball_addr, destructor_moving));
    scm_c_vector_set_x(ret, 2, scm_from_pointer(board_addr, destructor_moving));
    return ret;
  }

  
  static SCM create_prototype(SCM width, SCM height, SCM sprite_path, SCM collision_callback)
  {
    auto new_prototype = new entity{rect_shape{0.0f,0.0f,
					       static_cast<float>(scm_to_double(width)),
					       static_cast<float>(scm_to_double(height))},
				    scm_to_locale_string(sprite_path)};
    static const auto destructor = [](void *e){delete reinterpret_cast<entity*>(e);};
    new_prototype->register_ball_callback(collision_callback);
    return scm_from_pointer(new_prototype, destructor);
  }

  static SCM push_prototype(SCM prototype_ref, SCM x, SCM y)
  {
    auto prototype = * reinterpret_cast<const entity*>(scm_to_pointer(prototype_ref));
    entity cloned(prototype);
    cloned.set_x(static_cast<float>(scm_to_double(x)));
    cloned.set_y(static_cast<float>(scm_to_double(y)));
    assert(new_game);
    new_game->push_brick(std::move(cloned));
    return SCM_UNSPECIFIED;
  }

  static SCM push_prototype_border(SCM prototype_ref, SCM x, SCM y)
  {
    auto prototype = * reinterpret_cast<const entity*>(scm_to_pointer(prototype_ref));
    entity cloned(prototype);
    cloned.set_x(static_cast<float>(scm_to_double(x)));
    cloned.set_y(static_cast<float>(scm_to_double(y)));
    assert(new_game);
    new_game->push_bound(std::move(cloned));
    return SCM_UNSPECIFIED;
  }

  static SCM push_entity(SCM x, SCM y, SCM width, SCM height, SCM sprite_path, SCM collision_callback)
  {
    entity new_entity(
		      rect_shape{static_cast<float>(scm_to_double(x)),
			  static_cast<float>(scm_to_double(y)),
			  static_cast<float>(scm_to_double(width)),
			  static_cast<float>(scm_to_double(height))},
		      scm_to_locale_string(sprite_path));
    new_entity.register_ball_callback(collision_callback);
    new_game->push_brick(std::move(new_entity));
    return SCM_UNSPECIFIED;
  }

  static SCM push_entity_bound(SCM x, SCM y, SCM width, SCM height, SCM sprite_path, SCM collision_callback)
  {
    entity new_entity(
		      rect_shape{static_cast<float>(scm_to_double(x)),
			  static_cast<float>(scm_to_double(y)),
			  static_cast<float>(scm_to_double(width)),
			  static_cast<float>(scm_to_double(height))},
		      scm_to_locale_string(sprite_path));
    new_entity.register_ball_callback(collision_callback);
    new_game->push_bound(std::move(new_entity));
    return SCM_UNSPECIFIED;
  }

  static SCM register_entity_ball_callback(SCM entity_ref, SCM cb)
  {
    auto new_entity = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    new_entity->register_ball_callback(cb);
    return SCM_UNSPECIFIED;
  }
  
  static SCM set_entity_x(SCM entity_ref, SCM x)
  {
    auto new_entity = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    new_entity->set_x(static_cast<float>(scm_to_double(x)));
    return SCM_UNSPECIFIED;
  }

  static SCM set_entity_y(SCM entity_ref, SCM y)
  {
    auto new_entity = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    new_entity->set_y(static_cast<float>(scm_to_double(y)));
    return SCM_UNSPECIFIED;
  }

  static SCM set_moving_entity_vel_x(SCM moving_entity_ref, SCM new_v)
  {
    auto e = reinterpret_cast<moving_entity*>(scm_to_pointer(moving_entity_ref));
    e->set_x_velocity(static_cast<float>(scm_to_double(new_v)));
    return SCM_UNSPECIFIED;
  }

  static SCM set_moving_entity_vel_y(SCM moving_entity_ref, SCM new_v)
  {
    auto e = reinterpret_cast<moving_entity*>(scm_to_pointer(moving_entity_ref));
    e->set_y_velocity(static_cast<float>(scm_to_double(new_v)));
    return SCM_UNSPECIFIED;
  }

  static SCM set_entity_sprite(SCM entity_ref, SCM path)
  {
    auto e = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    e->set_texture(scm_to_locale_string(path));
    return SCM_UNSPECIFIED;
  }

  static SCM scm_render()
  {
    new_game->render_all();
    return SCM_UNSPECIFIED;
  }

  static SCM set_entity_width(SCM entity_ref, SCM w)
  {
    auto new_entity = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    new_entity->collider().w=(static_cast<float>(scm_to_double(w)));
    return SCM_UNSPECIFIED;
  }

  static SCM set_entity_height(SCM entity_ref, SCM h)
  {
    auto new_entity = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    new_entity->collider().h=(static_cast<float>(scm_to_double(h)));
    return SCM_UNSPECIFIED;
  }

  static SCM run_game()
  {
    new_game->run();
    return SCM_UNSPECIFIED;
  }

  static SCM get_entity_x(SCM entity_ref)
  {
    auto e = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    return scm_from_double(e->collider().x);
  }

  static SCM get_entity_y(SCM entity_ref)
  {
    auto e = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    return scm_from_double(e->collider().y);
  }

  static SCM get_entity_w(SCM entity_ref)
  {
    auto e = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    return scm_from_double(e->collider().w);
  }

  static SCM get_entity_h(SCM entity_ref)
  {
    auto e = reinterpret_cast<entity*>(scm_to_pointer(entity_ref));
    return scm_from_double(e->collider().h);
  }

  static SCM get_moving_entity_v_x(SCM entity_ref)
  {
    auto e = reinterpret_cast<moving_entity*>(scm_to_pointer(entity_ref));
    return scm_from_double(e->x_velocity());
  }

  static SCM get_moving_entity_v_y(SCM entity_ref)
  {
    auto e = reinterpret_cast<moving_entity*>(scm_to_pointer(entity_ref));
    return scm_from_double(e->y_velocity());
  }

  static SCM die_scm()
  {
    new_game->die();
    return SCM_UNSPECIFIED;
  }
    

  
  
  static void *inner_main(void *data)
  {
    //Register functions
    scm_c_define_gsubr("register-ball-callback", 2, 0, 0, reinterpret_cast<void*>(register_entity_ball_callback));
    scm_c_define_gsubr("remove-brick!", 1, 0, 0, reinterpret_cast<void*>(remove_brick_scm));
    scm_c_define_gsubr("get-entity-x", 1, 0, 0, reinterpret_cast<void*>(get_entity_x));
    scm_c_define_gsubr("get-entity-y", 1, 0, 0, reinterpret_cast<void*>(get_entity_y));
    scm_c_define_gsubr("get-entity-w", 1, 0, 0, reinterpret_cast<void*>(get_entity_w));
    scm_c_define_gsubr("get-entity-h", 1, 0, 0, reinterpret_cast<void*>(get_entity_h));
    scm_c_define_gsubr("get-moving-entity-vel-x", 1, 0, 0, reinterpret_cast<void*>(get_moving_entity_v_x));
    scm_c_define_gsubr("get-moving-entity-vel-y", 1, 0, 0, reinterpret_cast<void*>(get_moving_entity_v_y));
    scm_c_define_gsubr("register-left-callback!", 1, 0, 0, reinterpret_cast<void*>(register_left));
    scm_c_define_gsubr("register-right-callback!", 1, 0, 0, reinterpret_cast<void*>(register_right));
    scm_c_define_gsubr("set-entity-w!", 2, 0, 0, reinterpret_cast<void*>(set_entity_width));
    scm_c_define_gsubr("set-entity-h!", 2, 0, 0, reinterpret_cast<void*>(set_entity_height));
    scm_c_define_gsubr("construct-game", 6, 0, 0, reinterpret_cast<void*>(construct_game));
    scm_c_define_gsubr("create-prototype", 4, 0, 0, reinterpret_cast<void*>(create_prototype));
    scm_c_define_gsubr("push-prototype", 3, 0, 0, reinterpret_cast<void*>(push_prototype));
    scm_c_define_gsubr("push-prototype_border", 3, 0, 0, reinterpret_cast<void*>(push_prototype_border));
    
    scm_c_define_gsubr("push-entity", 6, 0, 0, reinterpret_cast<void*>(push_entity));
    scm_c_define_gsubr("push-entity-bound", 6, 0, 0, reinterpret_cast<void*>(push_entity_bound));

    scm_c_define_gsubr("set-entity-x!", 2, 0, 0, reinterpret_cast<void*>(set_entity_x));
    scm_c_define_gsubr("set-entity-y!", 2, 0, 0, reinterpret_cast<void*>(set_entity_y));

    scm_c_define_gsubr("render-all", 0, 0, 0, reinterpret_cast<void*>(scm_render));
    
    scm_c_define_gsubr("set-entity-x!", 2, 0, 0, reinterpret_cast<void*>(set_entity_x));
    scm_c_define_gsubr("set-entity-y!", 2, 0, 0, reinterpret_cast<void*>(set_entity_y));

    scm_c_define_gsubr("set-moving-entity-vel-x!", 2, 0, 0, reinterpret_cast<void*>(set_moving_entity_vel_x));
    scm_c_define_gsubr("set-moving-entity-vel-y!", 2, 0, 0, reinterpret_cast<void*>(set_moving_entity_vel_y));

    scm_c_define_gsubr("set-entity-sprite!", 2, 0, 0, reinterpret_cast<void*>(set_entity_sprite));

    scm_c_define_gsubr("register-space-callback!", 1, 0, 0, reinterpret_cast<void*>(register_space));

    scm_c_define_gsubr("run-game", 0, 0, 0, reinterpret_cast<void*>(run_game));

    scm_c_define_gsubr("die-game", 0, 0, 0, reinterpret_cast<void*>(die_scm));

    
    auto game_conf_path = *reinterpret_cast<const std::string*>(data);//Will be used later
    scm_c_primitive_load("config.scm");//Default config
    scm_c_primitive_load(game_conf_path.c_str());
    return nullptr;
  }


  
  
  void initialize_game(const std::string &scm_path)
  {
    scm_with_guile(inner_main, reinterpret_cast<void*>(&const_cast<std::string&>(scm_path)));
  }

  void game::game_over()
  {
    life_is_too_much=true;
    std::cout<<"You lose!"<<std::endl;
  }

}
