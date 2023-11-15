﻿#pragma once

class VECS_UiRenderSystem : public System
{
    friend class SystemManager;

    void Awake() override;
    void Start() override;
    void Update(float delta) override;
    void Render() override;
};
