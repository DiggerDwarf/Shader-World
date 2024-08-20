uniform vec2        u_resolution;
uniform mat3        u_camera_rot;
uniform sampler2D   u_texture;

#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define M_1_PI		0.31830988618379067154
#define M_1_2PI     0.159154943092
#define M_PI_4      0.785398163397
#define M_2_PI      0.636619772368


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

    vec2 uv = coord(gl_FragCoord.xy);
    uv = - uv;

    vec3 rd = normalize(vec3(uv*0.8, 1));
    rd *= u_camera_rot;


    float theta = (atan(rd.z, rd.x)+M_PI)/(2*M_PI);
    float phi = (asin(rd.y)+M_PI_4)*M_2_PI;

    if (phi > 1.0 || phi < 0.0)
    {
        gl_FragColor = vec4(0, 0, 0, 1);
        return;
    }

    #define EPSILON 0.0005

    gl_FragColor  = texture(u_texture, vec2(theta        , phi        ));


    // gl_FragColor += texture(u_texture, vec2(theta + EPSILON, phi          ));
    // gl_FragColor += texture(u_texture, vec2(theta - EPSILON, phi          ));
    // gl_FragColor += texture(u_texture, vec2(theta          , phi + EPSILON));
    // gl_FragColor += texture(u_texture, vec2(theta          , phi - EPSILON));
    // gl_FragColor *= 0.2;

    // gl_FragColor.r = fract(uv.y);

}