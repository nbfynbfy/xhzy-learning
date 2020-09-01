const float k = 0.01;

float rand (vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 random2(vec2 co) {
    float tmp1 = rand(co/4.);
    float tmp2 = rand(co*3./4.);
    return vec2(tmp1, tmp2);
}

vec2 getTrans(vec2 co) {
    vec2 tmp = random2(co);
    return tmp*2.;
}

vec4 transition(vec2 uv) {
    vec2 delta = getTrans(vec2(progress, progress));
    delta = vec2(delta.x*k, delta.y*k);
    vec2 pos = vec2((uv.x+delta.x)/(1.+k), (uv.y+delta.y)/(1.+k));
    return texture2D(to, pos);
}