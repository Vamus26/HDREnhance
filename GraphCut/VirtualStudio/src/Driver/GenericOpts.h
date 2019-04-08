#ifndef __GENERIC_OPTS_H__
#define __GENERIC_OPTS_H__

typedef struct _GenericOpts
{
	vector<string> strList1;
	vector<string> strList2;
	vector<int>    intList1;
	vector<int>    intList2;
	vector<float>   floatList;

	void Dump(const string prefix)
	{
		const char* prefixPtr = prefix.c_str();	
		printf("\n");
		printf("%sGeneric params:\n", prefixPtr);

		if(this->strList1.size() > 0)
		{
			printf("%s  String list-1:\n", prefixPtr);
			for(uint iElem = 0; iElem < this->strList1.size(); iElem++)
			{
				printf("%s\t%s\n", prefixPtr, this->strList1[iElem].c_str());
			}
		}
		if(this->strList2.size() > 0)
		{
			printf("%s  String list-2:\n", prefixPtr);
			for(uint iElem = 0; iElem < this->strList2.size(); iElem++)
			{
				printf("%s\t%s\n", prefixPtr, this->strList2[iElem].c_str());
			}
		}

		if(this->intList1.size() > 0)
		{
			printf("%s  Int list-1:\n", prefixPtr);
			for(uint iElem = 0; iElem < this->intList1.size(); iElem++)
			{
				printf("%s\t%i\n", prefixPtr, this->intList1[iElem]);
			}
		}
		if(this->intList2.size() > 0)
		{
			printf("%s  Int list-2:\n", prefixPtr);
			for(uint iElem = 0; iElem < this->intList2.size(); iElem++)
			{
				printf("%s\t%i\n", prefixPtr, this->intList2[iElem]);
			}
		}

		if(this->floatList.size() > 0)
		{
			printf("%s  Float list:\n", prefixPtr);
			for(uint iElem = 0; iElem < this->floatList.size(); iElem++)
			{
				printf("%s\t%f\n", prefixPtr, this->floatList[iElem]);
			}
		}


		printf("\n");
	}

} GenericOpts;

#endif
