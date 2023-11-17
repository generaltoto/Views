#pragma once 

#include "Engine/IResourceObject.h"

class VGTexture : public IResourceObject
{
public:
	explicit VGTexture(const std::string& name);
	~VGTexture() override;

	void Load(const std::string& filepath) final;

private:
	void LoadTexture();
};