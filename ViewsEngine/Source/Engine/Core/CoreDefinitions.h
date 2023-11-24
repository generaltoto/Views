#pragma once

#define MAX_NAME_STRING 256
#define MAX_LAYERS 8
#define CELL_SIZE 1
#define HInstance() GetModuleHandle(NULL)

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

typedef std::wstring WSTRING;
typedef std::string  STRING;

#define DELPTR(ptr) if (ptr) { delete (ptr); (ptr) = nullptr; }
#define NULLPTR(ptr) if (ptr) { (ptr) = nullptr; }
#define RELPTR(ptr) if (ptr) { (ptr)->Release(); (ptr) = nullptr; }
#define DELARR(ptr) if (ptr) { delete[] (ptr); (ptr) = nullptr; }

#define I(x) x::GetInstance() 

#define _CRTDBG_MAP_ALLOC


#if defined(DEBUG) | defined(_DEBUG)
	#include <crtdbg.h>
	// Replace new to check for memory leaks
	#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)

#endif
