#include "variables.h"
#include "renderoptions.h"
#include "rendererinternal.h"

RendererInternal::RendererInternal() : ObjectInternal(ObjectTypeRenderer), queue_(Geometry) {
}

RendererInternal::~RendererInternal() {
	ClearRenderOptions();
}

void RendererInternal::Render(Surface surface) {
	BindRenderOptions();
	DrawCall(surface);
	UnbindRenderOptions();
}

void RendererInternal::AddOption(RenderCapacity cap, RenderParameter parameter0, RenderParameter parameter1) {
	RenderState* option = nullptr;
	switch (cap) {
		case Cull:
			option = Memory::Create<Cull>(parameter0);
			break;
		case DepthTest:
			option = Memory::Create<DepthTest>(parameter0);
			break;
		case Blend:
			option = Memory::Create<Blend>(parameter0, parameter1);
			break;
		case DepthWrite:
			option = Memory::Create<DepthWrite>(parameter0);
			break;
		default:
			Debug::LogError("invalid render capacity " + std::to_string(cap));
			break;
	}

	if (option != nullptr) {
		options_.push_back(option);
	}
}

void RendererInternal::DrawCall(Surface surface) {
	surface->Bind();
	for (int i = 0; i < surface->GetMeshCount(); ++i) {
		Mesh mesh = surface->GetMesh(i);

		for (int j = 0; j < GetMaterialCount(); ++j) {
			DrawMesh(mesh, GetMaterial(j));
		}
	}

	surface->Unbind();
}

void RendererInternal::DrawMesh(Mesh mesh, Material material) {
	MaterialTextures textures = mesh->GetMaterialTextures();

	if (textures.normal) {
		material->SetTexture(Variables::normalTexture, textures.normal);
	}

	if (textures.diffuse) {
		material->SetTexture(Variables::mainTexture, textures.diffuse);
	}

	if (textures.specular) {
		material->SetTexture(Variables::specularTexture, textures.specular);
	}

	material->Bind();
	unsigned vertexCount, baseVertex, baseIndex;
	mesh->GetTriangles(vertexCount, baseVertex, baseIndex);

	glDrawElementsBaseVertex(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned)* baseIndex), baseVertex);
	material->Unbind();
}

void RendererInternal::ClearRenderOptions() {
	for (int i = 0; i < options_.size(); ++i) {
		Memory::Release(options_[i]);
	}

	options_.clear();
}

void RendererInternal::BindRenderOptions() {
	for (int i = 0; i < options_.size(); ++i) {
		options_[i]->Bind();
	}
}

void RendererInternal::UnbindRenderOptions() {
	for (int i = 0; i < options_.size(); ++i) {
		options_[i]->Unbind();
	}
}
