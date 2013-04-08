#include "luaopen_LuaProxy.h"
#include "CCBProxy.h"
#include "tolua_CC_Extension.h"
#include "tolua_CCBProxy.h"
#include "tolua_CursorTextField.h"

//copyAssetFileToData(const char *src, const char *tar)
static int tolua_copyAssetFileToData(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isstring(l, 1, 0, &err) || !tolua_isstring(l, 2, 0, &err)){
		tolua_error(l,"#ferror in function 'copyAssetFileToData'.",&err);
		return 0;
	}
#endif
	CCFileUtils *fu = CCFileUtils::sharedFileUtils();
	unsigned long len = 0;
	std::string src = tolua_tostring(l, 1, NULL),
		tar = tolua_tostring(l, 2, NULL);
	if(tar.length() == 0){tar = src;}
#if COCOS2D_VERSION < 0x00020100
	src = fu->fullPathFromRelativePath(src.c_str());
	std::string p = fu->getWriteablePath();
#else
	src = fu->fullPathForFilename(src.c_str());
	std::string p = fu->getWritablePath();
#endif
	tar.insert(0, p);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	unsigned char *c = fu->getFileData(src.c_str(), "r", &len);
	if(len > 0){
		FILE *f = fopen(tar.c_str(), "w+");
		len = fwrite(c, len, 1, f) > 0;
		fclose(f);
		delete[] c;
		c = NULL;
	}
#endif
	if(len){tolua_pushstring(l, tar.c_str());}
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
// touchedNodesChild(CCNode *node, int x, int y, const char *toTypeName)
static int tolua_touchedNodesChild(lua_State *l){
#ifndef TOLUA_RELEASE
	tolua_Error err;
	if(!tolua_isusertype(l, 1, "CCNode", 0, &err) || !tolua_isnumber(l, 2, 0, &err) || !tolua_isnumber(l, 3, 0, &err)
		|| !(tolua_isnoobj(l, 4, &err) || tolua_isstring(l, 4, 0, &err))){
			tolua_error(l, "#ferror in function 'touchedNodesChild'.", &err);
			return 0;
	}
#endif
	CCNode *n = (CCNode *)tolua_tousertype(l, 1, NULL),
		*r = NULL;
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
	if(r){CCBProxy::nodeToTypeForLua(l, r, t);}
	else{	tolua_pushusertype(l, NULL, t);}
	return 1;
}

TOLUA_API int luaopen_LuaProxy(lua_State* l){
	tolua_CC_Extension_open(l);
	tolua_open(l);
	tolua_usertype(l, "CCBProxy");
	tolua_usertype(l, "CursorTextField");
	tolua_usertype(l, "LuaCallFuncInterval");
	tolua_usertype(l, "LuaEventHandler");
	tolua_usertype(l, "LuaTableView");
	tolua_module(l, NULL, 0);
	tolua_beginmodule(l, NULL);
		tolua_function(l, "copyAssetFileToData", tolua_copyAssetFileToData);
		tolua_function(l, "touchedNodesChild", tolua_touchedNodesChild);
		tolua_constant(l, "CC_TARGET_PLATFORM", CC_TARGET_PLATFORM);
		tolua_constant(l, "CC_PLATFORM_IOS", CC_PLATFORM_IOS);
		tolua_constant(l, "CC_PLATFORM_ANDROID", CC_PLATFORM_ANDROID);
		tolua_constant(l, "CC_PLATFORM_WIN32", CC_PLATFORM_WIN32);
		tolua_constant(l, "CC_PLATFORM_MAC", CC_PLATFORM_MAC);
		tolua_constant(l, "CC_PLATFORM_LINUX", CC_PLATFORM_LINUX);
		tolua_constant(l, "CC_PLATFORM_UNKNOW", CC_PLATFORM_UNKNOWN);
//		tolua_function(l, "iconv", tolua_iconv);
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
		tolua_cclass(l, "LuaCallFuncInterval", "LuaCallFuncInterval", "CCActionInterval", NULL);
		tolua_beginmodule(l, "LuaCallFuncInterval");
			tolua_function(l, "create", tolua_LuaCallFuncInterval_create);
		tolua_endmodule(l);
		tolua_cclass(l, "LuaEventHandler", "LuaEventHandler", "CCLayer", NULL);
		tolua_beginmodule(l, "LuaEventHandler");
			tolua_function(l, "create", tolua_LuaEventHandler_create);
			tolua_function(l, "createAppHandler", tolua_LuaEventHandler_createAppHandler);
			tolua_function(l, "handleHttpRequest", tolua_LuaEventHandler_handleHttpRequest);
		tolua_endmodule(l);
		tolua_cclass(l, "LuaTableView", "LuaTableView", "CCTableView", NULL);
		tolua_beginmodule(l, "LuaTableView");
			tolua_function(l, "createWithHandler", tolua_LuaTableView_createWithHandler);
			tolua_function(l, "reloadData", tolua_LuaTableView_reloadData);
			tolua_function(l, "setScrollBar", tolua_LuaTableView_setScrollBar);
			tolua_function(l, "setScrollOffset", tolua_LuaTableView_setScrollOffset);
		tolua_endmodule(l);
	tolua_endmodule(l);
	return 1;
}
