#include "functions.hpp"
#include <iostream>
#include <ios>
#include "struct.hpp"

bool petrov::parsePerson(std::istream& in, petrov::Person& person)
{
  person.id = 0;
  person.info.clear();

  char c = '\0';
  bool hasId = false;
  bool readingId = true;
  bool hasInfo = false;

  while (in >> std::noskipws >> c && c != '\n')
  {
    if (c == '\r')
    {
      continue;
    }

    if (readingId)
    {
      if (c == ' ' || c == '\t')
      {
        if (hasId)
        {
          readingId = false;
        }
      }
      else if (c >= '0' && c <= '9')
      {
        hasId = true;
        person.id = person.id * 10 + static_cast< size_t >(c - '0');
      }
      else
      {
        while (in >> std::noskipws >> c && c != '\n')
        {
          continue;
        }
        return false;
      }
    }
    else
    {
      if (!hasInfo && (c == ' ' || c == '\t'))
      {
        continue;
      }
      hasInfo = true;
      person.info += c;
    }
  }
  return hasId && hasInfo;
}

void petrov::processStream(std::istream& in, Data& data)
{
  Person current;
  char dummy = '\0';
  while (in)
  {
    int p = in.peek();
    if (p == std::char_traits< char >::eof())
    {
      break;
    }
    if (p == '\n' || p == '\r')
    {
      in >> std::noskipws >> dummy;
      continue;
    }

    if (parsePerson(in, current))
    {
      bool duplicate = false;
      for (size_t i = 0; i < data.saved_; ++i)
      {
        if (data.data_[i].id == current.id)
        {
          duplicate = true;
          break;
        }
      }

      if (duplicate)
      {
        data.ignored_++;
      }
      else
      {
        if (data.saved_ % 8 == 0)
        {
          Person* newData = new Person[data.saved_ + 8];
          for (size_t i = 0; i < data.saved_; ++i)
          {
            newData[i] = data.data_[i];
          }
          delete[] data.data_;
          data.data_ = newData;
        }
        data.data_[data.saved_] = current;
        data.saved_++;
      }
    }
    else
    {
      data.ignored_++;
    }
  }
}

void petrov::printData(std::ostream& out, Data& data)
{
  if (data.saved_ == 0)
  {
    out << '\n';
  }
  for (size_t i = 0; i < data.saved_; ++i)
  {
    out << data.data_[i].id << ' ' << data.data_[i].info << '\n';
  }
  std::cerr << data.saved_ << ' ' << data.ignored_ << '\n';
}
