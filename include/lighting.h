#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include "gfc_vector.h"

#define LIGHT_UBO_MAX 8 // max amount of lights youu should have

typedef struct { // this is the same exact thing made in gfc_model.h, but cut and pasted here (i didnt delete it in gf3d_mdoel.h yet)
	GFC_Vector4D lightPos;
	GFC_Vector4D lightDir;
	GFC_Vector4D lightColor;
	float angle;
	float brightness;
	float falloff;
	float inUse;
	// float padding (old because the lighting will be compleltely black instead)
}Light;

#endif