#include "surface.h"
#include "renderer.h"
#include "internal/base/objectinternal.h"

class RenderState;

class RendererInternal : virtual public IRenderer, public ObjectInternal {
public:
	RendererInternal(ObjectType type);
	~RendererInternal();

public:
	virtual void Render(Surface surface);

	virtual void AddMaterial(Material material) { materials_.push_back(material); }
	virtual Material GetMaterial(int index) { return materials_[index]; }
	virtual void SetMaterial(int index, Material value) { materials_[index] = value; }
	virtual int GetMaterialCount() { return materials_.size(); }

	virtual int GetRenderQueue() { return queue_; }
	virtual void SetRenderQueue(int value) { queue_ = value; }

	virtual void SetRenderState(RenderStateType type, RenderStateParameter parameter0, RenderStateParameter parameter1);

private:
	void DrawCall(Surface surface);
	void DrawMesh(Mesh mesh, Material material);

	void BindRenderStates();
	void UnbindRenderStates();

private:
	int queue_;
	RenderState** states_;
	std::vector<Material> materials_;
};

class SurfaceRendererInternal : public ISurfaceRenderer, public RendererInternal {
	DEFINE_FACTORY_METHOD(SurfaceRenderer)

public:
	SurfaceRendererInternal() : RendererInternal(ObjectTypeSurfaceRenderer) {}
};

class SkinnedSurfaceRendererInternal : public ISkinnedSurfaceRenderer, public RendererInternal {
	DEFINE_FACTORY_METHOD(SkinnedSurfaceRenderer)

public:
	SkinnedSurfaceRendererInternal() : RendererInternal(ObjectTypeSkinnedSurfaceRenderer) {}

public:
	virtual void Render(Surface surface);
	virtual void SetSkeleton(const Skeleton* value) { skeleton_ = value; }

private:
	const Skeleton* skeleton_;
};
