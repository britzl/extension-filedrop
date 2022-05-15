#include "filedrop.h"
#include <dmsdk/sdk.h>

#define LIB_NAME "FileDrop"
#define MODULE_NAME "filedrop"
#define DLIB_LOG_DOMAIN LIB_NAME

#if defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_OSX)

struct FileDrop
{
	FileDrop()
	{
		memset(this, 0, sizeof(*this));
	}
	dmScript::LuaCallbackInfo* m_Callback;
};

static FileDrop g_FileDrop;

static void FileDrop_OnEventListener(const char* event, const char* filename, const uint8_t* data, int data_length)
{
	if (g_FileDrop.m_Callback == 0)
	{
		return;
	}

	lua_State* L = dmScript::GetCallbackLuaContext(g_FileDrop.m_Callback);

	DM_LUA_STACK_CHECK(L, 0);

	if (!dmScript::IsCallbackValid(g_FileDrop.m_Callback))
	{
		g_FileDrop.m_Callback = 0;
		return;
	}

	if (!dmScript::SetupCallback(g_FileDrop.m_Callback))
	{
		dmScript::DestroyCallback(g_FileDrop.m_Callback);
		g_FileDrop.m_Callback = 0;
		return;
	}

	lua_pushstring(L, event);
	if (filename != 0)
	{
		lua_pushstring(L, filename);
	}
	else
	{
		lua_pushnil(L);
	}
	if (data != 0)
	{
		lua_pushlstring(L, (const char*)data, data_length);
	}
	else
	{
		lua_pushnil(L);
	}
	dmScript::PCall(L, 4, 0);

	dmScript::TeardownCallback(g_FileDrop.m_Callback);
}

static int FileDrop_SetListener(lua_State* L)
{
	DM_LUA_STACK_CHECK(L, 0);

	if (g_FileDrop.m_Callback)
	{
		dmScript::DestroyCallback(g_FileDrop.m_Callback);
	}

	g_FileDrop.m_Callback = dmScript::CreateCallback(L, 1);

	FileDrop_PlatformSetEventListener((OnEventCallback)FileDrop_OnEventListener);
	return 0;
}


static const luaL_reg Module_methods[] =
{
	{"set_listener", FileDrop_SetListener},
	{0, 0}
};

static void LuaInit(lua_State* L)
{
	int top = lua_gettop(L);
	luaL_register(L, MODULE_NAME, Module_methods);

	lua_pop(L, 1);
	assert(top == lua_gettop(L));
}
#endif

dmExtension::Result AppInitializeFileDropExtension(dmExtension::AppParams* params)
{
	return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeFileDropExtension(dmExtension::Params* params)
{
	#if defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_OSX)
	LuaInit(params->m_L);
	#else
	printf("Extension %s is not supported\n", MODULE_NAME);
	#endif
	return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeFileDropExtension(dmExtension::AppParams* params)
{
	return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeFileDropExtension(dmExtension::Params* params)
{
	#if defined(DM_PLATFORM_HTML5) || defined(DM_PLATFORM_OSX)
	if (g_FileDrop.m_Callback != 0)
	{
		dmScript::DestroyCallback(g_FileDrop.m_Callback);
		g_FileDrop.m_Callback = 0;
	}
	FileDrop_Finilize();
	#endif
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(FileDrop, LIB_NAME, AppInitializeFileDropExtension, AppFinalizeFileDropExtension, InitializeFileDropExtension, 0, 0, FinalizeFileDropExtension)
