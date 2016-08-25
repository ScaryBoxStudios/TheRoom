/*********************************************************************************************************************/
/*                                                  /===-_---~~~~~~~~~------____                                     */
/*                                                 |===-~___                _,-'                                     */
/*                  -==\\                         `//~\\   ~~~~`---.___.-~~                                          */
/*              ______-==|                         | |  \\           _-~`                                            */
/*        __--~~~  ,-/-==\\                        | |   `\        ,'                                                */
/*     _-~       /'    |  \\                      / /      \      /                                                  */
/*   .'        /       |   \\                   /' /        \   /'                                                   */
/*  /  ____  /         |    \`\.__/-~~ ~ \ _ _/'  /          \/'                                                     */
/* /-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`                                                      */
/*                   \_|      /        _)   ;  ),   __--~~                                                           */
/*                     '~~--_/      _-~/-  / \   '-~ \                                                               */
/*                    {\__--_/}    / \\_>- )<__\      \                                                              */
/*                    /'   (_/  _-~  | |__>--<__|      |                                                             */
/*                   |0  0 _/) )-~     | |__>--<__|     |                                                            */
/*                   / /~ ,_/       / /__>---<__/      |                                                             */
/*                  o o _//        /-~_>---<__-~      /                                                              */
/*                  (^(~          /~_>---<__-      _-~                                                               */
/*                 ,/|           /__>--<__/     _-~                                                                  */
/*              ,//('(          |__>--<__|     /                  .----_                                             */
/*             ( ( '))          |__>--<__|    |                 /' _---_~\                                           */
/*          `-)) )) (           |__>--<__|    |               /'  /     ~\`\                                         */
/*         ,/,'//( (             \__>--<__\    \            /'  //        ||                                         */
/*       ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'                                          */
/*     `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/                                                     */
/*   ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~                                                       */
/*    ;'( ')/ ,)(                              ~~~~~~~~~~                                                            */
/*   ' ') '( (/                                                                                                      */
/*     '   '  `                                                                                                      */
/*********************************************************************************************************************/
#ifndef _PROPERTIES_LOADER_HPP_
#define _PROPERTIES_LOADER_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <stdint.h>

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
#include <rapidjson/document.h>
WARN_GUARD_OFF

// Converts JSON data to a JSON Document object
void ParseJson(const std::vector<std::uint8_t>& data, rapidjson::Document& doc);

class PropertiesLoader
{
    public:
        using Buffer = std::vector<std::uint8_t>;

        // -----
        // LoadBulk is a utility to load any number of files in one shot. LoadBulk expects
        // pairs of input/ouput variables and loads every input to its respeective output
        // -----
        using InputContainer = std::unordered_map<std::string, const Buffer&>; // Alias for LoadBulk input
        template <typename T>
        using OutputContainer = std::unordered_map<std::string, T>;            // Alias for LoadBulk output

        // Empty load function to terminate variadic recursion
        void LoadBulk() {}

        template <typename T, typename... Args>
        void LoadBulk(const InputContainer& input, OutputContainer<T>& output, Args&&... args)
        {
            for (const auto& p : input)
                output.insert({p.first, Load<T>(p.second)});
            LoadBulk(std::forward<Args>(args)...);
        }

        // Parses a JSON from Buffer and initiates the JSON Document loading
        template <typename T>
        T Load(const Buffer& buf)
        {
            // Parse JSON
            rapidjson::Document doc;
            ParseJson(buf, doc);
            return Load<T>(doc);
        }

    private:
        template <typename T>
        T Load(rapidjson::Document& doc);
};

#endif // ! _PROPERTIES_LOADER_HPP_
