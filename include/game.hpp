#ifndef ALS_GAME_HPP
#define ALS_GAME_HPP
#include "entity.hpp"
#include "moving_entity.hpp"
#include "renderer.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <map>
namespace als
{
  static SCM construct_game(SCM screen_width, SCM screen_height, SCM background, SCM window_title,
			    SCM ball_sprite, SCM board_sprite);
  class game;
  void initialize_game(const std::string &scm_path);
  
  class game
  {
  public:
    friend SCM construct_game(SCM screen_width, SCM screen_height, SCM background, SCM window_title,
			      SCM ball_sprite, SCM board_sprite);
    game(unsigned int window_width, unsigned int window_height, const std::string &window_title,
	 const std::string &bkg_path);
    bool run();
    void push_brick(const entity &e);
    void push_brick(entity &&e);
    void push_bound(const entity &e);
    void push_bound(entity &&e);
    void render_all();
    void register_space_callback(SCM cb);
    void register_space_release_callback(SCM cb);
    void register_left_callback(SCM cb);
    void register_left_release_callback(SCM cb);
    void register_right_callback(SCM cb);
    void register_right_release_callback(SCM cb);
    void die();
    void remove_brick(entity *const addr);
    void win();
  protected:
    bool ball_board_sync = true;
    unsigned score = 0u;
    void game_over();
    unsigned life = 3u;
    bool life_is_too_much = false;
    void handle_input();
    sf::RenderWindow window;
    renderer rend;
    std::vector<entity>bricks; //These are the bricks you have to break
    std::vector<entity>bound_or_something_else;
    moving_entity ball;
    moving_entity board;
    std::chrono::milliseconds update_interval = std::chrono::milliseconds(15);
    entity background;
    SCM space_callback;
    SCM space_release_callback;
    SCM left_callback;
    SCM left_release_callback;
    SCM right_callback;
    SCM right_release_callback;
    int chances;
  };
}
#endif
