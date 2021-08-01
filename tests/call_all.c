#define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_USE_DEPRECATED_OPENCL_2_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_2_APIS
#define CL_USE_DEPRECATED_OPENCL_3_0_APIS
#include "CL/cl.h"
#include "CL/cl_ext.h"

void call_all(void)
{
#ifdef cl_khr_create_command_queue
    clCreateCommandQueueWithPropertiesKHR(NULL, NULL, NULL, NULL);
#endif // cl_khr_create_command_queue
#ifdef cl_intel_unified_shared_memory
    clHostMemAllocINTEL(NULL, NULL, 0, 0, NULL);
    clDeviceMemAllocINTEL(NULL, NULL, NULL, 0, 0, NULL);
    clSharedMemAllocINTEL(NULL, NULL, NULL, 0, 0, NULL);
    clMemFreeINTEL(NULL, NULL);
    clMemBlockingFreeINTEL(NULL, NULL);
    clGetMemAllocInfoINTEL(NULL, NULL, CL_MEM_ALLOC_DEVICE_INTEL, 0, NULL, NULL);
    clSetKernelArgMemPointerINTEL(NULL, 0, NULL);
    clEnqueueMemsetINTEL(NULL, NULL, 0, 0, 0, NULL, NULL);
    clEnqueueMemFillINTEL(NULL, NULL, NULL, 0, 0, 0, NULL, NULL);
    clEnqueueMemcpyINTEL(NULL, CL_FALSE, NULL, NULL, 0, 0, NULL, NULL);
    clEnqueueMemAdviseINTEL(NULL, NULL, 0, 0, 0, NULL, NULL);
#if defined(CL_VERSION_1_2)
    clEnqueueMigrateMemINTEL(NULL, NULL, 0, CL_MIGRATE_MEM_OBJECT_HOST, 0, NULL, NULL);
#endif // CL_VERSION_1_2
#endif // cl_intel_unified_shared_memory
}
