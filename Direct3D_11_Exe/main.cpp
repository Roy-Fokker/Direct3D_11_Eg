#ifdef _DEBUG
// CRT Memory Leak detection
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>
#endif

#include "application.h"


auto main() -> int
{
#ifdef _DEBUG
	// Detects memory leaks upon program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	using namespace direct3d_11_eg;
	application app; // TODO: Command Line Parameters struct???

	return app.run();
}
