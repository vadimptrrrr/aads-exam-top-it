#include "meet.hpp"
#include <iostream>
#include <fstream>
#include <exception>

void petrov::cleanupMeets(petrov::MeetData& meets)
{
  if (meets.data_ != nullptr)
  {
    for (size_t i = 0; i < meets.saved_; ++i)
    {
      delete meets.data_[i].first;
      delete meets.data_[i].second;
    }
    delete[] meets.data_;
    meets.data_ = nullptr;
  }
}

bool petrov::loadMeets(std::istream& in, MeetData& meets)
{
  size_t id1 = 0;
  size_t id2 = 0;
  size_t time = 0;
  while (in >> id1 >> id2 >> time)
  {
    if (id1 == id2)
    {
      continue;
    }
    if (meets.saved_ % 8 == 0)
    {
      petrov::Meet* tmp = new petrov::Meet[meets.saved_ + 8];
      for (size_t i = 0; i < meets.saved_; ++i)
      {
        tmp[i] = meets.data_[i];
      }
      delete[] meets.data_;
      meets.data_ = tmp;
    }
    meets.data_[meets.saved_].first = new petrov::Person();
    meets.data_[meets.saved_].first->id = id1;
    meets.data_[meets.saved_].second = new petrov::Person();
    meets.data_[meets.saved_].second->id = id2;
    meets.data_[meets.saved_].time = time;
    meets.saved_++;
  }
  return !(in.peek() != std::char_traits< char >::eof() && in.fail());
}


bool hasPerson(size_t id, const petrov::Data& persons)
{
  for (size_t i = 0; i < persons.saved_; ++i)
  {
    if (persons.data_[i].id == id)
    {
      return true;
    }
  }
  return false;
}

bool hasAny(size_t id, const petrov::Data& persons, const petrov::MeetData& meets)
{
  if (hasPerson(id, persons))
  {
    return true;
  }
  for (size_t i = 0; i < meets.saved_; ++i)
  {
    if (meets.data_[i].first->id == id || meets.data_[i].second->id == id)
    {
      return true;
    }
  }
  return false;
}

void petrov::executeCommands(std::istream& in, std::ostream& out, petrov::Data& persons, MeetData& meets)
{
  std::string cmd;
  while (in >> cmd)
  {
    if (cmd == "anons")
    {
      size_t* arr = new size_t[meets.saved_ * 2];
      size_t cnt = 0;
      for (size_t i = 0; i < meets.saved_; ++i)
      {
        if (!hasPerson(meets.data_[i].first->id, persons))
        {
          bool dup = false;
          for (size_t j = 0; j < cnt; ++j)
          {
            if (arr[j] == meets.data_[i].first->id)
            {
              dup = true;
            }
          }
          if (!dup)
          {
            arr[cnt] = meets.data_[i].first->id;
            cnt++;
          }
        }
        if (!hasPerson(meets.data_[i].second->id, persons))
        {
          bool dup = false;
          for (size_t j = 0; j < cnt; ++j)
          {
            if (arr[j] == meets.data_[i].second->id)
            {
              dup = true;
            }
          }
          if (!dup)
          {
            arr[cnt] = meets.data_[i].second->id;
            cnt++;
          }
        }
      }
      for (size_t i = 0; i < cnt; ++i)
      {
        for (size_t j = i + 1; j < cnt; ++j)
        {
          if (arr[i] > arr[j])
          {
            size_t t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
          }
        }
      }
      for (size_t i = 0; i < cnt; ++i)
      {
        out << arr[i] << '\n';
      }
      delete[] arr;
    }
    else if (cmd == "deanon")
    {
      size_t aid = 0;
      size_t id = 0;
      if (!(in >> aid >> id))
      {
        throw std::runtime_error("cmd invalid");
      }
      if (hasPerson(aid, persons) || !hasPerson(id, persons))
      {
        throw std::runtime_error("cmd invalid");
      }
      for (size_t i = 0; i < meets.saved_; ++i)
      {
        if (meets.data_[i].first->id == aid)
        {
          meets.data_[i].first->id = id;
        }
        if (meets.data_[i].second->id == aid)
        {
          meets.data_[i].second->id = id;
        }
      }
      size_t w = 0;
      for (size_t i = 0; i < meets.saved_; ++i)
      {
        if (meets.data_[i].first->id != meets.data_[i].second->id)
        {
          meets.data_[w] = meets.data_[i];
          w++;
        }
        else
        {
          delete meets.data_[i].first;
          delete meets.data_[i].second;
        }
      }
      meets.saved_ = w;
    }
    else if (cmd == "desc")
    {
      size_t id = 0;
      if (!(in >> id) || !hasAny(id, persons, meets))
      {
        throw std::runtime_error("cmd invalid");
      }
      if (!hasPerson(id, persons))
      {
        out << "<ANON>\n";
      }
      else
      {
        for (size_t i = 0; i < persons.saved_; ++i)
        {
          if (persons.data_[i].id == id)
          {
            out << persons.data_[i].info << '\n';
          }
        }
      }
    }
    else if (cmd == "redesc")
    {
      size_t id = 0;
      if (!(in >> id))
      {
        throw std::runtime_error("cmd invalid");
      }
      char c = '\0';
      while (in >> std::noskipws >> c && (c == ' ' || c == '\t'))
      {
        continue;
      }
      if (c != '"')
      {
        throw std::runtime_error("cmd invalid");
      }
      std::string d = "";
      while (in >> std::noskipws >> c && c != '"' && c != '\n')
      {
        d += c;
      }
      if (c != '"')
      {
        throw std::runtime_error("cmd invalid");
      }

      bool found = false;
      for (size_t i = 0; i < persons.saved_; ++i)
      {
        if (persons.data_[i].id == id)
        {
          persons.data_[i].info = d;
          found = true;
          break;
        }
      }
      if (!found)
      {
        if (persons.saved_ % 8 == 0)
        {
          Person* tmp = new Person[persons.saved_ + 8];
          for (size_t i = 0; i < persons.saved_; ++i)
          {
            tmp[i] = persons.data_[i];
          }
          delete[] persons.data_;
          persons.data_ = tmp;
        }
        persons.data_[persons.saved_].id = id;
        persons.data_[persons.saved_].info = d;
        persons.saved_++;
      }
    }
    else if (cmd == "meets" || cmd == "less" || cmd == "greater")
    {
      size_t filter = 0;
      size_t id = 0;
      if (cmd == "less" || cmd == "greater")
      {
        if (!(in >> filter))
        {
          throw std::runtime_error("cmd invalid");
        }
      }
      if (!(in >> id) || !hasAny(id, persons, meets))
      {
        throw std::runtime_error("cmd invalid");
      }

      Meet* arr = new Meet[meets.saved_];
      size_t cnt = 0;
      for (size_t i = 0; i < meets.saved_; ++i)
      {
        if (meets.data_[i].first->id == id || meets.data_[i].second->id == id)
        {
          if (cmd == "less" && meets.data_[i].time >= filter)
          {
            continue;
          }
          if (cmd == "greater" && meets.data_[i].time <= filter)
          {
            continue;
          }
          arr[cnt].first = new Person;
          arr[cnt].first->id = (meets.data_[i].first->id == id) ? meets.data_[i].second->id : meets.data_[i].first->id;
          arr[cnt].time = meets.data_[i].time;
          cnt++;
        }
      }
      for (size_t i = 0; i < cnt; ++i)
      {
        for (size_t j = i + 1; j < cnt; ++j)
        {
          if (arr[i].first->id > arr[j].first->id || (arr[i].first->id == arr[j].first->id && arr[i].time > arr[j].time))
          {
            Meet t = arr[i];
            arr[i] = arr[j];
            arr[j] = t;
          }
        }
      }
      for (size_t i = 0; i < cnt; ++i)
      {
        out << arr[i].first->id << ' ' << arr[i].time << '\n';
        delete arr[i].first;
      }
      delete[] arr;
    }
    else if (cmd == "commons")
    {
      size_t id1 = 0;
      size_t id2 = 0;
      if (!(in >> id1 >> id2) || !hasAny(id1, persons, meets) || !hasAny(id2, persons, meets))
      {
        throw std::runtime_error("cmd invalid");
      }

      size_t* a1 = new size_t[meets.saved_];
      size_t c1 = 0;
      size_t* a2 = new size_t[meets.saved_];
      size_t c2 = 0;
      for (size_t i = 0; i < meets.saved_; ++i)
      {
        if (meets.data_[i].first->id == id1)
        {
          a1[c1] = meets.data_[i].second->id;
          c1++;
        }
        if (meets.data_[i].second->id == id1)
        {
          a1[c1] = meets.data_[i].first->id;
          c1++;
        }
        if (meets.data_[i].first->id == id2)
        {
          a2[c2] = meets.data_[i].second->id;
          c2++;
        }
        if (meets.data_[i].second->id == id2)
        {
          a2[c2] = meets.data_[i].first->id;
          c2++;
        }
      }
      size_t* res = new size_t[meets.saved_];
      size_t cr = 0;
      for (size_t i = 0; i < c1; ++i)
      {
        for (size_t j = 0; j < c2; ++j)
        {
          if (a1[i] == a2[j])
          {
            bool dup = false;
            for (size_t k = 0; k < cr; ++k)
            {
              if (res[k] == a1[i])
              {
                dup = true;
              }
            }
            if (!dup)
            {
              res[cr] = a1[i];
              cr++;
            }
            break;
          }
        }
      }
      for (size_t i = 0; i < cr; ++i)
      {
        for (size_t j = i + 1; j < cr; ++j)
        {
          if (res[i] > res[j])
          {
            size_t t = res[i];
            res[i] = res[j];
            res[j] = t;
          }
        }
      }
      for (size_t i = 0; i < cr; ++i)
      {
        out << res[i] << '\n';
      }
      delete[] a1;
      delete[] a2;
      delete[] res;
    }
    else if (cmd == "out-persons")
    {
      std::string fname;
      if (!(in >> fname))
      {
        throw std::runtime_error("cmd invalid");
      }
      std::ofstream fout(fname);
      if (!fout.is_open())
      {
        throw std::runtime_error("cmd invalid");
      }
      for (size_t i = 0; i < persons.saved_; ++i)
      {
        fout << persons.data_[i].id << ' ' << persons.data_[i].info << '\n';
      }
    }
    else
    {
      throw std::runtime_error("cmd invalid");
    }
  }

  for (size_t i = 0; i < meets.saved_; ++i)
  {
    delete meets.data_[i].first;
    delete meets.data_[i].second;
  }
}
