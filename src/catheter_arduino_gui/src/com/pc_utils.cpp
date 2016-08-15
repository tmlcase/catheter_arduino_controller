#include "com/catheter_commands.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

using namespace std;

/* parse a playfile into a command vector */
int loadPlayFile(const char* fileIn, std::vector<CatheterChannelCmdSet>& outputCmdsVect) {

    ifstream inFile(fileIn, ifstream::in);
    if(inFile.bad()) return -1;
	
    outputCmdsVect.clear();
    int npackets = 0;

    string line;
    int linenum = 0;
	CatheterChannelCmdSet singleSet;
	

    while (getline (inFile, line)) {
		
        string item;

        size_t posOcto1  = line.find ("#"); // line comment
        size_t posComma1 = line.find (","); // after channel, before current
        size_t posComma2 = line.find (",", posComma1 + 1); // after current (MilliAmp), before delay (MilliSec)

        //verify the line's validity.
        if((posComma1 < posComma2) && (posComma2 < posOcto1)) {  //line is ok.

            istringstream linestream(line);
        
            CatheterChannelCmd singleCmd;
       
            /* parse channel */
            getline (linestream, item, ','); 
            int channelIn = atoi(item.c_str());
            if(!(channelIn >= 0 && channelIn <= NCHANNELS)) continue;   // bad channel; skip line
            singleCmd.channel = channelIn;

            /* parse current data, given in MilliAamp */
            getline (linestream, item, ',');
            singleCmd.currentMilliAmp = atof(item.c_str());

            singleCmd.poll = false;

            /* parse delay */
            getline (linestream, item, '#'); //works even if there is no #.
            int waitTime = atoi(item.c_str());
            if(!(waitTime >= 0)) continue;  // bad delay; skip line

			singleSet.commandList.push_back(singleCmd);
            if (waitTime > 0)
			{
				singleSet.delayTime = waitTime;
				outputCmdsVect.push_back(singleSet);
				singleSet.commandList.clear();
				singleSet.delayTime = 0;
			}
            linenum++;
        }
    }
    inFile.close();
    return npackets;
}

bool writePlayFile(const char * fname, const std::vector<CatheterChannelCmdSet>& cmdVect) {
    ofstream outFile(fname, ofstream::out);
    if (outFile.bad())
        return false;
    CatheterChannelCmd cmd;
    for (int i = 0; i < cmdVect.size(); i++) {
		for ( int j(0); j < cmdVect[i].commandList.size(); j++)
		{
			cmd = cmdVect[i].commandList[j];
			outFile << cmd.channel << ", " << cmd.currentMilliAmp << ", ";
			if ( (j+1) < cmdVect[i].commandList.size())
			{
				outFile << 0 << std::endl;
			}
			else
			{
				outFile << cmdVect[i].delayTime << std::endl;
			}
		}
    }
    outFile.close();
    return true;
}

bool writeBytes(const char *fname, const std::vector<uint8_t>& bytes) {
	FILE* f = fopen(fname, "w");
	if (f == NULL)
		return false;
	for (int i = 0; i < bytes.size(); i++) {
		fprintf(f, "%u\n", bytes[i]);
	}
	fclose(f);
	return true;
}
