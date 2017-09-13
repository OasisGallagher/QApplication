#include "object.h"

enum RenderCapacity {
	RC_Cull,
	RC_Blend,
	RC_DepthTest,
};

enum RenderParameter {
	None,
	Front,
	Back,
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
	virtual void Render() = 0;
	virtual void SetSurface(Surface surface) = 0;
	
	virtual void AddMaterial(Material material) = 0;
	virtual Material GetMaterial(int index) = 0;
	virtual void SetMaterial(int index, Material value) = 0;
	virtual int GetMaterialCount() const = 0;

	virtual void AddOption(RenderCapacity cap, RenderParameter parameter0, RenderParameter parameter1 = (RenderParameter)None) = 0;
};

typedef smart_ptr<IRenderer> Renderer;
