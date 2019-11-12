#include <fstream>
#include <nlohmann/json.hpp>
#include <Doxybook/Config.hpp>
#include "ExceptionUtils.hpp"

class ConfigArg {
public:
    ConfigArg(std::string Doxybook2::Config::*ref, std::string key) : key(std::move(key)) {
        loadFunc = [=](const ConfigArg& self, Doxybook2::Config& config, const nlohmann::json& json) {
            try {
                if (json.contains(self.key)) {
                    config.*ref = json.at(self.key).get<std::string>();
                }
            } catch (std::exception& e) {
                throw EXCEPTION("Failed to get config string value {} error: {}", self.key, e.what());
            }
        };
        saveFunc = [=](const ConfigArg& self, const Doxybook2::Config& config,  nlohmann::json& json) {
            json[self.key] = config.*ref;
        };
    }

    ConfigArg(bool Doxybook2::Config::*ref, std::string key) : key(std::move(key)) {
        loadFunc = [=](const ConfigArg& self, Doxybook2::Config& config, const nlohmann::json& json) {
            try {
                if (json.contains(self.key)) {
                    config.*ref = json.at(self.key).get<bool>();
                }
            } catch (std::exception& e) {
                throw EXCEPTION("Failed to get config bool value {} error: {}", self.key, e.what());
            }
        };
        saveFunc = [=](const ConfigArg& self, const Doxybook2::Config& config,  nlohmann::json& json) {
            json[self.key] = config.*ref;
        };
    }

    std::string key;
    std::function<void(const ConfigArg&, Doxybook2::Config& config, const nlohmann::json&)> loadFunc;
    std::function<void(const ConfigArg&, const Doxybook2::Config& config, nlohmann::json&)> saveFunc;
};

static const std::vector<ConfigArg> configArgs = {
    ConfigArg(&Doxybook2::Config::baseUrl, "baseUrl"),
    ConfigArg(&Doxybook2::Config::fileExt, "fileExt"),
    ConfigArg(&Doxybook2::Config::linkSuffix, "linkSuffix"),
    ConfigArg(&Doxybook2::Config::linkLowercase, "linkLowercase"),
    ConfigArg(&Doxybook2::Config::copyImages, "copyImages"),
    ConfigArg(&Doxybook2::Config::useFolders, "useFolders"),
    ConfigArg(&Doxybook2::Config::imagesFolder, "imagesFolder"),
    ConfigArg(&Doxybook2::Config::mainPageName, "mainPageName"),
    ConfigArg(&Doxybook2::Config::mainPageInRoot, "mainPageInRoot"),
    ConfigArg(&Doxybook2::Config::folderClassesName, "folderClassesName"),
    ConfigArg(&Doxybook2::Config::folderFilesName, "folderFilesName"),
    ConfigArg(&Doxybook2::Config::folderGroupsName, "folderGroupsName"),
    ConfigArg(&Doxybook2::Config::folderNamespacesName, "folderNamespacesName"),
    ConfigArg(&Doxybook2::Config::folderRelatedPagesName, "folderRelatedPagesName"),
    ConfigArg(&Doxybook2::Config::indexInFolders, "indexInFolders"),
    ConfigArg(&Doxybook2::Config::indexClassesName, "indexClassesName"),
    ConfigArg(&Doxybook2::Config::indexFilesName, "indexFilesName"),
    ConfigArg(&Doxybook2::Config::indexGroupsName, "indexGroupsName"),
    ConfigArg(&Doxybook2::Config::indexNamespacesName, "indexNamespacesName"),
    ConfigArg(&Doxybook2::Config::indexRelatedPagesName, "indexRelatedPagesName"),
    ConfigArg(&Doxybook2::Config::templateIndexClasses, "templateIndexClasses"),
    ConfigArg(&Doxybook2::Config::templateIndexFiles, "templateIndexFiles"),
    ConfigArg(&Doxybook2::Config::templateIndexGroups, "templateIndexGroups"),
    ConfigArg(&Doxybook2::Config::templateIndexNamespaces, "templateIndexNamespaces"),
    ConfigArg(&Doxybook2::Config::templateIndexRelatedPages, "templateIndexRelatedPages"),
    ConfigArg(&Doxybook2::Config::templateKindGroup, "templateKindGroup"),
    ConfigArg(&Doxybook2::Config::templateKindClass, "templateKindClass"),
    ConfigArg(&Doxybook2::Config::templateKindDir, "templateKindDir"),
    ConfigArg(&Doxybook2::Config::templateKindPage, "templateKindPage"),
    ConfigArg(&Doxybook2::Config::templateKindInterface, "templateKindInterface"),
    ConfigArg(&Doxybook2::Config::templateKindFile, "templateKindFile"),
    ConfigArg(&Doxybook2::Config::templateKindNamespace, "templateKindNamespace"),
    ConfigArg(&Doxybook2::Config::templateKindStruct, "templateKindStruct"),
    ConfigArg(&Doxybook2::Config::templateKindUnion, "templateKindUnion"),
};

void Doxybook2::loadConfig(Config& config, const std::string& path) {
    std::ifstream file(path);
    if (!file) throw EXCEPTION("Failed to open file {} for reading", path);
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    try {
        const auto json = nlohmann::json::parse(str);

        for (const auto& arg : configArgs) {
            arg.loadFunc(arg, config, json);
        }
    } catch (std::exception& e) {
        throw EXCEPTION("Failed to pase config error {}", e.what());
    }
}

void Doxybook2::loadConfigData(Config& config, const std::string& src) {
    try {
        const auto json = nlohmann::json::parse(src);

        for (const auto& arg : configArgs) {
            arg.loadFunc(arg, config, json);
        }
    } catch (std::exception& e) {
        throw EXCEPTION("Failed to pase config error {}", e.what());
    }
}

void Doxybook2::saveConfig(Config& config, const std::string& path) {
    Log::i("Creating default config {}", path);
    std::ofstream file(path);
    if (!file) throw EXCEPTION("Failed to open file {} for writing", path);

    nlohmann::json json;
    for (const auto& arg : configArgs) {
        arg.saveFunc(arg, config, json);
    }

    file << json.dump(2);
}
