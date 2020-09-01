const vec4 black = vec4(0., 0., 0., 1.);
const vec2 center = vec2(0.5, 0.5);
const float secondspertime = 1.5;
const float begin = 1./3.;
const float end = 2./3.;

//  在椭圆中
float calEllipse(vec2 tmp, float a, float b) { return (tmp.x-center.x)*(tmp.x-center.x)/a/a+(tmp.y-center.y)*(tmp.y-center.y)/b/b; }
float exchange(float tmp) {
	const float delta = 0.01;
	if (tmp < 1.-delta) {
		return 0.;
	} else if (tmp > 1.+delta) {
		return 1.;
	} else {
		return (tmp-1.+delta)/delta/2.;
	}
}

vec4 transition(vec2 uv) {
	vec4 color;
	float Radius = sqrt(0.5);
	float T = mod(time, secondspertime) / secondspertime;
	if (T < begin || T > end) {
		color = texture2D(to, uv);
	} else {
		T = (T-begin)/(end-begin);
		float radius = mix(0., Radius, abs(T*2.-1.));
		color = mix(texture2D(to, uv), black, exchange(calEllipse(uv, Radius, radius)));
	}
	return color;
}