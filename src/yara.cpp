
#include <filesystem>
#include <iostream>
#include <vector>

#include "../headers/yara.h"
#include "../yaracpp/include/yaracpp/yara_detector/yara_detector.h"
#include "../headers/data_management.h"
#include "../headers/quarantine.h"

yaracpp::YaraDetector *yaraDetector;
extern std::vector<std::string> filesAddedToQuarantine;

bool InitializeYaraDetector(const std::vector<std::filesystem::path> &rule_paths) {
    yaraDetector = new yaracpp::YaraDetector();
    for (const std::filesystem::path &rule_path: rule_paths) {
        if (!yaraDetector->addRuleFile(rule_path)) {
            std::cerr << "Cannot compile rule: " << rule_path.string() << "\n";
            return false;
        }
    }
    return true;
}

bool ScanUsingYaraDetector(const std::filesystem::path &path) {
    std::optional<std::filesystem::path> regularFilePath = CheckFileBeforeScanning(path);
    if (!regularFilePath) {
        return false;
    }
    if (!yaraDetector->analyze(regularFilePath->string())) {
        std::cout << regularFilePath.value().string() << " -> Cannot analyze file" << "\n";
        yaraDetector->clearCachedResults();
        return false;
    }
    std::vector<yaracpp::YaraRule> rules = yaraDetector->getDetectedRules();
    yaraDetector->clearCachedResults();
    if (rules.empty()) {
        std::cout << regularFilePath.value().string() << " -> OK" << "\n";
        return true;
    }
    std::cout << regularFilePath.value().string() << " -> matched rules:" << "\n"; // hash was in hash database
    for (const yaracpp::YaraRule &rule: rules) {
        std::cout << "\tRule: " << rule.getName() << "\n";
    }
    if (!DoQuarantine(regularFilePath.value())) { // moving to quarantine
        std::cerr << "Quarantine was not successfully imposed" << "\n";
        return false;
    }
    filesAddedToQuarantine.push_back(path.string());
    std::cout << "Quarantine was successfully imposed" << "\n";
    return true;
}


