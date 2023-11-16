#pragma once 

#include "Engine/IResourceObject.h"

class Texture : public IResourceObject
{
public:
	explicit Texture(const std::string& name);
	~Texture() override;

	void Load(const std::string& filepath) final;

private:
	void LoadTexture();
};