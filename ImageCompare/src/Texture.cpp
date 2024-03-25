#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

Texture_Filter get_mipmap_filter(Texture_Filter f) {
	switch (f)
	{
	case Texture_Filter::linear:
		return Texture_Filter::linear_mipmap;
	case Texture_Filter::nearest:
		return Texture_Filter::nearest_mipmap;
	}
}

// into single function
Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const float* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map)
	: Texture(n, res_x, res_y, (const void*)data, GL_FLOAT, internal_format, format, filter, wrap, mip_map)
{}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const unsigned char* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map)
	: Texture(n, res_x, res_y, (const void*)data, GL_UNSIGNED_BYTE, internal_format, format, filter, wrap, mip_map)
{}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, const void* data, int data_type, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap, bool mip_map)
{
	name = n;
	type = GL_TEXTURE_2D;
	can_resize = (data == nullptr);

	glGenTextures(1, &id);
	glBindTexture(type, id);

	i_form = internal_format;
	form = format;
	d_type = data_type;
	glTexImage2D(type, 0, i_form, res_x, res_y, 0, form, d_type, data);

	if (mip_map) {
		glGenerateMipmap(type);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, get_mipmap_filter(filter));
	}
	else {
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filter);
	}
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int res_z, const float* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap)
{
	name = n;
	type = GL_TEXTURE_3D;
	can_resize = (data == nullptr);

	glGenTextures(1, &id);
	glBindTexture(type, id);

	glTexImage3D(type, 0, internal_format, res_x, res_y, res_z, 0, format, GL_FLOAT, data);

	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(type, GL_TEXTURE_WRAP_R, wrap);
}

Texture::Texture(const std::string& n, unsigned int res_x, unsigned int res_y, unsigned int res_z, const unsigned char* data, int internal_format, int format, Texture_Filter filter, Texture_Wrap wrap)
{
	name = n;
	type = GL_TEXTURE_3D;
	can_resize = (data == nullptr);

	glGenTextures(1, &id);
	glBindTexture(type, id);

	glTexImage3D(type, 0, internal_format, res_x, res_y, res_z, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(type, GL_TEXTURE_WRAP_R, wrap);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::set_texture_unit(Program& program, int unit)
{
	texture_unit = unit;
	program.set1i(name.c_str(), texture_unit);
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(type, id);
}

void Texture::bind_texture_unit(Program& program, int unit)
{
	set_texture_unit(program, unit);
	bind();
}

// see https://gamedev.stackexchange.com/a/156707
void Texture::resize(unsigned int res_x, unsigned int res_y)
{
	assert(can_resize);

	glBindTexture(type, id);
	glTexImage2D(type, 0, i_form, res_x, res_y, 0, form, d_type, nullptr);
}

TextureP texture_from_path(std::string path, std::string name)
{
	int width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if (data) {
		// should choose internal format, potential issues with format not being one of the defined interal formats
		GLenum format;
		if (channels == 1)
			format = GL_RED;
		if (channels == 2)
			format = GL_RG;
		if (channels == 3)
			format = GL_RGB;
		if (channels == 4)
			format = GL_RGBA;

		TextureP t = std::make_unique<Texture>(
			name,
			width,
			height,
			data,
			format,
			format,
			Texture_Filter::linear,
			Texture_Wrap::repeat,
			true
		);

		stbi_image_free(data);
		return t;
	}
	else
	{
		std::cerr << "Failed to load texture at: " << path << std::endl;
		stbi_image_free(data);
		throw std::runtime_error("No texture found");
	}
}

void get_texture_size_from_path(std::string path, int& width, int& height) {
	int channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		std::cerr << "Failed to load texture at: " << path << std::endl;
		stbi_image_free(data);
		throw std::runtime_error("No texture found");
	}
	stbi_image_free(data);
}
