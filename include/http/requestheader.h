// Copyright Mattias Larsson SKöld 2020

#pragma once

#include "../common/filesystem.h"
#include <algorithm>
#include <asio/ip/tcp.hpp>

namespace http {

//! Header for http request
struct RequestHeader {
    std::string protocol;      // Could be GET POST etc
    filesystem::path location; // files location
    std::string version;

    std::vector<std::pair<std::string, std::string>> attributes;

    std::string get(std::string name, std::string def = {}) {
        auto it = std::find_if(
            attributes.begin(), attributes.end(), [&name](const auto &val) {
                return val.first == name;
            });

        if (it != attributes.end()) {
            return it->second;
        }
        else {
            return def;
        }
    }

    RequestHeader(std::string data) {
        attributes.reserve(20);

        std::istringstream ss(data);

        {
            std::string line;
            std::getline(ss, line);

            std::istringstream liness(line);

            std::string location;

            liness >> protocol;
            liness >> location;
            liness >> version;

            if (location.find("..") != std::string::npos) {
                location = ""; // Should not be possible to exit root folder
            }

            if (location.front() == '/') {
                location.insert(location.begin(), '.');
            }

            this->location = location;
        }

        for (std::string line; std::getline(ss, line);) {
            if (auto f = line.find(':'); f != std::string::npos) {
                attributes.emplace_back(line.substr(0, f), line.substr(f + 1));
            }
            else {
                break;
            }
        }

        for (auto &attribute : attributes) {
            while (!attribute.first.empty() &&
                   isspace(attribute.first.back())) {
                attribute.first.pop_back();
            }
            while (!attribute.second.empty() &&
                   isspace(attribute.second.front())) {
                attribute.second.erase(0, 1);
            }
        }
    }
};

} // namespace http
