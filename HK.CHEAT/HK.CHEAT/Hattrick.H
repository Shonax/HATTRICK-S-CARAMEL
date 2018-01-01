#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <intrin.h>

#include <vector>
#include <ctime>
#include <string>

#include "XorString.h"

class Hattrick
{
public:

	Hattrick(VOID)
	{
		Clear();
	};

	~Hattrick(VOID)
	{
		Clear();
	};

	VOID Clear(VOID)
	{
		m_vecParsedModules.clear();
	};

	class ModuleTuple
	{
	public:

		HMODULE m_pHandle;
		MODULEINFO m_Info;
		PVOID m_pCreateInterfaceAddress;

		std::string m_InsensitiveName;

		ModuleTuple(VOID)
		{
			Clear();
		};

		~ModuleTuple(VOID)
		{
			Clear();
		};

		VOID Clear(VOID)
		{
			ZeroMemory(&m_Info, sizeof m_Info);
			m_InsensitiveName.clear();
			m_pHandle = NULL;
			m_pCreateInterfaceAddress = NULL;
		};
	};

	std::vector <ModuleTuple> m_vecParsedModules;

	ModuleTuple * ProcessModule(std::string ModuleName)
	{
		static ModuleTuple m_Module;
		static std::vector <ModuleTuple> ::iterator m_Object;

		for (m_Object = m_vecParsedModules.begin(); m_Object != m_vecParsedModules.end(); m_Object++)
		{
			if (_stricmp((*m_Object).m_InsensitiveName.c_str(), ModuleName.c_str()) == 0)
			{
				return &(*m_Object);
			};
		};

		m_Module.m_InsensitiveName.assign(ModuleName);

		if (!(m_Module.m_pHandle = GetModuleHandle(ModuleName.c_str())))
		{
			return NULL;
		};

		ZeroMemory(&m_Module.m_Info, sizeof m_Module.m_Info);

		if (!GetModuleInformation(GetCurrentProcess(), m_Module.m_pHandle, &m_Module.m_Info, sizeof m_Module.m_Info))
		{
			return NULL;
		};

		m_Module.m_pCreateInterfaceAddress = GetProcAddress(m_Module.m_pHandle, XorString("CreateInterface"));

		m_vecParsedModules.push_back(m_Module);
		return &m_Module;
	};

	VOID ReplaceAll(std::string & String, std::string What, std::string To)
	{
		static std::string::size_type Pos = 0;

		Pos = 0;

		while ((Pos = String.find(What, Pos)) != -1)
		{
			String.replace(Pos, What.length(), To);
			Pos += To.length();
		};
	};
};
