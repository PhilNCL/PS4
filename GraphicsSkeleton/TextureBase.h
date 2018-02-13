#pragma once

class TextureBase
{
public:
	TextureBase();
	~TextureBase();

	int GetWidth() const {
		return width;
	}

	int GetHeight() const {
		return height;
	}

	int GetBytesPerPixel() const {
		return bpp;
	}

protected:
	int width;
	int height;
	int bpp;
};

