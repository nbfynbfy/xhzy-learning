const float duration1 = .5;
const float pi = 3.14159;
const float speed = pi;
const float centerX1 = 2./3.;
const float centerX2 = 1.;

float Move1(float t) { return -1./6.*cos(8.*t*speed)+5./6.;  }
float Move2(float t) { return 1./10.*cos(8.*t*speed)+1./2.;  }
float Move3(float t) { return -1./20.*cos(t*speed*4.)+9./20.; }
float Move4(float t) { return 1./10.*cos(t*speed*4.)+1./10.; }
vec4 Move(float t) { 
	float a = Move1(t), b = Move2(t), c = Move3(t), d = Move4(t);
	if (c <= b) {
		return vec4(a,b,c,d);
	} else {
		return vec4(a,c,b,d);
	}
}

float GetBigger1(float t) { return 1.-.1*sin(t*2.*pi); }
float GetBigger2(float t) { return 1.-.2*sin(t*2.*pi); }

vec2 GetPos1(vec2 pos, float t) { return vec2((pos.x-centerX1)*GetBigger1(t)+centerX1,pos.y); }
vec2 GetPos2(vec2 pos, float t) { return vec2((pos.x-centerX2)*GetBigger2(t)+centerX2,pos.y); }

vec4 transition(vec2 pos) {
	vec4 line_pos = Move(progress);
	vec2 tmp_pos = pos;
	if (progress<duration1) {
	  	if (pos.y>line_pos.x) { tmp_pos = GetPos1(pos, progress); }
	  	else if (pos.y>line_pos.y) { tmp_pos = GetPos2(pos, progress); }
	  	else if (pos.y>line_pos.z) { tmp_pos = GetPos1(pos, progress); }
	  	else if (pos.y>line_pos.w) { tmp_pos = GetPos2(pos, progress); }
	  	else { tmp_pos = GetPos1(pos, progress); }
	}
	 return texture2D(to, tmp_pos);
}