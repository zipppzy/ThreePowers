#ifndef EVENT_H
#define EVENT_H

#include "toml.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Event
{
public:
    struct Choice {
        std::string name;

        Choice() = default;
        explicit Choice(const toml::table& choiceTable);
    };

    Event() = default;
    Event(std::string title, std::string body);
    Event(toml::table eventTable);

    static void loadEventDatabase(std::string path);
    static std::optional<std::reference_wrapper<const Event>> checkEventDatabase(std::string name);

    std::string id;
    std::string title;
    std::string body;
    std::vector<Choice> choices;

private:
    static std::unordered_map<std::string, Event> eventDatabase;
};

#endif // EVENT_H
