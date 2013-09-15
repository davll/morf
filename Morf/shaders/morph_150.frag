#version 150
#define MAX_LINES 64

in vec4 v_TexCoord0;

out vec4 f_Color0;

//

uniform sampler2D inputImage;
uniform vec2 texcoordScale;
uniform vec3 algoParams; // P, A, B

uniform int linesCount;

uniform linesPQBlock
{
  uniform vec4 linesPQ[MAX_LINES];
};

uniform linesPQbarBlock
{
  uniform vec4 linesPQbar[MAX_LINES];
};

//

void main()
{
  float param_p = algoParams.x;
  float param_a = algoParams.y;
  float param_b = algoParams.z;
  
  vec2 pt_x = v_TexCoord0.xy;
  pt_x = pt_x * 2.0 - vec2(1.0);
  
  vec2 pt_sum = vec2(0.0);
  float weight_sum = 0.0;
  
  for(int i=0; i<linesCount; ++i)
  {
    vec2 v_px = pt_x - linesPQ[i].xy;
    vec2 v_pq = linesPQ[i].zw - linesPQ[i].xy;
    vec2 v_pq_r = v_pq.yx * vec2(-1.0,1.0);
    float len2_pq = dot(v_pq, v_pq);
    
    float u = dot(v_px, v_pq) / len2_pq;
    
    float v_numer = dot(v_px, v_pq_r);
    
    float dist;
    if(u >= 0.0 && u <= 1.0)
    {
      dist = abs(dot(v_px, v_pq_r)) / length(v_pq);
    }
    else
    {
      float d1 = length(v_px);
      float d2 = length(v_px - v_pq);
      dist = min(d1, d2);
    }
    
    float w_numer = pow(len2_pq, param_p * 0.5);
    float w_denom = param_a + dist;
    float weight = pow(w_numer/w_denom, param_b);
    
    vec2 pt_pbar = linesPQbar[i].xy;
    vec2 v_pqbar = linesPQbar[i].zw - linesPQbar[i].xy;
    vec2 v_pqbar_r = v_pqbar.yx * vec2(-1.0,1.0);
    
    float v = v_numer * inversesqrt(len2_pq * dot(v_pqbar, v_pqbar));
    
    vec2 pt_xbar = pt_pbar + u * v_pqbar + v * v_pqbar_r;
    
    pt_sum += weight * pt_xbar;
    weight_sum += weight;
  }
  
  vec2 tc = pt_sum / weight_sum * texcoordScale;
  tc = tc * 0.5 + vec2(0.5);
  
  f_Color0 = texture(inputImage, tc);
}
