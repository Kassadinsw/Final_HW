#include "ImageLib.h"
#include "FreeImage.h"
#include "GL/glew.h"

unsigned int LoadTexture2DFromFile(const char *file_name)
{
	// Get image file format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; // Image Format
	// Check the file signature and deduce its format
	fif = FreeImage_GetFileType(file_name, 0);
	// If unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(file_name);
	if(fif == FIF_UNKNOWN)
		return 0;

	// Load image from file
	FIBITMAP* img = FreeImage_Load(fif, file_name);
	if (img==NULL) return 0;

	// Get image information
	int image_width = FreeImage_GetWidth(img);
	int image_height = FreeImage_GetHeight(img);

	// Convert image to 24bit (BGR)
	FIBITMAP *img_temp=FreeImage_ConvertTo24Bits(img);
	FreeImage_Unload(img);
	img=img_temp;

	unsigned int itex;

	glGenTextures(1, &itex);
	glBindTexture(GL_TEXTURE_2D, itex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		image_width, image_height, 0,
		GL_BGR, GL_UNSIGNED_BYTE, FreeImage_GetBits(img));
	glGenerateMipmap(GL_TEXTURE_2D);

	FreeImage_Unload(img);

	return itex;
}

unsigned int LoadTextureCubeMapFromFile(
	const char *file_names[6])
{
	unsigned int itex;

	glGenTextures(1, &itex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, itex);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);

	for (int face=0; face<6; face++) {
	// Get image file format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; // Image Format
	// Check the file signature and deduce its format
	fif = FreeImage_GetFileType(file_names[face], 0);
	// If unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(file_names[face]);
	if(fif == FIF_UNKNOWN)
	{
		glDeleteTextures(1, &itex);
		return 0;
	}

	// Load image from file
	FIBITMAP* img = FreeImage_Load(fif, file_names[face]);
	if (img==NULL)
	{
		glDeleteTextures(1, &itex);
		return 0;
	}

	// Get image information
	int image_width = FreeImage_GetWidth(img);
	int image_height = FreeImage_GetHeight(img);

	// Convert image to 24bit (BGR)
	FIBITMAP *img_temp=FreeImage_ConvertTo24Bits(img);
	FreeImage_Unload(img);
	img=img_temp;

	FreeImage_FlipHorizontal(img);
	FreeImage_FlipVertical(img);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, 
		0, GL_RGB, 
		image_width, image_height, 0,
		GL_BGR, GL_UNSIGNED_BYTE, FreeImage_GetBits(img));

	FreeImage_Unload(img);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return itex;
}

unsigned int LoadBumpMap2DFromHeightMapFile(const char *file_name)
{
	// Get image file format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; // Image Format
	// Check the file signature and deduce its format
	fif = FreeImage_GetFileType(file_name, 0);
	// If unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(file_name);
	if(fif == FIF_UNKNOWN)
		return 0;

	// Load image from file
	FIBITMAP* img = FreeImage_Load(fif, file_name);
	if (img==NULL) return 0;

	// Get image information
	int image_width = FreeImage_GetWidth(img);
	int image_height = FreeImage_GetHeight(img);

	// Convert image to 24bit (BGR)
	FIBITMAP *img_temp=FreeImage_ConvertTo8Bits(img);
	FreeImage_Unload(img);
	img=img_temp;

	BYTE *height_image=FreeImage_GetBits(img);
	float *bump_image=new float [image_width*image_height*3];

	int i, j, k;
	float tx, ty;
	k=0;
	for (j=0; j<image_height; ++j)
	{
		for (i=0; i<image_width; ++i, ++k)
		{
			if (i==0)
				tx=height_image[k]-height_image[k+1];
			else if (i==image_width-1)
				tx=height_image[k-1]-height_image[k];
			else
				tx=0.5f*(height_image[k-1]-height_image[k+1]);

			if (j==0)
				ty=height_image[k]-height_image[k+image_width];
			else if (j==image_height-1)
				ty=height_image[k-image_width]-height_image[k];
			else
				ty=0.5f*(height_image[k-image_width]
					-height_image[k+image_width]);

			bump_image[k*3]=tx/255.0f;
			bump_image[k*3+1]=ty/255.0f;
			bump_image[k*3+2]=1.0f;
		}
	}

	unsigned int itex;

	glGenTextures(1, &itex);
	glBindTexture(GL_TEXTURE_2D, itex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 
		image_width, image_height, 0,
		GL_RGB, GL_FLOAT, bump_image);
	glGenerateMipmap(GL_TEXTURE_2D);

	FreeImage_Unload(img);

	delete [] bump_image;

	return itex;
}
