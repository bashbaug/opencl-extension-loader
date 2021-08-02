#define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_USE_DEPRECATED_OPENCL_2_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_2_APIS
#define CL_USE_DEPRECATED_OPENCL_3_0_APIS
#include "CL/cl.h"
#include <CL/cl_ext.h>

void call_all(void)
{
#ifdef cl_khr_create_command_queue
    clCreateCommandQueueWithPropertiesKHR(NULL, NULL, NULL, NULL);
#endif // cl_khr_create_command_queue

#ifdef cl_khr_il_program
    clCreateProgramWithILKHR(NULL, NULL, 0, NULL);
#endif // cl_khr_il_program

#ifdef cl_khr_subgroups
    clGetKernelSubGroupInfoKHR(NULL, NULL, CL_KERNEL_MAX_SUB_GROUP_SIZE_FOR_NDRANGE_KHR, 0, NULL, 0, NULL, NULL);
#endif // cl_khr_subgroups

#ifdef cl_khr_suggested_local_work_size
    clGetKernelSuggestedLocalWorkSizeKHR(NULL, NULL, 0, NULL, NULL, NULL);
#endif // cl_khr_suggested_local_work_size

#ifdef cl_khr_terminate_context
    clTerminateContextKHR(NULL);
#endif // cl_khr_terminate_context

#ifdef cl_ext_device_fission
    clReleaseDeviceEXT(NULL);
    clRetainDeviceEXT(NULL);
    clCreateSubDevicesEXT(NULL, NULL, 0, NULL, NULL);
#endif // cl_ext_device_fission

#ifdef cl_ext_migrate_memobject
    clEnqueueMigrateMemObjectEXT(NULL, 0, NULL, CL_MIGRATE_MEM_OBJECT_HOST_EXT, 0, NULL, NULL);
#endif // cl_ext_migrate_memobject

#ifdef cl_APPLE_SetMemObjectDestructor
    clSetMemObjectDestructorAPPLE(NULL, NULL, NULL);
#endif // cl_APPLE_SetMemObjectDestructor

#ifdef cl_arm_import_memory
    clImportMemoryARM(NULL, CL_MEM_READ_WRITE, NULL, NULL, 0, NULL);
#endif // cl_arm_import_memory

#ifdef cl_arm_shared_virtual_memory
    clSVMAllocARM(NULL, CL_MEM_SVM_FINE_GRAIN_BUFFER_ARM, 0, 0);
    clSVMFreeARM(NULL, NULL);
    clEnqueueSVMFreeARM(NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    clEnqueueSVMMemcpyARM(NULL, CL_FALSE, NULL, NULL, 0, 0, NULL, NULL);
    clEnqueueSVMMemFillARM(NULL, NULL, NULL, 0, 0, 0, NULL, NULL);
    clEnqueueSVMMapARM(NULL, CL_FALSE, CL_MAP_READ, NULL, 0, 0, NULL, NULL);
    clEnqueueSVMUnmapARM(NULL, NULL, 0, NULL, NULL);
    clSetKernelArgSVMPointerARM(NULL, 0, NULL);
    clSetKernelExecInfoARM(NULL, CL_KERNEL_EXEC_INFO_SVM_PTRS_ARM, 0, NULL);
#endif // cl_arm_shared_virtual_memory

#ifdef cl_img_generate_mipmap
    clEnqueueGenerateMipmapIMG(NULL, NULL, NULL, CL_MIPMAP_FILTER_ANY_IMG, NULL, NULL, 0, NULL, NULL);
#endif // cl_img_generate_mipmap

#ifdef cl_img_use_gralloc_ptr
    clEnqueueAcquireGrallocObjectsIMG(NULL, 0, NULL, 0, NULL, NULL);
    clEnqueueReleaseGrallocObjectsIMG(NULL, 0, NULL, 0, NULL, NULL);
#endif // cl_img_use_gralloc_ptr

#ifdef cl_intel_accelerator
    clCreateAcceleratorINTEL(NULL, 0, 0, NULL, NULL);
    clGetAcceleratorInfoINTEL(NULL, CL_ACCELERATOR_DESCRIPTOR_INTEL, 0, NULL, NULL);
    clRetainAcceleratorINTEL(NULL);
    clReleaseAcceleratorINTEL(NULL);
#endif // cl_intel_accelerator

#ifdef cl_intel_create_buffer_with_properties
    clCreateBufferWithPropertiesINTEL(NULL, NULL, CL_MEM_READ_WRITE, 0, NULL, NULL);
#endif // cl_intel_create_buffer_with_properties

#ifdef cl_intel_unified_shared_memory
    clHostMemAllocINTEL(NULL, NULL, 0, 0, NULL);
    clDeviceMemAllocINTEL(NULL, NULL, NULL, 0, 0, NULL);
    clSharedMemAllocINTEL(NULL, NULL, NULL, 0, 0, NULL);
    clMemFreeINTEL(NULL, NULL);
    clMemBlockingFreeINTEL(NULL, NULL);
    clGetMemAllocInfoINTEL(NULL, NULL, CL_MEM_ALLOC_TYPE_INTEL, 0, NULL, NULL);
    clSetKernelArgMemPointerINTEL(NULL, 0, NULL);
    clEnqueueMemsetINTEL(NULL, NULL, 0, 0, 0, NULL, NULL);
    clEnqueueMemFillINTEL(NULL, NULL, NULL, 0, 0, 0, NULL, NULL);
    clEnqueueMemcpyINTEL(NULL, CL_FALSE, NULL, NULL, 0, 0, NULL, NULL);
    clEnqueueMemAdviseINTEL(NULL, NULL, 0, 0, 0, NULL, NULL);
#endif // cl_intel_unified_shared_memory

#ifdef cl_intel_unified_shared_memory
#if defined(CL_VERSION_1_2)
    clEnqueueMigrateMemINTEL(NULL, NULL, 0, CL_MIGRATE_MEM_OBJECT_HOST, 0, NULL, NULL);
#endif // defined(CL_VERSION_1_2)
#endif // cl_intel_unified_shared_memory

#ifdef cl_qcom_ext_host_ptr
    clGetDeviceImageInfoQCOM(NULL, 0, 0, NULL, CL_IMAGE_ROW_ALIGNMENT_QCOM, 0, NULL, NULL);
#endif // cl_qcom_ext_host_ptr

}
