#version 150

in vec4 v_TexCoord0;

out vec4 f_Color0;

//

uniform sampler2D fboTexture;

//

void main()
{
  f_Color0 = texture(fboTexture, v_TexCoord0.xy);
}
