#version 150

in vec4 in_Vertex;

out vec4 v_TexCoord0;

//

uniform vec2 quad_scale = vec2(1.0);

//

void main()
{
  vec2 pos = in_Vertex.xy;
  
  gl_Position = vec4(pos * quad_scale, 0.0, 1.0);
  
  pos = vec2(1.0) + vec2(pos.x, -pos.y);
  v_TexCoord0 = vec4(pos * 0.5, 0.0, 1.0);
}
