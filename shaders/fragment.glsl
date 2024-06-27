
/* Main function, uniforms & utils */
#ifdef GL_ES
    precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec3 u_camera_pos;

#define PI_TWO			1.570796326794897
#define PI				3.141592653589793
#define TWO_PI			6.283185307179586

float t = 0.0;

/* Coordinate and unit utils */
vec2 coord(in vec2 p) {
    p = p / u_resolution.xy *2.0 -1.0;
    if (u_resolution.x > u_resolution.y) {
        p.x *= u_resolution.x / u_resolution.y;
    } else {
        p.y *= u_resolution.y / u_resolution.x;
    }
    return p;
}
#define FAR 50.0
#define max_steps 200.0
uniform float nb_reflections;


vec4 power(vec4 v, float p) {
    return vec4(
        pow(v.x,p),
        pow(v.y,p),
        pow(v.z,p),
        pow(v.w,p)
    );
}

float sdBox( vec3 p, vec3 b ) {
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float round(float x) {
    return floor(x+0.5);
}

float sphere(vec3 p) { return length(p-vec3(-1.5,0,0))-1.0; }
float box   (vec3 p) { return sdBox(p-vec3(1.5,0,0), vec3(0.6))-0.2;}
float plane (vec3 p) { return p.y+1.0; }
// float light (vec3 p) { return length(p-light_pos)-0.5; }

float map(vec3 p) {
    return min(min(
        sphere(p),
        plane(p)),
        box(p)
    );
}

#define normal_offset 0.001
vec3 get_normal(vec3 pos) {
    vec2 Dir = vec2(normal_offset,0);
    return normalize(vec3(
        map(pos+Dir.xyy)-map(pos-Dir.xyy),
		map(pos+Dir.yxy)-map(pos-Dir.yxy),
		map(pos+Dir.yyx)-map(pos-Dir.yyx)
    ));
}

vec3 get_color(vec3 p) {
    return vec3(
        step(sphere(p),0.001*t)*vec3(0.1843, 0.451, 0.549) +
        step(plane(p),0.001*t)*vec3(0.2627, 0.298, 0.1176) +
        step(box(p),0.001*t)*vec3(0.5882, 0.1647, 0.1647)
        // step(light(p),0.001*t)*light_color
        //step(FAR-5., length(p))*vec3(0.851, 0.6863, 0.0235)
    );
}

vec3 get_ambient(vec3 p, float strength) {
    return get_color(p)*strength;
}

vec3 get_diffuse(vec3 p, vec3 l) {
    vec3 normal = get_normal(p);
    vec3 color = get_color(p);
    return max(dot(normal,normalize(l-p)),0.0)*color;
}

float get_specular(vec3 p, vec3 va, vec3 l) {
    vec3 normal = get_normal(p);
    vec3 light_out = reflect((-l+p),normal);
    return pow(clamp(dot(-va,light_out)/20., 0.0, 1.0),8.);
}


void main() {
    vec2 uv = coord(gl_FragCoord.xy);

    vec3 ro = u_camera_pos;
    // vec3 target = vec3(0.0, 0.0, 0.0);
	// vec3 ww = normalize( target - ro );
    // vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    // vec3 vv = normalize( cross(uu,ww));
	// vec3 rd = normalize( uv.x*uu + uv.y*vv + ww );

    vec3 rd = normalize(vec3(uv, 1));

    t = 0.0;
    float dist;
    vec3 light_pos = vec3(1,3,1);
    vec3 sky_color = vec3(0.1686, 0.451, 0.8157);

    // vec3 light_pos = 10.*vec3(cos(u_time/3.),2,sin(u_time/3.));
    // vec3 light_color = vec3(0.8235, 0.251, 0.5686);
    float light_strength = 0.4;

    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    

    for(float reflection = 0.0; reflection < nb_reflections; reflection++) {
        for (float steps = 0.0; steps < max_steps; steps++) {
            dist = map(ro);
            t += dist;
            ro += dist*rd;

            if (dist < 0.0001*t || t > FAR) break;
        }

        if (t > FAR) {
            gl_FragColor += vec4(sky_color*exp(-0.5*reflection), 1.0);
            return;
        }

        gl_FragColor += power(vec4(
            get_ambient(ro, light_strength) +
            get_diffuse(ro, light_pos)*2.0 +
            get_specular(ro, rd, light_pos)*0.3
            ,1),1.5) * exp(-0.5*reflection);
        
        rd = reflect(rd, get_normal(ro));
        ro += get_normal(ro)*0.01;
    }
}