#include "nclPS4Interface.h"

// Converts an nclgl matrix to a PS4 matrix. 
sce::Vectormath::Scalar::Aos::Matrix4 nclToPS4(const nclgl::Maths::Matrix4& mat)
{
	// PS4 matrix is column major but nclgl matrix is row major
	sce::Vectormath::Scalar::Aos::Matrix4 outMat;
	outMat.setCol0(sce::Vectormath::Scalar::Aos::Vector4(mat.values[0], mat.values[1], mat.values[2], mat.values[3]));
	outMat.setCol1(sce::Vectormath::Scalar::Aos::Vector4(mat.values[4], mat.values[5], mat.values[6], mat.values[7]));
	outMat.setCol2(sce::Vectormath::Scalar::Aos::Vector4(mat.values[8], mat.values[9], mat.values[10], mat.values[11]));
	outMat.setCol3(sce::Vectormath::Scalar::Aos::Vector4(mat.values[12], mat.values[13], mat.values[14], mat.values[15]));

	return outMat;
}

// Converts  a PS4 matrix to an nclgl matrix
nclgl::Maths::Matrix4 PS4ToNcl(const sce::Vectormath::Scalar::Aos::Matrix4& mat)
{
	// PS4 vector is column vector but nclgl vector is row major
	nclgl::Maths::Matrix4 outMat;
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			outMat.values[row + col] = mat.getElem(row, col);
		}
	}

	return outMat;
}