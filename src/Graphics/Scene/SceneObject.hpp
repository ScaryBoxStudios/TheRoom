/**********************************************************************************************************************/
/*               ______             ______                                                                            */
/*              /     /\           /     /\                                                                           */
/*             /     /  \         /     /  \                                                                          */
/*            /     / /\ \       /     / /\ \                                                                         */
/*           /_____/ /  \ \     /     / /  \ \                                                                        */
/*           \     \ \___\/    /     /  \___\ |                                                                       */
/*         ___\_    \ \       /     / /\ \  / /                                                                       */
/*        /_____/\___\ \     /_____/ /  \ \/ /                                                                        */
/*        \     \ \  / /     \     \ \___\  /                                                                         */
/*         \     \ \/ /       \     \ \  / /                                                                          */
/*          \     \  /         \     \ \/ /                                                                           */
/*           \_____\/           \_____\__/                                                                            */
/*                                                                                                                    */
/*                                                                                                                    */
/*      (C) 2015, ScaryBox Studios. All rights reserved.                                                              */
/*                                                                                                                    */
/*      Authors: Agorgianitis Loukas <agorg_louk@icloud.com>                                                          */
/*               Vlachakis Dimitris  <dimitrisvlh@gmail.com>                                                          */
/*                                                                                                                    */
/**********************************************************************************************************************/

#ifndef _SCENEOBJECT_HPP
#define _SCENEOBJECT_HPP

#include <string>
#include <forward_list>
#include "Transform.hpp"

/// Scene Object Categories
enum class SceneObjCategory
{
    Normal,
    Light
};

struct SceneObject
{
    Transform transform;                      /// Object's current transformation
    std::string model;                        /// Object's model name
    SceneObjCategory category;                /// Object's category

    std::string parent;                      /// Object's parent object
    std::forward_list<std::string> children; /// Object's children objects

}; //~ SceneObject

#endif //~ _SCENEOBJECT_HPP