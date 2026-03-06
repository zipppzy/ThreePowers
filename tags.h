#ifndef TAGS_H
#define TAGS_H

#include <set>
#include <string>
#include <optional>
#include <string_view>
#include <QString>

namespace Tags {
inline constexpr std::string_view UI_PREFIX = "ui:";

inline std::optional<std::string> getSectionName(const std::set<std::string>& tags) {
    for (const auto& tag : tags) {
        if (tag.rfind(UI_PREFIX, 0) == 0) {
            return tag.substr(UI_PREFIX.size());
        }
    }
    return std::nullopt;
}

inline QString toDisplayName(const std::string& sectionTag) {
    if (sectionTag.empty()) return "General";
    QString s = QString::fromStdString(sectionTag);
    s[0] = s[0].toUpper();
    return s;
}
}

#endif // TAGS_H
