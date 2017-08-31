#include "renderer.hpp"
#include "texture.hpp"
#include "rect_shape.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
namespace als
{
  renderer::renderer(sf::RenderWindow *wref):window_ref_(wref)
  {
  }

  void renderer::render(const texture &tex, const rect_shape &shape)const
  {
    sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(shape.w,shape.h));
    rect.setTexture(&tex.get_texture());
    rect.setPosition(shape.x, shape.y);
    window_ref_->draw(rect);
  }
  void renderer::clear()const
  {
    window_ref_->clear();
  }
}
