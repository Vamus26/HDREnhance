#include "Application.h"

int main(int argc, char** argv)
{
<<<<<<< HEAD
	if (argc != 4)
=======
	if (argc != 3)
>>>>>>> 5302a5fef9735646a13a986f336553b17cd0cab5
	{
		std::cout << "usage: IBLViewer.exe <modelFile> <panoFile>";
		return -1;
	}

	std::string modelFile(argv[1]);
<<<<<<< HEAD
	std::string panoFile(argv[3]);
	std::string panoFileNew(argv[3]);
	std::string txtFile(argv[2]);
	Application app("IBLViewer", 1280, 720);
	if (app.init(modelFile,txtFile, panoFile))
=======
	std::string panoFile(argv[2]);

	Application app("IBLViewer", 1280, 720);
	if (app.init(modelFile, panoFile))
>>>>>>> 5302a5fef9735646a13a986f336553b17cd0cab5
	{
		app.loop();
	}
	return 0;
}