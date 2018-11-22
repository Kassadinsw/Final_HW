#include <stdio.h>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GraphicsObj.h"
#include "GLHelper.h"
#include "mat.h"
#include "Camera.h"
#include "ImageLib.h"
#include <stack>
#include "vec.h"




class CModel3D
{
public:
	CGraphicsObj mesh;
	mat4 model_matrix;
	vec4 diffuse_color;
	GLuint texture;
};

CModel3D g_models;


class CSceneNode
{
public:
	CGraphicsObj *p_mesh;
	mat4 model_matrix;
	vec4 diffuse_color;
	GLuint texture;

	mat4 from_parent_matrix;
	mat4 instance_matrix;
	CSceneNode *p_child;
	CSceneNode *p_sibling;

	CSceneNode(void)
	{
		diffuse_color=vec4(1.0f);
	}
};

std::stack <mat4> g_matrix_stack;


enum {
	NODE_GROUND=0,
	NODE_SUN,
	NODE_EARTH,
	NODE_MOON,
	NODE_JUPITER,
	NODE_Uranus,//天王星
	NUM_NODES
};

enum {
	OBJ_RECT = 0,
	OBJ_CUBE,
	OBJ_SPHERE,
	NUM_OBJS
};

enum {
	JOINT_GROUND_TO_SUN=0,
	JOINT_SUN_TO_EARTH,
	JOINT_EARTH_TO_MOON,
	JOINT_EARTH_TO_JUPITER,
	JOINT_EARTH_TO_Uranus,
	NUM_JOINTS
};


CSceneNode g_scene_nodes[NUM_NODES];
CGraphicsObj g_scene_objects[NUM_OBJS];

float g_joint_angles[NUM_JOINTS]={0.0f, 0.0f, 0.0f, 0.0f,0.0f};

#define BASE_WIDTH 0.5f
#define BASE_HEIGHT 1.5f
#define LOWER_ARM_WIDTH 0.2f
#define LOWER_ARM_HEIGHT 1.0f
#define UPPER_ARM_WIDTH 0.1f
#define UPPER_ARM_HEIGHT 0.8f
#define ARM_DISTANCE 0.3f

GLuint g_simple_GLSL_prog;

int g_window_width, g_window_height;

CCamera g_camera;
float g_camera_move_step=0.1f;
bool g_camera_rotation_mode=false;
int g_mouse_pos[2];

float g_FOV=60.0f;

float g_rot_angle=0.0f;
float g_rot_angle_1 = 0.0f;

float g_current_time;

int g_lights_enable[3]={1, 1, 0};

GLuint g_main_texture;

void AnimateRobot(void);

void TraverseSceneNode(CSceneNode *p_node, mat4& current_matrix);


GLuint CreateCheckerboardTexture(void)
{
	GLubyte my_image[64][64][3];
	int i, j;
	for (j=0; j<64; ++j)
	{
		for (i=0; i<64; ++i)
		{
			if ((i/16)%2==(j/16)%2)
			{
				my_image[i][j][0]=255;
				my_image[i][j][1]=0;
				my_image[i][j][2]=0;
			}
			else
			{
				my_image[i][j][0]=255;
				my_image[i][j][1]=255;
				my_image[i][j][2]=255;
			}
		}
	}

	GLuint itex;
	glGenTextures(1, &itex);
	glBindTexture(GL_TEXTURE_2D, itex);
	glTexImage2D(GL_TEXTURE_2D, 0,
		GL_RGB, 64, 64, 0,
		GL_RGB, GL_UNSIGNED_BYTE, my_image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	float border_color[4]={0.0f, 0.0f, 1.0f, 1.0f};
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 
//		border_color);
	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, 
		GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return itex;
}

void init_shaders(void)
{
	g_simple_GLSL_prog=InitShader(
		//"..\\shaders\\single_light-vs.txt",
		//"..\\shaders\\single_light-fs.txt"
		"..\\shaders\\multi_lights-multi_tex-vs.txt",
		"..\\shaders\\multi_lights-multi_tex-fs.txt"
		);
	glUseProgram(g_simple_GLSL_prog);

	int loc;
	mat4 M;
	loc=glGetUniformLocation(g_simple_GLSL_prog, "global_ambient_light");
	glUniform4f(loc, 0.2f, 0.2f, 0.2f, 1.0f);

	loc=glGetUniformLocation(g_simple_GLSL_prog, "specular_reflectivity");
	glUniform4f(loc, 0.8f, 0.8f, 0.8f, 1.0f);

	loc=glGetUniformLocation(g_simple_GLSL_prog, "shininess");
	glUniform1f(loc, 128.0f);

	loc=glGetUniformLocation(g_simple_GLSL_prog, "tex_map");
	glUniform1i(loc, 0);
	loc=glGetUniformLocation(g_simple_GLSL_prog, "detail_map");
	glUniform1i(loc, 1);

	loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[0].enable");
	glUniform1i(loc, g_lights_enable[0]);
	loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[0].position");
	glUniform4f(loc, 1.0f, 1.5f, 2.0f, 0.0f);
	loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[0].color");
	glUniform4f(loc, 1.0f, 1.0f, 1.0f, 1.0f);

	loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[1].enable");
	glUniform1i(loc, g_lights_enable[1]);
	loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[1].attenuation");
	glUniform3f(loc, 1.0f, 0.0f, 0.01f);
	loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[1].color");
	glUniform4f(loc, 0.0f, 1.0f, 0.0f, 1.0f);

	loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[2].enable");
	glUniform1i(loc, g_lights_enable[2]);
}




void init_models(void)
{
	

	//生成犹他茶壶 第二个参数是细分递归层数，太卡可以设小点
	g_models.mesh.CreateUtah(".\\teapot.txt", 6);

	g_models.diffuse_color = vec4(1.0f, 0.3f, 0.6f, 1.0f);
	float t = 5.5f;
	g_models.model_matrix = Translate(-t, t, 0.0f)*Scale(0.8f, 0.8f, 0.8f);

	//模拟行星运动 有 地球和木星围绕着太阳转，然后月球围绕着地球转 
	g_scene_objects[OBJ_RECT].CreateRect(20.0f, 20.0f, 12, 10, 6.0f, 5.0f);
	g_scene_objects[OBJ_SPHERE].CreateSphere(0.6f, 32, 16, 1.0f, 1.0f);

	//给星球贴纹理
	g_scene_nodes[NODE_GROUND].texture=LoadTexture2DFromFile(
		"..\\textures\\sea.jpg");
	g_scene_nodes[NODE_SUN].texture=LoadTexture2DFromFile(
		"..\\textures\\sun.jpg");
	
	g_scene_nodes[NODE_EARTH].texture=LoadTexture2DFromFile(
		"..\\textures\\earth.jpg");

	g_scene_nodes[NODE_MOON].texture=LoadTexture2DFromFile(
	"..\\textures\\moon.jpg");
	
	g_scene_nodes[NODE_JUPITER].texture=LoadTexture2DFromFile(
	"..\\textures\\jupiter.jpg");

	g_scene_nodes[NODE_Uranus].texture = LoadTexture2DFromFile(
		"..\\textures\\jupiter.jpg");

	//生成星球
	g_scene_nodes[NODE_GROUND].p_mesh=&g_scene_objects[OBJ_RECT];
	g_scene_nodes[NODE_SUN].p_mesh=&g_scene_objects[OBJ_SPHERE];
	
	g_scene_nodes[NODE_EARTH].p_mesh=&g_scene_objects[OBJ_SPHERE];
	g_scene_nodes[NODE_MOON].p_mesh=&g_scene_objects[OBJ_SPHERE];
	g_scene_nodes[NODE_JUPITER].p_mesh=&g_scene_objects[OBJ_SPHERE];
	g_scene_nodes[NODE_Uranus].p_mesh = &g_scene_objects[OBJ_SPHERE];

	//设计树形结构，设定各个星球之间的关系
	g_scene_nodes[NODE_GROUND].p_child=&g_scene_nodes[NODE_SUN];
	g_scene_nodes[NODE_GROUND].p_sibling=NULL;
	//
	g_scene_nodes[NODE_SUN].p_child=&g_scene_nodes[NODE_EARTH];
	g_scene_nodes[NODE_SUN].p_sibling=NULL;

	g_scene_nodes[NODE_EARTH].p_child=&g_scene_nodes[NODE_MOON];
	//g_scene_nodes[NODE_EARTH].p_sibling = NULL;
	g_scene_nodes[NODE_EARTH].p_sibling = &g_scene_nodes[NODE_Uranus];


	//try
	g_scene_nodes[NODE_MOON].p_sibling= &g_scene_nodes[NODE_JUPITER];
	//g_scene_nodes[NODE_JUPITER].p_sibling = &g_scene_nodes[NODE_Uranus];

	g_scene_nodes[NODE_JUPITER].p_child = NULL;
	g_scene_nodes[NODE_MOON].p_child = NULL;
	g_scene_nodes[NODE_Uranus].p_child = NULL;

	//设定平移 旋转 缩放 
	g_scene_nodes[NODE_GROUND].from_parent_matrix=mat4();
	g_scene_nodes[NODE_GROUND].instance_matrix=mat4();
	g_scene_nodes[NODE_SUN].instance_matrix=
		Translate(0.0f, 0.0f, 2.0f*BASE_HEIGHT)
		*Scale(2.0f, 2.0f, 2.0f);
	g_scene_nodes[NODE_EARTH].instance_matrix=
		Translate(2.2f, 0.0f, 1.5f)
		*Scale(1.0f, 1.0f, 1.0f);

	g_scene_nodes[NODE_MOON].instance_matrix =
		Translate(2.2f, 0.0f, 1.5f)
		*Scale(0.4f, 0.4f, 0.4f);

	g_scene_nodes[NODE_JUPITER].instance_matrix=
	Translate(4.2f, 0.0f, 0.2f)
	*Scale(1.2f, 1.2f, 1.2f);

	g_scene_nodes[NODE_Uranus].instance_matrix =
		Translate(6.0f, 0.0f, 0.2f)
		*Scale(1.4f, 1.4f, 1.4f);

}

void init(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//WANGGE
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	printf("Vender: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	init_shaders();

	init_models();

	g_main_texture=CreateCheckerboardTexture();

	g_camera.Init(vec3(0.0f, 10.0f, 2.5f),
		vec3(0.5f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f));

	g_current_time=0.001f*glutGet(GLUT_ELAPSED_TIME);

	AnimateRobot();
}

void TraverseSceneNode(CSceneNode *p_node, mat4& current_matrix)
{
	if (p_node==NULL) return;
	g_matrix_stack.push(current_matrix);
	current_matrix=current_matrix*p_node->from_parent_matrix;
	p_node->model_matrix=current_matrix*p_node->instance_matrix;
	if (p_node->p_child!=NULL)
		TraverseSceneNode(p_node->p_child, current_matrix);
	current_matrix=g_matrix_stack.top();
	g_matrix_stack.pop();
	if (p_node->p_sibling!=NULL)
		TraverseSceneNode(p_node->p_sibling, current_matrix);
}


//公转
void AnimateRobot(void)
{
	g_scene_nodes[NODE_SUN].from_parent_matrix=
		RotateZ(g_joint_angles[JOINT_GROUND_TO_SUN]);
	
	g_scene_nodes[NODE_EARTH].from_parent_matrix=
		Translate(-0.5f*ARM_DISTANCE, 0.0f, BASE_HEIGHT)
		*RotateZ(g_joint_angles[JOINT_SUN_TO_EARTH]);

	g_scene_nodes[NODE_MOON].from_parent_matrix =
		Translate(0.0f, 0.0f, LOWER_ARM_HEIGHT)
		*RotateX(g_joint_angles[JOINT_EARTH_TO_MOON]);

	g_scene_nodes[NODE_JUPITER].from_parent_matrix =
		Translate(0.5f*ARM_DISTANCE, 0.0f, BASE_HEIGHT)
		*RotateZ(g_joint_angles[JOINT_EARTH_TO_JUPITER]);

	g_scene_nodes[NODE_Uranus].from_parent_matrix =
		Translate(0.5f*ARM_DISTANCE, 0.0f, BASE_HEIGHT)
		*RotateZ(g_joint_angles[JOINT_EARTH_TO_Uranus]);

	mat4 M;
	TraverseSceneNode(&g_scene_nodes[NODE_GROUND], M);
}




void reshape(int w, int h)
{
	g_window_width=w;
	g_window_height=h;

	glViewport(0, 0, w, h);
}

void display(void)
{
	int loc;
	mat4 M;
	mat3 Mn;

	g_camera.GetViewMatrix(M);
	loc=glGetUniformLocation(g_simple_GLSL_prog, "view_matrix");
	glUniformMatrix4fv(loc, 1, GL_TRUE, M);

	M=Perspective(g_FOV, (float)g_window_width/(float)g_window_height,
		0.01f, 20.0f);
	glUseProgram(g_simple_GLSL_prog);
	loc=glGetUniformLocation(g_simple_GLSL_prog, "projection_matrix");
	glUniformMatrix4fv(loc, 1, GL_TRUE, M);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_main_texture);

	glActiveTexture(GL_TEXTURE0);
	//我修改过NUM_NODES
	for (int i=0; i< NUM_NODES; ++i)
	{
		M=g_scene_nodes[i].model_matrix;

		loc=glGetUniformLocation(g_simple_GLSL_prog, "model_matrix");
		glUniformMatrix4fv(loc, 1, GL_TRUE, M);

		Mn=Normal(M);
		loc=glGetUniformLocation(g_simple_GLSL_prog, "normal_matrix");
		glUniformMatrix3fv(loc, 1, GL_TRUE, Mn);

		loc=glGetUniformLocation(g_simple_GLSL_prog, "diffuse_reflectivity");
		glUniform4fv(loc, 1, g_scene_nodes[i].diffuse_color);

		loc=glGetUniformLocation(g_simple_GLSL_prog, "enable_tex_map");
		glUniform1i(loc, g_scene_nodes[i].texture!=0);

		if (g_scene_nodes[i].texture!=0)
			glBindTexture(GL_TEXTURE_2D, g_scene_nodes[i].texture);

		g_scene_nodes[i].p_mesh->Draw();

	}

	/*
	M = mat4();
	loc = glGetUniformLocation(g_simple_GLSL_prog, "model_matrix");
	glUniformMatrix4fv(loc, 1, GL_TRUE, M);
	*/
	//犹他壶
	M = g_models.model_matrix;

	loc = glGetUniformLocation(g_simple_GLSL_prog, "model_matrix");
	glUniformMatrix4fv(loc, 1, GL_TRUE, M);

	Mn = Normal(M);
	loc = glGetUniformLocation(g_simple_GLSL_prog, "normal_matrix");
	glUniformMatrix3fv(loc, 1, GL_TRUE, Mn);

	loc = glGetUniformLocation(g_simple_GLSL_prog, "diffuse_reflectivity");
	glUniform4fv(loc, 1, g_models.diffuse_color);

	loc = glGetUniformLocation(g_simple_GLSL_prog, "enable_tex_map");
	glUniform1i(loc, g_models.texture != 0);


	g_models.mesh.Draw();

	glFlush();
}

void mouse(int button, int state, int x, int y)
{
	if (button==GLUT_RIGHT_BUTTON)
	{
		if (state==GLUT_DOWN)
		{
			g_camera_rotation_mode=true;
			g_mouse_pos[0]=x;
			g_mouse_pos[1]=y;
		}
		else
		{
			g_camera_rotation_mode=false;
		}
	}
}

void mouse_motion(int x, int y)
{
	if (g_camera_rotation_mode)
	{
		int dx=x-g_mouse_pos[0];
		int dy=y-g_mouse_pos[1];
		g_mouse_pos[0]=x;
		g_mouse_pos[1]=y;
		g_camera.TurnLeft(-1.0f*dx);
		g_camera.LookUp(-1.0f*dy);
		glutPostRedisplay();
	}
}

void keyboard(unsigned char key, int x, int y)
{
	const float joint_angle_delta=5.0f;
	switch(key)
	{
	case 'w':
	case 'W':
		g_camera.MoveForward(g_camera_move_step);
		glutPostRedisplay();
		break;
	case 's':
	case 'S':
		g_camera.MoveForward(-g_camera_move_step);
		glutPostRedisplay();
		break;
	case 'a':
	case 'A':
		g_camera.MoveLeft(g_camera_move_step);
		glutPostRedisplay();
		break;
	case 'd':
	case 'D':
		g_camera.MoveLeft(-g_camera_move_step);
		glutPostRedisplay();
		break;
	case 'r':
	case 'R':
		g_camera.MoveUp(g_camera_move_step);
		glutPostRedisplay();
		break;
	case 'f':
	case 'F':
		g_camera.MoveUp(-g_camera_move_step);
		glutPostRedisplay();
		break;
	case '1':
		g_joint_angles[JOINT_GROUND_TO_SUN]=
			fmod(g_joint_angles[JOINT_GROUND_TO_SUN]
				+joint_angle_delta, 360.0f);
		AnimateRobot();
		glutPostRedisplay();
		break;
	case '2':
		g_joint_angles[JOINT_GROUND_TO_SUN]=
			fmod(g_joint_angles[JOINT_GROUND_TO_SUN]
				-joint_angle_delta, 360.0f);
		AnimateRobot();
		glutPostRedisplay();
		break;

	}
}

void special_key(int key, int x, int y)
{
	int loc;

	switch(key)
	{
	case GLUT_KEY_PAGE_UP:
		g_FOV+=0.5f;
		if (g_FOV>178.0f) g_FOV=178.0f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_PAGE_DOWN:
		g_FOV-=0.5f;
		if (g_FOV<1.0f) g_FOV=1.0f;
		glutPostRedisplay();
		break;
	case GLUT_KEY_F1:
		glUseProgram(g_simple_GLSL_prog);
		g_lights_enable[0]=!g_lights_enable[0];
		loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[0].enable");
		glUniform1i(loc, g_lights_enable[0]);
		glutPostRedisplay();
		break;
	case GLUT_KEY_F2:
		glUseProgram(g_simple_GLSL_prog);
		g_lights_enable[1]=!g_lights_enable[1];
		loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[1].enable");
		glUniform1i(loc, g_lights_enable[1]);
		glutPostRedisplay();
		break;
	}
}

void idle(void)
{
	float t=0.001f*glutGet(GLUT_ELAPSED_TIME);
	float dt=t-g_current_time;
	g_current_time=t;

	g_rot_angle+=1.0f*dt;
	g_rot_angle_1 += 25.0f*dt;

	g_rot_angle=fmod(g_rot_angle, 360.0f);

	//g_rot_angle_1 = fmod(g_rot_angle_1, 360.0f);


	vec3 P;
	P.x=4.0f*cos(g_rot_angle);
	P.y=4.0f*sin(g_rot_angle);
	P.z=3.0f;

	glUseProgram(g_simple_GLSL_prog);
	int loc=glGetUniformLocation(g_simple_GLSL_prog, "lights[1].position");
	glUniform4f(loc, P.x, P.y, P.z, 1.0f);

	//设定公转速度
	g_joint_angles[JOINT_SUN_TO_EARTH] =
		fmod(g_rot_angle_1, 360.0f);

	g_joint_angles[JOINT_EARTH_TO_MOON] =
		fmod(g_rot_angle_1, 360.0f);

	g_joint_angles[JOINT_EARTH_TO_JUPITER] =
		fmod(g_rot_angle_1, 360.0f);


	g_joint_angles[JOINT_EARTH_TO_Uranus] =
		fmod(g_rot_angle_1, 360.0f);


	AnimateRobot();



	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE);
	glutInitWindowSize(640, 480);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("3D Scene");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_motion);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_key);
	glutIdleFunc(idle);

	glewExperimental=GL_TRUE;
	glewInit();

	init();

	glutMainLoop();

	return 1;
}
