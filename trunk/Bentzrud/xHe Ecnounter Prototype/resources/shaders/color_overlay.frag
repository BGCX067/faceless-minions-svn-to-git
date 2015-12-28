uniform vec4 overlay_color;
uniform sampler2D texture;

void main()
{
	vec4 final_color = gl_Color * texture2D(texture, gl_TexCoord[0].st);
	if (final_color.a != 0.0)
		final_color = overlay_color;
    gl_FragColor = final_color;
}