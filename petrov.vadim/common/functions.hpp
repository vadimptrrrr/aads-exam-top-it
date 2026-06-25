#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <iostream>
#include "struct.hpp"
namespace petrov
{
  bool parsePerson(std::istream& in, Person& person);
  void processStream(std::istream& in, Data& data);
  void printData(std::ostream& out, Data& data);
}

#endif
