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
#ifndef _PROPERTIES_VALIDATOR_HPP_
#define _PROPERTIES_VALIDATOR_HPP_

#include <vector>
#include <unordered_map>
#include <string>
#include "PropertiesLoader.hpp"

class PropertiesValidator
{
    public:
        using ErrorCode = int;
        using WarningCode = int;

        class Result
        {
            public:
                std::vector<ErrorCode> errors;
                std::vector<WarningCode> warnings;

                Result& operator+=(const Result& b);
                Result& operator+=(const Result&& b);
        };

        template <typename T>
        Result Validate(const T& input);

        // -----
        // Validate Bulk is a utility to validate many elements at once. The results
        // are merged into a single Result struct.
        // -----
        template <typename T>
        using InputContainer = PropertiesLoader::OutputContainer<T>; // Alias for ValidateBulk input

        // Function without parameters to terminate variadic recursion
        Result ValidateBulk() { Result r = {}; return r; }

        template <typename T, typename... Args>
        Result ValidateBulk(const InputContainer<T>& input, Args&&... args)
        {
            Result r = {};

            // Validate input
            for (const auto& p : input)
                r += Validate(p.second);

            return r += ValidateBulk(std::forward<Args>(args)...);
        }

        // Convert codes to strings
        static std::string ErrorToString(ErrorCode err);
        static std::string WarnToString(WarningCode warn);

    private:
        const static std::unordered_map<ErrorCode, std::string> mErrMap;
        const static std::unordered_map<WarningCode, std::string> mWarnMap;

};

#endif // ! _PROPERTIES_VALIDATOR_HPP_
