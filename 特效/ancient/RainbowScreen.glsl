const float k_line = -0.5;
const float deltab_line = 0.02;
const vec4 red = vec4(1., 0., 0., 1.);
const vec4 green = vec4(0., 1., 0., 1.);
const vec4 blue = vec4(0., 0., 1., 1.);
const float k = 0.05;
const float k_progress = 1.;

vec4 GetRainbowColor(vec2 pos) {
	float tmp = mod(pos.y-k_line*pos.x+progress*k_progress, deltab_line*3.);
	if (tmp < deltab_line) {
		return mix(red, green, tmp/deltab_line);
	} else if (tmp < 2.*deltab_line) {
		return mix(green, blue, (tmp-deltab_line)/deltab_line);
	} else {
		return mix(blue, red, (tmp-2.*deltab_line)/deltab_line);
	}
}

vec4 GetColor(sampler2D sampler, vec2 pos) {
  return mix(texture2D(sampler, pos), GetRainbowColor(pos), k);
}

vec4 transition(vec2 uv) {
	return GetColor(to, uv);
}