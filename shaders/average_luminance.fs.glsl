
#version 330 core

layout(location = 0) out vec4 out_color;

uniform sampler2D u_luminance_texture;
uniform vec2 u_texel_size;
uniform int u_previous_level;
uniform int u_step;

in vec2 vs_out_texture;

void main() {

	const vec3 LUMINANCE_VECTOR = vec3(0.2126, 0.7152, 0.0722);

    if (u_step == 0) {

    	float log_sum = 0;

		for (float x = -0.5; x < 1.5; x += 1) {
			for (float y = -0.5; y < 1.5; y += 1) {
				log_sum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(x, y) * u_texel_size).rgb, LUMINANCE_VECTOR));
			}
		}

    	log_sum /= 9;

    	out_color = vec4(log_sum, 0, 0, 1);

    }

    if (u_step > 0) {

    	ivec2 location = ivec2(gl_FragCoord.xy);
    	float sum = 0;

		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				sum += texelFetch(u_luminance_texture, location * 4 + ivec2(x, y), u_previous_level).r;
			}
		}

    	sum /= 16;

        if (u_step == 1) {
        	out_color = vec4(sum, 0, 0, 1);
        } else if (u_step == 2) {
        	out_color = vec4(exp(sum), 0, 0, 1);
        }

    }

}
