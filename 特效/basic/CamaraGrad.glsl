const float linewidth = 0.001;
const vec4 black = vec4(0., 0., 0., 1.);

vec4 transition(vec2 uv) {
	if ((uv.x<1./3.+linewidth && uv.x>1./3.-linewidth) || (uv.x<2./3.+linewidth&&uv.x>2./3.-linewidth)) {
		return black;
	} else if ((uv.y<1./3.+linewidth&&uv.y>1./3.-linewidth) || (uv.y<2./3.+linewidth&&uv.y>2./3.-linewidth)) {
		return black;
	} else return texture2D(to, uv);
}