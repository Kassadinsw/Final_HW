#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "mat.h"

class CCamera
{
protected:
	float theta, phi;
	vec3 u0, v0, n0;
	vec3 n1;

	void CalculateVectors(void);

public:
	vec3 u, v, n;
	vec3 P0;

	CCamera(void);

	void Init(const vec3& P0_in,
		const vec3& backward_dir,
		const vec3& up_dir);

	void GetViewMatrix(mat4& M);

	void TurnLeft(float angle);
	void LookUp(float angle);
	void MoveForward(float step);
	void MoveUp(float step);
	void MoveLeft(float step);
};

#endif
