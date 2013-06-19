/****************************************************************************
Copyright (c) 2013 Shawn Clovie

http://mcspot.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "CCBProxy.h"
#include "tolua++.h"

CCBProxy::CCBProxy():_selectorHandler(0){
	_memVars = CCDictionary::create();
	_memVars->retain();
	_handlers = CCArray::create();
	_handlers->retain();
}
CCBProxy::~CCBProxy(){
	CC_SAFE_RELEASE(_memVars);
	CC_SAFE_RELEASE(_handlers);
	CC_SAFE_RELEASE(_selectorHandler);
	CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}
void CCBProxy::releaseMembers(){
/*
	CCDictElement *e;
	CCNode *n;
	CCDICT_FOREACH(_memVars, e){
		n = (CCNode *)e->getObject();
		n->removeFromParentAndCleanup(true);
		n->autorelease();
	}
	CCObject *o;
	CCARRAY_FOREACH(_handlers, o){
		o->autorelease();
	}
//*/
	_memVars->removeAllObjects();
	_handlers->removeAllObjects();
}

CCBProxy * CCBProxy::initProxy(lua_State *l){
	_lua = l;
	return this;
}

SEL_MenuHandler CCBProxy::onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char * pSelectorName){
	return menu_selector(CCBProxy::menuItemCallback);
}

SEL_MenuHandler CCBProxy::onResolveCCBCCMenuItemSelector(CCObject * pTarget, CCString * pSelectorName){
	return onResolveCCBCCMenuItemSelector(pTarget, pSelectorName->getCString());
}

SEL_CCControlHandler CCBProxy::onResolveCCBCCControlSelector(CCObject * pTarget, const char * pSelectorName){
	return cccontrol_selector(CCBProxy::controlCallback);
}

SEL_CCControlHandler CCBProxy::onResolveCCBCCControlSelector(CCObject * pTarget, CCString * pSelectorName){
	return onResolveCCBCCControlSelector(pTarget, pSelectorName->getCString());
}

bool CCBProxy::onAssignCCBMemberVariable(CCObject * t, const char * v, CCNode * n){
	if(n && v && strlen(v) > 0){
		_memVars->setObject(n, v);
	}else{
		printf("CCBProxy assigned node (%s:%x) not saved\n", v, n);
	}
	return true;
}

// assign member variable to temp dictionary
bool CCBProxy::onAssignCCBMemberVariable(CCObject * t, CCString * v, CCNode * n) {
	return onAssignCCBMemberVariable(t, v->getCString(), n);
}

void CCBProxy::onNodeLoaded(CCNode * pNode, CCNodeLoader * pNodeLoader){
}

CCBSelectorResolver * CCBProxy::createNew(){
	CCBProxy *p = new CCBProxy();
	return dynamic_cast<CCBSelectorResolver *>(p);
}

void CCBProxy::handleEvent(CCControl *n, const int handler, bool multiTouches, CCControlEvent e){
#if COCOS2D_VERSION > 0x00020100
	n->addHandleOfControlEvent(handler, e);
#else
	LuaEventHandler *h = getHandler(handler);
	if(!h){
		h = addHandler(handler, multiTouches)
			->setTypename("CCControlButton");
	}
	n->addTargetWithActionForControlEvents(h, cccontrol_selector(LuaEventHandler::controlAction), e);
#endif
}

#ifdef LUAPROXY_CCEDITBOX_ENABLED
void CCBProxy::handleEvent(CCEditBox *n, const int handler){
	LuaEventHandler *h = addHandler(handler, false)
		->setTypename("CCEditBox");
	n->setDelegate(h);
}
#endif

void CCBProxy::handleEvent(CCBAnimationManager *m, const int handler){
	addHandler(handler)->handle(m);
}

void CCBProxy::handleKeypad(const int handler){
	CCDirector::sharedDirector()->getKeypadDispatcher()->addDelegate(addHandler(handler));
}

LuaEventHandler * CCBProxy::addHandler(const int handler, bool multiTouches){
	LuaEventHandler *h = LuaEventHandler::create(_lua)
		->handle(handler, multiTouches, 0, false);
	_handlers->addObject(h);
	return h;
}

LuaEventHandler * CCBProxy::getHandler(const int handler){
	CCObject *o;
	LuaEventHandler *h;
	CCARRAY_FOREACH(_handlers, o){
		h = (LuaEventHandler *)o;
		if(h->getHandler() == handler){
			return h;
		}
	}
	return NULL;
}

LuaEventHandler * CCBProxy::removeHandler(LuaEventHandler *h){
	if(h)_handlers->removeObject(h);
	return h;
}

LuaEventHandler * CCBProxy::removeFunction(int handler){
	return removeHandler(getHandler(handler));
}

LuaEventHandler * CCBProxy::removeKeypadHandler(int handler){
	LuaEventHandler *h = removeFunction(handler);
	if(h)CCDirector::sharedDirector()->getKeypadDispatcher()->removeDelegate(h);
	return h;
}

void CCBProxy::setSelectorHandler(LuaEventHandler *h){
	CC_SAFE_RELEASE(_selectorHandler);
	_selectorHandler = h;
	CC_SAFE_RETAIN(h);
}

LuaEventHandler * CCBProxy::handleSelector(const int handler){
	LuaEventHandler *h = NULL;
	if(handler > 0){
		h = LuaEventHandler::create(_lua)->handle(handler);
	}
	setSelectorHandler(h);
	return h;
}

void CCBProxy::menuItemCallback(CCObject *pSender) {
	if(_selectorHandler){
		_selectorHandler->action(pSender);
	}
}

void CCBProxy::controlCallback(CCObject *pSender, CCControlEvent event) {
	if(_selectorHandler){
		_selectorHandler->controlAction(pSender, event);
	}
}

CCDictionary * CCBProxy::getMemberVariables(){return _memVars;}

const char * CCBProxy::getMemberName(CCObject *n){
	CCDictElement *e;
	CCDICT_FOREACH(_memVars, e){
		if(e->getObject() == n)
			return e->getStrKey();
	}
	return "";
}

CCNode * CCBProxy::getNode(const char *n){
	return (CCNode *)_memVars->objectForKey(n);
}

void CCBProxy::nodeToTypeForLua(lua_State *l, CCObject *o, const char *t){
	if(strcmp("CCSprite", t) == 0)				tolua_pushusertype(l, dynamic_cast<CCSprite *>(o), t);
	else if(strcmp("CCControlButton", t) == 0)	tolua_pushusertype(l, dynamic_cast<CCControlButton *>(o), t);
	else if(strcmp("CCLayer", t) == 0)			tolua_pushusertype(l, dynamic_cast<CCLayer *>(o), t);
	else if(strcmp("CCLayerColor", t) == 0)		tolua_pushusertype(l, dynamic_cast<CCLayerColor *>(o), t);
	else if(strcmp("CCLayerGradient", t) == 0)	tolua_pushusertype(l, dynamic_cast<CCLayerGradient *>(o), t);
	else if(strcmp("CCScrollView", t) == 0)		tolua_pushusertype(l, dynamic_cast<CCScrollView *>(o), t);
	else if(strcmp("CCScale9Sprite", t) == 0)	tolua_pushusertype(l, dynamic_cast<CCScale9Sprite *>(o), t);
	else if(strcmp("CCLabelTTF", t) == 0)		tolua_pushusertype(l, dynamic_cast<CCLabelTTF *>(o), t);
	else if(strcmp("CCLabelBMFont", t) == 0)	tolua_pushusertype(l, dynamic_cast<CCLabelBMFont *>(o), t);
	else if(strcmp("CCMenu", t) == 0)			tolua_pushusertype(l, dynamic_cast<CCMenu *>(o), t);
	else if(strcmp("CCMenuItemImage", t) == 0)	tolua_pushusertype(l, dynamic_cast<CCMenuItemImage *>(o), t);
	else if(strcmp("CCString", t) == 0)			tolua_pushusertype(l, dynamic_cast<CCString *>(o), t);
	else if(strcmp("CCParticleSystemQuad", t) == 0)tolua_pushusertype(l, dynamic_cast<CCParticleSystemQuad *>(o), t);
	else if(strcmp("CCBFile", t) == 0)			tolua_pushusertype(l, dynamic_cast<CCBFile *>(o), t);
	else tolua_pushusertype(l, dynamic_cast<CCNode *>(o), "CCNode");
}

CCNode * CCBProxy::readCCBFromFile(const char *f){
	//assert(f && strlen(f) > 0, "File name must not be null or empty string.");
	CCBReader * reader = new CCBReader(CCNodeLoaderLibrary::sharedCCNodeLoaderLibrary());
#if COCOS2D_VERSION < 0x00020100
	reader->hasScriptingOwner = true;
#endif
	CCNode *node = reader->readNodeGraphFromFile(f, this);
	CCBAnimationManager *m = reader->getAnimationManager();
	reader->autorelease();
	node->setUserObject(m);
	return node;
}

void CCBProxy::fixLabel(CCNode *o, const float rate, bool withChild, const char *font){
	CCLabelTTF *l = dynamic_cast<CCLabelTTF *>(o);
	if(l){
		l->setScale(1 / rate);
		l->setFontSize(l->getFontSize() * rate);
		CCSize s = l->getDimensions();
		s.width *= rate;
		s.height *= rate;
		l->setDimensions(s);
		if(font){
			l->setFontName(font);
		}
	}
	if(withChild){
		CCObject *s;
		CCARRAY_FOREACH(o->getChildren(), s){
			fixLabel((CCNode *)s, rate, true, font);
		}
	}
}
void CCBProxy::fixParticle(CCNode *o, const float dur, const float life, bool withChild){
	CCParticleSystemQuad *e = dynamic_cast<CCParticleSystemQuad *>(o);
	if(e){
		printf("ccbproxy fixPar %x %x\n", e, e->getTexture());
		e->setDuration(dur);
		e->setLife(life);
		e->setAutoRemoveOnFinish(true);
	}
	if(withChild){
		CCObject *s;
		CCARRAY_FOREACH(o->getChildren(), s){
			fixParticle((CCNode *)s, dur, life, true);
		}
	}
}
void CCBProxy::fixParticleWithHandler(CCNode *o, LuaEventHandler *h, bool withChild){
	CCParticleSystemQuad *e = dynamic_cast<CCParticleSystemQuad *>(o);
	if(e){
		h->action(e);
	}
	if(withChild){
		CCObject *s;
		CCARRAY_FOREACH(o->getChildren(), s){
			fixParticleWithHandler((CCNode *)s, h, true);
		}
	}
}
#define CREATE_AND_DUPLICATE(r,T,n) if(dynamic_cast<T *>(n)){T *e = T::create();duplicate(e, (T *)n);r = e;}
CCNode * CCBProxy::copyNode(CCNode *n){
	CCNode *r = NULL;
	if(n){
		if(!r){	CREATE_AND_DUPLICATE(r, CCLabelTTF, n);}
		if(!r){	CREATE_AND_DUPLICATE(r, CCLabelBMFont, n);}
		if(!r){	CREATE_AND_DUPLICATE(r, CCParticleSystemQuad, n);}
		if(!r){	CREATE_AND_DUPLICATE(r, CCSprite, n);}
		if(!r){	CREATE_AND_DUPLICATE(r, CCScale9Sprite, n);}
		if(!r){
			r = CCNode::create();
			duplicate(r, n);
		}
		CCObject *o = NULL;
		CCARRAY_FOREACH(n->getChildren(), o){
			r->addChild(copyNode((CCNode *)o));
		}
	}
	return r;
}
void CCBProxy::duplicate(CCScale9Sprite *n, CCScale9Sprite *o){
	if(!n || !o)return;
	n->setPreferredSize(o->getPreferredSize());
	n->setCapInsets(o->getCapInsets());
	n->setOpacity(o->getOpacity());
	n->setColor(o->getColor());
	duplicate((CCNode *)n, (CCNode *)o);
}
void CCBProxy::duplicate(CCSprite *n, CCSprite *o){
	if(!n || !o)return;
	n->setDisplayFrame(o->displayFrame());
	n->setOpacity(o->getOpacity());
	n->setColor(o->getColor());
	n->setFlipX(o->isFlipX());
	n->setFlipY(o->isFlipY());
	n->setBlendFunc(o->getBlendFunc());
	duplicate((CCNode *)n, (CCNode *)o);
}
void CCBProxy::duplicate(CCLabelBMFont *n, CCLabelBMFont *o){
	if(!n || !o)return;
	n->setFntFile(o->getFntFile());
	n->setOpacity(o->getOpacity());
	n->setColor(o->getColor());
	n->setBlendFunc(o->getBlendFunc());
	duplicate((CCNode *)n, (CCNode *)o);
}
void CCBProxy::duplicate(CCLabelTTF *n, CCLabelTTF *o){
	if(!n || !o)return;
	n->setFontName(o->getFontName());
	n->setFontSize(o->getFontSize());
	n->setOpacity(o->getOpacity());
	n->setDimensions(o->getDimensions());
	n->setHorizontalAlignment(o->getHorizontalAlignment());
	n->setVerticalAlignment(o->getVerticalAlignment());
	duplicate((CCNode *)n, (CCNode *)o);
}
void CCBProxy::duplicate(CCNode *n, CCNode *o){
	if(!n || !o)return;
	n->setPosition(o->getPosition());
	n->setContentSize(o->getContentSize());
	n->setAnchorPoint(o->getAnchorPoint());
	n->setScaleX(o->getScaleX());
	n->setScaleY(o->getScaleY());
	n->setRotation(o->getRotation());
	n->setVisible(o->isVisible());
	n->setVertexZ(o->getVertexZ());
}
void CCBProxy::duplicate(CCParticleSystemQuad *n, CCParticleSystemQuad *o){
	if(!n || !o)return;
	duplicate((CCParticleSystem *)n, (CCParticleSystem *)o);
}
void CCBProxy::duplicate(CCParticleSystem *n, CCParticleSystem *o){
	if(!n || !o)return;
	n->setEmitterMode(o->getEmitterMode());
	n->setBatchNode(o->getBatchNode());
	n->setDuration(o->getDuration());
	n->setSourcePosition(o->getSourcePosition());
	n->setPosVar(o->getPosVar());
	n->setLife(o->getLife());
	n->setLifeVar(o->getLifeVar());
	n->setAngle(o->getAngle());
	n->setAngleVar(o->getAngleVar());
	if(n->getEmitterMode() == kCCParticleModeRadius){
		n->setStartRadius(o->getStartRadius());
		n->setStartRadiusVar(o->getStartRadiusVar());
		n->setEndRadius(o->getEndRadius());
		n->setEndRadiusVar(o->getEndRadiusVar());
		n->setRotatePerSecond(o->getRotatePerSecond());
		n->setRotatePerSecondVar(o->getRotatePerSecondVar());
	}else if(n->getEmitterMode() == kCCParticleModeGravity){
		n->setTangentialAccel(o->getTangentialAccel());
		n->setTangentialAccelVar(o->getTangentialAccelVar());
		n->setGravity(o->getGravity());
		n->setSpeed(o->getSpeed());
		n->setSpeedVar(o->getSpeedVar());
		n->setRadialAccel(o->getRadialAccel());
		n->setRadialAccelVar(o->getRadialAccelVar());
	}
	n->setScaleX(o->getScaleX());
	n->setScaleY(o->getScaleY());
	n->setRotation(o->getRotation());
	n->setBlendAdditive(o->isBlendAdditive());
	n->setStartSize(o->getStartSize());
	n->setStartSizeVar(o->getStartSizeVar());
	n->setEndSize(o->getEndSize());
	n->setEndSizeVar(o->getEndSizeVar());
	n->setStartColor(o->getStartColor());
	n->setStartColorVar(o->getStartColorVar());
	n->setEndColor(o->getEndColor());
	n->setEndColorVar(o->getEndColorVar());
	n->setStartSpin(o->getStartSpin());
	n->setStartSpinVar(o->getStartSpinVar());
	n->setEndSpin(o->getEndSpin());
	n->setEndSpinVar(o->getEndSpinVar());
	n->setEmissionRate(o->getEmissionRate());
	n->setTotalParticles(o->getTotalParticles());
	n->setTexture(o->getTexture());
	n->setBlendFunc(o->getBlendFunc());
	n->setOpacityModifyRGB(o->getOpacityModifyRGB());
	n->setPositionType(o->getPositionType());
	n->setAutoRemoveOnFinish(o->isAutoRemoveOnFinish());
	duplicate((CCNode *)n, (CCNode *)o);
}
