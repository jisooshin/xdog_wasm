#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "util.hpp"


extern "C" {
	using namespace std;

	EMSCRIPTEN_KEEPALIVE uint8_t* create_buffer(int width, int height){
		return static_cast<uint8_t*>(malloc(width * height * 4 * sizeof(uint8_t)));
	}
	
	EMSCRIPTEN_KEEPALIVE void destroy_buffer(uint8_t* p){
		cout << "freed " << static_cast<void*>(p) << endl;
		free(p);
	}


}