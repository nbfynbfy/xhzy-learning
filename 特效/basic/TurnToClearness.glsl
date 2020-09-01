// start

const vec4 black = vec4(0., 0., 0., 1.);
const vec2 center = vec2(.5, .5);

vec4 show(vec4 color, float progress) { return (progress<0.5)?mix(black, color, progress*2.):color; }
vec2 bigger(vec2 pos, float porgress) {
	float k = -.4*(progress-.5)*(progress-.5)+1.1;
	return (center+(pos-center)/k);
}
vec4 dim(sampler2D sampler, vec2 pos, float progress) {
	float k = -.2*(progress-.5)*(progress-.5)+.05;
	float total = 0.;
	vec3 ans_color = vec3(0.);
	for (float dx=0.;dx<3.;dx++) for (float dy=0.;dy<3.;dy++) {
		total += 1.;
		vec2 tmp = pos+vec2(k*dx/10., k*dy/10.);
		ans_color += texture2D(sampler, tmp).rgb;
	}
	ans_color /= total;
	return vec4(ans_color, 1.);
}


vec4 transition(vec2 uv) {
  vec2 pos = bigger(uv,progress);
	return show(dim(to, pos, progress),progress);
}