#pragma once

#include <iostream>
#include <string>
#include <vector>

struct CssAttribute {
    std::string name;
    std::string value;

    friend std::ostream &operator<<(std::ostream &stream,
                                    const CssAttribute &attribute) {
        stream << "  " << attribute.name << " = " << attribute.value << ";\n";
        return stream;
    }
};

struct CssRule {
    std::string name;
    std::vector<CssAttribute> attributes;

    friend std::ostream &operator<<(std::ostream &stream, const CssRule &rule) {
        stream << rule.name << "\{{\n";

        for (const auto &attribute : rule.attributes) {
            stream << attribute;
        }

        stream << "}}\n\n";
        return stream;
    }
};

//! Usage example:
//! <pre>
//! auto sheet = CssSheet{
//!    {
//!        "img",
//!        {{"width", "100"}},
//!    },
//!    {
//!        "div.hello",
//!        {
//!            {"width", "100"},
//!            {"height", "100"},
//!        },
//!    },
//!};
//! </pre>
struct CssSheet {
    std::vector<CssRule> rules;
    CssSheet(std::initializer_list<CssRule> rules)
        : rules(rules) {}

    friend std::ostream &operator<<(std::ostream &stream,
                                    const CssSheet &sheet) {
        for (const auto &rule : sheet.rules) {
            stream << rule;
        }
        return stream;
    }
};
