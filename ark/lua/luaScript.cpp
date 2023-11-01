#include <iostream>
#include <filesystem>
#include <Windows.h>
#include "luaScript.h"

void format_table_to_string(lua_State* L, int idx, std::string& res)
{
	if (!lua_istable(L, idx))
	{
		return;
	}
	else
	{
		res.append("{");

		// ��table������ջ��
		lua_pushvalue(L, idx);
		int it = lua_gettop(L);

		// ѹ��һ��nilֵ���䵱��ʼ��key
		lua_pushnil(L);
		while (lua_next(L, it))
		{
			// ���ڵ�ջ��-1 => value; -2 => key; index => table
			// ����һ�� key ��ջ����Ȼ������� lua_tostring �Ͳ���ı�ԭʼ�� key ֵ��
			lua_pushvalue(L, -2);

			// ���ڵ�ջ��-1 => key; -2 => value; -3 => key; index => table
			if (!lua_istable(L, -2))
			{
				// ����table��ֱ��ȡ��������
				const char* key = lua_tostring(L, -1);
				char t[256] = {};
				if (lua_isboolean(L, -2))
				{
					sprintf_s(t, "%s=%s,", key, lua_toboolean(L, -2) ? "true" : "false");
				}
				else if (lua_isfunction(L, -2))
				{
					sprintf_s(t, "%s=%s,", key, "function");
				}
				else
				{
					sprintf_s(t, "%s=%s,", key, lua_tostring(L, -2));
				}
				res.append(t);

			}
			else
			{
				const char* key = lua_tostring(L, -1);
				res.append(key);
				res.append("=");

				// �˿�-2 => value
				format_table_to_string(L, -2, res);

				//lua_pop(L, 1);
				res += ',';
			}

			// ���� value �Ϳ����� key������ԭʼ�� key ��Ϊ��һ�� lua_next �Ĳ���
			lua_pop(L, 2);
			// ���ڵ�ջ��-1 => key; index => table
		}

		// ���ڵ�ջ��index => table ����� lua_next ���� 0 ��ʱ�����Ѿ�����һ�����µ� key �������ˣ�
		res.substr(0, res.length() - 1);
		res.append("}");

		// �������汻������table
		lua_pop(L, 1);
	}
}

int luax_print(lua_State* L)
{
	int n = lua_gettop(L);
	std::string strPrint;
	for (int p = 1; p <= n; p++)
	{
		if (lua_istable(L, p))
		{
			format_table_to_string(L, p, strPrint);
		}
		else if (lua_isboolean(L, p))
		{
			strPrint.append(lua_toboolean(L, p) ? "true" : "false");
		}
		else if (lua_isfunction(L, p))
		{
			strPrint.append("function");
		}
		else
		{
			strPrint.append(lua_tostring(L, p));
		}
		strPrint.append("\t");
	}

	strPrint.substr(0, strPrint.length() - 1);
	printf("%s\n", strPrint.c_str());
	return 0;
}

const struct luaL_Reg printlib[] = {
  {"print", luax_print},
  {NULL, NULL} /* end of array */
};

LuaScript::LuaScript()
{
	if (!lua_state_)
	{
		lua_state_ = luaL_newstate();
		luaL_openlibs(lua_state_);
		// �ض���print
		lua_getglobal(lua_state_, "_G");
		luaL_setfuncs(lua_state_, printlib, 0);
		lua_pop(lua_state_, 1);
	}
}

LuaScript::~LuaScript()
{
	if (!lua_state_)
	{
		lua_close(lua_state_);
	}
}

bool LuaScript::exec_from_string(std::string script)
{
	int status = luaL_dostring(lua_state_,script.c_str());
	if (status != LUA_OK)
	{
		last_error_ = lua_tostring(lua_state_,-1);
	}

	return status == LUA_OK;
}

bool LuaScript::exec_from_file(std::string filepath)
{
	if (!std::filesystem::exists(filepath))
	{
		return false;
	}

	int status = luaL_dofile(lua_state_, filepath.c_str());
	if (status != LUA_OK)
	{
		last_error_ = lua_tostring(lua_state_, -1);
	}

	return status == LUA_OK;
}