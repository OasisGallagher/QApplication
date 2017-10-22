#include <map>

#include "surface.h"
#include "renderer.h"
#include "animation.h"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiNodeAnim;
struct aiAnimation;

class ModelImporter {
public:
	enum {
		MaskNone = 0,
		MaskImportAnimation = 1,
		MaskCreateRenderer = 2,
		MaskAll = -1,
	};

public:
	bool Import(const std::string& path, int mask = MaskAll);
	
public:
	Surface GetSurface() { return surface_; }
	Renderer GetRenderer() { return renderer_; }
	Animation GetAnimation() { return animation_; }

private:
	void Clear();

	bool ImportSurface(Surface& surface);
	void ImportTextures(MaterialTextures* textures);
	void ImportMeshAttributes(const aiMesh* aimesh, int nm, SurfaceAttribute& attribute);
	void ImportSurfaceAttributes(Surface surface, SurfaceAttribute& attribute, MaterialTextures* textures);
	void ImportBoneAttributes(const aiMesh* aimesh, int nm, Surface surface, SurfaceAttribute& attribute);

	bool ImportAnimation(Animation& animation);
	void ImportAnimationClip(const aiAnimation* anim, AnimationClip clip);
	const aiNodeAnim * FindChannel(const aiAnimation* anim, const char* name);

	bool InitRenderer(Animation animation, Renderer& renderer);

private:
	Skeleton skeleton_;
	std::string path_;
	const aiScene* scene_;
	std::map<std::string, int> boneMap_;

	Surface surface_;
	Renderer renderer_;
	Animation animation_;
};
