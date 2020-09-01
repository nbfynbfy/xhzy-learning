// Author: nbfynbfy
// License: MIT

const vec2 direct_up = vec2(0.0, -1.0);
const vec2 direct_down = vec2(0.0, 1.0);
const vec2 direct_left = vec2(1.0, 0.0);
const vec2 direct_right = vec2(-1.0, 0.0);

vec2 getMirrorBottom(vec2 tmp) { return vec2(tmp.x, -tmp.y); }
vec2 getMirrorUp(vec2 tmp) { return vec2(tmp.x, 2.-tmp.y); }
vec2 getMirrorLeft(vec2 tmp) { return vec2(-tmp.x, tmp.y); }
vec2 getMirrorRight(vec2 tmp) { return vec2(2.-tmp.x, tmp.y); }
vec2 getMirrorLeftUp(vec2 tmp) { return getMirrorLeft(getMirrorUp(tmp)); }
vec2 getMirrorLeftBottom(vec2 tmp) { return getMirrorLeft(getMirrorBottom(tmp)); }
vec2 getMirrorRightUp(vec2 tmp) { return getMirrorRight(getMirrorUp(tmp)); }
vec2 getMirrorRightBottom(vec2 tmp) { return getMirrorRight(getMirrorBottom(tmp)); }

vec2 AddMoveUp(vec2 tmp, float progress) { return vec2(progress*direct_up.x + tmp.x, progress*direct_up.y + tmp.y); }
vec2 AddMoveDown(vec2 tmp, float progress) { return vec2(progress*direct_down.x + tmp.x, progress*direct_down.y + tmp.y); }
vec2 AddMoveLeft(vec2 tmp, float progress) { return vec2(progress*direct_left.x + tmp.x, progress*direct_left.y + tmp.y); }
vec2 AddMoveRight(vec2 tmp, float progress) { return vec2(progress*direct_right.x + tmp.x, progress*direct_right.y + tmp.y); }
vec2 AddMoveRightDown(vec2 tmp, float progress) { return AddMoveRight(AddMoveDown(tmp, progress), progress); }

float quarticInOut(float t) {
  return t < 0.5
  ? +8.0 * pow(t, 4.0)
  : -8.0 * pow(t - 1.0, 4.0) + 1.0;
}

float rand (vec2 co) {
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 linerBlur(sampler2D texture, vec2 texCoord, float progress, int key) {
    //vec2 texCoord = uv;

    float strength = 0.08*progress; // 模糊程度    
    float dissolve = 1.0;

    float offset = rand(texCoord);
    vec2 direct;
    if (key == 5) { direct = vec2(1., 1.); }
    else if (key == 6) { direct = vec2(1. , -1.); }
    else if (key == 7) { direct = vec2(-1., 1.); }
    else if (key == 8) { direct = vec2(-1., -1.); }
    float total = 0.0;
    vec3 color = vec3(0.0);
    for (float t = 0.0; t <= 40.0; t++) {
        float percent = (t + offset) / 40.0;
        float weight = percent * percent + 1.;
        vec2 fade = texCoord +  direct * percent * strength;
        color += texture2D(texture, fade).rgb * weight;
        total += weight;
    }
    return vec4(color / total, 1.0);
}
//  key:
//  0: 0,0
//  1: 0,1
//  2: 0,-1
//  3: 1,0
//  4: -1,0
//  5: 1,1
//  6: 1,-1
//  7: -1,1
//  8: -1,-1



vec4 transition (vec2 uv) {
  float a = quarticInOut(progress);
  if (a<0.5) {
    if (uv.y > 1.-a*2.0 && uv.x > a*2.){
      return linerBlur(from, (getMirrorUp(AddMoveRightDown(uv, a*2.0))), a*2., 5);
      //  up
    } else if (uv.y < 1.- a*2.0 && uv.x > a*2.) {
      return linerBlur(from, (AddMoveRightDown(uv, a*2.0)), a*2., 6);
      //  self
    } else if (uv.y < 1.- a*2.0 && uv.x < a*2.) {
      return linerBlur(from, (getMirrorLeft(AddMoveRightDown(uv, a*2.0))), a*2., 8);
      //  left
    } else if (uv.y > 1.- a*2.0 && uv.x < a*2.) {
      return linerBlur(from, (getMirrorLeftUp(AddMoveRightDown(uv, a*2.0))), a*2., 7);
      //  left-up
    }
  } else {
    if (uv.y > 1.-(a-0.5)*2.0 && uv.x > (a-0.5)*2.){
      return linerBlur(to, (getMirrorRight(AddMoveRightDown(uv, (a-1.)*2.0))), (1.0-a)/0.5, 5);
      //  right
    } else if (uv.y < 1.- (a-0.5)*2.0 && uv.x > (a-0.5)*2.) {
      return linerBlur(to, (getMirrorRightBottom(AddMoveRightDown(uv, (a-1.)*2.0))), (1.0-a)/0.5, 6);
      //  right-bottom
    } else if (uv.y < 1.- (a-0.5)*2.0 && uv.x < (a-0.5)*2.) {
      return linerBlur(to, (getMirrorBottom(AddMoveRightDown(uv, (a-1.)*2.0))), (1.0-a)/0.5, 8);
      //  bottom
    } else if (uv.y > 1.- (a-0.5)*2.0 && uv.x < (a-0.5)*2.) {
      return linerBlur(to, (AddMoveRightDown(uv, (a-1.)*2.0)), (1.0-a)/0.5, 7);
      //  self
    }
  }
}