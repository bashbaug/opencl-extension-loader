/*******************************************************************************
// Copyright (c) 2021 Ben Ashbaugh
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
*/

#include <CL/cl.h>
#include <CL/cl_ext.h>

#include <vector>

static inline cl_platform_id _get_platform(cl_device_id device)
{
    if (device == NULL) return NULL;

    cl_platform_id platform = NULL;
    clGetDeviceInfo(
        device,
        CL_DEVICE_PLATFORM,
        sizeof(platform),
        &platform,
        NULL);
    return platform;
}

static inline cl_platform_id _get_platform(cl_command_queue command_queue)
{
    if (command_queue == NULL) return NULL;

    cl_device_id device = NULL;
    clGetCommandQueueInfo(
        command_queue,
        CL_QUEUE_DEVICE,
        sizeof(device),
        &device,
        NULL);
    return _get_platform(device);
}

static inline cl_platform_id _get_platform(cl_context context)
{
    if (context == NULL) return NULL;

    cl_uint numDevices = 0;
    clGetContextInfo(
        context,
        CL_CONTEXT_NUM_DEVICES,
        sizeof(numDevices),
        &numDevices,
        NULL );

    if( numDevices == 1 )   // fast path, no dynamic allocation
    {
        cl_device_id    device = NULL;
        clGetContextInfo(
            context,
            CL_CONTEXT_DEVICES,
            sizeof(cl_device_id),
            &device,
            NULL );
        return _get_platform(device);
    }

    // slower path, dynamic allocation
    std::vector<cl_device_id> devices(numDevices);
    clGetContextInfo(
        context,
        CL_CONTEXT_DEVICES,
        numDevices * sizeof(cl_device_id),
        devices.data(),
        NULL );
    return _get_platform(devices[0]);
}

static inline cl_platform_id _get_platform(cl_kernel kernel)
{
    if (kernel == NULL) return NULL;

    cl_context context = NULL;
    clGetKernelInfo(
        kernel,
        CL_KERNEL_CONTEXT,
        sizeof(context),
        &context,
        NULL);
    return _get_platform(context);
}

static inline cl_platform_id _get_platform(cl_mem memobj)
{
    if (memobj == NULL) return NULL;

    cl_context context = NULL;
    clGetMemObjectInfo(
        memobj,
        CL_MEM_CONTEXT,
        sizeof(context),
        &context,
        NULL);
    return _get_platform(context);
}

#if defined(cl_intel_accelerator)

static inline cl_platform_id _get_platform(cl_accelerator_intel accelerator)
{
    if (accelerator == NULL) return NULL;

    cl_context context = NULL;
    clGetAcceleratorInfoINTEL(
        accelerator,
        CL_ACCELERATOR_CONTEXT_INTEL,
        sizeof(context),
        &context,
        NULL);
    return _get_platform(context);
}

#endif // defined(cl_intel_accelerator)

/***************************************************************
* Function Pointer Typedefs
***************************************************************/

/* cl_khr_create_command_queue */

typedef cl_command_queue (CL_API_CALL* clCreateCommandQueueWithPropertiesKHR_clextfn)(
    cl_context context,
    cl_device_id device,
    const cl_queue_properties_khr* properties,
    cl_int* errcode_ret);

/* cl_khr_depth_images */

/* cl_khr_device_uuid */

/* cl_khr_extended_versioning */

/* cl_khr_fp16 */

/* cl_khr_fp64 */

/* cl_khr_il_program */

typedef cl_program (CL_API_CALL* clCreateProgramWithILKHR_clextfn)(
    cl_context context,
    const void* il,
    size_t length,
    cl_int* errcode_ret);

/* cl_khr_image2D_from_buffer */

/* cl_khr_initialize_memory */

/* cl_khr_integer_dot_product */

/* cl_khr_mipmap_image */

/* cl_khr_pci_bus_info */

/* cl_khr_priority_hints */

/* cl_khr_spir */

/* cl_khr_subgroup_named_barrier */

/* cl_khr_subgroups */

typedef cl_int (CL_API_CALL* clGetKernelSubGroupInfoKHR_clextfn)(
    cl_kernel in_kernel,
    cl_device_id in_device,
    cl_kernel_sub_group_info param_name,
    size_t input_value_size,
    const void* input_value,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret);

/* cl_khr_suggested_local_work_size */

typedef cl_int (CL_API_CALL* clGetKernelSuggestedLocalWorkSizeKHR_clextfn)(
    cl_command_queue command_queue,
    cl_kernel kernel,
    cl_uint work_dim,
    const size_t* global_work_offset,
    const size_t* global_work_size,
    size_t* suggested_local_work_size);

/* cl_khr_terminate_context */

typedef cl_int (CL_API_CALL* clTerminateContextKHR_clextfn)(
    cl_context context);

/* cl_khr_throttle_hints */

/* cl_ext_cxx_for_opencl */

/* cl_ext_device_fission */

typedef cl_int (CL_API_CALL* clReleaseDeviceEXT_clextfn)(
    cl_device_id device);

typedef cl_int (CL_API_CALL* clRetainDeviceEXT_clextfn)(
    cl_device_id device);

typedef cl_int (CL_API_CALL* clCreateSubDevicesEXT_clextfn)(
    cl_device_id in_device,
    const cl_device_partition_property_ext* properties,
    cl_uint num_entries,
    cl_device_id* out_devices,
    cl_uint* num_devices);

/* cl_ext_migrate_memobject */

typedef cl_int (CL_API_CALL* clEnqueueMigrateMemObjectEXT_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_mem_objects,
    const cl_mem* mem_objects,
    cl_mem_migration_flags_ext flags,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

/* cl_APPLE_SetMemObjectDestructor */

typedef cl_int (CL_API_CALL* clSetMemObjectDestructorAPPLE_clextfn)(
    cl_mem memobj,
    void (CL_CALLBACK* pfn_notify)(cl_mem memobj, void* user_data),
    void* user_data);

/* cl_amd_device_attribute_query */

/* cl_arm_controlled_kernel_termination */

/* cl_arm_get_core_id */

/* cl_arm_import_memory */

typedef cl_mem (CL_API_CALL* clImportMemoryARM_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    const cl_import_properties_arm* properties,
    void* memory,
    size_t size,
    cl_int* errcode_ret);

/* cl_arm_printf */

/* cl_arm_scheduling_controls */

/* cl_arm_shared_virtual_memory */

typedef void* (CL_API_CALL* clSVMAllocARM_clextfn)(
    cl_context context,
    cl_svm_mem_flags_arm flags,
    size_t size,
    cl_uint alignment);

typedef void (CL_API_CALL* clSVMFreeARM_clextfn)(
    cl_context context,
    void* svm_pointer);

typedef cl_int (CL_API_CALL* clEnqueueSVMFreeARM_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_svm_pointers,
    void* svm_pointers[],
    void (CL_CALLBACK* pfn_free_func)(cl_command_queue queue, cl_uint num_svm_pointers, void * svm_pointers[], void *user_data),
    void* user_data,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueSVMMemcpyARM_clextfn)(
    cl_command_queue command_queue,
    cl_bool blocking_copy,
    void* dst_ptr,
    const void* src_ptr,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueSVMMemFillARM_clextfn)(
    cl_command_queue command_queue,
    void* svm_ptr,
    const void* pattern,
    size_t pattern_size,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueSVMMapARM_clextfn)(
    cl_command_queue command_queue,
    cl_bool blocking_map,
    cl_map_flags flags,
    void* svm_ptr,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueSVMUnmapARM_clextfn)(
    cl_command_queue command_queue,
    void* svm_ptr,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clSetKernelArgSVMPointerARM_clextfn)(
    cl_kernel kernel,
    cl_uint arg_index,
    const void* arg_value);

typedef cl_int (CL_API_CALL* clSetKernelExecInfoARM_clextfn)(
    cl_kernel kernel,
    cl_kernel_exec_info_arm param_name,
    size_t param_value_size,
    const void* param_value);

/* cl_img_cached_allocations */

/* cl_img_generate_mipmap */

typedef cl_int (CL_API_CALL* clEnqueueGenerateMipmapIMG_clextfn)(
    cl_command_queue command_queue,
    cl_mem src_image,
    cl_mem dst_image,
    cl_mipmap_filter_mode_img mipmap_filter_mode,
    const size_t* array_region,
    const size_t* mip_region,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

/* cl_img_mem_properties */

/* cl_img_use_gralloc_ptr */

typedef cl_int (CL_API_CALL* clEnqueueAcquireGrallocObjectsIMG_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueReleaseGrallocObjectsIMG_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

/* cl_img_yuv_image */

/* cl_intel_accelerator */

typedef cl_accelerator_intel (CL_API_CALL* clCreateAcceleratorINTEL_clextfn)(
    cl_context context,
    cl_accelerator_type_intel accelerator_type,
    size_t descriptor_size,
    const void* descriptor,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clGetAcceleratorInfoINTEL_clextfn)(
    cl_accelerator_intel accelerator,
    cl_accelerator_info_intel param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret);

typedef cl_int (CL_API_CALL* clRetainAcceleratorINTEL_clextfn)(
    cl_accelerator_intel accelerator);

typedef cl_int (CL_API_CALL* clReleaseAcceleratorINTEL_clextfn)(
    cl_accelerator_intel accelerator);

/* cl_intel_advanced_motion_estimation */

/* cl_intel_command_queue_families */

/* cl_intel_create_buffer_with_properties */

typedef cl_mem (CL_API_CALL* clCreateBufferWithPropertiesINTEL_clextfn)(
    cl_context context,
    const cl_mem_properties_intel* properties,
    cl_mem_flags flags,
    size_t size,
    void* host_ptr,
    cl_int* errcode_ret);

/* cl_intel_device_partition_by_names */

/* cl_intel_device_side_avc_motion_estimation */

/* cl_intel_driver_diagnostics */

/* cl_intel_egl_image_yuv */

/* cl_intel_mem_channel_property */

/* cl_intel_mem_force_host_memory */

/* cl_intel_motion_estimation */

/* cl_intel_packed_yuv */

/* cl_intel_planar_yuv */

/* cl_intel_required_subgroup_size */

/* cl_intel_sharing_format_query */

/* cl_intel_simultaneous_sharing */

/* cl_intel_thread_local_exec */

/* cl_intel_unified_shared_memory */

typedef void* (CL_API_CALL* clHostMemAllocINTEL_clextfn)(
    cl_context context,
    const cl_mem_properties_intel* properties,
    size_t size,
    cl_uint alignment,
    cl_int* errcode_ret);

typedef void* (CL_API_CALL* clDeviceMemAllocINTEL_clextfn)(
    cl_context context,
    cl_device_id device,
    const cl_mem_properties_intel* properties,
    size_t size,
    cl_uint alignment,
    cl_int* errcode_ret);

typedef void* (CL_API_CALL* clSharedMemAllocINTEL_clextfn)(
    cl_context context,
    cl_device_id device,
    const cl_mem_properties_intel* properties,
    size_t size,
    cl_uint alignment,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clMemFreeINTEL_clextfn)(
    cl_context context,
    void* ptr);

typedef cl_int (CL_API_CALL* clMemBlockingFreeINTEL_clextfn)(
    cl_context context,
    void* ptr);

typedef cl_int (CL_API_CALL* clGetMemAllocInfoINTEL_clextfn)(
    cl_context context,
    const void* ptr,
    cl_mem_info_intel param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret);

typedef cl_int (CL_API_CALL* clSetKernelArgMemPointerINTEL_clextfn)(
    cl_kernel kernel,
    cl_uint arg_index,
    const void* arg_value);

typedef cl_int (CL_API_CALL* clEnqueueMemsetINTEL_clextfn)(
    cl_command_queue command_queue,
    void* dst_ptr,
    cl_int value,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueMemFillINTEL_clextfn)(
    cl_command_queue command_queue,
    void* dst_ptr,
    const void* pattern,
    size_t pattern_size,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueMemcpyINTEL_clextfn)(
    cl_command_queue command_queue,
    cl_bool blocking,
    void* dst_ptr,
    const void* src_ptr,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueMemAdviseINTEL_clextfn)(
    cl_command_queue command_queue,
    const void* ptr,
    size_t size,
    cl_mem_advice_intel advice,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

#if defined(CL_VERSION_1_2)

typedef cl_int (CL_API_CALL* clEnqueueMigrateMemINTEL_clextfn)(
    cl_command_queue command_queue,
    const void* ptr,
    size_t size,
    cl_mem_migration_flags flags,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

#endif

/* cl_nv_device_attribute_query */

/* cl_qcom_android_native_buffer_host_ptr */

/* cl_qcom_ext_host_ptr */

typedef cl_int (CL_API_CALL* clGetDeviceImageInfoQCOM_clextfn)(
    cl_device_id device,
    size_t image_width,
    size_t image_height,
    const cl_image_format* image_format,
    cl_image_pitch_info_qcom param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret);

/* cl_qcom_ext_host_ptr_iocoherent */

/* cl_qcom_ion_host_ptr */


/***************************************************************
* Extension Function Pointer Dispatch Table
***************************************************************/

struct openclext_dispatch_table {
    /* cl_khr_create_command_queue */
    clCreateCommandQueueWithPropertiesKHR_clextfn clCreateCommandQueueWithPropertiesKHR;

    /* cl_khr_depth_images */

    /* cl_khr_device_uuid */

    /* cl_khr_extended_versioning */

    /* cl_khr_fp16 */

    /* cl_khr_fp64 */

    /* cl_khr_il_program */
    clCreateProgramWithILKHR_clextfn clCreateProgramWithILKHR;

    /* cl_khr_image2D_from_buffer */

    /* cl_khr_initialize_memory */

    /* cl_khr_integer_dot_product */

    /* cl_khr_mipmap_image */

    /* cl_khr_pci_bus_info */

    /* cl_khr_priority_hints */

    /* cl_khr_spir */

    /* cl_khr_subgroup_named_barrier */

    /* cl_khr_subgroups */
    clGetKernelSubGroupInfoKHR_clextfn clGetKernelSubGroupInfoKHR;

    /* cl_khr_suggested_local_work_size */
    clGetKernelSuggestedLocalWorkSizeKHR_clextfn clGetKernelSuggestedLocalWorkSizeKHR;

    /* cl_khr_terminate_context */
    clTerminateContextKHR_clextfn clTerminateContextKHR;

    /* cl_khr_throttle_hints */

    /* cl_ext_cxx_for_opencl */

    /* cl_ext_device_fission */
    clReleaseDeviceEXT_clextfn clReleaseDeviceEXT;
    clRetainDeviceEXT_clextfn clRetainDeviceEXT;
    clCreateSubDevicesEXT_clextfn clCreateSubDevicesEXT;

    /* cl_ext_migrate_memobject */
    clEnqueueMigrateMemObjectEXT_clextfn clEnqueueMigrateMemObjectEXT;

    /* cl_APPLE_SetMemObjectDestructor */
    clSetMemObjectDestructorAPPLE_clextfn clSetMemObjectDestructorAPPLE;

    /* cl_amd_device_attribute_query */

    /* cl_arm_controlled_kernel_termination */

    /* cl_arm_get_core_id */

    /* cl_arm_import_memory */
    clImportMemoryARM_clextfn clImportMemoryARM;

    /* cl_arm_printf */

    /* cl_arm_scheduling_controls */

    /* cl_arm_shared_virtual_memory */
    clSVMAllocARM_clextfn clSVMAllocARM;
    clSVMFreeARM_clextfn clSVMFreeARM;
    clEnqueueSVMFreeARM_clextfn clEnqueueSVMFreeARM;
    clEnqueueSVMMemcpyARM_clextfn clEnqueueSVMMemcpyARM;
    clEnqueueSVMMemFillARM_clextfn clEnqueueSVMMemFillARM;
    clEnqueueSVMMapARM_clextfn clEnqueueSVMMapARM;
    clEnqueueSVMUnmapARM_clextfn clEnqueueSVMUnmapARM;
    clSetKernelArgSVMPointerARM_clextfn clSetKernelArgSVMPointerARM;
    clSetKernelExecInfoARM_clextfn clSetKernelExecInfoARM;

    /* cl_img_cached_allocations */

    /* cl_img_generate_mipmap */
    clEnqueueGenerateMipmapIMG_clextfn clEnqueueGenerateMipmapIMG;

    /* cl_img_mem_properties */

    /* cl_img_use_gralloc_ptr */
    clEnqueueAcquireGrallocObjectsIMG_clextfn clEnqueueAcquireGrallocObjectsIMG;
    clEnqueueReleaseGrallocObjectsIMG_clextfn clEnqueueReleaseGrallocObjectsIMG;

    /* cl_img_yuv_image */

    /* cl_intel_accelerator */
    clCreateAcceleratorINTEL_clextfn clCreateAcceleratorINTEL;
    clGetAcceleratorInfoINTEL_clextfn clGetAcceleratorInfoINTEL;
    clRetainAcceleratorINTEL_clextfn clRetainAcceleratorINTEL;
    clReleaseAcceleratorINTEL_clextfn clReleaseAcceleratorINTEL;

    /* cl_intel_advanced_motion_estimation */

    /* cl_intel_command_queue_families */

    /* cl_intel_create_buffer_with_properties */
    clCreateBufferWithPropertiesINTEL_clextfn clCreateBufferWithPropertiesINTEL;

    /* cl_intel_device_partition_by_names */

    /* cl_intel_device_side_avc_motion_estimation */

    /* cl_intel_driver_diagnostics */

    /* cl_intel_egl_image_yuv */

    /* cl_intel_mem_channel_property */

    /* cl_intel_mem_force_host_memory */

    /* cl_intel_motion_estimation */

    /* cl_intel_packed_yuv */

    /* cl_intel_planar_yuv */

    /* cl_intel_required_subgroup_size */

    /* cl_intel_sharing_format_query */

    /* cl_intel_simultaneous_sharing */

    /* cl_intel_thread_local_exec */

    /* cl_intel_unified_shared_memory */
    clHostMemAllocINTEL_clextfn clHostMemAllocINTEL;
    clDeviceMemAllocINTEL_clextfn clDeviceMemAllocINTEL;
    clSharedMemAllocINTEL_clextfn clSharedMemAllocINTEL;
    clMemFreeINTEL_clextfn clMemFreeINTEL;
    clMemBlockingFreeINTEL_clextfn clMemBlockingFreeINTEL;
    clGetMemAllocInfoINTEL_clextfn clGetMemAllocInfoINTEL;
    clSetKernelArgMemPointerINTEL_clextfn clSetKernelArgMemPointerINTEL;
    clEnqueueMemsetINTEL_clextfn clEnqueueMemsetINTEL;
    clEnqueueMemFillINTEL_clextfn clEnqueueMemFillINTEL;
    clEnqueueMemcpyINTEL_clextfn clEnqueueMemcpyINTEL;
    clEnqueueMemAdviseINTEL_clextfn clEnqueueMemAdviseINTEL;
#if defined(CL_VERSION_1_2)
    clEnqueueMigrateMemINTEL_clextfn clEnqueueMigrateMemINTEL;
#endif

    /* cl_nv_device_attribute_query */

    /* cl_qcom_android_native_buffer_host_ptr */

    /* cl_qcom_ext_host_ptr */
    clGetDeviceImageInfoQCOM_clextfn clGetDeviceImageInfoQCOM;

    /* cl_qcom_ext_host_ptr_iocoherent */

    /* cl_qcom_ion_host_ptr */

};

/***************************************************************
* Dispatch Table Initialization (TODO)
***************************************************************/

static openclext_dispatch_table _dispatch = {0};

static void _init(cl_platform_id platform, openclext_dispatch_table* dispatch_ptr)
{
#define GET_EXTENSION(_funcname)                                               \
    dispatch_ptr->##_funcname =                                                \
        (_funcname##_fn)clGetExtensionFunctionAddressForPlatform(platform,     \
                                                                 #_funcname);

    /* cl_khr_create_command_queue */
    GET_EXTENSION(clCreateCommandQueueWithPropertiesKHR);

    /* cl_khr_depth_images */

    /* cl_khr_device_uuid */

    /* cl_khr_extended_versioning */

    /* cl_khr_fp16 */

    /* cl_khr_fp64 */

    /* cl_khr_il_program */
    GET_EXTENSION(clCreateProgramWithILKHR);

    /* cl_khr_image2D_from_buffer */

    /* cl_khr_initialize_memory */

    /* cl_khr_integer_dot_product */

    /* cl_khr_mipmap_image */

    /* cl_khr_pci_bus_info */

    /* cl_khr_priority_hints */

    /* cl_khr_spir */

    /* cl_khr_subgroup_named_barrier */

    /* cl_khr_subgroups */
    GET_EXTENSION(clGetKernelSubGroupInfoKHR);

    /* cl_khr_suggested_local_work_size */
    GET_EXTENSION(clGetKernelSuggestedLocalWorkSizeKHR);

    /* cl_khr_terminate_context */
    GET_EXTENSION(clTerminateContextKHR);

    /* cl_khr_throttle_hints */

    /* cl_ext_cxx_for_opencl */

    /* cl_ext_device_fission */
    GET_EXTENSION(clReleaseDeviceEXT);
    GET_EXTENSION(clRetainDeviceEXT);
    GET_EXTENSION(clCreateSubDevicesEXT);

    /* cl_ext_migrate_memobject */
    GET_EXTENSION(clEnqueueMigrateMemObjectEXT);

    /* cl_APPLE_SetMemObjectDestructor */
    GET_EXTENSION(clSetMemObjectDestructorAPPLE);

    /* cl_amd_device_attribute_query */

    /* cl_arm_controlled_kernel_termination */

    /* cl_arm_get_core_id */

    /* cl_arm_import_memory */
    GET_EXTENSION(clImportMemoryARM);

    /* cl_arm_printf */

    /* cl_arm_scheduling_controls */

    /* cl_arm_shared_virtual_memory */
    GET_EXTENSION(clSVMAllocARM);
    GET_EXTENSION(clSVMFreeARM);
    GET_EXTENSION(clEnqueueSVMFreeARM);
    GET_EXTENSION(clEnqueueSVMMemcpyARM);
    GET_EXTENSION(clEnqueueSVMMemFillARM);
    GET_EXTENSION(clEnqueueSVMMapARM);
    GET_EXTENSION(clEnqueueSVMUnmapARM);
    GET_EXTENSION(clSetKernelArgSVMPointerARM);
    GET_EXTENSION(clSetKernelExecInfoARM);

    /* cl_img_cached_allocations */

    /* cl_img_generate_mipmap */
    GET_EXTENSION(clEnqueueGenerateMipmapIMG);

    /* cl_img_mem_properties */

    /* cl_img_use_gralloc_ptr */
    GET_EXTENSION(clEnqueueAcquireGrallocObjectsIMG);
    GET_EXTENSION(clEnqueueReleaseGrallocObjectsIMG);

    /* cl_img_yuv_image */

    /* cl_intel_accelerator */
    GET_EXTENSION(clCreateAcceleratorINTEL);
    GET_EXTENSION(clGetAcceleratorInfoINTEL);
    GET_EXTENSION(clRetainAcceleratorINTEL);
    GET_EXTENSION(clReleaseAcceleratorINTEL);

    /* cl_intel_advanced_motion_estimation */

    /* cl_intel_command_queue_families */

    /* cl_intel_create_buffer_with_properties */
    GET_EXTENSION(clCreateBufferWithPropertiesINTEL);

    /* cl_intel_device_partition_by_names */

    /* cl_intel_device_side_avc_motion_estimation */

    /* cl_intel_driver_diagnostics */

    /* cl_intel_egl_image_yuv */

    /* cl_intel_mem_channel_property */

    /* cl_intel_mem_force_host_memory */

    /* cl_intel_motion_estimation */

    /* cl_intel_packed_yuv */

    /* cl_intel_planar_yuv */

    /* cl_intel_required_subgroup_size */

    /* cl_intel_sharing_format_query */

    /* cl_intel_simultaneous_sharing */

    /* cl_intel_thread_local_exec */

    /* cl_intel_unified_shared_memory */
    GET_EXTENSION(clHostMemAllocINTEL);
    GET_EXTENSION(clDeviceMemAllocINTEL);
    GET_EXTENSION(clSharedMemAllocINTEL);
    GET_EXTENSION(clMemFreeINTEL);
    GET_EXTENSION(clMemBlockingFreeINTEL);
    GET_EXTENSION(clGetMemAllocInfoINTEL);
    GET_EXTENSION(clSetKernelArgMemPointerINTEL);
    GET_EXTENSION(clEnqueueMemsetINTEL);
    GET_EXTENSION(clEnqueueMemFillINTEL);
    GET_EXTENSION(clEnqueueMemcpyINTEL);
    GET_EXTENSION(clEnqueueMemAdviseINTEL);
#if defined(CL_VERSION_1_2)
    GET_EXTENSION(clEnqueueMigrateMemINTEL);
#endif

    /* cl_nv_device_attribute_query */

    /* cl_qcom_android_native_buffer_host_ptr */

    /* cl_qcom_ext_host_ptr */
    GET_EXTENSION(clGetDeviceImageInfoQCOM);

    /* cl_qcom_ext_host_ptr_iocoherent */

    /* cl_qcom_ion_host_ptr */

#undef GET_EXTENSION
}

static openclext_dispatch_table* _dispatch_ptr = NULL;

template<typename T>
static inline openclext_dispatch_table* _get_dispatch(T object)
{
    if (_dispatch_ptr == NULL) {
        cl_platform_id platform = _get_platform(object);
        _init(platform, &_dispatch);
        _dispatch_ptr = &_dispatch;
    }
    return _dispatch_ptr;
}

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* Extension Functions
***************************************************************/

/* cl_khr_create_command_queue */

cl_command_queue clCreateCommandQueueWithPropertiesKHR(
    cl_context context,
    cl_device_id device,
    const cl_queue_properties_khr* properties,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateCommandQueueWithPropertiesKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateCommandQueueWithPropertiesKHR(
        context,
        device,
        properties,
        errcode_ret);
}

/* cl_khr_depth_images */

/* cl_khr_device_uuid */

/* cl_khr_extended_versioning */

/* cl_khr_fp16 */

/* cl_khr_fp64 */

/* cl_khr_il_program */

cl_program clCreateProgramWithILKHR(
    cl_context context,
    const void* il,
    size_t length,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateProgramWithILKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateProgramWithILKHR(
        context,
        il,
        length,
        errcode_ret);
}

/* cl_khr_image2D_from_buffer */

/* cl_khr_initialize_memory */

/* cl_khr_integer_dot_product */

/* cl_khr_mipmap_image */

/* cl_khr_pci_bus_info */

/* cl_khr_priority_hints */

/* cl_khr_spir */

/* cl_khr_subgroup_named_barrier */

/* cl_khr_subgroups */

cl_int clGetKernelSubGroupInfoKHR(
    cl_kernel in_kernel,
    cl_device_id in_device,
    cl_kernel_sub_group_info param_name,
    size_t input_value_size,
    const void* input_value,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(in_kernel);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetKernelSubGroupInfoKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetKernelSubGroupInfoKHR(
        in_kernel,
        in_device,
        param_name,
        input_value_size,
        input_value,
        param_value_size,
        param_value,
        param_value_size_ret);
}

/* cl_khr_suggested_local_work_size */

cl_int clGetKernelSuggestedLocalWorkSizeKHR(
    cl_command_queue command_queue,
    cl_kernel kernel,
    cl_uint work_dim,
    const size_t* global_work_offset,
    const size_t* global_work_size,
    size_t* suggested_local_work_size)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetKernelSuggestedLocalWorkSizeKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetKernelSuggestedLocalWorkSizeKHR(
        command_queue,
        kernel,
        work_dim,
        global_work_offset,
        global_work_size,
        suggested_local_work_size);
}

/* cl_khr_terminate_context */

cl_int clTerminateContextKHR(
    cl_context context)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clTerminateContextKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clTerminateContextKHR(
        context);
}

/* cl_khr_throttle_hints */

/* cl_ext_cxx_for_opencl */

/* cl_ext_device_fission */

cl_int clReleaseDeviceEXT(
    cl_device_id device)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(device);
    if (dispatch_ptr == NULL || dispatch_ptr->clReleaseDeviceEXT == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clReleaseDeviceEXT(
        device);
}

cl_int clRetainDeviceEXT(
    cl_device_id device)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(device);
    if (dispatch_ptr == NULL || dispatch_ptr->clRetainDeviceEXT == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clRetainDeviceEXT(
        device);
}

cl_int clCreateSubDevicesEXT(
    cl_device_id in_device,
    const cl_device_partition_property_ext* properties,
    cl_uint num_entries,
    cl_device_id* out_devices,
    cl_uint* num_devices)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(in_device);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateSubDevicesEXT == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clCreateSubDevicesEXT(
        in_device,
        properties,
        num_entries,
        out_devices,
        num_devices);
}

/* cl_ext_migrate_memobject */

cl_int clEnqueueMigrateMemObjectEXT(
    cl_command_queue command_queue,
    cl_uint num_mem_objects,
    const cl_mem* mem_objects,
    cl_mem_migration_flags_ext flags,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueMigrateMemObjectEXT == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueMigrateMemObjectEXT(
        command_queue,
        num_mem_objects,
        mem_objects,
        flags,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

/* cl_APPLE_SetMemObjectDestructor */

cl_int clSetMemObjectDestructorAPPLE(
    cl_mem memobj,
    void (CL_CALLBACK* pfn_notify)(cl_mem memobj, void* user_data),
    void* user_data)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(memobj);
    if (dispatch_ptr == NULL || dispatch_ptr->clSetMemObjectDestructorAPPLE == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clSetMemObjectDestructorAPPLE(
        memobj,
        pfn_notify,
        user_data);
}

/* cl_amd_device_attribute_query */

/* cl_arm_controlled_kernel_termination */

/* cl_arm_get_core_id */

/* cl_arm_import_memory */

cl_mem clImportMemoryARM(
    cl_context context,
    cl_mem_flags flags,
    const cl_import_properties_arm* properties,
    void* memory,
    size_t size,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clImportMemoryARM == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clImportMemoryARM(
        context,
        flags,
        properties,
        memory,
        size,
        errcode_ret);
}

/* cl_arm_printf */

/* cl_arm_scheduling_controls */

/* cl_arm_shared_virtual_memory */

void* clSVMAllocARM(
    cl_context context,
    cl_svm_mem_flags_arm flags,
    size_t size,
    cl_uint alignment)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clSVMAllocARM == NULL) {
        return NULL;
    }
    return dispatch_ptr->clSVMAllocARM(
        context,
        flags,
        size,
        alignment);
}

void clSVMFreeARM(
    cl_context context,
    void* svm_pointer)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clSVMFreeARM == NULL) {
        return;
    }
    dispatch_ptr->clSVMFreeARM(
        context,
        svm_pointer);
}

cl_int clEnqueueSVMFreeARM(
    cl_command_queue command_queue,
    cl_uint num_svm_pointers,
    void* svm_pointers[],
    void (CL_CALLBACK* pfn_free_func)(cl_command_queue queue, cl_uint num_svm_pointers, void * svm_pointers[], void *user_data),
    void* user_data,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueSVMFreeARM == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueSVMFreeARM(
        command_queue,
        num_svm_pointers,
        svm_pointers,
        pfn_free_func,
        user_data,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clEnqueueSVMMemcpyARM(
    cl_command_queue command_queue,
    cl_bool blocking_copy,
    void* dst_ptr,
    const void* src_ptr,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueSVMMemcpyARM == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueSVMMemcpyARM(
        command_queue,
        blocking_copy,
        dst_ptr,
        src_ptr,
        size,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clEnqueueSVMMemFillARM(
    cl_command_queue command_queue,
    void* svm_ptr,
    const void* pattern,
    size_t pattern_size,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueSVMMemFillARM == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueSVMMemFillARM(
        command_queue,
        svm_ptr,
        pattern,
        pattern_size,
        size,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clEnqueueSVMMapARM(
    cl_command_queue command_queue,
    cl_bool blocking_map,
    cl_map_flags flags,
    void* svm_ptr,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueSVMMapARM == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueSVMMapARM(
        command_queue,
        blocking_map,
        flags,
        svm_ptr,
        size,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clEnqueueSVMUnmapARM(
    cl_command_queue command_queue,
    void* svm_ptr,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueSVMUnmapARM == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueSVMUnmapARM(
        command_queue,
        svm_ptr,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clSetKernelArgSVMPointerARM(
    cl_kernel kernel,
    cl_uint arg_index,
    const void* arg_value)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(kernel);
    if (dispatch_ptr == NULL || dispatch_ptr->clSetKernelArgSVMPointerARM == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clSetKernelArgSVMPointerARM(
        kernel,
        arg_index,
        arg_value);
}

cl_int clSetKernelExecInfoARM(
    cl_kernel kernel,
    cl_kernel_exec_info_arm param_name,
    size_t param_value_size,
    const void* param_value)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(kernel);
    if (dispatch_ptr == NULL || dispatch_ptr->clSetKernelExecInfoARM == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clSetKernelExecInfoARM(
        kernel,
        param_name,
        param_value_size,
        param_value);
}

/* cl_img_cached_allocations */

/* cl_img_generate_mipmap */

cl_int clEnqueueGenerateMipmapIMG(
    cl_command_queue command_queue,
    cl_mem src_image,
    cl_mem dst_image,
    cl_mipmap_filter_mode_img mipmap_filter_mode,
    const size_t* array_region,
    const size_t* mip_region,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueGenerateMipmapIMG == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueGenerateMipmapIMG(
        command_queue,
        src_image,
        dst_image,
        mipmap_filter_mode,
        array_region,
        mip_region,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

/* cl_img_mem_properties */

/* cl_img_use_gralloc_ptr */

cl_int clEnqueueAcquireGrallocObjectsIMG(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueAcquireGrallocObjectsIMG == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueAcquireGrallocObjectsIMG(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clEnqueueReleaseGrallocObjectsIMG(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueReleaseGrallocObjectsIMG == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueReleaseGrallocObjectsIMG(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

/* cl_img_yuv_image */

/* cl_intel_accelerator */

cl_accelerator_intel clCreateAcceleratorINTEL(
    cl_context context,
    cl_accelerator_type_intel accelerator_type,
    size_t descriptor_size,
    const void* descriptor,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateAcceleratorINTEL == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateAcceleratorINTEL(
        context,
        accelerator_type,
        descriptor_size,
        descriptor,
        errcode_ret);
}

cl_int clGetAcceleratorInfoINTEL(
    cl_accelerator_intel accelerator,
    cl_accelerator_info_intel param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(accelerator);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetAcceleratorInfoINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetAcceleratorInfoINTEL(
        accelerator,
        param_name,
        param_value_size,
        param_value,
        param_value_size_ret);
}

cl_int clRetainAcceleratorINTEL(
    cl_accelerator_intel accelerator)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(accelerator);
    if (dispatch_ptr == NULL || dispatch_ptr->clRetainAcceleratorINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clRetainAcceleratorINTEL(
        accelerator);
}

cl_int clReleaseAcceleratorINTEL(
    cl_accelerator_intel accelerator)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(accelerator);
    if (dispatch_ptr == NULL || dispatch_ptr->clReleaseAcceleratorINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clReleaseAcceleratorINTEL(
        accelerator);
}

/* cl_intel_advanced_motion_estimation */

/* cl_intel_command_queue_families */

/* cl_intel_create_buffer_with_properties */

cl_mem clCreateBufferWithPropertiesINTEL(
    cl_context context,
    const cl_mem_properties_intel* properties,
    cl_mem_flags flags,
    size_t size,
    void* host_ptr,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateBufferWithPropertiesINTEL == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateBufferWithPropertiesINTEL(
        context,
        properties,
        flags,
        size,
        host_ptr,
        errcode_ret);
}

/* cl_intel_device_partition_by_names */

/* cl_intel_device_side_avc_motion_estimation */

/* cl_intel_driver_diagnostics */

/* cl_intel_egl_image_yuv */

/* cl_intel_mem_channel_property */

/* cl_intel_mem_force_host_memory */

/* cl_intel_motion_estimation */

/* cl_intel_packed_yuv */

/* cl_intel_planar_yuv */

/* cl_intel_required_subgroup_size */

/* cl_intel_sharing_format_query */

/* cl_intel_simultaneous_sharing */

/* cl_intel_thread_local_exec */

/* cl_intel_unified_shared_memory */

void* clHostMemAllocINTEL(
    cl_context context,
    const cl_mem_properties_intel* properties,
    size_t size,
    cl_uint alignment,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clHostMemAllocINTEL == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clHostMemAllocINTEL(
        context,
        properties,
        size,
        alignment,
        errcode_ret);
}

void* clDeviceMemAllocINTEL(
    cl_context context,
    cl_device_id device,
    const cl_mem_properties_intel* properties,
    size_t size,
    cl_uint alignment,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clDeviceMemAllocINTEL == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clDeviceMemAllocINTEL(
        context,
        device,
        properties,
        size,
        alignment,
        errcode_ret);
}

void* clSharedMemAllocINTEL(
    cl_context context,
    cl_device_id device,
    const cl_mem_properties_intel* properties,
    size_t size,
    cl_uint alignment,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clSharedMemAllocINTEL == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clSharedMemAllocINTEL(
        context,
        device,
        properties,
        size,
        alignment,
        errcode_ret);
}

cl_int clMemFreeINTEL(
    cl_context context,
    void* ptr)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clMemFreeINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clMemFreeINTEL(
        context,
        ptr);
}

cl_int clMemBlockingFreeINTEL(
    cl_context context,
    void* ptr)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clMemBlockingFreeINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clMemBlockingFreeINTEL(
        context,
        ptr);
}

cl_int clGetMemAllocInfoINTEL(
    cl_context context,
    const void* ptr,
    cl_mem_info_intel param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetMemAllocInfoINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetMemAllocInfoINTEL(
        context,
        ptr,
        param_name,
        param_value_size,
        param_value,
        param_value_size_ret);
}

cl_int clSetKernelArgMemPointerINTEL(
    cl_kernel kernel,
    cl_uint arg_index,
    const void* arg_value)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(kernel);
    if (dispatch_ptr == NULL || dispatch_ptr->clSetKernelArgMemPointerINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clSetKernelArgMemPointerINTEL(
        kernel,
        arg_index,
        arg_value);
}

cl_int clEnqueueMemsetINTEL(
    cl_command_queue command_queue,
    void* dst_ptr,
    cl_int value,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueMemsetINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueMemsetINTEL(
        command_queue,
        dst_ptr,
        value,
        size,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clEnqueueMemFillINTEL(
    cl_command_queue command_queue,
    void* dst_ptr,
    const void* pattern,
    size_t pattern_size,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueMemFillINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueMemFillINTEL(
        command_queue,
        dst_ptr,
        pattern,
        pattern_size,
        size,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clEnqueueMemcpyINTEL(
    cl_command_queue command_queue,
    cl_bool blocking,
    void* dst_ptr,
    const void* src_ptr,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueMemcpyINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueMemcpyINTEL(
        command_queue,
        blocking,
        dst_ptr,
        src_ptr,
        size,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int clEnqueueMemAdviseINTEL(
    cl_command_queue command_queue,
    const void* ptr,
    size_t size,
    cl_mem_advice_intel advice,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueMemAdviseINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueMemAdviseINTEL(
        command_queue,
        ptr,
        size,
        advice,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

#if defined(CL_VERSION_1_2)

cl_int clEnqueueMigrateMemINTEL(
    cl_command_queue command_queue,
    const void* ptr,
    size_t size,
    cl_mem_migration_flags flags,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueMigrateMemINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueMigrateMemINTEL(
        command_queue,
        ptr,
        size,
        flags,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

#endif

/* cl_nv_device_attribute_query */

/* cl_qcom_android_native_buffer_host_ptr */

/* cl_qcom_ext_host_ptr */

cl_int clGetDeviceImageInfoQCOM(
    cl_device_id device,
    size_t image_width,
    size_t image_height,
    const cl_image_format* image_format,
    cl_image_pitch_info_qcom param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(device);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetDeviceImageInfoQCOM == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetDeviceImageInfoQCOM(
        device,
        image_width,
        image_height,
        image_format,
        param_name,
        param_value_size,
        param_value,
        param_value_size_ret);
}

/* cl_qcom_ext_host_ptr_iocoherent */

/* cl_qcom_ion_host_ptr */

#ifdef __cplusplus
}
#endif
