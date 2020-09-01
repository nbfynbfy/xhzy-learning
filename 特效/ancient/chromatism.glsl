const float k = 0.01;
const float dis = 0.005;
const float rand_dis = .01;

float random1 (vec2 co) {
    return 0.05*sin(mod(53.456*co.y*co.x,12.321)+20.987*co.y);
    //
}
float random2 (vec2 co) {
  float tmp = .1*sin(5.123*co.x+20.987*co.y);
    return (tmp<0.)?(tmp-rand_dis):(tmp+rand_dis);
    //
}
float rand (vec2 co) {
    return random1(co)+random2(co);
    //
}
vec4 getColor(sampler2D sampler, vec2 uv) {
  vec2 tmp = vec2(dis, 0.);
  return vec4(texture2D(sampler, uv+tmp).r,texture2D(sampler, uv).g,texture2D(sampler, uv-tmp).b,texture2D(sampler, uv).a);
}

vec2 getPos(vec2 pos) {
  return vec2(pos.x + rand(vec2(pos.y, progress))*k, pos.y);
}

vec4 transition(vec2 uv) {
  return getColor(to,getPos(uv));
}