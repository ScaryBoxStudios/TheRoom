#include "Scene.hpp"
#include <algorithm>

//--------------------------------------------------
// Public functions
//--------------------------------------------------
Scene::Scene()
{
    // TODO: decide model, uuid and position for root node
    //mRootNode = CreateNode("", "", SceneNodeCategory::Invalid, glm::vec3(0, 0, 0));
    // Create node
    mNodes[""] = std::make_unique<SceneNode>("", "", SceneNodeCategory::Invalid, AABB());
}

SceneNode* Scene::CreateNode(
    const std::string& model,
    const std::string& uuid,
    SceneNodeCategory category,
    const AABB& initAABB,
    bool isCulled /* = false */)
{
    // Create node
    std::unique_ptr<SceneNode> ptr =
        std::make_unique<SceneNode>(model, uuid, category, initAABB, isCulled);

    // Save it to category map
    mCategories[category].push_back(ptr.get());

    // Save the pointer
    SceneNode* rVal = ptr.get();

    // Move the unique pointer to node bank
    mNodes[rVal->GetUUID()] = std::move(ptr);

    return rVal;
}

void Scene::AttachToParent(SceneNode* child, const std::string& parentUuid)
{
    auto parent = FindNodeByUuid(parentUuid);

    // Do nothing if parent doesn't exist
    if(parent == nullptr)
        return;

    // Add child to parent
    parent->AddChild(child);
}

void Scene::DetachFromParent(const std::string& childUuid, const std::string& parentUuid)
{
    SceneNode* parent = FindNodeByUuid(parentUuid);

    // Do nothing if parent doesn't exist
    if(parent == nullptr)
        return;

    // Remove child from parent
    parent->RemoveChild(childUuid);
}

void Scene::Move(const std::string& uuid, const glm::vec3& pos, bool moveChildren /* = false */)
{
    SceneNode* node = FindNodeByUuid(uuid);

    if(node == nullptr)
        return;
    else
        node->Move(pos, moveChildren);
}

void Scene::Rotate(const std::string& uuid, RotationAxis axis, float angle, bool rotateChildren /* = false */)
{
    SceneNode* node = FindNodeByUuid(uuid);

    if(node == nullptr)
        return;
    else
        node->Rotate(axis, angle, rotateChildren);
}

void Scene::Scale(const std::string& uuid, const glm::vec3& scale, bool scaleChildren /* = false */)
{
    SceneNode* node = FindNodeByUuid(uuid);

    if(node == nullptr)
        return;
    else
        node->Scale(scale, scaleChildren);
}

const Scene::NodeBank& Scene::GetNodes() const
{
    return mNodes;
}

const Scene::NodeCategoryMap& Scene::GetCategories() const
{
    return mCategories;
}

//--------------------------------------------------
// Private functions
//--------------------------------------------------
inline SceneNode* Scene::FindNodeByUuid(const std::string& uuid)
{
    auto it = mNodes.find(uuid);
    return (it == std::end(mNodes)) ? nullptr : it->second.get();
}
