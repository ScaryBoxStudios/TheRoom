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
    //mNodes[""] = std::make_unique<SceneNode>("", "", "", SceneNodeCategory::Invalid, AABB());
}

SceneNode* Scene::CreateNode(
    const std::string& model,
    const std::vector<std::string>& material,
    const std::string& uuid,
    Category category,
    const AABB& initAABB,
    bool isCulled /* = false */)
{
    // Create node
    std::unique_ptr<SceneNode> ptr =
        std::make_unique<SceneNode>(model, material, uuid, category, initAABB, isCulled);

    // Save the pointer
    SceneNode* rVal = ptr.get();

    // Move the unique pointer to node bank
    mNodes[rVal->GetUUID()] = std::move(ptr);

    // If it is light, add it to lights
    if(category == Category::Light)
        mLights.push_back(rVal);

    return rVal;
}

void Scene::DeleteNode(const std::string& uuid, bool deleteChildren /* = false */)
{
    SceneNode* node = FindNodeByUuid(uuid);

    // If node doesn't exist, do nothing
    if(node == nullptr)
        return;
    else
        DeleteNode(node, deleteChildren);
}

void Scene::DeleteNode(SceneNode* const node, bool deleteChildren /* = false */)
{
    // Erase it from lights vector if it is a light
    if(node->GetCategory() == Category::Light)
    {
        auto light = std::find_if(std::begin(mLights), std::end(mLights),
        [&node](SceneNode* lightNode) -> bool
        {
            return node->GetUUID().compare(lightNode->GetUUID()) == 0;
        });

        if(light != std::end(mLights))
            mLights.erase(light);
    }

    // Delete children if necessary
    if(deleteChildren)
        for(SceneNode* child : node->GetChildren())
            DeleteNode(child, deleteChildren);


    // Erase node
    auto nodeIt = mNodes.find(node->GetUUID());
    mNodes.erase(nodeIt);
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

void Scene::SetTransformation(const std::string& uuid, const glm::mat4& mat)
{
    SetTransformation(FindNodeByUuid(uuid), mat);
}

void Scene::SetTransformation(SceneNode* const node, const glm::mat4& mat)
{
    node->SetTransformation(mat);
}

void Scene::Move(const std::string& uuid, const glm::vec3& pos, bool moveChildren /* = false */)
{
    Move(FindNodeByUuid(uuid), pos, moveChildren);
}

void Scene::Move(SceneNode* const node, const glm::vec3& pos, bool moveChildren /* = false */)
{
    if(node == nullptr)
        return;
    else
        node->Move(pos, moveChildren);
}

void Scene::Rotate(const std::string& uuid, RotationAxis axis, float angle, bool rotateChildren /* = false */)
{
    Rotate(FindNodeByUuid(uuid), axis, angle, rotateChildren);
}

void Scene::Rotate(SceneNode* const node, RotationAxis axis, float angle, bool rotateChildren /* = false */)
{
    if(node == nullptr)
        return;
    else
        node->Rotate(axis, angle, rotateChildren);
}

void Scene::Scale(const std::string& uuid, const glm::vec3& scale, bool scaleChildren /* = false */)
{
    Scale(FindNodeByUuid(uuid), scale, scaleChildren);
}

void Scene::Scale(SceneNode* const node, const glm::vec3& scale, bool scaleChildren /* = false */)
{
    if(node == nullptr)
        return;
    else
        node->Scale(scale, scaleChildren);
}

const Scene::NodeBank& Scene::GetNodes() const
{
    return mNodes;
}

const Scene::PointLights& Scene::GetLights() const
{
    return mLights;
}

SceneNode* Scene::FindNodeByUuid(const std::string& uuid)
{
    auto it = mNodes.find(uuid);
    return (it == std::end(mNodes)) ? nullptr : it->second.get();
}
