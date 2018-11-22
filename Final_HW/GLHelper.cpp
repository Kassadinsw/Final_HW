#include <windows.h>
#include "GL/glew.h"
#include <stdio.h>

static char * ReadShaderSource(const char *file_name)
{
	FILE *fp=fopen(file_name, "rb");
	if (fp==NULL) return NULL;

	fseek(fp, 0, SEEK_END);
	long size=ftell(fp);
	char *str=new char [size+1];

	fseek(fp, 0, SEEK_SET);
	fread(str, 1, size, fp);
	fclose(fp);

	str[size]=0;

	return str;
}

GLuint InitShader(
	const char *vShaderFile, 
	const char *fShaderFile)
{
	struct {
		const char *file_name;
		GLenum type;
	} shaders[2]=
	{
		{vShaderFile, GL_VERTEX_SHADER},
		{fShaderFile, GL_FRAGMENT_SHADER}
	};

	GLuint prog=glCreateProgram();

	for (int i=0; i<2; i++)
	{
		char *src=ReadShaderSource(shaders[i].file_name);
		if (src==NULL)
		{
			printf("Unable to read source codes from %s.\n",
				shaders[i].file_name);
			exit(0);
		}
		
		GLuint shader=glCreateShader(shaders[i].type);
		glShaderSource(shader, 1, (const char **)&src, NULL);
		glCompileShader(shader);

		GLint compile_status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
		if (!compile_status)
		{
			printf("Failed to compile shader %s.\n",
				shaders[i].file_name);
			GLint info_log_len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, 
				&info_log_len);
			char *info_log=new char [info_log_len];
			glGetShaderInfoLog(shader, 
				info_log_len, NULL, info_log);
			printf("%s\n", info_log);
			delete [] info_log;
			exit(0);
		}

		glAttachShader(prog, shader);

		delete [] src;
	}

	glLinkProgram(prog);

	GLint link_status;
	glGetProgramiv(prog, GL_LINK_STATUS, &link_status);
	if (!link_status)
	{
		printf("Failed to link program.\n");
		GLint info_log_len;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, 
			&info_log_len);
		char *info_log=new char [info_log_len];
		glGetProgramInfoLog(prog, 
			info_log_len, NULL, info_log);
		printf("%s\n", info_log);
		delete [] info_log;
		exit(0);
	}

	glUseProgram(prog);

	return prog;
}
