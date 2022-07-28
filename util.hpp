#pragma once
#include <emscripten/emscripten.h>
#include <iostream>
#include <vector>
#include <emscripten/bind.h>


enum Interpolation_Type {
	CONSTANT
};

extern "C" {
	using namespace std;
	EMSCRIPTEN_KEEPALIVE void rgb2gray(uint8_t* src, int width, int height);

	EMSCRIPTEN_KEEPALIVE void xdog(
		uint8_t* src, size_t width, size_t height, size_t kernelSize = 11, float sigma = 0.8,
		float gamma = 0.98, float phi = 200., float epsilon = 0.1, float kappa = 1.6);

}
std::vector<float> getGaussianKernel1D(size_t size, float sigma); 
std::vector<std::vector<float>> getGaussianKernel2D(size_t size, float sigma); 
std::vector<float> gaussianBlur(
		uint8_t* src, size_t width, size_t height, size_t kernelSize, float sigma);

std::vector<float> filter2D(
	uint8_t* src, size_t width, size_t height, std::vector<std::vector<float>> kernel);



