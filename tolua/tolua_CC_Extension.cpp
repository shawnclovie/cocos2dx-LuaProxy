#include "tolua_CC_Extension.h"
#include "CCBProxy.h"
#include "../ui/CursorTextField.h"
#include "tolua_CCBAnimationManager.h"
#include "tolua_CCBProxy.h"
#include "tolua_CCScrollView.h"
#include "tolua_CCTableView.h"
#include "tolua_CCScale9Sprite.h"

//copyAssetFileToData
static int tolua_copyAssetFileToData(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isstring(l, 1, 0, &err) || !tolua_isstring(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'copyAssetFileToData'.",&err);
		return 0;
	}
#endif
	CCFileUtils *fu = CCFileUtils::sharedFileUtils();
	unsigned long len = 0;
	std::string src = tolua_tostring(l, 1, NULL);
	std::string tar = tolua_tostring(l, 2, NULL);
	if(tar.length() == 0)tar = src;
	tar.insert(0, fu->getWriteablePath());
	src = fu->fullPathFromRelativePath(src.c_str());
//std::cout<<"copy from "<<src.c_str()<<"\n";
//std::cout<<"copy to "<<tar.c_str()<<"\n";
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	unsigned char *c = fu->getFileData(src.c_str(), "r", &len);
//std::cout<<"before copy, len "<<len<<"\n";
	if(len > 0){
		FILE *f = fopen(tar.c_str(), "w+");
		len = fwrite(c, len, 1, f) > 0;
		fclose(f);
		delete[] c;
		c = NULL;
	}
#endif
//std::cout<<"len "<<len<<"\n";
	if(len)tolua_pushstring(l, tar.c_str());
	return 1;
}

//#include "iconv.h"
//iconv
/*static int tolua_iconv(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isstring(l, 1, NULL, &err) || !tolua_isstring(l, 2, NULL, &err)
		|| !tolua_isstring(l, 3, NULL, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'iconv'.",&err);
		return 0;
	}
#endif
	char *r = "";
	const char *s = tolua_tostring(l, 1, r);
	iconv_t h = iconv_open(tolua_tostring(l, 2, NULL), tolua_tostring(l, 3, NULL));
	if(h != 0){
		size_t len = strlen(s);
		size_t lenout = len * 4;
		const char **pin = &s;
		char *outbuf = (char *)malloc(lenout);
		memset(outbuf, 0, lenout);
		if(-1 == libiconv(h, pin, &len, &outbuf, &lenout)){
CCLog("iconv convert failed");
			free(outbuf);
		}else{
CCLog("iconv converted, %s", outbuf);
			r = outbuf;
		}
		iconv_close(h);
	}
	tolua_pushstring(l, r);
	return 1;
}*/
static int tolua_touchedNodesChild(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCNode", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnumber(l, 3, 0, &err)
		|| !(tolua_isnoobj(l, 4, &err) || tolua_isstring(l, 4, 0, &err)) || !tolua_isnoobj(l, 5, &err)){
			tolua_error(l, "#ferror in function 'touchedNodesChild'.", &err);
			return 0;
	}
#endif
	CCNode *n = (CCNode *)tolua_tousertype(l, 1, NULL), *r = NULL;
	if(n){
		int x = tolua_tonumber(l, 2, 0), y = tolua_tonumber(l, 3, 0);
		CCPoint p = n->convertToNodeSpace(ccp(x, y));
		CCObject *o;
		CCNode *ns;
		CCARRAY_FOREACH(n->getChildren(), o){
			ns = (CCNode *)o;
			if(ns->boundingBox().containsPoint(p)){
				r = ns;
				break;
			}
		}
	}
	const char *t = tolua_tostring(l, 4, NULL);
	if(r)CCBProxy::nodeToTypeForLua(l, r, t);
	else tolua_pushusertype(l, NULL, t);
	return 1;
}
//######################################## CCBFile ##########################
//CCBFile::getCCBFileNode
static int tolua_CCBFile_getCCBFileNode(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBFile", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCBFile.getCCBFileNode'.",&err);
		return 0;
	}
#endif
	CCBFile *o = (CCBFile *)tolua_tousertype(l, 1, NULL);
	tolua_pushusertype(l, o? o->getCCBFileNode() : 0, "CCNode");
	return 1;
}
//CCBFile::setCCBFileNode
static int tolua_CCBFile_setCCBFileNode(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBFile", 0, &err) || !tolua_isusertype(l, 2, "CCNode", 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCBFile.setDuration'.",&err);
		return 0;
	}
#endif
	CCBFile *o = (CCBFile *)tolua_tousertype(l, 1, NULL);
	if(o)o->setCCBFileNode((CCNode *)tolua_tousertype(l, 2, 0));
	tolua_pushusertype(l, o, "CCBFile");
	return 1;
}
//######################################## CCBSequence ##########################
//CCBSequence::getDuration
static int tolua_CCBSequence_getDuration(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBSequence", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCBSequence.getDuration'.",&err);
		return 0;
	}
#endif
	CCBSequence *o = (CCBSequence *)tolua_tousertype(l, 1, NULL);
	tolua_pushnumber(l, o? o->getDuration() : 0);
	return 1;
}
//CCBSequence::setDuration
static int tolua_CCBSequence_setDuration(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBSequence", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCBSequence.setDuration'.",&err);
		return 0;
	}
#endif
	CCBSequence *o = (CCBSequence *)tolua_tousertype(l, 1, NULL);
	if(o)o->setDuration(tolua_tonumber(l, 2, 0));
	tolua_pushusertype(l, o, "CCBSequence");
	return 1;
}
//CCBSequence::getName
static int tolua_CCBSequence_getName(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBSequence", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCBSequence.getName'.",&err);
		return 0;
	}
#endif
	CCBSequence *o = (CCBSequence *)tolua_tousertype(l, 1, NULL);
	tolua_pushstring(l, o? o->getName() : "");
	return 1;
}
//CCBSequence::setName
static int tolua_CCBSequence_setName(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBSequence", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCBSequence.setName'.",&err);
		return 0;
	}
#endif
	CCBSequence *o = (CCBSequence *)tolua_tousertype(l, 1, NULL);
	if(o)o->setName(tolua_tostring(l, 2, ""));
	tolua_pushusertype(l, o, "CCBSequence");
	return 1;
}
//CCBSequence::getSequenceId
static int tolua_CCBSequence_getSequenceId(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBSequence", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCBSequence.getSequenceId'.",&err);
		return 0;
	}
#endif
	CCBSequence *o = (CCBSequence *)tolua_tousertype(l, 1, NULL);
	tolua_pushnumber(l, o? o->getSequenceId() : 0);
	return 1;
}
//CCBSequence::setSequenceId
static int tolua_CCBSequence_setSequenceId(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBSequence", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCBSequence.setSequenceId'.",&err);
		return 0;
	}
#endif
	CCBSequence *o = (CCBSequence *)tolua_tousertype(l, 1, NULL);
	if(o)o->setSequenceId(tolua_tonumber(l, 2, 0));
	tolua_pushusertype(l, o, "CCBSequence");
	return 1;
}
//CCBSequence::getChainedSequenceId
static int tolua_CCBSequence_getChainedSequenceId(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBSequence", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCBSequence.getChainedSequenceId'.",&err);
		return 0;
	}
#endif
	CCBSequence *o = (CCBSequence *)tolua_tousertype(l, 1, NULL);
	tolua_pushnumber(l, o? o->getChainedSequenceId() : 0);
	return 1;
}
//CCBSequence::setChainedSequenceId
static int tolua_CCBSequence_setChainedSequenceId(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCBSequence", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCBSequence.setChainedSequenceId'.",&err);
		return 0;
	}
#endif
	CCBSequence *o = (CCBSequence *)tolua_tousertype(l, 1, NULL);
	if(o)o->setChainedSequenceId(tolua_tonumber(l, 2, 0));
	tolua_pushusertype(l, o, "CCBSequence");
	return 1;
}
//######################################## CCControl ##########################
//CCControl::isEnabled
static int tolua_CCControl_isEnabled(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControl", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'isEnabled'.",&err);
		return 0;
	}
#endif
	CCControl *o = (CCControl *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushboolean(l, o->isEnabled());
	return 1;
}
//CCControl::isHighlighted
static int tolua_CCControl_isHighlighted(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControl", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'isHighlighted'.",&err);
		return 0;
	}
#endif
	CCControl *o = (CCControl *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushboolean(l, o->isHighlighted());
	return 1;
}
//CCControl::isSelected
static int tolua_CCControl_isSelected(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControl", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'isSelected'.",&err);
		return 0;
	}
#endif
	CCControl *o = (CCControl *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushboolean(l, o->isSelected());
	return 1;
}
//CCControl::setSelected
static int tolua_CCControl_setSelected(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControl", 0, &err) || !tolua_isboolean(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setSelected'.",&err);
		return 0;
	}
#endif
	CCControl *o = (CCControl *)tolua_tousertype(l, 1, NULL);
	if(o){
		o->setSelected(tolua_toboolean(l, 2, false) > 0);
		tolua_pushusertype(l, o, "CCControl");
	}
	return 1;
}
//CCControl::getState
static int tolua_CCControl_getState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControl", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'getState'.",&err);
		return 0;
	}
#endif
	CCControl *o = (CCControl *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushnumber(l, o->getState());
	return 1;
}
//CCControl::isTouchInside
static int tolua_CCControl_isTouchInside(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControl", 0, &err) || !tolua_isusertype(l, 2, "CCTouch", 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'isTouchInside'.",&err);
		return 0;
	}
#endif
	CCControl *o = (CCControl *)tolua_tousertype(l, 1, NULL);
	CCTouch *t = (CCTouch *)tolua_tousertype(l, 2, NULL);
	tolua_pushboolean(l, o && t? o->isTouchInside(t) : false);
	return 1;
}
//CCControl::getTouchLocation
static int tolua_CCControl_getTouchLocation(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControl", 0, &err) || !tolua_isusertype(l, 2, "CCTouch", 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'getTouchLocation'.",&err);
		return 0;
	}
#endif
	CCControl *o = (CCControl *)tolua_tousertype(l, 1, NULL);
	CCTouch *t = (CCTouch *)tolua_tousertype(l, 2, NULL);
	CCPoint p = o && t? o->getTouchLocation(t) : CCPointMake(0, 0);
	tolua_pushusertype(l, Mtolua_new((CCPoint)(p)), "CCPoint");
	return 1;
}
//CCControl::hasVisibleParents
static int tolua_CCControl_hasVisibleParents(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControl", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'hasVisibleParents'.",&err);
		return 0;
	}
#endif
	CCControl *o = (CCControl *)tolua_tousertype(l, 1, NULL);
	tolua_pushboolean(l, o? o->hasVisibleParents() : false);
	return 1;
}
//######################################## CCControlButton ##########################
//CCControlButton::create
static int tolua_CCControlButton_create(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertable(l, 1, "CCControlButton", 0, &err)
		|| !(tolua_isnoobj(l, 2, &err) || tolua_isusertype(l, 2, "CCScale9Sprite", 0, &err))
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.create'.",&err);
		return 0;
	}
#endif
	CCScale9Sprite *s = (CCScale9Sprite *)tolua_tousertype(l, 2, NULL);
	CCControlButton *o = s? CCControlButton::create(s) : CCControlButton::create();
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getBackgroundSprite
static int tolua_CCControlButton_getBackgroundSprite(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getBackgroundSprite'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushusertype(l, o->getBackgroundSprite(), "CCScale9Sprite");
	return 1;
}
//CCControlButton::setBackgroundSprite
static int tolua_CCControlButton_setBackgroundSprite(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isusertype(l, 2, "CCScale9Sprite", 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setBackgroundSprite'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setBackgroundSprite((CCScale9Sprite *)tolua_tousertype(l, 2, NULL));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getBackgroundSpriteForState
static int tolua_CCControlButton_getBackgroundSpriteForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, CCControlStateNormal, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getBackgroundSpriteForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushusertype(l, o->getBackgroundSpriteForState(tolua_tonumber(l, 2, CCControlStateNormal)), "CCScale9Sprite");
	return 1;
}
//CCControlButton::setBackgroundSpriteForState
static int tolua_CCControlButton_setBackgroundSpriteForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isusertype(l, 2, "CCScale9Sprite", 0, &err)
		|| !tolua_isnumber(l, 3, CCControlStateNormal, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setBackgroundSpriteForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setBackgroundSpriteForState((CCScale9Sprite *)tolua_tousertype(l, 2, NULL),
		tolua_tonumber(l, 3, CCControlStateNormal));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::setBackgroundSpriteFrameForState
static int tolua_CCControlButton_setBackgroundSpriteFrameForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isusertype(l, 2, "CCSpriteFrame", 0, &err)
		|| !tolua_isnumber(l, 3, CCControlStateNormal, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setBackgroundSpriteFrameForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setBackgroundSpriteFrameForState((CCSpriteFrame *)tolua_tousertype(l, 2, NULL),
		tolua_tonumber(l, 3, CCControlStateNormal));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getCurrentTitle
static int tolua_CCControlButton_getCurrentTitle(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getCurrentTitle'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushstring(l, o->getCurrentTitle()->getCString());
	return 1;
}
//CCControlButton::setEnabled
static int tolua_CCControlButton_setEnabled(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isboolean(l, 2, false, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setEnabled'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setEnabled(tolua_toboolean(l, 2, true) > 0);
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getLabelAnchorPoint
static int tolua_CCControlButton_getLabelAnchorPoint(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getLabelAnchorPoint'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushusertype(l, Mtolua_new((CCPoint)(o->getLabelAnchorPoint())), "CCPoint");
	return 1;
}
//CCControlButton::setLabelAnchorPoint
static int tolua_CCControlButton_setLabelAnchorPoint(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, 0, &err)
		|| !tolua_isnumber(l, 3, 0, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setLabelAnchorPoint'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setLabelAnchorPoint(ccp(tolua_tonumber(l, 2, 0), tolua_tonumber(l, 3, 0)));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::setMargins
static int tolua_CCControlButton_setMargins(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, 0, &err)
		|| !tolua_isnumber(l, 3, 0, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setMargins'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setMargins(tolua_tonumber(l, 2, 0), tolua_tonumber(l, 3, 0));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getOpacity
static int tolua_CCControlButton_getOpacity(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getOpacity'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushnumber(l, o->getOpacity());
	return 1;
}
//CCControlButton::setOpacity
static int tolua_CCControlButton_setOpacity(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setOpacity'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setOpacity(tolua_tonumber(l, 2, 255));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getPreferredSize
static int tolua_CCControlButton_getPreferredSize(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getPreferredSize'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushusertype(l, Mtolua_new((CCSize)(o->getPreferredSize())), "CCSize");
	return 1;
}
//CCControlButton::setPreferredSize
static int tolua_CCControlButton_setPreferredSize(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, 0, &err)
		|| !tolua_isnumber(l, 3, 0, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setPreferredSize'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setPreferredSize(CCSizeMake(tolua_tonumber(l, 2, 0), tolua_tonumber(l, 3, 0)));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::isPushed
static int tolua_CCControlButton_isPushed(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.isPushed'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushboolean(l, o->isPushed());
	return 1;
}
//CCControlButton::getTitleLabel
static int tolua_CCControlButton_getTitleLabel(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getTitleLabel'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o){
		CCNode *n = o->getTitleLabel();
		CCLabelTTF *ttf = dynamic_cast<CCLabelTTF *>(n);
		if(ttf)tolua_pushusertype(l, ttf, "CCLabelTTF");
		else{
			CCLabelBMFont *bm = dynamic_cast<CCLabelBMFont *>(n);
			if(bm)tolua_pushusertype(l, bm, "CCLabelBMFont");
			else{
				CCLabelAtlas *at = dynamic_cast<CCLabelAtlas *>(n);
				tolua_pushusertype(l, at? at : n, at? "CCLabelAtlas" : "CCNode");
			}
		}
	}
	return 1;
}
//CCControlButton::setTitleLabel
static int tolua_CCControlButton_setTitleLabel(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isusertype(l, 2, "CCNode", 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setTitleLabel'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setTitleLabel((CCNode *)tolua_tousertype(l, 2, NULL));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getTitleBMFontForState
static int tolua_CCControlButton_getTitleBMFontForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, CCControlStateNormal, &err)
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getTitleBMFontForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushstring(l, o->getTitleBMFontForState(tolua_tonumber(l, 2, CCControlStateNormal)));
	return 1;
}
//CCControlButton::setTitleBMFontForState
static int tolua_CCControlButton_setTitleBMFontForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isstring(l, 2, 0, &err)
		|| !tolua_isnumber(l, 3, CCControlStateNormal, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setTitleBMFontForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setTitleBMFontForState(tolua_tostring(l, 2, NULL), tolua_tonumber(l, 3, CCControlStateNormal));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getTitleColorForState
static int tolua_CCControlButton_getTitleColorForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, CCControlStateNormal, &err)
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getTitleColorForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushusertype(l, Mtolua_new((ccColor3B)(o->getTitleColorForState(tolua_tonumber(l, 2, CCControlStateNormal)))), "ccColor3B");
	return 1;
}
//CCControlButton::setTitleColorForState
static int tolua_CCControlButton_setTitleColorForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isusertype(l, 2, "ccColor3B", 0, &err)
		|| !tolua_isnumber(l, 3, CCControlStateNormal, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setTitleColorForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setTitleColorForState(*(ccColor3B *)(l, 2, NULL), tolua_tonumber(l, 3, CCControlStateNormal));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getTitleTTFForState
static int tolua_CCControlButton_getTitleTTFForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, CCControlStateNormal, &err)
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getTitleTTFForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushstring(l, o->getTitleTTFForState(tolua_tonumber(l, 2, CCControlStateNormal)));
	return 1;
}
//CCControlButton::setTitleTTFForState
static int tolua_CCControlButton_setTitleTTFForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isstring(l, 2, 0, &err)
		|| !tolua_isnumber(l, 3, CCControlStateNormal, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setTitleTTFForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setTitleTTFForState(tolua_tostring(l, 2, NULL), tolua_tonumber(l, 3, CCControlStateNormal));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}
//CCControlButton::getTitleForState
static int tolua_CCControlButton_getTitleForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isnumber(l, 2, CCControlStateNormal, &err)
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.getTitleForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)tolua_pushstring(l, o->getTitleForState(tolua_tonumber(l, 2, CCControlStateNormal))->getCString());
	return 1;
}
//CCControlButton::setTitleForState
static int tolua_CCControlButton_setTitleForState(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCControlButton", 0, &err) || !tolua_isstring(l, 2, 0, &err)
		|| !tolua_isnumber(l, 3, CCControlStateNormal, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CCControlButton.setTitleForState'.",&err);
		return 0;
	}
#endif
	CCControlButton *o = (CCControlButton *)tolua_tousertype(l, 1, NULL);
	if(o)o->setTitleForState(CCString::create(tolua_tostring(l, 2, NULL)), tolua_tonumber(l, 3, CCControlStateNormal));
	tolua_pushusertype(l, o, "CCControlButton");
	return 1;
}

//* 2.0.4 no subclass of CCEditBoxImpl, __createSystemEditBox error LNK2019
//######################################## CCEditBox ##########################
//CCEditBox::create
static int tolua_CCEditBox_create(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertable(l, 1, "CCEditBox", 0, &err) ||
		!tolua_isnumber(l, 2, 0, &err) || !tolua_isnumber(l, 3, 0, &err) ||
		!tolua_isusertype(l, 4, "CCScale9Sprite", 0, &err) || !tolua_isnoobj(l, 5, &err)){
		tolua_error(l,"#ferror in function 'create'.",&err);
		return 0;
	}
#endif
	int w = tolua_tonumber(l, 2, 0), h = tolua_tonumber(l, 3, 0);
	CCScale9Sprite *sp = (CCScale9Sprite *)tolua_tousertype(l, 4, NULL);
	tolua_pushusertype(l, CCEditBox::create(CCSizeMake(w, h), sp), "CCEditBox");
	return 1;
}
//CCEditBox::setContentSize
static int tolua_CCEditBox_setContentSize(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isnumber(l, 2, 0, &err) ||
		!tolua_isnumber(l, 3, 0, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'setContentSize'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	if(o)o->setContentSize(CCSizeMake(tolua_tonumber(l, 2, 0), tolua_tonumber(l, 3, 0)));
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//CCEditBox::setFontColor
static int tolua_CCEditBox_setFontColor(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isusertype(l, 2, "ccColor3B", 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setFontColor'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	ccColor3B c = *((ccColor3B*)tolua_tousertype(l, 2, NULL));
	if(o)o->setFontColor(c);
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//CCEditBox::setInputFlag
static int tolua_CCEditBox_setInputFlag(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setInputFlag'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	if(o)o->setInputFlag((EditBoxInputFlag)(int)tolua_tonumber(l, 2, 0));
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//CCEditBox::setInputMode
static int tolua_CCEditBox_setInputMode(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setInputMode'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	if(o)o->setInputMode((EditBoxInputMode)(int)tolua_tonumber(l, 2, 0));
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//CCEditBox::setMaxLength
static int tolua_CCEditBox_setMaxLength(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setMaxLength'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	if(o)o->setMaxLength(tolua_tonumber(l, 2, 0));
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//CCEditBox::setPlaceHolder
static int tolua_CCEditBox_setPlaceHolder(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isstring(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setPlaceHolder'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	if(o)o->setPlaceHolder(tolua_tostring(l, 2, ""));
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//CCEditBox::setPlaceHolderFontColor
static int tolua_CCEditBox_setPlaceHolderFontColor(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isusertype(l, 2, "ccColor3B", 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setPlaceHolderFontColor'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	ccColor3B c = *((ccColor3B*)tolua_tousertype(l, 2, NULL));
	if(o)o->setPlaceholderFontColor(c);
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//CCEditBox::setReturnType
static int tolua_CCEditBox_setReturnType(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setReturnType'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	if(o)o->setReturnType((KeyboardReturnType)(int)tolua_tonumber(l, 2, 0));
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//CCEditBox::getText
static int tolua_CCEditBox_getText(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'getText'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	tolua_pushstring(l, o? o->getText() : "");
	return 1;
}
//CCEditBox::setText
static int tolua_CCEditBox_setText(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCEditBox", 0, &err) ||
		!tolua_isstring(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setText'.",&err);
		return 0;
	}
#endif
	CCEditBox *o = (CCEditBox *)tolua_tousertype(l, 1, NULL);
	if(o)o->setText(tolua_tostring(l, 2, ""));
	tolua_pushusertype(l, o, "CCEditBox");
	return 1;
}
//*/
//######################################## CursorTextField ##########################
//CursorTextField::create
static int tolua_CursorTextField_create(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertable(l, 1, "CursorTextField", 0, &err) || !tolua_isstring(l, 2, 0, &err)
		|| !tolua_isnumber(l, 3, 0, &err) || !tolua_isnoobj(l, 4, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.create'.",&err);
		return 0;
	}
#endif
	tolua_pushusertype(l, CursorTextField::create(tolua_tostring(l, 2, "Arial"), tolua_tonumber(l, 3, 16)), "CursorTextField");
	return 1;
}
//CursorTextField::createWithPlaceHolder
static int tolua_CursorTextField_createWithPlaceHolder(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertable(l, 1, "CursorTextField", 0, &err) || !tolua_isstring(l, 2, 0, &err)
		|| !tolua_isstring(l, 3, 0, &err) || !tolua_isnumber(l, 4, 0, &err) || !tolua_isnoobj(l, 5, &err)){
		tolua_error(l,"#ferror in function 'createWithPlaceHolder'.",&err);
		return 0;
	}
#endif
	tolua_pushusertype(l, CursorTextField::createWithPlaceHolder(
		tolua_tostring(l, 2, ""), tolua_tostring(l, 3, "Arial"), tolua_tonumber(l, 4, 16)), "CursorTextField");
	return 1;
}
//CursorTextField::setColor
static int tolua_CursorTextField_setColor(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isusertype(l, 2, "ccColor3B", 0, &err)
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'setColor'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)p->setColor(*((ccColor3B *)tolua_tousertype(l, 2, NULL)));
	tolua_pushusertype(l, p, "CursorTextField");
	return 1;
}
//CursorTextField::getDesignedSize
static int tolua_CursorTextField_getDesignedSize(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.getDesignedSize'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)tolua_pushusertype(l, Mtolua_new((CCSize)(p->getDesignedSize())), "CCSize");
	return 1;
}
//CursorTextField::setDesignedSize
static int tolua_CursorTextField_setDesignedSize(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isusertype(l, 2, "CCSize", 0, &err)
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.setDesignedSize'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)p->setDesignedSize(*((CCSize *)tolua_tousertype(l, 2, NULL)));
	tolua_pushusertype(l, p, "CursorTextField");
	return 1;
}
//CursorTextField::getMaxLength
static int tolua_CursorTextField_getMaxLength(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.getMaxLength'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)tolua_pushnumber(l, p->getMaxLength());
	return 1;
}
//CursorTextField::setMaxLength
static int tolua_CursorTextField_setMaxLength(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isnumber(l, 2, 0, &err)
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.setMaxLength'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)p->setMaxLength(tolua_tonumber(l, 2, 0));
	tolua_pushusertype(l, p, "CursorTextField");
	return 1;
}
//CursorTextField::isPassword
static int tolua_CursorTextField_isPassword(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.isPassword'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)tolua_pushboolean(l, p->isPassword());
	return 1;
}
//CursorTextField::setPassword
static int tolua_CursorTextField_setPassword(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isboolean(l, 2, 0, &err)
		|| !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.setPassword'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)p->setPassword(tolua_toboolean(l, 2, 0) > 0);
	tolua_pushusertype(l, p, "CursorTextField");
	return 1;
}
//CursorTextField::getRect
static int tolua_CursorTextField_getRect(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isnoobj(l, 2, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.getRect'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)tolua_pushusertype(l, Mtolua_new((CCRect)(p->getRect())), "CCRect");
	return 1;
}
//CursorTextField::setString
static int tolua_CursorTextField_setString(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CursorTextField", 0, &err) || !tolua_isstring(l, 2, 0, &err) || !tolua_isnoobj(l, 3, &err)){
		tolua_error(l,"#ferror in function 'CursorTextField.setString'.",&err);
		return 0;
	}
#endif
	CursorTextField *p = (CursorTextField *)tolua_tousertype(l, 1, NULL);
	if(p)p->setString(tolua_tostring(l, 2, ""));
	tolua_pushusertype(l, p, "CursorTextField");
	return 1;
}

TOLUA_API int tolua_CC_Extension_open(lua_State* l){
	tolua_open(l);
	tolua_usertype(l, "CCBAnimationManager");
	tolua_usertype(l, "CCBFile");
	tolua_usertype(l, "CCBProxy");
	tolua_usertype(l, "CCBReader");
	tolua_usertype(l, "CCBSequence");
	tolua_usertype(l, "CCControl");
	tolua_usertype(l, "CCControlButton");
	tolua_usertype(l, "CCScale9Sprite");
	tolua_usertype(l, "CCScrollView");
	tolua_usertype(l, "CCTableView");
	tolua_usertype(l, "CCTableViewCell");
	tolua_usertype(l, "CCTableViewDataSource");
	tolua_usertype(l, "CCTableViewDelegate");
	tolua_usertype(l, "CursorTextField");
	tolua_usertype(l, "LuaEventHandler");
	tolua_usertype(l, "LuaTableView");
	tolua_module(l, NULL, 0);
	tolua_beginmodule(l, NULL);
		tolua_function(l, "touchedNodesChild", tolua_touchedNodesChild);
		//tolua_function(l, "runningPlatform", tolua_runningPlatform);
		tolua_constant(l, "CC_TARGET_PLATFORM", CC_TARGET_PLATFORM);
		tolua_constant(l, "CC_PLATFORM_IOS", CC_PLATFORM_IOS);
		tolua_constant(l, "CC_PLATFORM_ANDROID", CC_PLATFORM_ANDROID);
		tolua_constant(l, "CC_PLATFORM_WIN32", CC_PLATFORM_WIN32);
		tolua_constant(l, "CC_PLATFORM_MAC", CC_PLATFORM_MAC);
		tolua_constant(l, "CC_PLATFORM_LINUX", CC_PLATFORM_LINUX);
		tolua_constant(l, "CC_PLATFORM_UNKNOW", CC_PLATFORM_UNKNOWN);
//		tolua_function(l, "iconv", tolua_iconv);
		tolua_function(l, "copyAssetFileToData", tolua_copyAssetFileToData);
		tolua_cclass(l,"CCBAnimationManager","CCBAnimationManager","CCObject",NULL);
		tolua_beginmodule(l,"CCBAnimationManager");
			tolua_function(l, "pickFromNode", tolua_CCBAnimationManager_pickFromNode);
			tolua_function(l, "getSequences", tolua_CCBAnimationManager_getSequences);
			tolua_function(l, "getAutoPlaySequenceId", tolua_CCBAnimationManager_getAutoPlaySequenceId);
			tolua_function(l, "setAutoPlaySequenceId", tolua_CCBAnimationManager_setAutoPlaySequenceId);
			tolua_function(l, "getRootNode", tolua_CCBAnimationManager_getRootNode);
			tolua_function(l, "setRootNode", tolua_CCBAnimationManager_setRootNode);
			tolua_function(l, "getRootContainerSize", tolua_CCBAnimationManager_getRootContainerSize);
			tolua_function(l, "setRootContainerSize", tolua_CCBAnimationManager_setRootContainerSize);
			tolua_function(l, "getRunningSequenceName", tolua_CCBAnimationManager_getRunningSequenceName);
			tolua_function(l, "getContainerSize", tolua_CCBAnimationManager_getContainerSize);
			tolua_function(l, "addNode", tolua_CCBAnimationManager_addNode);
			tolua_function(l, "setBaseValue", tolua_CCBAnimationManager_setBaseValue);
			tolua_function(l, "runAnimations", tolua_CCBAnimationManager_runAnimations);
			tolua_function(l, "runAnimationsForSequenceIdTweenDuration", tolua_CCBAnimationManager_runAnimationsForSequenceIdTweenDuration);
		tolua_endmodule(l);
		tolua_cclass(l,"CCBFile","CCBFile","CCNode",NULL);
		tolua_beginmodule(l,"CCBFile");
			tolua_function(l, "getCCBFileNode", tolua_CCBFile_getCCBFileNode);
			tolua_function(l, "setCCBFileNode", tolua_CCBFile_setCCBFileNode);
		tolua_endmodule(l);
		tolua_cclass(l,"CCBProxy","CCBProxy","CCLayer",NULL);
		tolua_beginmodule(l,"CCBProxy");
			tolua_function(l, "create", tolua_CCBProxy_create);
			tolua_function(l, "releaseMembers", tolua_CCBProxy_releaseMembers);
			tolua_function(l, "getMemberName", tolua_CCBProxy_getMemberName);
			tolua_function(l, "getNode", tolua_CCBProxy_getNode);
			tolua_function(l, "getNodeWithType", tolua_CCBProxy_getNodeWithType);
			tolua_function(l, "nodeToType", tolua_CCBProxy_nodeToType);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
			tolua_function(l, "handleEditEvent", tolua_CCBProxy_handleEditEvent);
#endif
			tolua_function(l, "handleButtonEvent", tolua_CCBProxy_handleButtonEvent);
			tolua_function(l, "handleKeypad", tolua_CCBProxy_handleKeypad);
			tolua_function(l, "handleMenuEvent", tolua_CCBProxy_handleMenuEvent);
			tolua_function(l, "handleAnimationComplate", tolua_CCBProxy_handleAnimationComplate);
			tolua_function(l, "handleSelector", tolua_CCBProxy_handleSelector);
			tolua_function(l, "removeHandler", tolua_CCBProxy_removeHandler);
			tolua_function(l, "removeFunction", tolua_CCBProxy_removeFunction);
			tolua_function(l, "removeKeypadHandler", tolua_CCBProxy_removeKeypadHandler);
			tolua_function(l, "getSelectorHandler", tolua_CCBProxy_getSelectorHandler);
			tolua_function(l, "setSelectorHandler", tolua_CCBProxy_setSelectorHandler);
			tolua_function(l, "deliverChildren", tolua_CCBProxy_deliverChildren);
			tolua_function(l, "readCCBFromFile", tolua_CCBProxy_readCCBFromFile);
			tolua_function(l, "fixLabel", tolua_CCBProxy_fixLabel);
			tolua_function(l, "duplicate", tolua_CCBProxy_duplicate);
		tolua_endmodule(l);
		tolua_cclass(l,"CCBReader","CCBReader","CCObject",NULL);
		tolua_beginmodule(l,"CCBReader");
//			tolua_function(l,"load",tolua_CCBReader_load);
		tolua_endmodule(l);
		tolua_cclass(l,"CCBSequence","CCBSequence","CCObject",NULL);
		tolua_beginmodule(l,"CCBSequence");
			tolua_function(l,"getDuration",tolua_CCBSequence_getDuration);
			tolua_function(l,"setDuration",tolua_CCBSequence_setDuration);
			tolua_function(l,"getName",tolua_CCBSequence_getName);
			tolua_function(l,"setName",tolua_CCBSequence_setName);
			tolua_function(l,"getSequenceId",tolua_CCBSequence_getSequenceId);
			tolua_function(l,"setSequenceId",tolua_CCBSequence_setSequenceId);
			tolua_function(l,"getChainedSequenceId",tolua_CCBSequence_getChainedSequenceId);
			tolua_function(l,"setChainedSequenceId",tolua_CCBSequence_setChainedSequenceId);
		tolua_endmodule(l);
		tolua_cclass(l, "CCControl", "CCControl", "CCLayer", NULL);
		tolua_beginmodule(l, "CCControl");
			tolua_function(l, "isEnabled", tolua_CCControl_isEnabled);
			tolua_function(l, "isHighlighted", tolua_CCControl_isHighlighted);
			tolua_function(l, "isSelected", tolua_CCControl_isSelected);
			tolua_function(l, "setSelected", tolua_CCControl_setSelected);
			tolua_function(l, "getState", tolua_CCControl_getState);
			tolua_function(l, "isTouchInside", tolua_CCControl_isTouchInside);
			tolua_function(l, "getTouchLocation", tolua_CCControl_getTouchLocation);
			tolua_function(l, "hasVisibleParents", tolua_CCControl_hasVisibleParents);
		tolua_endmodule(l);
		tolua_cclass(l, "CCControlButton", "CCControlButton", "CCControl", NULL);
		tolua_beginmodule(l, "CCControlButton");
			tolua_function(l, "create", tolua_CCControlButton_create);
			tolua_function(l, "getBackgroundSprite", tolua_CCControlButton_getBackgroundSprite);
			tolua_function(l, "setBackgroundSprite", tolua_CCControlButton_setBackgroundSprite);
			tolua_function(l, "getBackgroundSpriteForState", tolua_CCControlButton_getBackgroundSpriteForState);
			tolua_function(l, "setBackgroundSpriteForState", tolua_CCControlButton_setBackgroundSpriteForState);
			tolua_function(l, "setBackgroundSpriteFrameForState", tolua_CCControlButton_setBackgroundSpriteFrameForState);
			tolua_function(l, "getCurrentTitle", tolua_CCControlButton_getCurrentTitle);
			tolua_function(l, "setEnabled", tolua_CCControlButton_setEnabled);
			tolua_function(l, "getLabelAnchorPoint", tolua_CCControlButton_getLabelAnchorPoint);
			tolua_function(l, "setLabelAnchorPoint", tolua_CCControlButton_setLabelAnchorPoint);
			tolua_function(l, "setMargins", tolua_CCControlButton_setMargins);
			tolua_function(l, "getOpacity", tolua_CCControlButton_getOpacity);
			tolua_function(l, "setOpacity", tolua_CCControlButton_setOpacity);
			tolua_function(l, "getPreferredSize", tolua_CCControlButton_getPreferredSize);
			tolua_function(l, "setPreferredSize", tolua_CCControlButton_setPreferredSize);
			tolua_function(l, "isPushed", tolua_CCControlButton_isPushed);
			tolua_function(l, "getTitleLabel", tolua_CCControlButton_getTitleLabel);
			tolua_function(l, "setTitleLabel", tolua_CCControlButton_setTitleLabel);
			tolua_function(l, "getTitleBMFontForState", tolua_CCControlButton_getTitleBMFontForState);
			tolua_function(l, "setTitleBMFontForState", tolua_CCControlButton_setTitleBMFontForState);
			tolua_function(l, "getTitleColorForState", tolua_CCControlButton_getTitleColorForState);
			tolua_function(l, "setTitleColorForState", tolua_CCControlButton_setTitleColorForState);
			tolua_function(l, "getTitleTTFForState", tolua_CCControlButton_getTitleTTFForState);
			tolua_function(l, "setTitleTTFForState", tolua_CCControlButton_setTitleTTFForState);
			tolua_function(l, "getTitleForState", tolua_CCControlButton_getTitleForState);
			tolua_function(l, "setTitleForState", tolua_CCControlButton_setTitleForState);
		tolua_endmodule(l);
		tolua_constant(l, "CCControlStateNormal", CCControlStateNormal);
		tolua_constant(l, "CCControlStateHighlighted", CCControlStateHighlighted);
		tolua_constant(l, "CCControlStateDisabled", CCControlStateDisabled);
		tolua_constant(l, "CCControlStateSelected", CCControlStateSelected);
/*
// comment those line, android version would not crash on startup.
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
		tolua_cclass(l, "CCEditBox", "CCEditBox", "CCControlButton", NULL);
		tolua_beginmodule(l, "CCEditBox");
			tolua_function(l, "create", tolua_CCEditBox_create);
			tolua_function(l, "setContentSize", tolua_CCEditBox_setContentSize);
			tolua_function(l, "setFontColor", tolua_CCEditBox_setFontColor);
			tolua_function(l, "setInputFlag", tolua_CCEditBox_setInputFlag);
			tolua_function(l, "setInputMode", tolua_CCEditBox_setInputMode);
			tolua_function(l, "setMaxLength", tolua_CCEditBox_setMaxLength);
			tolua_function(l, "setPlaceHolder", tolua_CCEditBox_setPlaceHolder);
			tolua_function(l, "setPlaceHolderFontColor", tolua_CCEditBox_setPlaceHolderFontColor);
			tolua_function(l, "setReturnType", tolua_CCEditBox_setReturnType);
			tolua_function(l, "getText", tolua_CCEditBox_getText);
			tolua_function(l, "setText", tolua_CCEditBox_setText);
		tolua_endmodule(l);
		tolua_constant(l, "kEditBoxInputFlagPassword", kEditBoxInputFlagPassword);
		tolua_constant(l, "kEditBoxInputFlagSensitive", kEditBoxInputFlagSensitive);
		tolua_constant(l, "kEditBoxInputFlagInitialCapsWord", kEditBoxInputFlagInitialCapsWord);
		tolua_constant(l, "kEditBoxInputFlagInitialCapsSentence", kEditBoxInputFlagInitialCapsSentence);
		tolua_constant(l, "kEditBoxInputFlagInitialCapsAllCharacters", kEditBoxInputFlagInitialCapsAllCharacters);
		tolua_constant(l, "kEditBoxInputModeAny", kEditBoxInputModeAny);
		tolua_constant(l, "kEditBoxInputModeEmailAddr", kEditBoxInputModeEmailAddr);
		tolua_constant(l, "kEditBoxInputModeNumeric", kEditBoxInputModeNumeric);
		tolua_constant(l, "kEditBoxInputModePhoneNumber", kEditBoxInputModePhoneNumber);
		tolua_constant(l, "kEditBoxInputModeUrl", kEditBoxInputModeUrl);
		tolua_constant(l, "kEditBoxInputModeDecimal", kEditBoxInputModeDecimal);
		tolua_constant(l, "kEditBoxInputModeSingleLine", kEditBoxInputModeSingleLine);
		tolua_constant(l, "kKeyboardReturnTypeDefault", kKeyboardReturnTypeDefault);
		tolua_constant(l, "kKeyboardReturnTypeDone", kKeyboardReturnTypeDone);
		tolua_constant(l, "kKeyboardReturnTypeSend", kKeyboardReturnTypeSend);
		tolua_constant(l, "kKeyboardReturnTypeSearch", kKeyboardReturnTypeSearch);
		tolua_constant(l, "kKeyboardReturnTypeGo", kKeyboardReturnTypeGo);
#endif
//*/
		tolua_cclass(l, "CCScale9Sprite", "CCScale9Sprite", "CCNode", NULL);
		tolua_beginmodule(l, "CCScale9Sprite");
			tolua_function(l, "create", tolua_CCScale9Sprite_create);
			tolua_function(l, "createWithSpriteFrame", tolua_CCScale9Sprite_createWithSpriteFrame);
			tolua_function(l, "setContentSize", tolua_CCScale9Sprite_setContentSize);
			tolua_function(l, "getCapInsets", tolua_CCScale9Sprite_getCapInsets);
			tolua_function(l, "setCapInsets", tolua_CCScale9Sprite_setCapInsets);
			tolua_function(l, "getColor", tolua_CCScale9Sprite_getColor);
			tolua_function(l, "setColor", tolua_CCScale9Sprite_setColor);
			tolua_function(l, "getOpacity", tolua_CCScale9Sprite_getOpacity);
			tolua_function(l, "setOpacity", tolua_CCScale9Sprite_setOpacity);
			tolua_function(l, "isOpacityModifyRGB", tolua_CCScale9Sprite_isOpacityModifyRGB);
			tolua_function(l, "setOpacityModifyRGB", tolua_CCScale9Sprite_setOpacityModifyRGB);
			tolua_function(l, "getPreferredSize", tolua_CCScale9Sprite_getPreferredSize);
			tolua_function(l, "setPreferredSize", tolua_CCScale9Sprite_setPreferredSize);
			tolua_function(l, "copyPreferredSize", tolua_CCScale9Sprite_copyPreferredSize);
			tolua_function(l, "setSpriteFrame", tolua_CCScale9Sprite_setSpriteFrame);
			tolua_function(l, "resizableSpriteWithCapInsets", tolua_CCScale9Sprite_resizableSpriteWithCapInsets);
			tolua_function(l, "updateWithBatchNode", tolua_CCScale9Sprite_updateWithBatchNode);
		tolua_endmodule(l);
		tolua_constant(l, "kCCScrollViewDirectionNone", kCCScrollViewDirectionNone);
		tolua_constant(l, "kCCScrollViewDirectionHorizontal", kCCScrollViewDirectionHorizontal);
		tolua_constant(l, "kCCScrollViewDirectionVertical", kCCScrollViewDirectionVertical);
		tolua_constant(l, "kCCScrollViewDirectionBoth", kCCScrollViewDirectionBoth);
		tolua_cclass(l, "CCScrollView", "CCScrollView", "CCLayer", NULL);
		tolua_beginmodule(l, "CCScrollView");
			tolua_function(l, "create", tolua_CCScrollView_create);
			tolua_function(l, "isBounceable", tolua_CCScrollView_isBounceable);
			tolua_function(l, "setBounceable", tolua_CCScrollView_setBounceable);
			tolua_function(l, "addChild", tolua_CCScrollView_addChild);
			tolua_function(l, "isClippingToBounds", tolua_CCScrollView_isClippingToBounds);
			tolua_function(l, "setClippingToBounds", tolua_CCScrollView_setClippingToBounds);
			tolua_function(l, "getContainer", tolua_CCScrollView_getContainer);
			tolua_function(l, "setContainer", tolua_CCScrollView_setContainer);
			tolua_function(l, "getContentOffset", tolua_CCScrollView_getContentOffset);
			tolua_function(l, "setContentOffset", tolua_CCScrollView_setContentOffset);
			tolua_function(l, "setContentOffsetInDuration", tolua_CCScrollView_setContentOffsetInDuration);
			tolua_function(l, "getContentSize", tolua_CCScrollView_getContentSize);
			tolua_function(l, "setContentSize", tolua_CCScrollView_setContentSize);
			tolua_function(l, "getDirection", tolua_CCScrollView_getDirection);
			tolua_function(l, "setDirection", tolua_CCScrollView_setDirection);
			tolua_function(l, "isDragging", tolua_CCScrollView_isDragging);
			tolua_function(l, "isNodeVisible", tolua_CCScrollView_isNodeVisible);
			tolua_function(l, "isTouchMoved", tolua_CCScrollView_isTouchMoved);
			tolua_function(l, "getViewSize", tolua_CCScrollView_getViewSize);
			tolua_function(l, "setViewSize", tolua_CCScrollView_setViewSize);
			tolua_function(l, "getZoomScale", tolua_CCScrollView_getZoomScale);
			tolua_function(l, "setZoomScale", tolua_CCScrollView_setZoomScale);
			tolua_function(l, "setZoomScaleInDuration", tolua_CCScrollView_setZoomScaleInDuration);
		tolua_endmodule(l);
		tolua_constant(l, "kCCTableViewFillTopDown", kCCTableViewFillTopDown);
		tolua_constant(l, "kCCTableViewFillBottomUp", kCCTableViewFillBottomUp);
		tolua_cclass(l, "CCTableViewDataSource", "CCTableViewDataSource", "", NULL);
		tolua_cclass(l, "CCTableViewDelegate", "CCTableViewDelegate", "", NULL);
		tolua_cclass(l, "CCTableView", "CCTableView", "CCScrollView", NULL);
		tolua_beginmodule(l, "CCTableView");
			tolua_function(l, "create", tolua_CCTableView_create);
			tolua_function(l, "getDataSource", tolua_CCTableView_getDataSource);
			tolua_function(l, "setDataSource", tolua_CCTableView_setDataSource);
			tolua_function(l, "getDelegate", tolua_CCTableView_getDelegate);
			tolua_function(l, "setDelegate", tolua_CCTableView_setDelegate);
			tolua_function(l, "getVerticalFillOrder", tolua_CCTableView_getVerticalFillOrder);
			tolua_function(l, "setVerticalFillOrder", tolua_CCTableView_setVerticalFillOrder);
			tolua_function(l, "updateCellAtIndex", tolua_CCTableView_updateCellAtIndex);
			tolua_function(l, "insertCellAtIndex", tolua_CCTableView_insertCellAtIndex);
			tolua_function(l, "removeCellAtIndex", tolua_CCTableView_removeCellAtIndex);
			tolua_function(l, "reloadData", tolua_CCTableView_reloadData);
			tolua_function(l, "dequeueCell", tolua_CCTableView_dequeueCell);
			tolua_function(l, "cellAtIndex", tolua_CCTableView_cellAtIndex);
		tolua_endmodule(l);
		tolua_cclass(l, "CCTableViewCell", "CCTableViewCell", "CCNode", NULL);
		tolua_beginmodule(l, "CCTableViewCell");
			tolua_function(l, "create", tolua_CCTableViewCell_create);
			tolua_function(l, "getIdx", tolua_CCTableViewCell_getIdx);
			tolua_function(l, "setIdx", tolua_CCTableViewCell_setIdx);
			tolua_function(l, "reset", tolua_CCTableViewCell_reset);
			tolua_function(l, "getObjectID", tolua_CCTableViewCell_getObjectID);
			tolua_function(l, "setObjectID", tolua_CCTableViewCell_setObjectID);
		tolua_endmodule(l);
		tolua_cclass(l, "CursorTextField", "CursorTextField", "CCTextFieldTTF", NULL);
		tolua_beginmodule(l, "CursorTextField");
			tolua_function(l, "create", tolua_CursorTextField_create);
			tolua_function(l, "createWithPlaceHolder", tolua_CursorTextField_createWithPlaceHolder);
			tolua_function(l, "setColor", tolua_CursorTextField_setColor);
			tolua_function(l, "getDesignedSize", tolua_CursorTextField_getDesignedSize);
			tolua_function(l, "setDesignedSize", tolua_CursorTextField_setDesignedSize);
			tolua_function(l, "getMaxLength", tolua_CursorTextField_getMaxLength);
			tolua_function(l, "setMaxLength", tolua_CursorTextField_setMaxLength);
			tolua_function(l, "isPassword", tolua_CursorTextField_isPassword);
			tolua_function(l, "setPassword", tolua_CursorTextField_setPassword);
			tolua_function(l, "getRect", tolua_CursorTextField_getRect);
			tolua_function(l, "setString", tolua_CursorTextField_setString);
		tolua_endmodule(l);
		tolua_constant(l, "kLuaEventKeyBack", kLuaEventKeyBack);
		tolua_constant(l, "kLuaEventKeyMenu", kLuaEventKeyMenu);
		tolua_constant(l, "kLuaEventAppEnterBackground", kLuaEventAppEnterBackground);
		tolua_constant(l, "kLuaEventAppEnterForeground", kLuaEventAppEnterForeground);
		tolua_cclass(l, "LuaEventHandler", "LuaEventHandler", "CCLayer", NULL);
		tolua_beginmodule(l, "LuaEventHandler");
			tolua_function(l, "createWithFunction", tolua_LuaEventHandler_createWithFunction);
			tolua_function(l, "createAppHandler", tolua_LuaEventHandler_createAppHandler);
		tolua_endmodule(l);
		tolua_cclass(l, "LuaTableView", "LuaTableView", "CCTableView", NULL);
		tolua_beginmodule(l, "LuaTableView");
			tolua_function(l, "createWithHandler", tolua_LuaTableView_createWithHandler);
		tolua_endmodule(l);
	tolua_endmodule(l);
	return 1;
}
