#include "SS_Setup.h"

SS_Setup::SS_SetupParams SS_Setup::SS_SetupParams::Default()
{
	SS_SetupParams params;
	params.targetDir = "." + OS_PATH_DELIMIT_STR;
	params.optFN = "opt.txt";
	params.listFN = "file_list.txt";
	params.noahBundleFN = "bundle.out";
	params.neighSpacing = 2;
	params.drewMode=false;
	return params;
}

void SS_Setup::SS_SetupParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sStatic Scene Setup params:\n", prefixPtr);
	printf("%s  targetDir: %s\n", prefixPtr, this->targetDir.c_str());
	printf("%s  optFN:     %s\n", prefixPtr, this->optFN.c_str());
	printf("%s  listFN:    %s\n", prefixPtr, this->listFN.c_str());
	printf("%s  neighSpacing:          %i\n", prefixPtr, this->neighSpacing);
	printf("%s  noahBundleFN:          %s\n", prefixPtr, this->noahBundleFN.c_str());
	printf("%s  drewMode:              %i\n", prefixPtr, (int) this->drewMode);
}
