#include "ShaderPreprocessor.hpp"
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <cctype>

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

// Replaces the include pragma lines with their respecting contents if found on resolve map
std::string PreprocessIncludes(const std::string& source, const std::unordered_map<std::string, std::string>& moduleMap)
{
    // Split string to line chunks initially
    std::vector<std::string> sourceChunks;
    std::istringstream iss(source);
    std::string lineBuf;
    while (std::getline(iss, lineBuf))
        sourceChunks.push_back(std::move(lineBuf));

    // Replace include module lines with resolved content
    for (std::size_t i = 0; i < sourceChunks.size(); ++i)
    {
        const std::string& curLine = sourceChunks[i];
        if (IsIncludePragma(curLine))
        {
            std::string include = ParseIncludeName(curLine);
            // Remove include line and replace it with module's contents
            auto modulesIt = moduleMap.find(include);
            if (modulesIt != moduleMap.end())
            {
                sourceChunks.erase(std::begin(sourceChunks) + i);
                sourceChunks.insert(std::begin(sourceChunks) + i, modulesIt->second);
            }
        }
    }

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
std::string PreprocessSource(const std::string& source, const std::unordered_map<std::string, std::string>& moduleMap)
{
    std::string result = source;
    RemoveModulePragma(result);
    return PreprocessIncludes(result, moduleMap);
}

auto ShaderPreprocessor::Preprocess(const std::string& source, const std::vector<std::string>& deps) -> std::string
{
    // Preparse all the needed deps
    std::vector<std::string> neededModules = GetModuleDeps(source);
    std::unordered_map<std::string, std::string> neededModulesSources;
    for (const auto& dep : deps)
    {
        // Get source's corresponding module name
        std::string moduleName = GetModuleName(dep);

        // Check if it is on the needed module list
        auto it = std::find(std::begin(neededModules), std::end(neededModules), moduleName);
        if (it != std::end(neededModules))
        {
            // Preprocess it and add its preprocessed source to the list
            std::string preprSrc = Preprocess(dep, deps);
            neededModulesSources.insert({moduleName, preprSrc});
        }
    }

    return PreprocessSource(source, neededModulesSources);
}
