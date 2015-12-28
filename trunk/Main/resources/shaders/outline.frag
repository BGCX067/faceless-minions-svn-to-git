uniform sampler2D texture;
uniform float img_width;
uniform float img_height;
uniform vec4 outline_color;
uniform float outline_width;
uniform float alpha;

vec4 get_pixel(in vec2 coords, in float dx, in float dy) 
{
	return texture2D(texture,coords + vec2(dx, dy));
}

// returns pixel color
int is_edge(in vec2 coords)
{
	float dxtex = 1.0 / img_width;
	float dytex = 1.0 / img_height;
	vec4 selected;
	int k = -1;
	int edge = 0;
	int width = int(outline_width);

	// read neighboring pixel intensities
	for(int i=-width; i<width+1; ++i)
	{
		for(int j=-width; j<width+1; ++j)
		{
			k++;
			selected = get_pixel(coords,float(i)*dxtex,float(j)*dytex);
			if (selected.a > 0.1)
			{
				edge = 1;
			}
		}
	}
	return edge;
}

void main()
{
	vec4 final_color = gl_Color * texture2D(texture, gl_TexCoord[0].st);
	if (is_edge(gl_TexCoord[0].st) == 1 && final_color.a <= 0.1)
	{
		final_color = outline_color;
		final_color.a = alpha;
	}
	gl_FragColor = final_color;
}