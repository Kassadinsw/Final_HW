#include "Camera.h"

void CCamera::CalculateVectors(void)
{
	float a, c, s;
	a=theta*DegreesToRadians;
	c=cos(a);
	s=sin(a);

	vec3 u1;
	u1=u0*c-n0*s;
	n1=u0*s+n0*c;

	a=phi*DegreesToRadians;
	c=cos(a);
	s=sin(a);

	u=u1;
	v=v0*c+n1*s;
	n=-v0*s+n1*c;
}

CCamera::CCamera(void)
{
	theta=0.0f;
	phi=0.0f;
}

void CCamera::Init(const vec3& P0_in,
	const vec3& backward_dir,
	const vec3& up_dir)
{
	P0=P0_in;
	u0=normalize(cross(up_dir, backward_dir));
	n0=normalize(backward_dir);
	v0=cross(n0, u0);
	CalculateVectors();
}

void CCamera::GetViewMatrix(mat4& M)
{
    vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
    mat4 c = mat4(vec4(u, 0.0f), vec4(v, 0.0f), vec4(n, 0.0f), t);
	M = c * Translate( -P0 );
}

void CCamera::TurnLeft(float angle)
{
	theta=fmod(theta+angle, 360.0f);
	CalculateVectors();
}

void CCamera::LookUp(float angle)
{
	phi=fmod(phi+angle, 360.0f);
	CalculateVectors();
}

void CCamera::MoveForward(float step)
{
	P0-=step*n1;
}

void CCamera::MoveUp(float step)
{
	P0+=step*v0;
}

void CCamera::MoveLeft(float step)
{
	P0-=step*u;
}
