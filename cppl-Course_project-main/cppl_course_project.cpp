// Курсовой проект «Парсер INI-файлов»

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <iomanip>
#include "parser.h"

int main()
{
    std::setlocale(LC_ALL, "ru_RU.UTF-8");

    try 
    {
        iniParser parser("fin.ini");
        std::string section = "Section1";
        std::string variable = "var2";

        auto value = parser.getValue<std::string>(section, variable);

        std::cout << "In " << section << ": " << variable << " = " << value << '\n';

    }

    catch (const std::out_of_range& ex)
    {
        std::cout << "The string cannot be converted to an ini number. " << ex.what() << std::endl;
    }
    catch (const std::invalid_argument& ex)
    {
        std::cout << "The string cannot be converted to an ini number. " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Something new" << std::endl;
    }

    return 0;
}