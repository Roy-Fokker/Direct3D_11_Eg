#include "application.h"


//#include <string>
//#include <Windows.h>


auto main() -> int
{
	//std::wstring cmd_line(GetCommandLine());

	using namespace direct3d_11_eg;
	application app; // TODO: Command Line Parameters struct???

	return app.run();
}
