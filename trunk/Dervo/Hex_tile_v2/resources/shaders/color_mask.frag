uniform vec4 color_mask;
uniform sampler2D texture;

void main()
{
	vec4 final_color = gl_Color * texture2D(texture, gl_TexCoord[0].st);
	if (final_color.r == 1.0 && final_color.b == 1.0)
		final_color = color_mask;
    gl_FragColor = final_color;
}