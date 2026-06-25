#ifndef MEET_HPP
#define MEET_HPP

#include <struct.hpp>
namespace petrov
{
  struct Meet
  {
    Person* first;
    Person* second;
    size_t time;
  };

  struct MeetData
  {
    Meet* data_;
    size_t saved_;
  };

  bool loadMeets(std::istream& in, MeetData& meets);
  void executeCommands(std::istream& in, std::ostream& out, petrov::Data& persons, MeetData& meets);
  void cleanupMeets(petrov::MeetData& meets);
}
#endif
