#pragma once
#ifndef __TOLUA_CC_EXTENSION_H__
#define __TOLUA_CC_EXTENSION_H__

extern "C" {
#include "tolua++.h"
#include "tolua_fix.h"
}

#include "tolua_fix.h"

TOLUA_API int tolua_CC_Extension_open(lua_State* tolua_S);

#endif // __TOLUA_CC_EXTENSION_H__
