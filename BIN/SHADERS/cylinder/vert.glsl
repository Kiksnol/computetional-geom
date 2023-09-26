/* */
#version 450

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec2 InTexCoord;
layout(location = 2) in vec3 InNormal;
layout(location = 3) in vec4 InColor;

uniform mat4 MatrWVP;
uniform mat4 MatrW;

struct cylinder
{
  int I1, I2;
};

layout(std430, binding = 6) buffer Cyl
{
  cylinder C[];
};

out VS_OUT
{
  vec4 DrawColor;    
  vec2 DrawTexCoord;
  vec3 DrawNormal; 
  vec3 DrawPos;
} vs_out;   

void main( void )
{
  vec3 p0 = S[C[gl_InstanceID].I1].P,
       p1 = S[C[gl_InstanceID].I2].P;

  float 
    h = distance(p1, p0);    
                                           
  vec3 pos = vec3(InPosition.x, 
                  InPosition.y * h,
                  InPosition.z);

  vec3 
    Dir, Right, Up, 
    At = p1 - p0,
    Loc = vec3(0, h, 0.001);

  Up = normalize(At);
  Dir = normalize(cross(Loc, At));
  Right = normalize(cross(Dir, At));
  
  mat3x3 
    Matrix = mat3x3(Right, Up, -Dir),
    MatrixInvTrans = inverse(transpose(Matrix));

  pos = Matrix * pos;                                
                                                                                                      
  vs_out.DrawColor = InColor;
  vs_out.DrawTexCoord = InTexCoord;
  vs_out.DrawPos = InPosition;
  vs_out.DrawNormal = MatrixInvTrans * InNormal;
  gl_Position = MatrWVP * vec4(pos + p0, 1);  
}