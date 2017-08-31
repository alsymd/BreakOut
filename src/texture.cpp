#include"texture.hpp"
namespace als
{
  texture::texture(const std::string &path)
  {
    texture_data_.loadFromFile(path);
  }

  const sf::Texture &texture::get_texture()const
  {
    return texture_data_;
  }

  void texture::reload_from_file(const std::string &path)
  {
    texture_data_.loadFromFile(path);
  }
}
