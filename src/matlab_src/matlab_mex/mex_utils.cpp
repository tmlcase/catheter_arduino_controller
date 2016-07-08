// mex_utils.cpp

//#ifdef MEXFILE

#include "mex_utils.h"

void mexCopy2DIntArray(int * A, mxArray * mexA, int dim0, int dim1) {    
    unsigned char * start_of_pr = (unsigned char *)mxGetData(mexA);
    size_t bytes_to_copy = dim0 * mxGetElementSize(mexA);
    for (int i=0; i<dim1; i++) {
        memcpy(start_of_pr, &(A[i]), bytes_to_copy);
        start_of_pr += bytes_to_copy;
    }
}

void mexSummarizeCmd(CatheterChannelCmd cmd) {
    mexPrintf("channel: %d\n",cmd.channel);
	mexPrintf("current: %f\n",cmd.currentMA);
    //mexPrintf("poll: %d\n", (cmd.cmd4 >> POL_B) & 1);
    //mexPrintf("enable: %d\n", (cmd.cmd4 >> ENA_B) & 1);
    //mexPrintf("update: %d\n", (cmd.cmd4 >> UPD_B) & 1);
    //mexPrintf("dir: %d\n", (cmd.cmd4 >> DIR_B) & 1);
    //mexPrintf("data: %d\n", cmd.data12);
    //if (cmd.waitTime)
    //    mexPrintf("EOP. delay: %dms\n", (int)cmd.waitTime);
}

//#endif //MEXFILE