#include "variables.h"
#include "renderoptions.h"
#include "rendererinternal.h"

RendererInternal::RendererInternal() : ObjectInternal(ObjectTypeRenderer) {
}

RendererInternal::~RendererInternal() {
	ClearRenderOptions();
}

void RendererInternal::Render() {
	if (!surface_) {
		Debug::LogError("invalid surface");
		return;
	}

	BindRenderOptions();
	DrawCall();
	UnbindRenderOptions();
}

void RendererInternal::SetSurface(Surface surface) {
	surface_ = surface;
}

void RendererInternal::AddOption(RenderCapacity cap, RenderParameter parameter0, RenderParameter parameter1) {
	RenderOption* option = nullptr;
	switch (cap) {
		case RC_Cull:
			option = Memory::Create<Cull>(parameter0);
			break;
		case RC_DepthTest:
			option = Memory::Create<DepthTest>(parameter0);
			break;
		case RC_Blend:
			option = Memory::Create<Blend>(parameter0, parameter1);
			break;
	}

	options_.push_back(option);
}

void RendererInternal::DrawCall() {
	surface_->Bind();
	for (int i = 0; i < surface_->GetMeshCount(); ++i) {
		Mesh mesh = surface_->GetMesh(i);

		for (int j = 0; j < GetMaterialCount(); ++j) {
			DrawMesh(mesh, GetMaterial(j));
		}
	}

	surface_->Unbind();
}

void RendererInternal::DrawMesh(Mesh mesh, Material material) {
	MeshTextures textures = mesh->GetTextures();

	if (textures.normal) {
		material->SetTexture(Variables::normalTexture, textures.normal);
	}

	if (textures.diffuse) {
		material->SetTexture(Variables::diffuseTexture, textures.diffuse);
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
