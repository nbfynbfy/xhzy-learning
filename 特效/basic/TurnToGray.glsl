const vec3 W = vec3(0.2125, 0.7154, 0.0721);

vec4 transition(vec2 uv) {
    vec4 textureColor = texture2D(to, uv);
    vec4 luminance = vec4(vec3(dot(textureColor.rgb,W)), textureColor.a);
    return mix(textureColor, luminance, progress);
}