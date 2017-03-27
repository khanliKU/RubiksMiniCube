varying vec4 color;
varying vec4 frag_color;
uniform mat4 cubeColor;

void main()
{
	gl_FragColor = cubeColor * color;
}
