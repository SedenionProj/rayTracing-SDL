#pragma once
#include "spectrum.h"
#include "utils/image.h"
#include <string>

class Texture {
public:
	virtual float eval(WaveLength lambda, glm::vec2 uv) = 0;
};

class ColorTexture : public Texture{
public:
	ColorTexture(const glm::vec3& color)
		: m_spectrum(color){}

	float eval(WaveLength lambda, glm::vec2 uv) {
		return m_spectrum(lambda);
	}

private:
	SampledSpectrum m_spectrum;
};

class ImageTexture : public Texture {
public:
	ImageTexture(const char* path) 
		: image(path){}

	float eval(WaveLength lambda, glm::vec2 uv) {
		glm::vec3 color = image.pixelColor(uv*glm::vec2(image.width, image.height))/255.f;
		return SampledSpectrum(color)(lambda);
	}
private:
	Image image;
};