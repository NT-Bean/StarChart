#version 330 core
out vec4 FragColor;

in vec4 vertexColor;
uniform sampler2D tex0;

void main()
{
    FragColor = vertexColor * texture(tex0, gl_PointCoord);
    // Discard fully transparent fragments to avoid useless blending work
    if (FragColor.a <= 0.01) discard;
}