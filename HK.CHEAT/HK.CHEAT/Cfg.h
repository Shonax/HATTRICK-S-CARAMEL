#pragma once

#include "Misc.h"

class CConfig
{
public:

	std::vector <CFGVal<INT> *> m_pInts;
	std::vector <CFGVal<bool> *> m_pBools;
	std::vector <CFGVal<FLOAT> *> m_pFloats;

	VOID SetupVal(INT &, INT, std::string, std::string);
	VOID SetupVal(bool &, bool, std::string, std::string);
	VOID SetupVal(FLOAT &, FLOAT, std::string, std::string);

	CConfig(VOID)
	{
		Setup();
		Load();
	};

	VOID Setup(VOID);
	VOID Save(VOID);
	VOID Load(VOID);
	VOID LoadLegitBase(VOID);
	VOID LoadRageBase(VOID);
};
