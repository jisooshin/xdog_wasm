#include <iostream>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <utility>
#include <algorithm>

#define MEM_SPARE 5
#define PI 3.141592f
using namespace std;

 vector<float> getGaussianKernel1D(size_t&& _size, float&& _sigma); 
 void a(int& t){
	 cout << t << endl;
 }

int main(){
	vector<float> a;
	a.reserve(10);
	a.emplace_back(1);
	a.emplace_back(2);
	a.emplace_back(3);

	cout << a[0] << a[1] << a[2] << endl;
	

	return 0;
}

vector<float> getGaussianKernel1D(size_t&& size, float&& sigma){
	if (sigma < 0){
		float sigma { 0.3f * ((float)(size - 1.f) * 0.5f - 1.f) + 0.8f } ;
	}

	vector<float> gKernel1D;
	vector<float> coord;
	gKernel1D.reserve((size + MEM_SPARE) * sizeof(float));
	coord.reserve((size + MEM_SPARE) * sizeof(float));

	float mu;
	{
		int count { static_cast<int>(size) };
		cout << count << endl;
		float sum  { 0.f };
		for (int i = 0; i < count; i++) {
			sum += static_cast<float>(i);
			coord.emplace_back(static_cast<float>(i));
		}
		mu = sum / static_cast<float>(count);
	}
	for (const auto elem: coord){
		gKernel1D.emplace_back(
			( 1 / sqrtf(2 * PI * pow(sigma, 2)) ) * 
			expf( -(pow(elem - mu, 2) / (2 * pow(sigma, 2))) )
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
