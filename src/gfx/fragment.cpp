static constexpr const char *FRAGMENT_SOURCE = R"frag(#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;

void main()
{
    color = texture(image, TexCoords);
}
)frag";