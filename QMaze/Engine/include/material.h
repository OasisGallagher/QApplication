#include <memory>

class Shader;
class Texture;

class Material {
public:
	Material();
	~Material();

public:
	void SetTexture(Texture* texture);
	Texture* GetTexture();

private:
	Shader* shader_;
	Texture* texture_;
};
