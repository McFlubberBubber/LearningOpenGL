#version 460 core

// The geometry shader first requires a declaration of the primitive input we are receiving from the vertex shader.
// It takes in: points (GL_POINTS), lines (GL_LINES or GL_LINE_STRIP), lines_adjacency (GL_LINES_ADJACENCY or
// GL_LINE_STRIP_ADJACENCY), triangles (all the GL_TRAINGLE related primitives) or triangles_adjacency (same thing
// like the lines).

// You also specify the output. This can take in points, line_strip or triangle_strip. With these 3, you can create
// any shape from the primitives. For example, if you wanted to generate a triangle, you would specify triangle_strip
// with max_vertices = 3.


layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

// INPUT BLOCK
in VS_OUT {
	vec3 color;
} gs_in[];

// OUTPUT
out vec3 fColor;

// Function to help draw the points of a 2D house
void build_house(vec4 position);


// With the help of the vertex shader, we can generate new data with the 2 functions
// called EmitVertex(); and EndPrimitive(); The shader expects you to generate, or
// output AT LEAST one of the primitive. In this case, we want to output at least
// one line strip.

// Each time we call EmitVertex(); the vector currently set to gl_Position is added
// to the output primitive. Whenever EndPrimitive(); is called, all emitted
// vertices for this primitive is combined into the specified output render
// primitive. By repeatedly calling EndPrimitive() after one or more EmitVertex()
// calls, multiple primitives can be generated.


void main() {
	build_house(gl_in[0].gl_Position);
}


// Modifying the position points passed from the vertex shader
void build_house(vec4 position) {
	fColor = gs_in[0].color;

	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:bottom-left
	EmitVertex();   

    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:bottom-right
    EmitVertex();

    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:top-left
    EmitVertex();

    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:top-right
    EmitVertex();

    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:top
	// You can change the color of each vertex by just changing the outputted color
	// vector. For example, we can make a white gradient at the top of the house
	// like so:
	fColor = vec3(1.0f, 1.0f, 1.0f);
	EmitVertex();		// Just make sure this call is done afterwards.

	EndPrimitive();	
}
