#include "ShaderPreprocessor.hpp"
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <functional>
#include <cctype>
#include <unordered_map>

// Trim from start
static std::string& ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// Trim from end
static std::string &rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// Trim from both ends
static std::string& trim(std::string& s)
{
    return ltrim(rtrim(s));
}

// Checks if given line describes a module pragma
bool IsModulePragma(const std::string& line)
{
    const std::string modulePragma = "#module";
    return modulePragma == line.substr(0, modulePragma.size());
}

// Parses the module name out of a valid module pragma line
std::string ParseModuleName(const std::string& line)
{
    const std::string modulePragma = "#module";
    std::string name = line;
    name.erase(0, modulePragma.size());
    trim(name);
    return name;
}

// Checks if given line describes an include pragma
bool IsIncludePragma(const std::string& line)
{
    const std::string includePragma = "#include";
    return includePragma == line.substr(0, includePragma.size());
}

// Parses the include module param out of a valid include pragma line
std::string ParseIncludeName(const std::string& line)
{
    const std::string includePragma = "#include";
    std::string name = line;
    name.erase(0, includePragma.size());
    trim(name);
    return name;
}

// Retrieves the module name for the given source file if it has one
std::string GetModuleName(const std::string& source)
{
    // Get first line
    std::istringstream iss(source);
    std::string lineBuf;
    std::getline(iss, lineBuf);

    // Check if first line is module definition
    std::string moduleName;
    if (IsModulePragma(lineBuf))
        moduleName = ParseModuleName(lineBuf);
    return moduleName;
}

// Removes the module pragma from the given source file if it has one
std::string& RemoveModulePragma(std::string& source)
{
    // Get first line
    std::istringstream iss(source);
    std::string lineBuf;
    std::getline(iss, lineBuf);

    // Erase first line if module definition
    if (IsModulePragma(lineBuf))
        source.erase(std::begin(source), std::begin(source) + lineBuf.size());
    return source;
}

// Retrieves the module dependecies that the given source has
std::vector<std::string> GetModuleDeps(const std::string& source)
{
    // Split source to line chunks
    std::vector<std::string> sourceChunks;
    std::istringstream iss(source);
    std::string lineBuf;
    while (std::getline(iss, lineBuf))
        sourceChunks.push_back(std::move(lineBuf));

    // Stores the module dependency names
    std::vector<std::string> moduleDeps;
    for (std::size_t i = 0; i < sourceChunks.size(); ++i)
    {
        const std::string& curLine = sourceChunks[i];
        if (IsIncludePragma(curLine))
        {
            std::string include = ParseIncludeName(curLine);
            moduleDeps.push_back(include);
        }
    }
    return moduleDeps;
}

std::vector<std::string> SplitToLineChunks(const std::string& source)
{
    // Split string to line chunks initially
    std::vector<std::string> sourceChunks;
    std::istringstream iss(source);
    std::string lineBuf;
    while (std::getline(iss, lineBuf))
        sourceChunks.push_back(std::move(lineBuf));
    return sourceChunks;
}

// Recursively replaces the include pragma lines with their respective contents
auto PreprocessIncludesR(const std::unordered_map<std::string, std::string>& moduleMap, std::vector<std::string>& usedModules, std::vector<std::string>& sourceChunks) -> std::vector<std::string>
{
    // Replace include module lines with resolved content
    for (std::size_t i = 0; i < sourceChunks.size(); ++i)
    {
        const std::string& curLine = sourceChunks[i];
        if (IsIncludePragma(curLine))
        {
            std::string include = ParseIncludeName(curLine);
            // Remove include line and replace it with module's contents
            auto modulesIt = moduleMap.find(include);
            auto usedIt = std::find(std::begin(usedModules), std::end(usedModules), include);
            if (modulesIt != std::end(moduleMap))
            {
                // Remove include line
                sourceChunks.erase(std::begin(sourceChunks) + i);

                // Add module source if not already added before
                if (usedIt == std::end(usedModules))
                {
                    // Split module to chunks and add them to the original source chunks
                    auto moduleChunks = SplitToLineChunks(modulesIt->second);
                    sourceChunks.insert(std::begin(sourceChunks) + i, std::begin(moduleChunks), std::end(moduleChunks));

                    // Mark module as used
                    usedModules.push_back(include);

                    // Start preprocessing from the start
                    return PreprocessIncludesR(moduleMap, usedModules, sourceChunks);
                }
            }
        }
    }

    return sourceChunks;
}

// Replaces include pragma in source and modules with their respective contents
std::string PreprocessIncludes(const std::string& source, const std::unordered_map<std::string, std::string>& moduleMap)
{
    // Split code to chunks
    std::vector<std::string> sourceChunks = SplitToLineChunks(source);

    // Preprocess those chunks
    std::vector<std::string> usedModules;
    sourceChunks = PreprocessIncludesR(moduleMap, usedModules, sourceChunks);

    // Join source chunks to single string
    std::string result;
    for (const auto& chunk : sourceChunks)
        result += chunk + "\n";
    return result;
}

//
// Preprocesses a given source, removing the module directive and replacing the include
// directives with the already parsed module sources in the given map
//
std::string PreprocessSource(std::string& source, std::unordered_map<std::string, std::string>& moduleMap)
{
    return PreprocessIncludes(source, moduleMap);
}

auto PreprocessModules(const std::string& source, const std::vector<std::string>& deps, std::unordered_map<std::string, std::string>& loadedModules) -> std::string
{
    std::string sourceCopy(source);

    // Preparse all the needed deps
    std::vector<std::string> neededModules = GetModuleDeps(source);

    for (const auto& dep : deps)
    {
        // Get source's corresponding module name
        std::string moduleName = GetModuleName(dep);

        // Check if it is on the needed module list
        auto it = std::find(std::begin(neededModules), std::end(neededModules), moduleName);
        if (it != std::end(neededModules))
        {
            // Check if module is already loaded
            if (loadedModules.find(moduleName) != std::end(loadedModules))
                continue;

            // Preprocess it and add its preprocessed source to the list
            std::string preprSrc = PreprocessModules(dep, deps, loadedModules);
            loadedModules.insert({moduleName, preprSrc});
        }
    }

    RemoveModulePragma(sourceCopy);
    return sourceCopy;
}

auto ShaderPreprocessor::Preprocess(const std::string& source, const std::vector<std::string>& deps) -> std::string
{
    std::unordered_map<std::string, std::string> loadedModules;
    std::string output = PreprocessModules(source, deps, loadedModules);
    return PreprocessSource(output, loadedModules);
}
