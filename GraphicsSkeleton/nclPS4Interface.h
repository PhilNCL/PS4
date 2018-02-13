#pragma once

#include <gnmx.h>
#include <graphics\api_gnm\toolkit\geommath\geommath.h>
#include "Matrix4.h"

// Converts an nclgl matrix to a PS4 matrix. 
sce::Vectormath::Scalar::Aos::Matrix4 nclToPS4(const nclgl::Maths::Matrix4& mat);

// Converts  a PS4 matrix to an nclgl matrix
nclgl::Maths::Matrix4 PS4ToNcl(const sce::Vectormath::Scalar::Aos::Matrix4& mat);