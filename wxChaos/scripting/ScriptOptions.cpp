#include "ScriptOptions.h"
#include <algorithm>

ScriptOption* ScriptOptions::Find(const std::string& name, const ScriptOptionType type)
{
    const auto option = std::find_if(_options.begin(), _options.end(), [&](const ScriptOption& value) {
        return value.name == name && value.type == type;
    });
    return option == _options.end() ? nullptr : &*option;
}

const ScriptOption* ScriptOptions::Find(const std::string& name, const ScriptOptionType type) const
{
    const auto option = std::find_if(_options.begin(), _options.end(), [&](const ScriptOption& value) {
        return value.name == name && value.type == type;
    });
    return option == _options.end() ? nullptr : &*option;
}

void ScriptOptions::AddInteger(const std::string& name, const std::string& label, const int defaultValue)
{
    if (ScriptOption* option = Find(name, ScriptOptionType::Integer))
    {
        option->label = label;
        option->integerValue = defaultValue;
        return;
    }
    _options.push_back({name, label, ScriptOptionType::Integer, defaultValue});
}

void ScriptOptions::AddDouble(const std::string& name, const std::string& label, const double defaultValue)
{
    if (ScriptOption* option = Find(name, ScriptOptionType::Double))
    {
        option->label = label;
        option->doubleValue = defaultValue;
        return;
    }
    ScriptOption option{name, label, ScriptOptionType::Double};
    option.doubleValue = defaultValue;
    _options.push_back(option);
}

void ScriptOptions::AddBool(const std::string& name, const std::string& label, const bool defaultValue)
{
    if (ScriptOption* option = Find(name, ScriptOptionType::Boolean))
    {
        option->label = label;
        option->boolValue = defaultValue;
        return;
    }
    ScriptOption option{name, label, ScriptOptionType::Boolean};
    option.boolValue = defaultValue;
    _options.push_back(option);
}

int ScriptOptions::GetInteger(const std::string& name) const
{
    const ScriptOption* option = Find(name, ScriptOptionType::Integer);
    return option == nullptr ? 0 : option->integerValue;
}

double ScriptOptions::GetDouble(const std::string& name) const
{
    const ScriptOption* option = Find(name, ScriptOptionType::Double);
    return option == nullptr ? 0.0 : option->doubleValue;
}

bool ScriptOptions::GetBool(const std::string& name) const
{
    const ScriptOption* option = Find(name, ScriptOptionType::Boolean);
    return option != nullptr && option->boolValue;
}

const std::vector<ScriptOption>& ScriptOptions::GetOptions() const
{
    return _options;
}

std::vector<ScriptOption>& ScriptOptions::GetOptions()
{
    return _options;
}
