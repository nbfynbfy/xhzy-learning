const float dis = 0.004;
const float k = 0.3;

float rand (vec2 co) {
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float random(vec2 uv, float f) {
  return rand(vec2(rand(uv),f));
}

vec4 getColor(sampler2D sampler, vec2 uv) {
  vec4 color1 = texture2D(sampler, uv);
  vec4 color2;
  float tmp = mod(uv.y,dis*5.)/dis;
  if (tmp<2.) { color2 = vec4(1.,1.,1.,1.); }
  else if (tmp<3.) { color2 = vec4(1.,0.,0.,1.);}
  else if (tmp<4.) { color2 = vec4(0.,1.,0.,1.);}
  else if (tmp<5.) { color2 = vec4(0.,0.,1.,1.);}
  if (random(uv,progress)<0.9) {
    color1 = mix(color1,color2,k);
  }
  return color1;
}

vec4 transition(vec2 uv) {
  return getColor(to, uv);
}