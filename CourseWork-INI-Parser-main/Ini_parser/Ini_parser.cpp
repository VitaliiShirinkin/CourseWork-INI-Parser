// Курсовой проект «Парсер INI-файлов»


#include <string>
#include <unordered_map>
#include <list>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream> 
#include <set> 
#include <string>


//функция удаления начальных пробелов
std::string& deleteSpaceBefore(std::string& str) {
    size_t startPosition = str.find_first_not_of(" \t\r\n\v\f");//функция std::string

    if (std::string::npos != startPosition) {
        str = str.substr(startPosition);
    }
    return str;
}

//функция удаления конечных пробелов
std::string& deleteSpaceAfter(std::string& str) {
    size_t endPosition = str.find_last_not_of(" \t\r\n\v\f");//функция std::string

    if (std::string::npos != endPosition) {
        str = str.substr(0, endPosition + 1);
    }
    return str;
}

//функция пропуска комментариев
std::string& commentIgnore(std::string& str) {
    size_t commentPosition = str.find(";");

    if (std::string::npos != commentPosition) {
        str = str.substr(0, commentPosition);
    }
    return str;
}

//функция добавления списка переменных секции в строку (для исключения)
std::string firstMapToString(std::unordered_map<std::string, std::string> const& mp) {
    std::ostringstream oss;
    for (auto const& map : mp) {
        oss << map.first << " ";
    }
    return oss.str();
}

struct Section {
    std::string name;
    std::unordered_map< std::string, std::string > variableValue;
};
//Класс для инкапсуляции текстового файла конфигурации в форме ini-файла
class IniParser {
public:
    std::string name;
    std::unordered_map< std::string, std::string > variableValue;

    IniParser(const std::string& fileName) {
        parse(fileName);
    }
    ~IniParser()
    {

    }

    template <class T>
    const T getValue(const std::string& sectionName, const std::string& variableName) {
        auto value = keyValue(sectionName, variableName);
        return value;
    }
    template <>
    const int getValue(const std::string& sectionName, const std::string& variableName) {
        int value = std::stoi(keyValue(sectionName, variableName));
        return value;
    }
    template <>
    const double getValue(const std::string& sectionName, const std::string& variableName) {
        double value = std::stod(keyValue(sectionName, variableName));
        return value;
    }

private:
    //функция доступа к секции по названию
    Section* getSection(const std::string& sectionName) {
        std::list<Section>::iterator search = std::find_if
        (
            listOfSections.begin(), listOfSections.end(), [sectionName](const Section& sect) {
                return sect.name.compare(sectionName) == 0;
            }
        );
        return search != listOfSections.end() ? &*search : nullptr;
    }

    //функция доступа к списку секций
    const std::list<Section>& getListOfSections() {
        return listOfSections;
    }

    //функция обработки повторяющейся секции
    void sectionAddition(const std::string sectionName, std::string variableName, std::string variableValue) {
        Section* section = getSection(sectionName);
        section->variableValue[variableName] = variableValue;
    }

    //функция обработки ini-файла (анализируем поток в структуру данных конфигурации)
    void parse(const std::string& fileName) {
        //верхний уровень структуры данных — хеш - таблица
        Section currentSection;
        std::ifstream fin;
        fin.open(fileName);
        bool key = false;
        if (!fin)
            throw std::invalid_argument(fileName + " could not be opened");

        int lineCount = 0;

        for (std::string line; std::getline(fin, line);) {
            //пропуск комментариев
            lineCount++;
            deleteSpaceBefore(line);//удаляем пробел
            commentIgnore(line);//игнорируем комментарий

            if (line[0] == '[') {
                /* "[section]" строка */
                size_t end = line.find_first_of(']');//находим обе скобки
                if (end != std::string::npos) {
                    if (!currentSection.name.empty()) {
                        listOfSections.push_back(currentSection);//если есть имя секции, то выписываем
                        currentSection.name.clear();
                        currentSection.variableValue.clear();//очищаем буфер
                    }
                    //это новый раздел, поэтому добавим его в хеш-таблицу
                    currentSection.name = line.substr(1, end - 1);
                    if (namesOfSections.count(currentSection.name)) {
                        key = true;
                        std::string notUniqueSection = currentSection.name;
                    }
                    else namesOfSections.insert(currentSection.name);
                }
                //а если section не имеет закрытия символа ] — игнорировать и вывести предупреждение
                else {
                    throw std::invalid_argument("Incorrect syntax in line " + std::to_string(lineCount));
                }      
            }
            //считываем ключ=значение
            if (!line.empty()) {
                if (!key == true) {
                    //должно быть пара имя[=:]значение
                    size_t end = line.find_first_of("=");
                    if (end != std::string::npos) {
                        std::string name = line.substr(0, end);
                        std::string value = line.substr(end + 1);
                        deleteSpaceBefore(deleteSpaceAfter(name));
                        deleteSpaceBefore(deleteSpaceAfter(value));
                        size_t valuePos = line.find_first_of("=");
                        if (value != "")
                            currentSection.variableValue[name] = value;
                        else { currentSection.variableValue[name] = " "; }
                    }
                    //если нет разделителя между ключ-значение — игнорировать
                }
                else {
                    size_t end = line.find_first_of("=");
                    if (end != std::string::npos) {
                        std::string name = line.substr(0, end);
                        std::string value = line.substr(end + 1);
                        getSection(currentSection.name);
                        sectionAddition(currentSection.name, name, value);//очищаем буфер
                    }
                }
            }
        }
        //если мы вышли из цикла, мы добавляем последний раздел
        //это новый раздел, поэтому, если у нас есть текущий раздел, добавим его в список
        if (!currentSection.name.empty()) {
            listOfSections.push_back(currentSection);
            currentSection.name = "";
            currentSection.variableValue.clear();
        }
    }

    //функция получения значения ключа с обработкой ошибок
    const std::string keyValue(const std::string& sectionName, const std::string& variableName) {
        Section* section = getSection(sectionName);
        if (section != nullptr) {
            const auto it = section->variableValue.find(variableName);
            if (it != section->variableValue.end()) {
                if (it->second != " ")
                    return it->second;
                else
                    throw std::invalid_argument("The variable " + variableName + " in " + sectionName
                        + " has no value");
            }
            else {
                throw std::invalid_argument("The " + sectionName + " has variables: "
                    + firstMapToString(section->variableValue));
            }
        }
        throw std::invalid_argument("The section does not exist");
    }

    std::list<Section> listOfSections;
    std::set<std::string> namesOfSections;
};



int main() {

    std::setlocale(LC_ALL, "ru_RU.UTF-8");

    try {
        IniParser parser("fin.ini");
        std::string section = "Section2";
        std::string variable = "var2";

        auto value = parser.getValue<std::string>(section, variable);

        std::cout << "In " << section << ": " << variable << " = " << value << '\n';

    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
    return 0;
}



