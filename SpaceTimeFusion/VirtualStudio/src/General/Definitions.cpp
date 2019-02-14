#include "Definitions.h"
#pragma warning(disable:4996)

void Tokenize(const string& str,
              vector<string>& tokens,
              const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.		
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


bool SilentSysCall(const char *cmd, bool silenceStdOut, bool silenceStdErr)
{
	int stderrID = fileno(stderr);
	int stdoutID = fileno(stdout);
	int stderrDupID = -1;
	int stdoutDupID = -1;

	if(silenceStdErr)
	{
		stderrDupID = dup(stderrID);
		freopen("stderr-temp.txt", "w", stderr);
	}
	if(silenceStdOut)
	{
		stdoutDupID = dup(stdoutID);
		freopen("stdout-temp.txt", "w", stdout);
	}

	bool result = (system(cmd) == 0);

	if(silenceStdErr)
	{
		dup2(stderrDupID, stderrID);	
		close(stderrDupID);		
	}
	if(silenceStdOut)
	{
		dup2(stdoutDupID, stdoutID);
		close(stdoutDupID);
	}	

	return result;
}


