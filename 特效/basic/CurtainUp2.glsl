const vec4 black = vec4(0.) ;

vec4 show(vec2 uv, float progress) { return (uv.x < 0.5+progress/2. && uv.x > 0.5-progress/2.)?vec4(1.):vec4(0.); }

vec4 transition(vec2 uv) {
	vec4 color = mix(black, texture2D(to, uv), show(uv, progress));
	return color;
}
