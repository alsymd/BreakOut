#ifndef ALS_RENDERER_HPP
#define ALS_RENDERER_HPP
#include <SFML/Graphics.hpp>
namespace als
{
  class texture;
  class rect_shape;
  class renderer
  {
  public:
    renderer()=default;
    renderer(sf::RenderWindow *wref);
    void render(const texture &tex, const rect_shape &shape)const;
    void clear()const;
  protected:
    sf::RenderWindow *window_ref_=nullptr;
  };
}
#endif
