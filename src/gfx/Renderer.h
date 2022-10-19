#pragma once

class Renderer
{
public:
    Renderer()
    : m_screenBuffer(0x30000, 128)
    {
    }

    void init();
    //void updateProjection(int width, int height);
    //void draw();
    void updateMainWindowTexture();

    std::vector<uint8_t>& screenBuffer()
    {
        return m_screenBuffer;
    }

    uint32_t getMainTextureID() const
    {
        return mainwindowTextureId;
    }

    uint64_t getUpdateTick() const 
    {
        return updateTick;
    }

private:
	static constexpr const char* UNIFORM_PROJECTION = "projection";
	static constexpr const char* UNIFORM_MODEL = "model";
	static constexpr const char* UNIFORM_IMAGE = "image";

    void initVertexBuffers();
    void initMainWindowTexture();

    void createProgramAndShaders();
    void initUniforms();

    std::vector<uint8_t> m_screenBuffer;
    uint32_t quadVAO;
	uint32_t mainwindowTextureId;
	uint32_t program;
    uint64_t updateTick{0};
};
