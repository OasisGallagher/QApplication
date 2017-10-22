#pragma once

#include "object.h"
#include "surface.h"
#include "material.h"

enum RenderQueue {
	Background = 1000,
	Geometry = 2000,
	Transparent = 4000,
};

enum RenderStateType {
	Cull,
	Blend,
	DepthTest,
	DepthWrite,
	RenderStateCount,
};

enum RenderStateParameter {
	None,
	Front,
	Back,
	On,
	Off,

	Never,
	Less,
	LessEqual,
	Equal,
	Greater,
	NotEqual,
	GreaterEqual,
	Always,

	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DestAlpha,
	OneMinusDestAlpha,
};

class ENGINE_EXPORT IRenderer : virtual public IObject {
public:
	virtual void Render(Surface surface) = 0;
	
	virtual void AddMaterial(Material material) = 0;
	virtual Material GetMaterial(int index) = 0;
	virtual void SetMaterial(int index, Material value) = 0;
	virtual int GetMaterialCount() = 0;

	virtual void SetRenderQueue(int value) = 0;
	virtual int GetRenderQueue() = 0;

	/**
	 * @param Cull: Front, Back, Off
	 * @param DepthTest: Never, Less, LessEqual, Equal, Greater, NotEqual, GreaterEqual, Always
	 * @param DepthWrite: On, Off
	 * @param Blend0: Off, Zero, One, SrcColor, OneMinusSrcColor, SrcAlpha, OneMinusSrcAlpha, DestAlpha, OneMinusDestAlpha
	 * @param Blend1: None, Zero, One, SrcColor, OneMinusSrcColor, SrcAlpha, OneMinusSrcAlpha, DestAlpha, OneMinusDestAlpha
	 */
	virtual void SetRenderState(RenderStateType type, RenderStateParameter parameter0, RenderStateParameter parameter1 = (RenderStateParameter)None) = 0;
};

class ENGINE_EXPORT ISurfaceRenderer : virtual public IRenderer {

};

struct Skeleton;

class ENGINE_EXPORT ISkinnedSurfaceRenderer : virtual public IRenderer {
public:
	virtual void SetSkeleton(const Skeleton* value) = 0;
};

typedef std::shared_ptr<IRenderer> Renderer;
typedef std::shared_ptr<ISurfaceRenderer> SurfaceRenderer;
typedef std::shared_ptr<ISkinnedSurfaceRenderer> SkinnedSurfaceRenderer;
