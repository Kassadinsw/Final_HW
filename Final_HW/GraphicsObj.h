#ifndef _GRAPHICS_OBJ_H_
#define _GRAPHICS_OBJ_H_

#include "GL/glew.h"
#include "vec.h"
#include <vector>

class CGraphicsObjVertex
{
public:
	point3 position;
	vec3 normal;
	vec2 texcoord;
};

class CGraphicsObj
{
public:
	GLuint vertex_array_obj;
	GLuint vertex_buffer_obj;
	GLuint vertex_index_obj;

	GLenum primitive_type;
	int num_vertices;
	int num_indices;

	CGraphicsObj(void);
	
	void Draw(void);

protected:
	void CreateGLBuffers(
		CGraphicsObjVertex *p, GLuint *indices);

protected:
	void Divide3DTetra(CGraphicsObjVertex *v_out, int &ipos,
		const vec3& v0_in, const vec3& v1_in,
		const vec3& v2_in, const vec3& v3_in,
		int depth);

public:
	void CreateGasket3D(
		const vec3 tetra_vertices[4], int depth);

	void CreateCube(float size, 
		float tex_nx, float tex_ny, float tex_nz);

	void CreateRect(float sx, float sy, int nx, int ny,
		float tex_nx, float tex_ny);

	void CreateSphere(float radius, int n_theta, int n_phi,
		float tex_ntheta, float tex_nphi);
	
	void CreateUtah(const char *file_name,int depth);

	void CreateCylinder(float radius, float height, int seg_count, float tex_nx, float tex_ny);

	void CreateCone(float radius, float height, int seg_count, float tex_nx, float tex_ny);

protected:
	void TesselateBezierPatches(vec3 cur_patch[4][4], int depth, CGraphicsObjVertex *p, int &buffer_index);
	void DivideBezierCurves(vec3 c[4], vec3 r[4], vec3 l[4]);
	void TransposeControlPoints(vec3 a[4][4]);

};

#endif
