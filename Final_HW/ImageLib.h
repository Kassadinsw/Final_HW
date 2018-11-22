#ifndef _IMAGE_LIB_
#define _IMAGE_LIB_

unsigned int LoadTexture2DFromFile(const char *file_name);
unsigned int LoadTextureCubeMapFromFile(
	const char *file_names[6]);
unsigned int LoadBumpMap2DFromHeightMapFile(const char *file_name);

#endif
