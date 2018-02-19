#include "PS4RenderNode.h"



PS4RenderNode::PS4RenderNode()
{
}


PS4RenderNode::~PS4RenderNode()
{
}

void	PS4RenderNode::Update(float msec)
{
	//RenderNodeBase::Update(msec);
}

void	PS4RenderNode::Draw()
{
	if (mesh)
	{
		mesh->Draw();
	}
}

void							PS4RenderNode::SetTransform(const nclgl::Maths::Matrix4 &matrix)
{
	transform = nclToPS4(matrix);
}

const nclgl::Maths::Matrix4&	PS4RenderNode::GetTransform() const
{
	return PS4ToNcl(transform);
}

const nclgl::Maths::Matrix4&	PS4RenderNode::GetWorldTransform() const
{
	return PS4ToNcl(worldTransform);
}

void							PS4RenderNode::SetWorldTransform(const nclgl::Maths::Matrix4 &matrix)
{
	worldTransform = nclToPS4(matrix);
}
					
void							PS4RenderNode::SetModelScale(const nclgl::Maths::Vector3& scale)
{
	modelScale = nclToPS4(scale);
}

const nclgl::Maths::Vector3&	PS4RenderNode::GetModelScale() const
{

}
						
nclgl::Maths::Vector4&			PS4RenderNode::GetColour()
{
	//return PS4ToNcl(colour);
}

void							PS4RenderNode::SetColour(const nclgl::Maths::Vector4 &c)
{
	colour = nclToPS4(c);
}