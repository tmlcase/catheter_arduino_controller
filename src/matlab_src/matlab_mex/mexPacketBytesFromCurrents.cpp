//packet bytes from current value (amps)

#include "mex_utils.h"
#include "com\common_utils.h"
#include <vector>
#include <stdlib.h>
#include <stdint.h>

using namespace std;
// currents supplied are in MILLIAMPS
void mexPacketBytesFromCurrents(double *currents, int nchannels, int pseqnum, std::vector<std::vector<uint8_t>>& packetVect) {
    packetVect.clear();    
    
	std::vector <CatheterChannelCmd> commands;
	commands.clear();
    CatheterChannelCmd c;

    for (int i=0;  i<nchannels; i++) {
        c.channel = i+1;
		c.currentMA = currents[i];
        c.poll = false;
		commands.push_back(c);
        mexSummarizeCmd(c);  
    }
	
    std::vector<int> pdelays;
	std::vector<int> ncmdsPerPacket;
	packetVect.clear();
    // getPacketBytes(pseqnum, commandVect, packetVect, pdelays, ncmdsPerPacket);  
    packetVect.push_back(compactPacketBytes(commands, pseqnum));
}

//inputs: array of current values in MilliAmps, the number of channels, and the packet number
//output: a single packet of bytes
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    // Check for appropriate number of arguments 
    if (nrhs != 3) {
        mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", 
            "getPacketStringsFromPlayfile requires 3 input arguments.");
    } else if (nlhs != 1) {
        mexErrMsgIdAndTxt("MATLAB:mexcpp:nargout",
            "getPacketStringsFromPlayfile requires 1 output arguments.");
    } 
    // check for appropriate types of input arguments
    if (!(mxGetM(prhs[2])==1 && mxGetN(prhs[2])==1)) {
        mexErrMsgIdAndTxt("MATLAB:mexcpp:mxGetN", "packet sequence number (input) must be a scalar");
    }
    if (!(mxGetM(prhs[1])==1 && mxGetN(prhs[1])==1)) {
        mexErrMsgIdAndTxt("MATLAB:mexcpp:mxGetN", "nchannels (input) must be a scalar");
    }
    
    int pseqnum = (int)(mxGetScalar(prhs[2]));        
    int nchannels = (int)(mxGetScalar(prhs[1]));
//     mexPrintf("pseqnum: %d\n", pseqnum);
//     mexPrintf("nchannels: %d\n", nchannels);
    
    mwSize m = mxGetM(prhs[0]);
    mwSize n = mxGetN(prhs[0]);    
    if (!(m==nchannels && n==1) || (m==1 && n==nchannels)) {
        mexErrMsgIdAndTxt("MATLAB:mexcpp:mxGetN", "incorrect size of currents argument. Found array of size %d, %d", m, n);
    }    
    
    double *currents = mxGetPr(prhs[0]);     

    std::vector<std::vector<uint8_t>> packetVect;

    mexPacketBytesFromCurrents(currents, nchannels, pseqnum, packetVect);       
    
    if (packetVect.empty() || packetVect[0].empty()) {
        mexErrMsgIdAndTxt("MATLAB:mexcpp:packetVect", "failed to create packet bytes");
    } else {
//         mexPrintf("mex: creating packet of %d bytes...\n", packetVect[0].size());
    }
    
    //allocate return array
    m = (m==1 ? m : packetVect[0].size());
    n = (n==1 ? n : packetVect[0].size());
    
    plhs[0] = mxCreateNumericMatrix(m, n, mxUINT8_CLASS, mxREAL);
    mwSize bytes_to_copy = m*n * mxGetElementSize(plhs[0]);
    
    memcpy(mxGetData(plhs[0]), packetVect[0].data(), bytes_to_copy);
}