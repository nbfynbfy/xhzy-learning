const vec4 black = vec4(0., 0., 0., 1.);
const vec4 half_black = vec4(0., 0., 0., .5);
const float secondspertime = 0.75;
const float min_pos1 = 1./8.;
const float min_pos2 = 1./8.;

// 计算相对距离
float cal_distance(float pos, float T, float deadline) { return (abs(.5-pos)-(.5-deadline))/(min_pos2+.3); }

// 缓动函数
float func(float tmp) { return (tmp-1.)*(tmp-1.); }


vec4 transition(vec2 uv) {
    float a = mod(time, secondspertime);
    float T = func(abs(2.*(a/secondspertime)-1.));  //  在一个周期中的相对时间

    vec4 color;
    if (uv.y < min_pos1*T || uv.y > 1.-min_pos1*T) {
        color = black;
    } else  {
        color = texture2D(to, uv);
        if (uv.x < min_pos2*T || uv.x > 1.-min_pos2*T) {
            color = mix(half_black, color, 1.-cal_distance(uv.x, T, T*min_pos2));
        }
        if (uv.y < (min_pos1+min_pos2)*T || uv.y > 1.-(min_pos1+min_pos2)*T) {
            color = mix(half_black, color, 1.-cal_distance(uv.y, T, (min_pos1+min_pos2)*T));
        }
    }
    return color;
}