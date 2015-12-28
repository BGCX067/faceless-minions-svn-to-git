uniform float factor;
uniform sampler2D texture;

void main()
{
	vec4 final_color = gl_Color * texture2D(texture, gl_TexCoord[0].st);
	
	float grayscale = (final_color.r*0.21) + (final_color.g*0.71) + (final_color.b*0.07);
	vec4 grayscalevec = vec4(grayscale, grayscale, grayscale, final_color.a);
	
	final_color = grayscalevec + (final_color - grayscalevec) * factor;
	
    gl_FragColor = final_color;
}