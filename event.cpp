#include "event.h"

Event::Choice::Choice(const toml::table& choiceTable) {
    name = choiceTable["name"].value_or("");
}

Event::Event(std::string title, std::string body)
    : title{title}, body{body}
{}

Event::Event(toml::table eventTable) {
    this->id = eventTable["id"].value_or("");
    this->title = eventTable["title"].value_or("");
    this->body = eventTable["body"].value_or("");

    if(auto choicesArray = eventTable["choices"].as_array()) {
        for(auto&& choiceNode : *choicesArray) {
            if(auto table = choiceNode.as_table()) {
                this->choices.push_back(Choice(*table));
            }
        }
    }
}

std::unordered_map<std::string, Event> Event::eventDatabase;

void Event::loadEventDatabase(std::string path) {
    auto in = toml::parse_file(path);

    Event::eventDatabase.clear();
    if(auto eventsArray = in["events"].as_array()) {
        for(auto& elem : *eventsArray) {
            if(auto eventTable = elem.as_table()) {
                Event::eventDatabase.insert_or_assign(eventTable->at("id").value_or(""), Event(*eventTable));
            }
        }
    }
}

std::optional<std::reference_wrapper<const Event>> Event::checkEventDatabase(std::string id) {
    if(auto search = Event::eventDatabase.find(id); search != Event::eventDatabase.end()) {
        return std::cref(search->second);
    } else {
        return std::nullopt;
    }
}
