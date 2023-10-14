#include "parser.h"


iniParser::iniParser(const std::string& filename) : filename_{ filename }
{
    std::ifstream file(filename_);

    if (!file.is_open())
    {
        throw std::runtime_error("File opening error: " + filename_);
    }

    while (std::getline(file, line_))
    {
        // Пропускаем комментарии и пустые строки
        if (line_.empty() || line_[0] == ';')
        {
            ++lineNumber_;
            continue;
        }

        // Находим новый раздел
        if ((line_[0] == '[') && (line_[line_.length() - 1] == ']'))
        {
            ++lineNumber_;
            // Найден новый раздел
            currentSection_ = line_.substr(1, line_.length() - 2);
        }
        else
        {
            ++lineNumber_;
            // Найдена строка с ключом и значением
            std::istringstream iss(line_);

            if (std::getline(iss, key_, '='))
            {
                if (std::getline(iss, value_))
                {
                    size_t pos = value_.find(';');  // находим позицию первого вхождения знака ";"

                    if (pos != std::string::npos) // если знак найден
                    {
                        value_ = value_.substr(0, pos);  // обрезаем строку, оставляя только часть до знака ";"
                    }
                    iniData_[currentSection_][key_] = value_;
                }
            }
        }
    }
    file.close();
}

std::string iniParser::getStringValue(const std::string currentSection, const std::string key, const int lineNumber)
{
    std::map<std::string, std::map<std::string, std::string>>::iterator it1 = iniData_.find(currentSection);
    if (it1 == iniData_.end())
    {
        printMap(currentSection, key);
        throw std::runtime_error(currentSection + " not found");
    }
    else
    {
        std::map<std::string, std::string>& innerMap = iniData_[currentSection];
        if (!innerMap.count(key))
        {
            printMap(currentSection, key);
            throw std::runtime_error("");
        }
    }
    return iniData_[currentSection][key];
}

void iniParser::printMap(const std::string currentSection, const std::string key)
{
    std::cout << "There are no key: " + key + ", in the section " + currentSection << std::endl;
    std::cout << "There are has: " << std::endl;

    const std::map<std::string, std::string>& innerMap = iniData_[currentSection];
    for (auto innerIt = innerMap.begin(); innerIt != innerMap.end(); ++innerIt)
    {
        const std::string& innerKey = innerIt->first;
        std::cout << innerKey << std::endl;
    }
}