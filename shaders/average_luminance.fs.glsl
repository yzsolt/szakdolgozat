
#version 330 core

layout(location = 0) out vec4 out_color;

uniform sampler2D u_luminance_texture;
uniform vec2 u_texel_size;
uniform int u_previous_level;
uniform int u_step;

in vec2 vs_out_texture;

void main() {

	const vec3 LUMINANCE_VECTOR = vec3(0.2125, 0.7154, 0.0721);

    if (u_step == 0) {

    	float logsum = 0.0;

    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(-0.5, -0.5) * u_texel_size).rgb, LUMINANCE_VECTOR));
    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(-0.5, 0.5) * u_texel_size).rgb, LUMINANCE_VECTOR));
    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(-0.5, 1.5) * u_texel_size).rgb, LUMINANCE_VECTOR));
    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(0.5, -0.5) * u_texel_size).rgb, LUMINANCE_VECTOR));
    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(0.5, 0.5) * u_texel_size).rgb, LUMINANCE_VECTOR));
    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(0.5, 1.5) * u_texel_size).rgb, LUMINANCE_VECTOR));
    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(1.5, -0.5) * u_texel_size).rgb, LUMINANCE_VECTOR));
    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(1.5, 0.5) * u_texel_size).rgb, LUMINANCE_VECTOR));
    	logsum += log(0.0001 + dot(texture(u_luminance_texture, vs_out_texture + vec2(1.5, 1.5) * u_texel_size).rgb, LUMINANCE_VECTOR));

    	logsum /= 9.0;

    	out_color = vec4(logsum, 0.0, 0.0, 1.0);

    }

    if (u_step > 0) {

    	ivec2 loc = ivec2(gl_FragCoord.xy);
    	float sum = 0.0;

    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(0, 0), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(0, 1), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(0, 2), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(0, 3), u_previous_level).r;

    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(1, 0), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(1, 1), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(1, 2), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(1, 3), u_previous_level).r;

    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(2, 0), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(2, 1), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(2, 2), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(2, 3), u_previous_level).r;

    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(3, 0), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(3, 1), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(3, 2), u_previous_level).r;
    	sum += texelFetch(u_luminance_texture, loc * 4 + ivec2(3, 3), u_previous_level).r;

    	sum *= 0.0625;

        if (u_step == 1) {
        	out_color = vec4(sum, 0.0, 0.0, 1.0);
        } else if (u_step == 2) {
        	// final
        	out_color = vec4(exp(sum), 0.0, 0.0, 1.0);
        }

    }

}
