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

	/*outMat.setCol0(sce::Vectormath::Scalar::Aos::Vector4(mat.values[0], mat.values[4], mat.values[8], mat.values[12]));
	outMat.setCol1(sce::Vectormath::Scalar::Aos::Vector4(mat.values[1], mat.values[5], mat.values[9], mat.values[13]));
	outMat.setCol2(sce::Vectormath::Scalar::Aos::Vector4(mat.values[2], mat.values[6], mat.values[10], mat.values[14]));
	outMat.setCol3(sce::Vectormath::Scalar::Aos::Vector4(mat.values[3], mat.values[7], mat.values[11], mat.values[15]));*/


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
			outMat.values[(row*4) + col] = mat.getElem(col, row);
		}
	}
	return outMat;
}

// Converts an nclgl Vector4 to a PS4 Vector4. 
sce::Vectormath::Scalar::Aos::Vector4 nclToPS4(const nclgl::Maths::Vector4& vec)
{
	sce::Vectormath::Scalar::Aos::Vector4 PSVec;
	PSVec[0] = vec.x;
	PSVec[1] = vec.y;
	PSVec[2] = vec.z;
	PSVec[3] = vec.w;

	return PSVec;
}

// Converts  a PS4 Vector4 to an nclgl Vector4
nclgl::Maths::Vector4 PS4ToNcl(const sce::Vectormath::Scalar::Aos::Vector4& vec)
{
	nclgl::Maths::Vector4 nclVec;
	nclVec.x = vec[0];
	nclVec.y = vec[1];
	nclVec.z = vec[2];
	nclVec.w = vec[3];

	return nclVec;
}


// Converts an nclgl Vector3 to a PS4 Vector3. 
sce::Vectormath::Scalar::Aos::Vector3 nclToPS4(const nclgl::Maths::Vector3& vec)
{
	sce::Vectormath::Scalar::Aos::Vector3 PSVec;
	PSVec[0] = vec.x;
	PSVec[1] = vec.y;
	PSVec[2] = vec.z;

	return PSVec;
}

// Converts  a PS4 Vector3 to an nclgl Vector3
nclgl::Maths::Vector3 PS4ToNcl(const sce::Vectormath::Scalar::Aos::Vector3& vec)
{
	nclgl::Maths::Vector3 nclVec;
	nclVec.x = vec[0];
	nclVec.y = vec[1];
	nclVec.z = vec[2];

	return nclVec;
}