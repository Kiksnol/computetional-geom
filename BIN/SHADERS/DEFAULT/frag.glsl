/* */

// shader language version (4.2)
#version 420

in vec4 DrawColor;   
in vec2 DrawTexCoord;
in vec3 DrawNormal;  
in vec3 DrawPos;  
   
//painting color
layout(location = 0) out vec4 OutColor;

//uniform mat4 MatrWVP;
//uniform float Time;
uniform vec3 Kd, Ks, Ka;
uniform float Ph;
uniform vec3 CamLoc;
 
layout(binding = 0) uniform sampler2D Texture0;
uniform bool IsTexture0;
 
vec3 Shade( vec3 P, vec3 N, vec3 L, vec3 LColor )
{
  vec3 V = normalize(P - vec3(1, 3, 2));
  vec3 color = vec3(0);

  // Reverse normal if need
  N = faceforward(N, V, N);

  // Ambient
  color += max(Ka, vec3(0.5));

  // Duffuse
  float nl = dot(N, L);
  nl = max(nl, 0);
  vec3 diff = LColor * Kd * nl;
  if (IsTexture0)
    diff *= texture(Texture0, DrawTexCoord).bgr;
  color += diff;
 
  // Specular
  vec3 R = reflect(V, N);
  float rl = max(dot(R, L), 0);
  color += rl * LColor * Ks * pow(rl, Ph);

  return color;
}

void main( void )
{
  vec3 N = normalize(DrawNormal);
  vec3 L = normalize(vec3(1, 1, 1));
  float dst = distance(DrawPos, CamLoc);

  if (dst > 300)
    discard;
  else if (CamLoc.y > 25 || dst > 200)
  {
    if (mod(DrawPos.x, 3) < 0.15 || mod(DrawPos.z, 3) < 0.15)
      OutColor = 3 * vec4(1) / log(distance(DrawPos, CamLoc) + 1);
    else
      discard;
  }
  else
  {
    if (mod(DrawPos.x, 1) < 0.1 || mod(DrawPos.z, 1) < 0.1)
      OutColor = 3 * vec4(1) / log(distance(DrawPos, CamLoc) + 1);
    else
      discard;
  }
//    OutColor = vec4(vec3(0.5), 1);//(Shade(DrawPos, N, L, vec3(0.66)), 1);
}