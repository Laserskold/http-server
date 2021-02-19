// Copyright Mattias Larsson SKöld 2020

#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

struct Html {
    std::string tag = "div";
    std::vector<std::pair<std::string, std::string>> attributes = {};
    std::vector<Html> children = {};
    std::string content = {};
    bool isEmptyType = false;

    void indent(std::ostream &stream, size_t level) const {
        for (size_t i = 0; i < level; ++i) {
            (void)i;
            stream << "  ";
        }
    }

    void print(std::ostream &stream, size_t level = 0) const {
        indent(stream, level);
        stream << "<" << tag;

        for (const auto &attribute : attributes) {
            stream << " " << attribute.first << "=\"" << attribute.second
                   << "\"";
        }

        if (isEmptyType) {
            stream << "/>\n";
        }
        else {

            if (content.empty()) {
                stream << ">\n";
            }
            else {
                stream << ">\n" << content << "\n";
            }

            for (const auto &child : children) {
                child.print(stream, level + 1);
            }

            indent(stream, level);
            stream << "</" << tag << ">\n";
        }
    }

    friend std::ostream &operator<<(std::ostream &stream, const Html &html) {
        html.print(stream);
        return stream;
    }

    static bool isSingleton(std::string_view tag) {
        return std::find(emptyTags.begin(), emptyTags.end(), tag) !=
               emptyTags.end();
    }

    Html &addChild(Html html) {
        children.push_back(std::move(html));
        return *this;
    }

    //! Add a attribute
    Html &attribute(std::string name, std::string value) {
        attributes.push_back({std::move(name), std::move(value)});
        return *this;
    }

    static inline constexpr std::array<std::string_view, 14> emptyTags = {
        "area",
        "base",
        "br",
        "col",
        "embed",
        "hr",
        "img",
        "input",
        "link",
        "meta",
        "param",
        "source",
        "track",
        "wbr",
    };
};
