const vec4 black = vec4(0., 0., 0., 1.);
const vec4 purple = vec4(0.22656, 0., 0.53125, 1.);
const vec2 center = vec2(0.5, 0.5);
const float loop_radius = 0.04;
//  包含能屏幕最大的半径

//  是否在圆里
bool inCircle(vec2 tmp, float radius) { return ((tmp.x-center.x)*(tmp.x-center.x)+(tmp.y-center.y)*(tmp.y-center.y) < radius*radius && radius > 0.)?true:false; }

//  圆心距
float distance_center(vec2 tmp) { return sqrt((tmp.x-center.x)*(tmp.x-center.x)+(tmp.y-center.y)*(tmp.y-center.y)); }

//  缓动函数
float func(float progress) { return progress * progress; }

//  择色
vec4 chooseColor(vec2 tmp, float real_r) {
	vec4 color;
	if (inCircle(tmp, real_r - loop_radius)) {
		color = texture2D(to, tmp);
	} else if (inCircle(tmp, real_r)) {
		color = mix(texture2D(to, tmp), purple, (distance_center(tmp)-real_r+loop_radius)/loop_radius);
		if (!inCircle(tmp, real_r-loop_radius/2.)) {
			color = mix(color, black, (distance_center(tmp)-real_r+loop_radius/2.)/loop_radius*2.);
		}
	} else {
		color = black;
	}
	return color;
}

vec4 transition(vec2 uv) {
	float R = sqrt(0.5);
	vec4 color;
	if (progress < 0.25) {
		float a = func(progress/0.25);
		float real_r = mix(R+loop_radius, (R+loop_radius)/4., a);
		color = chooseColor(uv, real_r);
	} else if ( progress > 0.75) {
		float a = (progress-0.75)/0.25;
		float real_r = mix((R+loop_radius)/4., 0., a);
		color = chooseColor(uv, real_r);
	} else {
		float real_r = (R+loop_radius)/4.;
		color = chooseColor(uv, real_r);
	}
	return color;
} 