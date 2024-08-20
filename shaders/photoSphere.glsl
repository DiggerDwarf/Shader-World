uniform vec2        u_resolution;
uniform mat3        u_camera_rot;
uniform sampler2D   u_texture;
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define M_1_PI		0.31830988618379067154
vec2 coord(in vec2 p) {
    p = p / u_resolution.xy *2.0 -1.0;
    if (u_resolution.x > u_resolution.y) {
        p.x *= u_resolution.x / u_resolution.y;
    } else {
        p.y *= u_resolution.y / u_resolution.x;
    }
    return p;
}
void main() {
    vec2 uv = -coord(gl_FragCoord.xy);
    vec3 rd = normalize(vec3(uv*0.8, 1)) * u_camera_rot;
    float theta = (atan(rd.z, rd.x)+M_PI)/(2*M_PI);
    float phi   = (asin(rd.y)+M_PI_2)*M_1_PI;
    gl_FragColor = texture(u_texture, vec2(theta, phi));
}