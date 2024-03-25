#version 460 core
out vec4 FragColor;

uniform sampler2D img1;
uniform sampler2D img2;

uniform float mouse_x;
float line_width = 0.001;
vec3 line_color = vec3(0,1,0.4);

in vec2 tex_coord;

void main()
{
	vec3 col_tex = (tex_coord.x < mouse_x) ? texture(img1, tex_coord).rgb : texture(img2, tex_coord).rgb;
	vec3 col_border = (abs(tex_coord.x - mouse_x) < line_width) ? line_color : col_tex;
	FragColor = vec4(col_border, 1);
}