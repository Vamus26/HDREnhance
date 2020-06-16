#include "Application.h"

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cout << "usage: IBLViewer.exe <modelFile> <panoFile>";
		return -1;
	}

	std::string modelFile(argv[1]);
	std::string panoFile(argv[3]);
	std::string panoFileNew(argv[3]);
	std::string txtFile(argv[2]);
	Application app("IBLViewer", 1280, 720);
	if (app.init(modelFile,txtFile, panoFile))
	{
		app.loop();
	}
	return 0;
}