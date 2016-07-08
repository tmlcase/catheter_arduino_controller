// mex_utils.h

//#ifdef MEXFILE
#ifndef MEX_UTILS_H
#define MEX_UTILS_H

#include "com/common_utils.h"

#include <tmwtypes.h>
#include <mex.h>
#include <matrix.h>

void mexSummarizeCmd(CatheterChannelCmd cmd);

void mexCopy2DIntArray(int * A, mxArray * mexA, int dim0, int dim1);

#endif //MEX_UTILS_H
//#endif //MEXFILE