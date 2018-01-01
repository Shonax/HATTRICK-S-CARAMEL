#pragma once

#include "Misc.h"

class Vector;
class QAngle;
class GameTrace_T;
class CUtlBuffer;
class Quaternion;

struct Model_T;
struct Plane_T;
struct StdHdr_T;
struct Matrix34_T;

class Quaternion
{
public:

	FLOAT m_pUnused[4];
};

struct StdHBoxSet_T
{
	INT iNameID;
	INT iHBoxes;
	INT iHBoxID;
	StdioBBox_T * pHBox(INT iHBoxNum) CONST
	{
		return (StdioBBox_T *)(((PBYTE) this) + iHBoxID) + iHBoxNum;
	};
};

struct StdBone_T
{
	INT iNameID;
	INT iParent;
	INT pBoneController[6];
	Vector vecPos;
	Quaternion qUnusedA;
	Vector vecRot;
	Vector vecPosScale;
	Vector vecRotscale;
	Matrix34_T vecPoseToBone;
	Quaternion qUnusedB;
	INT iFlags;
	INT iProcType;
	INT iProcID;
	mutable INT iPhysBone;
	INT pUnused[10];
};

struct StdHdr_T
{
	INT iID;
	INT iVersion;
	LONG lChecksum;
	CHAR szName[64];
	INT iLen;
	Vector vecEyePos;
	Vector vecIllumPos;
	Vector vecHullMin;
	Vector vecHullMax;
	Vector vecViewMin;
	Vector vecViewMax;
	INT iFlags;
	INT iNumBones;
	INT iBoneID;
	StdBone_T * pBone(INT iId) CONST
	{
		return (StdBone_T *)(((PBYTE) this) + iBoneID) + iId;
	};
	INT iNumBoneControllers;
	INT iBoneControllerID;
	INT iNumHBoxSets;
	INT iHBoxSetID;
	StdHBoxSet_T * pHBoxSet(INT iId) CONST
	{
		return (StdHBoxSet_T *)(((PBYTE) this) + iHBoxSetID) + iId;
	};
	INT iNumLocalAnim;
	INT iLocalAnimID;
	INT iNumLocalSeq;
	INT iLocalSeqID;
	mutable INT iActivityListVersion;
	mutable INT iIndexedEvents;
	INT iNumTextures;
	INT iTextureID;
	INT iNumCDTextures;
	INT iCDTextureID;
	INT iNumSkinRef;
	INT iNumSkinFamilies;
	INT iSkinID;
	INT iNumBodyParts;
	INT iBodyPartID;
};

class IVModelInfo
{
public:

	virtual ~IVModelInfo(void) { };
	virtual const Model_T * GetModel(int) const = 0;
	virtual int GetModelIndex(const char *) const = 0;
	virtual const char * GetModelName(const Model_T *) const = 0;
	virtual VOID *GetVCollide(const Model_T *) const = 0;
	virtual VOID *GetVCollide(int) const = 0;
	virtual void GetModelBounds(const Model_T *, Vector &, Vector &) const = 0;
	virtual	void GetModelRenderBounds(const Model_T *, Vector &, Vector &) const = 0;
	virtual int GetModelFrameCount(const Model_T *) const = 0;
	virtual int GetModelType(const Model_T *) const = 0;
	virtual void *GetModelExtraData(const Model_T *) = 0;
	virtual bool ModelHasMaterialProxy(const Model_T *) const = 0;
	virtual bool IsTranslucent(Model_T const *) const = 0;
	virtual bool IsTranslucentTwoPass(const Model_T *) const = 0;
	virtual void Unused0(void) { };
	virtual INT ComputeTranslucencyType(const Model_T *, int, int) = 0;
	virtual int GetModelMaterialCount(const Model_T *) const = 0;
	virtual void GetModelMaterials(const Model_T *, int, VOID **) = 0;
	virtual bool IsModelVertexLit(const Model_T *) const = 0;
	virtual const char *GetModelKeyValueText(const Model_T *) = 0;
	virtual bool GetModelKeyValue(const Model_T *, CUtlBuffer &) = 0;
	virtual float GetModelRadius(const Model_T *) = 0;
	virtual const StdHdr_T * FindModel(const StdHdr_T *, void **, const char *) const = 0;
	virtual const StdHdr_T * FindModel(void *) const = 0;
	virtual	VOID *GetVirtualModel(const StdHdr_T *) const = 0;
	virtual PBYTE GetAnimBlock(const StdHdr_T *, int) const = 0;
	virtual bool HasAnimBlockBeenPreloaded(StdHdr_T const *, int) const = 0;
	virtual void GetModelMaterialColorAndLighting(const Model_T *, Vector const &, QAngle const &, GameTrace_T *, Vector &, Vector &) = 0;
	virtual void GetIlluminationPoint(const Model_T *, VOID *, Vector const &, QAngle const &, Vector *) = 0;
	virtual int GetModelContents(int) const = 0;
	virtual StdHdr_T * GetStudioModel(const Model_T *) = 0;
	virtual int GetModelSpriteWidth(const Model_T *) const = 0;
	virtual int GetModelSpriteHeight(const Model_T *) const = 0;
	virtual void SetLevelScreenFadeRange(float, float) = 0;
	virtual void GetLevelScreenFadeRange(float *, float *) const = 0;
	virtual void SetViewScreenFadeRange(float, float) = 0;
	virtual unsigned char ComputeLevelScreenFade(const Vector &, float, float) const = 0;
	virtual unsigned char ComputeViewScreenFade(const Vector &, float, float) const = 0;
	virtual int GetAutoplayList(const StdHdr_T *, unsigned short **) const = 0;
	virtual void *GetCollideForVirtualTerrain(int) = 0;
	virtual bool IsUsingFBTexture(const Model_T *, int, int, void *) const = 0;
	virtual const Model_T *FindOrLoadModel(const char *) const = 0;
	virtual USHORT GetCacheHandle(const Model_T *) const = 0;
	virtual int GetBrushModelPlaneCount(const Model_T *) const = 0;
	virtual void GetBrushModelPlane(const Model_T *, int, Plane_T &, Vector *) const = 0;
	virtual int GetSurfacepropsForVirtualTerrain(int) = 0;
	virtual bool UsesEnvCubemap(const Model_T *) const = 0;
	virtual bool UsesStaticLighting(const Model_T *) const = 0;
};
