// Copyright Mattias Larsson SKöld 2020

#pragma once

#include "fmt/core.h"
#include "fmt/ostream.h"
#include <algorithm>
#include <string>
#include <vector>

class Html {
public:
    Html(std::string tag = "html",
         std::vector<std::pair<std::string, std::string>> attributes = {},
         std::vector<Html> children = {},
         std::string content = {})
        : _tag(tag)
        , _attributes(attributes)
        , _children(std::move(children))
        , _content(content)
        , _isEmptyType(isEmptyType(tag)) {}

    struct Args {
        std::string tag = "html";
        std::vector<std::pair<std::string, std::string>> attributes = {};
        std::vector<Html> children = {};
        std::string content = {};
    };

    //! Constructor but with "named argumnets"
    Html(Args args)
        : _tag(args.tag)
        , _attributes(std::move(args.attributes))
        , _children(std::move(args.children))
        , _content(std::move(args.content))
        , _isEmptyType(isEmptyType(_tag)) {}

    void indent(std::ostream &stream, size_t level) const {
        for (size_t i = 0; i < level; ++i) {
            (void)i;
            stream << "  ";
        }
    }

    void print(std::ostream &stream, size_t level = 0) const {
        indent(stream, level);
        fmt::print(stream, "<{}", _tag);

        for (const auto &attribute : _attributes) {
            fmt::print(stream, " {}=\"{}\"", attribute.first, attribute.second);
        }

        if (_isEmptyType) {
            fmt::print(stream, "/>\n");
        }
        else {

            if (_content.empty()) {
                fmt::print(stream, ">\n");
            }
            else {
                fmt::print(stream, ">\n{}\n", _content);
            }

            for (const auto &child : _children) {
                child.print(stream, level + 1);
            }

            indent(stream, level);
            fmt::print(stream, "</{}>\n", _tag);
        }
    }

    friend std::ostream &operator<<(std::ostream &stream, const Html &html) {
        html.print(stream);
        return stream;
    }

    static bool isEmptyType(std::string_view tag) {
        return std::find(emptyTags.begin(), emptyTags.end(), tag) !=
               emptyTags.end();
    }

    bool isEmptyType() {
        return _isEmptyType;
    }

    Html &addChild(Html html) {
        _children.push_back(std::move(html));
    }

    auto &children() {
        return _children;
    }

    const auto &children() const {
        return _children;
    }

    //! Add a attribute
    Html &attribute(std::string name, std::string value) {
        _attributes.push_back({std::move(name), std::move(value)});
        return *this;
    }

    Html &tag(std::string tag) {
        _tag = tag;

        return *this;
    }

    std::string _tag = "html";

    std::vector<std::pair<std::string, std::string>> _attributes;
    std::vector<Html> _children;
    std::string _content;
    bool _isEmptyType;

    static constexpr std::array<std::string_view, 14> emptyTags = {
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
