#ifndef ALS_TEXTURE_HPP
#define ALS_TEXTURE_HPP
#include<SFML/Graphics/Texture.hpp>
namespace als
{
  class texture
  {
  public:
    texture()=default;
    texture(const std::string &path);
    const sf::Texture &get_texture()const;
    void reload_from_file(const std::string &str);
  protected:
    sf::Texture texture_data_;
  };
}
#endif
