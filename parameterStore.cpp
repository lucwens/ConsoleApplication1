#include "ParameterStore.h" // Include the header file for the class declaration
#include <iostream>
#include <fstream>
#include <stdexcept> // For std::runtime_error or other exceptions if needed

// Default constructor implementation
ParameterStore::ParameterStore() = default;

// Constructor from command-line arguments implementation
ParameterStore::ParameterStore(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::string inputString = argv[1];
        if (deserialize(inputString))
        {
            initializedSuccessfullyFromJson_ = true;
        }
        else
        {
            std::cerr << "ParameterStore: Command-line input is not a valid JSON string." << std::endl;
        }
    }
    else
    {
        std::cout << "ParameterStore: No command-line argument provided. Initializing empty store." << std::endl;
    }
}

// isInitializedFromJson implementation
bool ParameterStore::isInitializedFromJson() const
{
    return initializedSuccessfullyFromJson_;
}

// set overloads implementation
void ParameterStore::set(const std::string &key, int value)
{
    parameters[key] = value;
}

void ParameterStore::set(const std::string &key, double value)
{
    parameters[key] = value;
}

void ParameterStore::set(const std::string &key, bool value)
{
    parameters[key] = value;
}

void ParameterStore::set(const std::string &key, const std::string &value)
{
    parameters[key] = value;
}

// get overloads implementation
int ParameterStore::getInt(const std::string &key, int defaultValue) const
{
    if (parameters.count(key) && parameters[key].is_number_integer())
    {
        return parameters[key].get<int>();
    }
    return defaultValue;
}

double ParameterStore::getDouble(const std::string &key, double defaultValue) const
{
    if (parameters.count(key) && parameters[key].is_number())
    {
        return parameters[key].get<double>();
    }
    return defaultValue;
}

bool ParameterStore::getBool(const std::string &key, bool defaultValue) const
{
    if (parameters.count(key) && parameters[key].is_boolean())
    {
        return parameters[key].get<bool>();
    }
    return defaultValue;
}

std::string ParameterStore::getString(const std::string &key, const std::string &defaultValue) const
{
    if (parameters.count(key) && parameters[key].is_string())
    {
        return parameters[key].get<std::string>();
    }
    return defaultValue;
}

// contains implementation
bool ParameterStore::contains(const std::string &key) const
{
    return parameters.count(key) > 0;
}

// remove implementation
void ParameterStore::remove(const std::string &key)
{
    parameters.erase(key);
}

// clear implementation
void ParameterStore::clear()
{
    parameters.clear();
}

// serialize implementation
std::string ParameterStore::serialize(int indent) const
{
    return parameters.dump(indent);
}

// deserialize implementation
bool ParameterStore::deserialize(const std::string &jsonString)
{
    try
    {
        parameters = json::parse(jsonString);
        return true;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "Error deserializing JSON string: " << e.what() << std::endl;
        return false;
    }
}

// saveToFile implementation
bool ParameterStore::saveToFile(const std::string &filename, int indent) const
{
    std::ofstream ofs(filename);
    if (!ofs.is_open())
    {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    ofs << parameters.dump(indent);
    ofs.close();
    return true;
}

// loadFromFile implementation
bool ParameterStore::loadFromFile(const std::string &filename)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open())
    {
        std::cerr << "Error: Could not open file for reading: " << filename << std::endl;
        return false;
    }
    try
    {
        ifs >> parameters;
        return true;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "Error parsing JSON from file '" << filename << "': " << e.what() << std::endl;
        return false;
    }
}

// getJsonObject implementations
json &ParameterStore::getJsonObject()
{
    return parameters;
}

const json &ParameterStore::getJsonObject() const
{
    return parameters;
}
