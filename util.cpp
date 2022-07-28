#include "util.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <stdio.h>
#include <stdexcept>

#define RC 0.299f
#define GC 0.587f
#define BC 0.114f
#define MEM_SPARE 5
#define PI 3.141592f

using namespace std;
typedef vector<vector<float>> vector2D;


void xdog(
	uint8_t* src, size_t width, size_t height, size_t kernelSize, float sigma,
	float gamma, float phi, float epsilon, float kappa){

	//gray
	rgb2gray(src, (int)width, (int)height);

	vector2D kernel1 { getGaussianKernel2D(kernelSize, sigma) };
	vector2D kernel2 { getGaussianKernel2D(kernelSize, sigma * kappa) };
		
	//gray
	// vector<float> gFiltered1 {filter2D(src, width, height, kernel1)};
	//vector<float> gFiltered2;
	vector<float> diff(width*height*4, 0.f);
	vector<float> xdog(width*height*4, 0.f);
	//gFiltered1.reserve(width * height * 4);
	//gFiltered2.reserve(width * height * 4);
	//diff.reserve(width * height * 4);

	vector<float> gFiltered1 = filter2D(src, width, height, kernel1);
	vector<float> gFiltered2 = filter2D(src, width, height, kernel2);

	// for (int i = 0; i < width * height * 4; i++){
	// 	src[i] = gFiltered2[i];
	// }


	for (int i = 0; i < width * height * 4; i++){
		diff[i] = gFiltered1[i] - (gamma * gFiltered2[i]);
	}

	for (int i = 0; i < width * height * 4; i++){
		if (diff[i] > epsilon){
			xdog[i] = 1.f;
		} else {
			xdog[i] = 1.f +  tanf(phi * (diff[i] - epsilon));
		}
	}
	for (int i = 0; i < width * height * 4; i++){
		if (xdog[i] != 1.){
			src[i] = static_cast<uint8_t>(0);
		} else {
			src[i] = static_cast<uint8_t>(255);
		}
	}

}

void rgb2gray(uint8_t* src, int width, int height){

	for (int i = 0; i < width * height; i++){
		uint8_t gray = RC * src[4*i] + GC * src[4*i+1] + BC * src[4*i+2];
		src[4*i] = gray;
		src[4*i+1] = gray;
		src[4*i+2] = gray;
	}

}

vector<float> gaussianBlur(uint8_t* src, size_t width, size_t height, size_t kernelSize, float sigma){
	// BORDER_REFLECT 스타일로 padding함
	size_t ks { kernelSize };
	vector<float> kernel1D;
	kernel1D = getGaussianKernel1D(kernelSize, sigma);

	vector<float> container;
	container.reserve(width * height * 4);

	for (int i = 0; i < width * height * 4; i++){
		container.emplace_back(src[i]);
	}
	int ikanchor { (int)ks / 2 };

	int ixcoord { 0 };
	int iycoord { 0 };

	for (int i = 0; i < width * height; i++){

		if ( (i % width) == 0 ) {
			ixcoord = 0;
			iycoord++;
		} else {
			ixcoord++;
		}
		int x { ixcoord };
		int y { iycoord - 1 };
		/*
		# x 방향 가우시안 #
		*/
		float sum1c { 0.f };
		float sum2c { 0.f };
		float sum3c { 0.f };
		int real_kindex { 0 };

		if (x < ikanchor) {
			// 0 쪽 가장자리
		 	for (int k = -ikanchor; k < ikanchor; k++){
		 		if (k < 0) {
		 			sum1c += container[4 * (i - k)    ] * kernel1D[real_kindex];
		 			sum2c += container[4 * (i - k) + 1] * kernel1D[real_kindex];
		 			sum3c += container[4 * (i - k) + 2] * kernel1D[real_kindex];
		 		} else {
		 			sum1c += container[4 * (i + k)    ] * kernel1D[real_kindex];
		 			sum2c += container[4 * (i + k) + 1] * kernel1D[real_kindex];
		 			sum3c += container[4 * (i + k) + 2] * kernel1D[real_kindex];
		 		}
				real_kindex++;
			}

		} else if ( x > (width - 1) - ikanchor) {
		 	for (int k = -ikanchor; k < ikanchor; k++){
				if (k > 0) {
		 			sum1c += container[4 * (i - k)    ] * kernel1D[real_kindex];
		 			sum2c += container[4 * (i - k) + 1] * kernel1D[real_kindex];
		 			sum3c += container[4 * (i - k) + 2] * kernel1D[real_kindex];
					
				} else {
		 			sum1c += container[4 * (i + k)    ] * kernel1D[real_kindex];
		 			sum2c += container[4 * (i + k) + 1] * kernel1D[real_kindex];
		 			sum3c += container[4 * (i + k) + 2] * kernel1D[real_kindex];
				}
				real_kindex++;
			}

		} else {
			// 가장자리 제외 
			for (int k = -ikanchor; k < ikanchor; k++){
				sum1c += container[4 * (i + k)    ] * kernel1D[real_kindex];
				sum2c += container[4 * (i + k) + 1] * kernel1D[real_kindex];
				sum3c += container[4 * (i + k) + 2] * kernel1D[real_kindex];
				real_kindex++;
			}
		}

		container[ 4 * i ]    = sum1c;
		container[ 4 * i + 1] = sum2c;
		container[ 4 * i + 2] = sum3c;

		/* y 방향 가우시안 
		*/
		sum1c = 0.f;
		sum2c = 0.f;
		sum3c = 0.f;
		real_kindex = 0;
		if (y < ikanchor) {
			for (int k = -ikanchor; k < ikanchor; k++){
				if (k < 0) {
					sum1c += container[ 4 * (i + (-k * width))    ]  * kernel1D[real_kindex];
					sum2c += container[ 4 * (i + (-k * width)) + 1]  * kernel1D[real_kindex];
					sum3c += container[ 4 * (i + (-k * width)) + 2]  * kernel1D[real_kindex];
				} else {
					sum1c += container[ 4 * (i + (k * width))    ]  * kernel1D[real_kindex];
					sum2c += container[ 4 * (i + (k * width)) + 1]  * kernel1D[real_kindex];
					sum3c += container[ 4 * (i + (k * width)) + 2]  * kernel1D[real_kindex];
				}
				real_kindex++;
			}

		} else if ( y > (height - 1) - ikanchor) {
			for (int k = -ikanchor; k < ikanchor; k++){
				if (k > 0) {
					sum1c += container[ 4 * (i + (-k * width))    ]  * kernel1D[real_kindex];
					sum2c += container[ 4 * (i + (-k * width)) + 1]  * kernel1D[real_kindex];
					sum3c += container[ 4 * (i + (-k * width)) + 2]  * kernel1D[real_kindex];
				} else {
					sum1c += container[ 4 * (i + (k * width))    ]  * kernel1D[real_kindex];
					sum2c += container[ 4 * (i + (k * width)) + 1]  * kernel1D[real_kindex];
					sum3c += container[ 4 * (i + (k * width)) + 2]  * kernel1D[real_kindex];
				}
				real_kindex++;
			}

		} else {
			for (int k = -ikanchor; k < ikanchor; k++){
				sum1c += container[ 4 * (i + (k * width))    ]  * kernel1D[real_kindex];
				sum2c += container[ 4 * (i + (k * width)) + 1]  * kernel1D[real_kindex];
				sum3c += container[ 4 * (i + (k * width)) + 2]  * kernel1D[real_kindex];
				real_kindex++;
			}
		}
		container[ 4 * i]     = sum1c;
		container[ 4 * i + 1] = sum2c;
		container[ 4 * i + 2] = sum3c;
	}
	return container;
}


vector<float> getGaussianKernel1D(size_t size, float sigma){

	if (sigma < 0){
		float sigma { 0.3f * ((float)(size - 1.f) * 0.5f - 1.f) + 0.8f } ;
	}

	vector<float> gKernel1D;
	vector<float> coord;
	gKernel1D.reserve((size) * sizeof(float)); coord.reserve((size) * sizeof(float));
	float mu;
	{
		float sum  { 0.f };
		for (int i = 0; i < size; i++) {
			sum += static_cast<float>(i);
			coord.emplace_back(static_cast<float>(i));
		}
		mu = sum / static_cast<float>(size);
	}
	for (const auto elem: coord){
		// gKernel1D.emplace_back(
		// 	( 1 / sqrtf(2 * PI * pow(sigma, 2)) ) * 
		// 	expf( -(pow(elem - mu, 2) / (2 * pow(sigma, 2))) )
		// );
		gKernel1D.emplace_back(
			expf(
				(-powf((elem - (size - 1) / 2), 2)) /
				(2 * powf(sigma, 2))
			)
		);
	}

	float sum_over_kernel { 0.f };
	float coeff { 0.f };
	for (const auto elem : gKernel1D){
		sum_over_kernel += elem;
	}
	coeff = 1 / sum_over_kernel;
	for (auto& elem : gKernel1D){
		elem *= coeff;
	}

	return gKernel1D;
}

vector2D getGaussianKernel2D(size_t size, float sigma){

	if (sigma < 0){
		float sigma { 0.3f * ((float)(size - 1.f) * 0.5f - 1.f) + 0.8f } ;
	}
	vector2D gKernel2D(size, vector<float>(size, 0.f));
	vector<float> gKernel1D;
	vector<float> coord;
	gKernel1D.reserve((size) * sizeof(float));
	coord.reserve((size) * sizeof(float));
	float mu;
	{
		float sum  { 0.f };
		for (int i = 0; i < size; i++) {
			sum += static_cast<float>(i);
			coord.emplace_back(static_cast<float>(i));
		}
		mu = sum / static_cast<float>(size);
	}
	for (const auto elem: coord){
		// gKernel1D.emplace_back(
		// 	( 1 / sqrtf(2 * PI * pow(sigma, 2)) ) * 
		// 	expf( -(pow(elem - mu, 2) / (2 * pow(sigma, 2))) )
		// );
		gKernel1D.emplace_back(
			expf(
				(-powf((elem - (size - 1) / 2), 2)) /
				(2 * powf(sigma, 2))
			)
		);
	}

	float sum_over_kernel { 0.f };
	float coeff { 0.f };
	for (const auto elem : gKernel1D){
		sum_over_kernel += elem;
	}
	coeff = 1 / sum_over_kernel;
	for (auto& elem : gKernel1D){
		elem *= coeff;
	}

	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			gKernel2D[i][j] = gKernel1D[i] * gKernel1D[j];
		}
	}
	return gKernel2D;
}


vector<float> filter2D(uint8_t* src, size_t width, size_t height, vector2D kernel){
	vector<float> container(height * width * 4);
	vector<float> flatten_kernel(kernel.size() * kernel.size());

	for (int i = 0; i < width * height * 4; i++){
		container[i] = src[i];
	}

	{
		int i { 0 };
		for (const auto row : kernel){
			for (const auto col : row){
				flatten_kernel[i] = col;
				i++;
			}
		}

	}

	int ikanchor { (int)kernel.size() / 2 };
	int _ixcoord { 0 };
	int _iycoord { 0 };
	float fsumChan1 { 0.f };
	float fsumChan2 { 0.f };
	float fsumChan3 { 0.f };
	int tmp { 0 };

	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			fsumChan1 = 0.f;
			fsumChan2 = 0.f;
			fsumChan3 = 0.f;
			// 커널 합
			for (int ky = 0; ky < kernel.size(); ky++){
				for (int kx = 0; kx < kernel.size(); kx++){
					int _tx = x + kx - ikanchor;
					int _ty = y + ky - ikanchor;
					if (_tx < 0) _tx = -(_tx);
					if (_ty < 0) _ty = -(_ty);
					if (_tx  > (width - 1)) _tx = (width - 1)  - (kx - ikanchor);
					if (_ty  > (height - 1)) _ty = (height - 1)  - (ky - ikanchor);
					fsumChan1 += flatten_kernel[(ky * kernel.size()) +  kx] * container[(_ty * width * 4) + (4 * _tx)    ];
					fsumChan2 += flatten_kernel[(ky * kernel.size()) +  kx] * container[(_ty * width * 4) + (4 * _tx) + 1];
					fsumChan3 += flatten_kernel[(ky * kernel.size()) +  kx] * container[(_ty * width * 4) + (4 * _tx) + 2];
				}
				
			}
			container[(width * y * 4) + (4 * x)] = fsumChan1;
			container[(width * y * 4) + (4 * x) + 1] = fsumChan2;
			container[(width * y * 4) + (4 * x) + 2] = fsumChan3;
		}

	}
	

	return container;
}