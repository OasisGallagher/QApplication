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
	virtual int GetMaterialCount() const = 0;

	virtual void SetRenderQueue(int value) = 0;
	virtual int GetRenderQueue() const = 0;

	virtual void AddOption(RenderStateType type, RenderStateParameter parameter0, RenderStateParameter parameter1 = (RenderStateParameter)None) = 0;
};

typedef smart_ptr<IRenderer> Renderer;
