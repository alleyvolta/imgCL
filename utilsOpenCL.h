//
#include <stdio.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>

#endif

#ifndef UTILSOPENCL
#define UTILSOPENCL

#ifdef __cplusplus
extern "C" {
#endif

void clPrintDevInfo(cl_device_id device);

#ifdef __cplusplus
}
#endif

#endif 
