#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <locale>

enum class iniWarning
{
    pointIsFound = 1, dataTypeFalse = -1
};

class iniParser
{
private:
    std::map<std::string, std::map<std::string, std::string>> iniData_;
    std::string filename_;
    std::string currentSection_;
    std::string line_;
    std::string key_;
    std::string value_;
    std::string tmpIniDataString_;
    int tmpIniDataInt_ = 0;
    double tmpIniDataDouble_ = 0.0;
    float tmpIniDataFloat_ = 0.0;
    int lineNumber_ = 0;

    std::string getStringValue(const std::string currentSection, const std::string key, const int lineNumber);
    void printMap(const std::string currentSection, const std::string key);

public:
    iniParser(const std::string& filename);

    //base
    template <class T>
    T getValue(const std::string& currentSection, const std::string key)
    {
        //проверка совпадения типов данных
        static_assert(sizeof(T) == static_cast<int> (iniWarning::dataTypeFalse), "Invalid data type");
    }

    //std::string
    template <>
    std::string getValue(const std::string& currentSection, const std::string key)
    {
        return getStringValue(currentSection, key, lineNumber_);
    }

    //int
    template <>
    int getValue(const std::string& currentSection, const std::string key)
    {
        tmpIniDataString_ = getStringValue(currentSection, key, lineNumber_);

        if (tmpIniDataString_.find(".") == static_cast<size_t>(iniWarning::pointIsFound))
        {
            std::cout << "Warning. Type of this value double or float converted to ini !!!" << std::endl;
        }

        try
        {
            tmpIniDataInt_ = std::stoi(tmpIniDataString_);
        }
        catch (const std::out_of_range& ex)
        {
            throw ex;
        }
        catch (const std::invalid_argument& ex)
        {
            throw ex;
        }
        return tmpIniDataInt_;
    }

    //double
    template <>
    double getValue(const std::string& currentSection, const std::string key)
    {
        try
        {
            tmpIniDataDouble_ = std::stod(getStringValue(currentSection, key, lineNumber_));
        }
        catch (const std::out_of_range& ex)
        {
            throw ex;
        }
        catch (const std::invalid_argument& ex)
        {
            throw ex;
        }
        return tmpIniDataDouble_;
    }

    //float
    template <>
    float getValue(const std::string& currentSection, const std::string key)
    {
        try
        {
            tmpIniDataFloat_ = std::stof(getStringValue(currentSection, key, lineNumber_));
        }
        catch (const std::out_of_range& ex)
        {
            throw ex;
        }
        catch (const std::invalid_argument& ex)
        {
            throw ex;
        }
        return tmpIniDataFloat_;
    }
};