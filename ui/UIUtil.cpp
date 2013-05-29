#include "UIUtil.h"

CCRenderTexture * UIUtil::createStroke(CCLabelTTF *label, float size, ccColor3B color){
	if(!label || size <= 0){ return NULL;}
	float x = label->getTexture()->getContentSize().width+size*2;
	float y = label->getTexture()->getContentSize().height+size*2;
	CCRenderTexture *rt = CCRenderTexture::create(x, y);
	CCPoint originalPos = label->getPosition();
	ccColor3B originalColor = label->getColor();
	label->setColor(color);
	ccBlendFunc originalBlend = label->getBlendFunc();
	ccBlendFunc tempBlend = {GL_SRC_ALPHA, GL_ONE};
	label->setBlendFunc(tempBlend);
	CCPoint center = ccp(x/2+size, y/2+size);
	rt->begin();
	for (int i = 0; i < 360; i += 15) {
		x = center.x+sin(CC_DEGREES_TO_RADIANS(i))*size;
		y = center.y+cos(CC_DEGREES_TO_RADIANS(i))*size;
		label->setPosition(ccp(x, y));
		label->visit();
	}
	rt->end();
	label->setPosition(originalPos);
	label->setColor(originalColor);
	label->setBlendFunc(originalBlend);
	x = originalPos.x-size;
	x = originalPos.y-size;
	rt->setPosition(ccp(x, y));
	return rt;
}

// Gray = R*0.299 + G*0.587 + B*0.114
CCGLProgram * UIUtil::shaderForKey(const char *key, const char *v, const char *f){
	CCGLProgram *p = CCShaderCache::sharedShaderCache()->programForKey(key);
	if(!p){
		p = new CCGLProgram();
		if(p && p->initWithVertexShaderByteArray(v, f)){
			p->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
			p->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
			p->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
			p->link();
			p->updateUniforms();
			CHECK_GL_ERROR_DEBUG();
			CCShaderCache::sharedShaderCache()->addProgram(p, key);
			p->autorelease();
		}else{
			CC_SAFE_DELETE(p);
		}
	}
	return p;
}

void UIUtil::setShaderWithChildren(CCNode *n, CCGLProgram *s){
	if(n){
		n->setShaderProgram(s);
		CCObject *o;
		CCARRAY_FOREACH(n->getChildren(), o){
			setShaderWithChildren((CCNode *)o, s);
		}
	}
}
