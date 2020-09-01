const vec4 black = vec4(0., 0., 0., 1.) ;

float show(vec2 pos, float t) { return (pos.y < 0.5+t/2.0 && pos.y > 0.5-t/2.0)?1.:0.; }

vec4 transition(vec2 uv) {
	vec4 color = mix(black, texture2D(to, uv), show(uv, progress));
	return color;
}
