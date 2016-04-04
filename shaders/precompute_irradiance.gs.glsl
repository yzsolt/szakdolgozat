
layout(triangles) in;
layout(triangles, max_vertices = 18) out;

in vec3 vColor[]; // Output from vertex shader for each vertex

out vec3 fColor; // Output to fragment shader

void main(
	in triangle GS_Input input[3],
	in out TriangleStream<GS_Output> stream) {

	GS_Output output;

	for (uint i = 0U; i < 6U; ++i ) {
		output.rtindex = i;

		output.pos = mul(mul(input[0].pos, matCubeViews[i]), matProj);
		output.view = input[0].view;

		stream.Append(output);

		output.pos = mul(mul(input[1].pos, matCubeViews[i]), matProj);
		output.view = input[1].view;

		stream.Append(output);

		output.pos = mul(mul(input[2].pos, matCubeViews[i]), matProj);
		output.view = input[2].view;

		stream.Append(output);
		//stream.RestartStrip();
        EndPrimitive();
	}
}
