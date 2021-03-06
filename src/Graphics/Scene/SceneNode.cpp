#include "SceneNode.hpp"
#include <algorithm>

//--------------------------------------------------
// Public functions
//--------------------------------------------------
SceneNode::SceneNode(
    const std::string& model,
    const std::vector<std::string>& materials,
    const std::string& uuid,
    Category category,
    AABB localAABB,
    bool isCulled /* = false */,
    SceneNode* parent /* = nullptr */)
    : mModel(model)
    , mMaterials(materials)
    , mCategory(category)
    , mUuid(uuid)
    , mAABB(localAABB)
    , mCulled(isCulled)
    , mParent(parent)
{
}

void SceneNode::AddChild(SceneNode* const child)
{
    // If child exists don't do anything
    if(FindChildByUuid(child->mUuid) != std::end(mChildren))
        return;

    // Add child to list
    mChildren.push_back(child);

    // Add a parent to newly added child
    child->mParent = this;
}

void SceneNode::RemoveChild(const std::string& uuid)
{
    // Find the child with the given uuid
    auto it = FindChildByUuid(uuid);

    // Return if child doesn't exist
    if(it == std::end(mChildren))
        return;

    // Remove child
    mChildren.erase(it);

    // Make the child orphan
    (*it)->mParent = nullptr;
}

void SceneNode::Move(const glm::vec3& pos, bool moveChildren /* = false */)
{
    // Move this node
    mTransform.Move(pos);

    // Move children (if necessary)
    if(moveChildren)
        MoveChildren(pos);
}

void SceneNode::Rotate(RotationAxis axis, float angle, bool rotateChildren /* = false */)
{
    // Rotate this node
    switch(axis)
    {
        case RotationAxis::X:
            mTransform.RotateX(angle);
            break;
        case  RotationAxis::Y:
            mTransform.RotateY(angle);
            break;
        case RotationAxis::Z:
            mTransform.RotateZ(angle);
            break;
    }

    // Rotate children (if necessary)
    if(rotateChildren)
        RotateChildren(axis, angle);
}

void SceneNode::Scale(const glm::vec3& scale, bool scaleChildren /* = false */)
{
    // Scale this node
    mTransform.Scale(scale);

    // Scale children (if necessary)
    if(scaleChildren)
        ScaleChildren(scale);
}

const std::string& SceneNode::GetModel() const
{
    return mModel;
}

const std::vector<std::string>& SceneNode::GetMaterials() const
{
    return mMaterials;
}

Transform& SceneNode::GetTransformation()
{
    return mTransform;
}

Category SceneNode::GetCategory() const
{
    return mCategory;
}

const std::string& SceneNode::GetUUID() const
{
    return mUuid;
}

AABB& SceneNode::GetAABB()
{
    return mAABB;
}

bool SceneNode::IsCulled() const
{
    return mCulled;
}

const SceneNode::ChildrenList& SceneNode::GetChildren() const
{
    return mChildren;
}

//--------------------------------------------------
// Private functions
//--------------------------------------------------
void SceneNode::MoveChildren(const glm::vec3& pos)
{
    for(SceneNode* child : mChildren)
        child->Move(pos, true);
}

void SceneNode::RotateChildren(RotationAxis axis, float angle)
{
    for(SceneNode* child : mChildren)
        child->Rotate(axis, angle, true);
}

void SceneNode::ScaleChildren(const glm::vec3& scale)
{
    for(SceneNode* child : mChildren)
        child->Scale(scale, true);
}

SceneNode::ChildrenList::iterator SceneNode::FindChildByUuid(const std::string& uuid)
{
    return std::find_if(std::begin(mChildren),
              std::end(mChildren),
              [&uuid](SceneNode* child) -> bool
              {
                  if(child == nullptr)
                      return false;

                  return child->mUuid.compare(uuid) == 0;
              }
    );
}
