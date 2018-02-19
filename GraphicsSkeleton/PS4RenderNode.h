#pragma once

#include "nclPS4Interface.h"

#include "PS4MeshNew.h"

namespace nclgl
{
	namespace Maths
	{
		class Matrix4;
		class Vector3;
		class Vector4;
	}
};

class PS4RenderNode
{
public:
	PS4RenderNode();
	~PS4RenderNode();

	 void	Update(float msec);
	 void	Draw() ;

	// Transforms
	 void							SetTransform(const nclgl::Maths::Matrix4 &matrix) ;
	 const nclgl::Maths::Matrix4&	GetTransform() const ;
	 const nclgl::Maths::Matrix4&	GetWorldTransform() const ;
	 void							SetWorldTransform(const nclgl::Maths::Matrix4 &matrix) ;

	 void							SetModelScale(const nclgl::Maths::Vector3& scale) ;
	 const nclgl::Maths::Vector3&	GetModelScale() const ;

	 nclgl::Maths::Vector4&			GetColour() ;
	 void							SetColour(const nclgl::Maths::Vector4 &c);

protected:
	sce::Vectormath::Scalar::Aos::Matrix4			worldTransform;
	sce::Vectormath::Scalar::Aos::Matrix4			transform;

	sce::Vectormath::Scalar::Aos::Vector4			colour;
	sce::Vectormath::Scalar::Aos::Vector3			modelScale;

	// Temp - should be a MeshBase* inherited from RenderNoseBase
	PS4MeshNew*	mesh;
};