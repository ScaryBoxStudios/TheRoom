#include "Scene.hpp"
#include <algorithm>

//--------------------------------------------------
// Public functions
//--------------------------------------------------
void Scene::AddObject(
    const std::string& name,
    const std::string& modelName,
    SceneObjCategory category,
    const std::string& parent /* = "" */ )
{
    // Check if object with that name already exists
    if(mObjects.find(name) != mObjects.end())
        return;

    // Create a new scene object
    SceneObject newObj;
    newObj.model = modelName;
    newObj.category = category;
    newObj.parent = parent;

    // Add the new object to scene map
    mObjects[name] = newObj;

    // Get an iterator to newly added object
    auto it = mObjects.find(name);

    // Add it to categories
    mCategories[category].push_back(it);
}

void Scene::RemoveObject(const std::string& name)
{
    // Get iterator to object
    auto it = mObjects.find(name);

    // Check if object with that name exists
    if(it == mObjects.end())
        return;

    // Erase it from category map
    auto& v = mCategories[it->second.category];
    auto eraseIt = std::find(v.begin(), v.end(), it);

    if(eraseIt != v.end())
        v.erase(eraseIt);

    // Erase object
    mObjects.erase(it);
}

void Scene::AttachObject(const std::string& child, const std::string& parent)
{
    // Check if objects exist
    if(mObjects.find(child) == mObjects.end() || mObjects.find(parent) == mObjects.end())
        return;

    // Add parent to child node
    mObjects[child].parent = parent;

    // Append child (if not already exists) to parent
    auto& parentChildren = mObjects[parent].children;
    auto it = std::find(parentChildren.begin(), parentChildren.end(), child);
    if(it == parentChildren.end())
        mObjects[parent].children.push_front(child);
}

void Scene::DetachObject(const std::string& child, const std::string& parent)
{
    // Check if objects exist
    if(mObjects.find(child) == mObjects.end() || mObjects.find(parent) == mObjects.end())
        return;

    // Remove parent from child
    mObjects[child].parent = "";

    // Remove child from parent
    mObjects[parent].children.remove_if(
        [&child](const std::string& value) -> bool
        {
            return value.compare(child) == 0;
        }
    );
}

void Scene::Move(const std::string& objName, const glm::vec3& pos)
{
    // Check if object with that name exists
    auto it = mObjects.find(objName);
    if(it != mObjects.end())
    {
        // Move the object
        mObjects[objName].transform.Move(pos);

        // Move its children (if any)
        for(const auto& str : it->second.children)
            Move(str, pos);
    }
}

void Scene::RotateX(const std::string& objName, float angle)
{
    // Check if object with that name exists
    auto it = mObjects.find(objName);
    if(it != mObjects.end())
    {
        // Rotate the object
        mObjects[objName].transform.RotateX(angle);

        // Rotate its children (if any)
        for(const auto& str : it->second.children)
            RotateX(str, angle);
    }
}

void Scene::RotateY(const std::string& objName, float angle)
{
    // Check if object with that name exists
    auto it = mObjects.find(objName);
    if(it != mObjects.end())
    {
        // Rotate the object
        mObjects[objName].transform.RotateY(angle);

        // Rotate its children (if any)
        for(const auto& str : it->second.children)
            RotateY(str, angle);
    }
}

void Scene::RotateZ(const std::string& objName, float angle)
{
    // Check if object with that name exists
    auto it = mObjects.find(objName);
    if(it != mObjects.end())
    {
        // Rotate the object
        mObjects[objName].transform.RotateZ(angle);

        // Rotate its children (if any)
        for(const auto& str : it->second.children)
            RotateZ(str, angle);
    }
}

void Scene::Scale(const std::string& objName, const glm::vec3& scale)
{
    // Check if object with that name exists
    auto it = mObjects.find(objName);
    if(it != mObjects.end())
    {
        // Scale the object
        mObjects[objName].transform.Scale(scale);

        // Scale its children (if any)
        for(const auto& str : it->second.children)
            Scale(str, scale);
    }
}

auto Scene::GetObjects() -> std::map<std::string, SceneObject>&
{
    return mObjects;
}

auto Scene::GetCategories() ->std::unordered_map<SceneObjCategory, std::vector<std::map<std::string, SceneObject>::iterator>>&
{
    return mCategories;
}
