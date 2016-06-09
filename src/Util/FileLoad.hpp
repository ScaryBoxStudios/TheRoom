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
#ifndef _FILELOAD_HPP_
#define _FILELOAD_HPP_

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <memory>
#include <vector>
#include <fstream>
#include <iterator>
#include <cstdint>

template <typename Buffer = std::vector<std::uint8_t>>
std::unique_ptr<Buffer> FileLoad(const std::string& file)
{
    /* Filesize in bytes */
    long int size = -1;

    /* Try open the file */
    FILE* f = 0;
    f = fopen(file.c_str(), "rb");
    if (!f)
        return std::unique_ptr<Buffer>(nullptr);

    /* Seek to its end */
    fseek(f, 0, SEEK_END);

    /* Tell the size of the current pos */
    size = ftell(f);

    /* Seek back */
    fseek(f, 0, SEEK_SET);

    // Create buffer with preallocated size
    Buffer buf(static_cast<std::size_t>(size));

    /* Read the file in single IO operation */
    fread(buf.data(), 1, size, f);

    /* Close the file handle */
    fclose(f);

    return std::make_unique<Buffer>(std::move(buf));
}
#endif // ! _FILELOAD_HPP_
