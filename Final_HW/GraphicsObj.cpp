#include <stdlib.h>
#include <stddef.h>
#include "GraphicsObj.h"
#include "vec.h"
#include <stdio.h>
#include <string.h>


using namespace std;

CGraphicsObj::CGraphicsObj(void)
{
	vertex_array_obj=0;
	vertex_buffer_obj=0;
	vertex_index_obj=0;
	num_vertices=0;
	num_indices=0;
	primitive_type=GL_TRIANGLES;
}

void CGraphicsObj::Draw(void)
{
	glBindVertexArray(vertex_array_obj);
	if (vertex_index_obj==0)
		glDrawArrays(primitive_type, 0, num_vertices);
	else
		glDrawElements(primitive_type, num_indices,
			GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}

void CGraphicsObj::CreateGLBuffers(
	CGraphicsObjVertex *p, GLuint *indices)
{
	glGenVertexArrays(1, &vertex_array_obj);
	glBindVertexArray(vertex_array_obj);

	glGenBuffers(1, &vertex_buffer_obj);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_obj);
	glBufferData(GL_ARRAY_BUFFER,
		num_vertices*sizeof(CGraphicsObjVertex), p,
		GL_STATIC_DRAW);

	if (indices!=NULL)
	{
		glGenBuffers(1, &vertex_index_obj);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_index_obj);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			num_indices*sizeof(GLuint), indices,
			GL_STATIC_DRAW);
		delete [] indices;
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3, GL_FLOAT, GL_FALSE, sizeof(CGraphicsObjVertex), 
		(GLvoid *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		3, GL_FLOAT, GL_FALSE, sizeof(CGraphicsObjVertex), 
		(GLvoid *)offsetof(CGraphicsObjVertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,
		2, GL_FLOAT, GL_FALSE, sizeof(CGraphicsObjVertex), 
		(GLvoid *)offsetof(CGraphicsObjVertex, texcoord));

	glBindVertexArray(0);

	delete [] p;
}

void CGraphicsObj::Divide3DTetra(
	CGraphicsObjVertex *v_out, int &ipos,
	const vec3& v0_in, const vec3& v1_in,
	const vec3& v2_in, const vec3& v3_in,
	int depth)
{
	int i, j;
	vec3 mid[6], N;
	if (depth>0)
	{
		mid[0]=0.5f*(v0_in+v3_in);
		mid[1]=0.5f*(v1_in+v3_in);
		mid[2]=0.5f*(v2_in+v3_in);
		mid[3]=0.5f*(v1_in+v2_in);
		mid[4]=0.5f*(v2_in+v0_in);
		mid[5]=0.5f*(v0_in+v1_in);

		Divide3DTetra(v_out, ipos,
			v0_in, mid[5], mid[4], mid[0], depth-1);
		Divide3DTetra(v_out, ipos,
			v1_in, mid[3], mid[5], mid[1], depth-1);
		Divide3DTetra(v_out, ipos,
			v2_in, mid[4], mid[3], mid[2], depth-1);
		Divide3DTetra(v_out, ipos,
			mid[0], mid[1], mid[2], v3_in, depth-1);
	}
	else
	{
		const vec3 *pv[4]={&v0_in, &v1_in, &v2_in, &v3_in};
		const int iv_faces[4][3]={
			{3,0,1},{3,1,2},{3,2,0},{0,2,1}
		};
		for (i=0; i<4; ++i)
		{
			N=TriangleNormal(
				*pv[iv_faces[i][0]],
				*pv[iv_faces[i][1]],
				*pv[iv_faces[i][2]]);
			for (j=0; j<3; ++j)
			{
				v_out[ipos].position=*pv[iv_faces[i][j]];
				v_out[ipos].normal=N;

				ipos++;
			}
		}
	}
}

void CGraphicsObj::CreateGasket3D(
	const vec3 tetra_vertices[4], int depth)
{
	primitive_type=GL_TRIANGLES;

	num_vertices=12;
	int i;
	for (i=0; i<depth; ++i)
		num_vertices*=4;

	CGraphicsObjVertex *p=new CGraphicsObjVertex [num_vertices];

	i=0;
	Divide3DTetra(p, i, 
		tetra_vertices[0], tetra_vertices[1], 
		tetra_vertices[2], tetra_vertices[3],
		depth);

	CreateGLBuffers(p, NULL);
}

void CGraphicsObj::CreateCube(float size,
	float tex_nx, float tex_ny, float tex_nz)
{
	primitive_type=GL_TRIANGLES;

	num_vertices=24;
	num_indices=36;

	CGraphicsObjVertex *p=new CGraphicsObjVertex [num_vertices];
	GLuint *indices=new GLuint [num_indices];

	static const point3 vpos[8]={
		point3(-1.0f, -1.0f, -1.0f),
		point3( 1.0f, -1.0f, -1.0f),
		point3( 1.0f,  1.0f, -1.0f),
		point3(-1.0f,  1.0f, -1.0f),
		point3(-1.0f, -1.0f,  1.0f),
		point3( 1.0f, -1.0f,  1.0f),
		point3( 1.0f,  1.0f,  1.0f),
		point3(-1.0f,  1.0f,  1.0f)
	};

	static const int face_iv[6][4]={
		{0,4,7,3},
		{1,2,6,5},
		{0,1,5,4},
		{3,7,6,2},
		{0,3,2,1},
		{4,5,6,7}
	};

	static const int face_xy_id[6][2]={
		{2, 1},
		{1, 2},
		{0, 2},
		{2, 0},
		{1, 0},
		{0, 1}
	};

	static const vec3 face_N[6]={
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.0f, 1.0f)
	};

	float tex_n[3]={tex_nx, tex_ny, tex_nz};

	float s_half=0.5f*size;
	int i, j, k;
	for (i=0, k=0; i<6; ++i)
	{
		p[k].texcoord=vec2(0.0f, 0.0f);
		p[k+1].texcoord=vec2(tex_n[face_xy_id[i][0]], 0.0f);
		p[k+2].texcoord=vec2(
			tex_n[face_xy_id[i][0]],
			tex_n[face_xy_id[i][1]]);
		p[k+3].texcoord=vec2(0.0f, tex_n[face_xy_id[i][1]]);
		for (j=0; j<4; ++j, ++k)
		{
			p[k].position=vpos[face_iv[i][j]]*s_half;
			p[k].normal=face_N[i];
		}
	}

	for (i=0, k=0; i<6; ++i)
	{
		j=i*4;
		indices[k++]=j;
		indices[k++]=j+1;
		indices[k++]=j+2;
		indices[k++]=j;
		indices[k++]=j+2;
		indices[k++]=j+3;
	}

	CreateGLBuffers(p, indices);
}

void CGraphicsObj::CreateRect(
	float sx, float sy, int nx, int ny, float tex_nx, float tex_ny)
{
	primitive_type=GL_TRIANGLES;

	num_vertices=(nx+1)*(ny+1);
	num_indices=6*nx*ny;
	float dx=sx/nx;
	float dy=sy/ny;
	float tex_dx=tex_nx/nx;
	float tex_dy=tex_ny/ny;

	CGraphicsObjVertex *p=new CGraphicsObjVertex [num_vertices];
	GLuint *indices=new GLuint [num_indices];

	int i, j, k;
	for (k=0, j=0; j<=ny; ++j)
	{
		float y=-0.5f*sy+j*dy;
		float texcoord_t=j*tex_dy;
		for (i=0; i<=nx; ++i, ++k)
		{
			p[k].position=point3(-0.5f*sx+i*dx, y, 0.0f);
			p[k].normal=vec3(0.0f, 0.0f, 1.0f);
			p[k].texcoord.x=i*tex_dx;
			p[k].texcoord.y=texcoord_t;
		}
	}

	k=0;
	int t[4];
	for (j=0; j<ny; ++j)
	{
		for (i=0; i<nx; ++i)
		{
			t[0]=j*(nx+1)+i;
			t[1]=t[0]+1;
			t[3]=t[0]+nx+1;
			t[2]=t[3]+1;

			if (i%2==j%2)
			{
				indices[k++]=t[0];
				indices[k++]=t[1];
				indices[k++]=t[2];
				indices[k++]=t[0];
				indices[k++]=t[2];
				indices[k++]=t[3];
			}
			else
			{
				indices[k++]=t[0];
				indices[k++]=t[1];
				indices[k++]=t[3];
				indices[k++]=t[1];
				indices[k++]=t[2];
				indices[k++]=t[3];
			}
		}
	}

	CreateGLBuffers(p, indices);
}

//生成圆球，做星球
void CGraphicsObj::CreateSphere(
	float radius, int n_theta, int n_phi,
	float tex_ntheta, float tex_nphi)
{
	primitive_type=GL_TRIANGLES;

	num_vertices=(n_theta+1)*(n_phi+1);
	num_indices=6*n_theta*n_phi;
	float dtheta=(M_PI+M_PI)/n_theta;
	float dphi=M_PI/n_phi;

	CGraphicsObjVertex *p=new CGraphicsObjVertex [num_vertices];
	GLuint *indices=new GLuint [num_indices];

	int i, j, k;
	for (k=0, j=0; j<=n_phi; ++j)
	{
		float phi=-0.5f*M_PI+j*dphi;
		float cphi=cos(phi);
		float sphi=sin(phi);
		float texcoord_t=(float)j/n_phi*tex_nphi;
		for (i=0; i<=n_theta; ++i, ++k)
		{
			float theta=i*dtheta;
			float ctheta=cos(theta);
			float stheta=sin(theta);
			p[k].normal=vec3(
				cphi*ctheta, cphi*stheta, sphi);
			p[k].position=radius*p[k].normal;
			p[k].texcoord.x=(float)i/n_theta*tex_ntheta;
			p[k].texcoord.y=texcoord_t;
		}
	}

	k=0;
	int t[4];
	for (j=0; j<n_phi; ++j)
	{
		for (i=0; i<n_theta; ++i)
		{
			t[0]=j*(n_theta+1)+i;
			t[1]=t[0]+1;
			t[3]=t[0]+n_theta+1;
			t[2]=t[3]+1;

			if (i%2==j%2)
			{
				indices[k++]=t[0];
				indices[k++]=t[1];
				indices[k++]=t[2];
				indices[k++]=t[0];
				indices[k++]=t[2];
				indices[k++]=t[3];
			}
			else
			{
				indices[k++]=t[0];
				indices[k++]=t[1];
				indices[k++]=t[3];
				indices[k++]=t[1];
				indices[k++]=t[2];
				indices[k++]=t[3];
			}
		}
	}

	CreateGLBuffers(p, indices);
}


//生成圆柱
void CGraphicsObj::CreateCylinder(float radius, float height, int seg_count, float tex_nx, float tex_ny)
{

	primitive_type = GL_TRIANGLES;
	int nx = seg_count + 1;

	num_vertices = 4 * nx + 2;
	num_indices = 12 * seg_count;

	CGraphicsObjVertex *p = new CGraphicsObjVertex[num_vertices];
	GLuint *indices = new GLuint[num_indices];

	p[0].position = point3(0.0f, 0.0f, -0.5f * height);
	p[0].normal = vec3(0.0f, 0.0f, -1.0f);
	p[0].texcoord = vec2(0.0f, 0.0f);

	p[1].position = point3(0.0f, 0.0f, 0.5f * height);
	p[1].normal = vec3(0.0f, 0.0f, 1.0f);
	p[1].texcoord = vec2(1.0f, 1.0f);

	int i, j, k = 2;

	//底面
	float z = -0.5f * height;
	float texcoord_t = radius / (height + 2 * radius) * tex_ny;
	for (i = 0; i <= seg_count; ++i, ++k)
	{
		float theta = (float)(i * M_PI * 2) / seg_count;
		float _sin = sinf(theta);
		float _cos = cosf(theta);
		p[k].position = point3(_cos * radius, _sin * radius, z);
		p[k].normal = vec3(0.0f, 0.0f, -1.0f);
		p[k].texcoord.x = (float)i / (float)seg_count * tex_nx;
		p[k].texcoord.y = texcoord_t;
	}

	//侧面
	for (j = 0; j <= 1; ++j)
	{
		float z = (j - 0.5f) * height;
		float texcoord_t = (height * j + radius) / (height + 2 * radius) * tex_ny;
		for (i = 0; i <= seg_count; ++i, ++k)
		{
			float theta = (float)(i * M_PI * 2) / seg_count;
			float _sin = sinf(theta);
			float _cos = cosf(theta);
			p[k].position = point3(_cos * radius, _sin * radius, z);
			p[k].normal = vec3(_cos, _sin, 0.0f);
			p[k].texcoord.x = (float)i / (float)seg_count * tex_nx;
			p[k].texcoord.y = texcoord_t;
		}
	}

	//顶面
	z = 0.5f * height;
	texcoord_t = (height + radius) / (height + 2 * radius) * tex_ny;
	for (i = 0; i <= seg_count; ++i, ++k)
	{
		float theta = (float)(i * M_PI * 2) / seg_count;
		float _sin = sinf(theta);
		float _cos = cosf(theta);
		p[k].position = point3(_cos * radius, _sin * radius, z);
		p[k].normal = vec3(0.0f, 0.0f, 1.0f);
		p[k].texcoord.x = (float)i / (float)seg_count * tex_nx;
		p[k].texcoord.y = texcoord_t;
	}
	k = 0;

	for (i = 0; i < seg_count; ++i)
	{
		indices[k++] = 0;
		indices[k++] = i + 3;
		indices[k++] = i + 2;

		indices[k++] = i + 2 + nx;
		indices[k++] = i + 3 + nx;
		indices[k++] = i + 3 + nx * 2;
	}

	for (i = 0; i < seg_count; ++i)
	{
		indices[k++] = i + 2 + nx;
		indices[k++] = i + 3 + nx * 2;
		indices[k++] = i + 2 + nx * 2;

		indices[k++] = 1;
		indices[k++] = i + 2 + nx * 3;
		indices[k++] = i + 3 + nx * 3;
	}

	CreateGLBuffers(p, indices);
}


//生成圆锥
void CGraphicsObj::CreateCone(float radius, float height, int seg_count, float tex_nx, float tex_ny)
{

	primitive_type = GL_TRIANGLES;
	int nx = seg_count + 1;
	num_vertices = 2 * nx + 2;
	num_indices = 6 * seg_count;

	CGraphicsObjVertex *p = new CGraphicsObjVertex[num_vertices];
	GLuint *indices = new GLuint[num_indices];

	p[0].position = point3(0.0f, 0.0f, -0.5f * height);
	p[0].normal = vec3(0.0f, 0.0f, -1.0f);
	p[0].texcoord = vec2(0.0f, 0.0f);

	p[1].position = point3(0.0f, 0.0f, 0.5f * height);
	p[1].normal = vec3(0.0f, 0.0f, 0.0f);
	p[1].texcoord = vec2(1.0f, 1.0f);

	//底面
	int i, k = 2;
	for (i = 0; i <= seg_count; ++i, ++k)
	{
		float theta = (float)(i * M_PI * 2) / seg_count;
		float _sin = sinf(theta);
		float _cos = cosf(theta);
		p[k].position = point3(_cos * radius, _sin * radius, -0.5f * height);
		p[k].normal = vec3(0.0f, 0.0f, -1.0f);
		p[k].texcoord.x = (float)i / (float)seg_count * tex_nx;
		p[k].texcoord.y = radius * tex_ny / (radius + height);
	}

	//锥面
	for (i = 0; i <= seg_count; ++i, ++k)
	{
		float theta = (float)(i * M_PI * 2) / seg_count;
		float _sin = sinf(theta);
		float _cos = cosf(theta);
		p[k].position = point3(_cos * radius, _sin * radius, -0.5f * height);
		p[k].normal = vec3(_cos, _sin, radius / height);
		p[k].texcoord.x = (float)i / (float)seg_count * tex_nx;
		p[k].texcoord.y = radius * tex_ny / (radius + height);
	}

	k = 0;

	for (i = 0; i < seg_count; ++i)
	{
		indices[k++] = 0;
		indices[k++] = i + 3;
		indices[k++] = i + 2;

		indices[k++] = i + 2 + nx;
		indices[k++] = i + 3 + nx;
		indices[k++] = 1;
	}

	CreateGLBuffers(p, indices);
}



//生成犹他壶 v_strip为两顶点间步长，per_count为每两个点间生成点的个数
void CGraphicsObj::CreateUtah(const char *file_name,int depth) {
	char data[100];
	//控制点
	vector<vec3> position_data;
	//索引
	vector<int> point_index;
	//控制深度
	//int depth = 4;
	//控制循环次数
	int	num_index = 32;
	

	//获取到控制点和索引值
	FILE *fp = fopen(file_name, "r"); 
	if (!fp)
	{ 
		printf("can't open file\n"); 
	
	}
	int q = 0;
	while (!feof(fp))
	{
		fscanf(fp, "%s", &data);  
		//分割字符串
		const char * split = ",";
		char *p = strtok(data, split);
		float every_data[3];
		int m = 0;
		if (q != 0 && q < 307) {
			while (p != NULL)
			{
				//char *转浮点
				every_data[m] = atof(p);
				p = strtok(NULL, split);
				m += 1;
			}
			vec3 every_data_vec = vec3(every_data[0], every_data[1], every_data[2]);
			position_data.push_back(every_data_vec);
		}
		else if (q > 307 ) {
			while (p != NULL)
			{
				//char* 转整型
				int num = atoi(p);
				point_index.push_back(num);
				p = strtok(NULL, split);
			}
		}
		q++;
	}
	//printf("\n"); 
	/*
	for (int i = 0; i < 306;i++) {
		printf("%f=%f=%f\n",position_data[i].x, position_data[i].y, position_data[i].z);
	}
	for (int j = 0; j < 512; j++) {
	
		printf("%d\n", point_index[j]);
	}*/

	fclose(fp);


	num_vertices = 6 * num_index * pow(4, (double)depth);
	num_indices = 0;

	CGraphicsObjVertex *p = new CGraphicsObjVertex[num_vertices];

	GLuint *indices = NULL;//不用索引，所以设定为NULL

	int buffer_index = 0;
	int index_num = 0;

	//32个面，每个面16个控制点，存储在4x4矩阵里
	for (int i = 0; i < num_index; i++) {
		vec3 cur_patch[4][4];//存储16个控制点
		//索引取坐标
		int p_index_temp;
		for (int ii = 0; ii < 4; ii++)
		{
			for (int jj = 0; jj < 4; jj++)
			{
				
				p_index_temp = point_index[index_num];
				//printf("%d=%d\n", index_num,point_index[index_num]);
				cur_patch[ii][jj] = position_data[p_index_temp-1];
				index_num++;

			}

		}

		//每个面递归进行细分
		TesselateBezierPatches(cur_patch, depth, p, buffer_index);
		printf("OK");
	
	}

	CreateGLBuffers(p, indices);//索引并没有用


}



//递归细分，32个面，每个面16个控制点，存储在4x4的矩阵
void CGraphicsObj::TesselateBezierPatches(vec3 cur_patch[4][4], int depth, CGraphicsObjVertex *p, int &buffer_index)
{
	if (depth > 0)
	{
		vec3 q[4][4], r[4][4], s[4][4], t[4][4];
		vec3 a[4][4], b[4][4];
		for (int k = 0; k < 4; k++)
		{
			DivideBezierCurves(cur_patch[k], a[k], b[k]);
		}
		TransposeControlPoints(a);
		TransposeControlPoints(b);

		for (int k = 0; k < 4; k++)
		{
			DivideBezierCurves(a[k], q[k], r[k]);
			DivideBezierCurves(b[k], s[k], t[k]);
		}
		TesselateBezierPatches(q, depth - 1, p, buffer_index);
		TesselateBezierPatches(r, depth - 1, p, buffer_index);
		TesselateBezierPatches(s, depth - 1, p, buffer_index);
		TesselateBezierPatches(t, depth - 1, p, buffer_index);
	}
	else
	{
		//算法线
		p[buffer_index].normal = normalize(cross(cur_patch[0][3] - cur_patch[0][0], cur_patch[3][0] - cur_patch[0][0]));
		p[buffer_index++].position = cur_patch[0][0];

		p[buffer_index].normal = normalize(cross(cur_patch[0][0] - cur_patch[3][0], cur_patch[3][3] - cur_patch[3][0]));
		p[buffer_index++].position = cur_patch[3][0];

		p[buffer_index].normal = normalize(cross(cur_patch[3][0] - cur_patch[3][3], cur_patch[0][3] - cur_patch[3][3]));
		p[buffer_index++].position = cur_patch[3][3];

		p[buffer_index].normal = normalize(cross(cur_patch[0][3] - cur_patch[0][0], cur_patch[3][0] - cur_patch[0][0]));
		p[buffer_index++].position = cur_patch[0][0];

		p[buffer_index].normal = normalize(cross(cur_patch[3][0] - cur_patch[3][3], cur_patch[0][3] - cur_patch[3][3]));
		p[buffer_index++].position = cur_patch[3][3];

		p[buffer_index].normal = normalize(cross(cur_patch[3][3] - cur_patch[0][3], cur_patch[0][0] - cur_patch[0][3]));
		p[buffer_index++].position = cur_patch[0][3];
	}
}

//细分函数
void CGraphicsObj::DivideBezierCurves(vec3 c[4], vec3 r[4], vec3 l[4])
{
	vec3 t, mid = (c[1] + c[2]) / 2;

	l[0] = c[0];
	l[1] = (c[0] + c[1]) / 2;
	l[2] = (l[1] + mid) / 2;

	r[3] = c[3];
	r[2] = (c[2] + c[3]) / 2;
	r[1] = (mid + r[2]) / 2;

	l[3] = r[0] = (l[2] + r[1]) / 2;
}

//控制点
void CGraphicsObj::TransposeControlPoints(vec3 a[4][4])
{
	for (int i = 0; i < 4; i++)
		for (int j = i; j < 4; j++)
		{
			vec3 trans_temp = a[i][j];
			a[i][j] = a[j][i];
			a[j][i] = trans_temp;
		}
}



