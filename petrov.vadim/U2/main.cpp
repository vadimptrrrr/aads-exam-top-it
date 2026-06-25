#include "struct.hpp"
#include "functions.hpp"
#include "meet.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

int main(int argc, char const *argv[])
{
  if (argc < 2 || argc > 3)
  {
    return 1;
  }

  std::string inFile = "";
  std::string dataFile = "";
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg.length() >= 3 && arg[0] == 'i' && arg[1] == 'n' && arg[2] == ':')
    {
      if (!inFile.empty())
      {
        return 1;
      }
      inFile = argv[i] + 3;
    }
    else if (arg.length() >= 4 && arg[0] == 'd' && arg[1] == 'a' && arg[2] == 't' && arg[3] == 'a' && arg[4] == ':')
    {
      if (!dataFile.empty())
      {
        return 1;
      }
      dataFile = argv[i] + 5;
    }
    else return 1;
  }

  if (dataFile.empty())
  {
    return 1;
  }

  petrov::Data persons{nullptr, 0, 0};
  petrov::MeetData meets{nullptr, 0};
  if (!inFile.empty())
  {
    std::ifstream fin(inFile);
    if (!fin.is_open()) return 2;
    petrov::processStream(fin, persons);
  }

  std::ifstream fdata(dataFile);
  if (!fdata.is_open())
  {
    delete[] persons.data_;
    return 2;
  }

  if (!petrov::loadMeets(fdata, meets))
  {
    delete[] persons.data_;
    delete[] meets.data_;
    return 3;
  }

  try
  {
    petrov::executeCommands(std::cin, std::cout, persons, meets);
  }
  catch (const std::exception& e)
  {
    std::cout << "<INVALID COMMAND>\n";
    std::cin.clear();
    char c = '\0';
    while (std::cin >> std::noskipws >> c && c != '\n');
  }
  petrov::cleanupMeets(meets);
  delete[] persons.data_;
}
