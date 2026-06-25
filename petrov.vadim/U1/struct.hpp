#ifndef STRUCT_HPP
#define STRUCT_HPP

#include <cstddef>
#include <string>
namespace petrov
{
  struct Person
  {
    size_t id;
    std::string info;
  };

  struct Data
  {
    Person* data_;
    size_t saved_;
    size_t ignored_;
  };
}
#endif
