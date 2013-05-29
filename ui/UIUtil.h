#include "cocos2d.h"

USING_NS_CC;

class UIUtil{
public:
	// Label Stroke
	static CCRenderTexture * createStroke(CCLabelTTF *label, float size, ccColor3B color);
	// Get shader for key
	// If the shader not exist, try to create new with (v) and (f) if both of they are not null
	static CCGLProgram * shaderForKey(const char *key, const char *v = NULL, const char *f = NULL);
	static void setShaderWithChildren(CCNode *n, CCGLProgram *s = NULL);
};
