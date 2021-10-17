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

#if defined _WIN32 || defined __CYGWIN__
    #ifdef __GNUC__
        #define CL_API_ENTRY __attribute__((dllexport))
    #else
        #define CL_API_ENTRY __declspec(dllexport)
    #endif
#else
    #if __GNUC__ >= 4
        #define CL_API_ENTRY __attribute__((visibility("default")))
    #else
        #define CL_API_ENTRY
    #endif
#endif

#define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_USE_DEPRECATED_OPENCL_2_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_2_APIS
#define CL_USE_DEPRECATED_OPENCL_3_0_APIS

#include <CL/cl.h>
#include <CL/cl_ext.h>
#if defined(CLEXT_INCLUDE_GL)
#include <CL/cl_gl.h>
#endif
#if defined(CLEXT_INCLUDE_EGL)
#include <CL/cl_egl.h>
#endif
#if defined(CLEXT_INCLUDE_DX9)
#include <CL/cl_dx9_media_sharing.h>
#endif
// Note: If both D3D10 and D3D11 are supported, the D3D11 header must be
// included first.
#if defined(CLEXT_INCLUDE_D3D11)
#include <CL/cl_d3d11.h>
#endif
#if defined(CLEXT_INCLUDE_D3D10)
#include <CL/cl_d3d10.h>
#endif
#if defined(CLEXT_INCLUDE_VA_API)
#include <CL/cl_va_api_media_sharing_intel.h>
#endif

#include <vector>

static inline cl_platform_id _get_platform(cl_platform_id platform)
{
    return platform;
}

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

#if defined(cl_khr_semaphore)

static inline cl_platform_id _get_platform(cl_semaphore_khr semaphore)
{
    if (semaphore == NULL) return NULL;

    cl_context context = NULL;
    clGetSemaphoreInfoKHR(
        semaphore,
        CL_SEMAPHORE_CONTEXT_KHR,
        sizeof(context),
        &context,
        NULL);
    return _get_platform(context);
}

#endif // defined(cl_khr_semaphore)

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

#if defined(CLEXT_INCLUDE_D3D10)

/* cl_khr_d3d10_sharing */

typedef cl_int (CL_API_CALL* clGetDeviceIDsFromD3D10KHR_clextfn)(
    cl_platform_id platform,
    cl_d3d10_device_source_khr d3d_device_source,
    void* d3d_object,
    cl_d3d10_device_set_khr d3d_device_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices);

typedef cl_mem (CL_API_CALL* clCreateFromD3D10BufferKHR_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    ID3D10Buffer* resource,
    cl_int* errcode_ret);

typedef cl_mem (CL_API_CALL* clCreateFromD3D10Texture2DKHR_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    ID3D10Texture2D* resource,
    UINT subresource,
    cl_int* errcode_ret);

typedef cl_mem (CL_API_CALL* clCreateFromD3D10Texture3DKHR_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    ID3D10Texture3D* resource,
    UINT subresource,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clEnqueueAcquireD3D10ObjectsKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueReleaseD3D10ObjectsKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

#endif // defined(CLEXT_INCLUDE_D3D10)

#if defined(CLEXT_INCLUDE_D3D11)

/* cl_khr_d3d11_sharing */

typedef cl_int (CL_API_CALL* clGetDeviceIDsFromD3D11KHR_clextfn)(
    cl_platform_id platform,
    cl_d3d11_device_source_khr d3d_device_source,
    void* d3d_object,
    cl_d3d11_device_set_khr d3d_device_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices);

typedef cl_mem (CL_API_CALL* clCreateFromD3D11BufferKHR_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    ID3D11Buffer* resource,
    cl_int* errcode_ret);

typedef cl_mem (CL_API_CALL* clCreateFromD3D11Texture2DKHR_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    ID3D11Texture2D* resource,
    UINT subresource,
    cl_int* errcode_ret);

typedef cl_mem (CL_API_CALL* clCreateFromD3D11Texture3DKHR_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    ID3D11Texture3D* resource,
    UINT subresource,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clEnqueueAcquireD3D11ObjectsKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueReleaseD3D11ObjectsKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

#endif // defined(CLEXT_INCLUDE_D3D11)

/* cl_khr_depth_images */

/* cl_khr_device_uuid */

#if defined(CLEXT_INCLUDE_DX9)

/* cl_khr_dx9_media_sharing */

typedef cl_int (CL_API_CALL* clGetDeviceIDsFromDX9MediaAdapterKHR_clextfn)(
    cl_platform_id platform,
    cl_uint num_media_adapters,
    cl_dx9_media_adapter_type_khr* media_adapter_type,
    void* media_adapters,
    cl_dx9_media_adapter_set_khr media_adapter_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices);

typedef cl_mem (CL_API_CALL* clCreateFromDX9MediaSurfaceKHR_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    cl_dx9_media_adapter_type_khr adapter_type,
    void* surface_info,
    cl_uint plane,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clEnqueueAcquireDX9MediaSurfacesKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueReleaseDX9MediaSurfacesKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

#endif // defined(CLEXT_INCLUDE_DX9)

#if defined(CLEXT_INCLUDE_EGL)

/* cl_khr_egl_event */

typedef cl_event (CL_API_CALL* clCreateEventFromEGLSyncKHR_clextfn)(
    cl_context context,
    CLeglSyncKHR sync,
    CLeglDisplayKHR display,
    cl_int* errcode_ret);

#endif // defined(CLEXT_INCLUDE_EGL)

#if defined(CLEXT_INCLUDE_EGL)

/* cl_khr_egl_image */

typedef cl_mem (CL_API_CALL* clCreateFromEGLImageKHR_clextfn)(
    cl_context context,
    CLeglDisplayKHR egldisplay,
    CLeglImageKHR eglimage,
    cl_mem_flags flags,
    const cl_egl_image_properties_khr* properties,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clEnqueueAcquireEGLObjectsKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueReleaseEGLObjectsKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

#endif // defined(CLEXT_INCLUDE_EGL)

/* cl_khr_extended_versioning */

/* cl_khr_external_memory */

typedef cl_int (CL_API_CALL* clEnqueueAcquireExternalMemObjectsKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_mem_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueReleaseExternalMemObjectsKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_mem_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

/* cl_khr_external_memory_dma_buf */

/* cl_khr_external_memory_dx */

/* cl_khr_external_memory_opaque_fd */

/* cl_khr_external_memory_win32 */

/* cl_khr_external_semaphore */

typedef cl_int (CL_API_CALL* clGetSemaphoreHandleForTypeKHR_clextfn)(
    cl_semaphore_khr sema_object,
    cl_device_id device,
    cl_external_semaphore_handle_type_khr handle_type,
    size_t handle_size,
    void* handle_ptr,
    size_t* handle_size_ret);

/* cl_khr_external_semaphore_dx_fence */

/* cl_khr_external_semaphore_opaque_fd */

/* cl_khr_external_semaphore_sync_fd */

/* cl_khr_external_semaphore_win32 */

/* cl_khr_fp16 */

/* cl_khr_fp64 */

#if defined(CLEXT_INCLUDE_GL)

/* cl_khr_gl_depth_images */

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_GL)

/* cl_khr_gl_event */

typedef cl_event (CL_API_CALL* clCreateEventFromGLsyncKHR_clextfn)(
    cl_context context,
    cl_GLsync sync,
    cl_int* errcode_ret);

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_GL)

/* cl_khr_gl_msaa_sharing */

#endif // defined(CLEXT_INCLUDE_GL)

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

/* cl_khr_semaphore */

typedef cl_semaphore_khr (CL_API_CALL* clCreateSemaphoreWithPropertiesKHR_clextfn)(
    cl_context context,
    const cl_semaphore_properties_khr* sema_props,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clEnqueueWaitSemaphoresKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_sema_objects,
    const cl_semaphore_khr* sema_objects,
    const cl_semaphore_payload_khr* sema_payload_list,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueSignalSemaphoresKHR_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_sema_objects,
    const cl_semaphore_khr* sema_objects,
    const cl_semaphore_payload_khr* sema_payload_list,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clGetSemaphoreInfoKHR_clextfn)(
    cl_semaphore_khr sema_object,
    cl_semaphore_info_khr param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret);

typedef cl_int (CL_API_CALL* clReleaseSemaphoreKHR_clextfn)(
    cl_semaphore_khr sema_object);

typedef cl_int (CL_API_CALL* clRetainSemaphoreKHR_clextfn)(
    cl_semaphore_khr sema_object);

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

/* cl_intel_device_attribute_query */

/* cl_intel_device_partition_by_names */

/* cl_intel_device_side_avc_motion_estimation */

/* cl_intel_driver_diagnostics */

#if defined(CLEXT_INCLUDE_DX9)

/* cl_intel_dx9_media_sharing */

typedef cl_int (CL_API_CALL* clGetDeviceIDsFromDX9INTEL_clextfn)(
    cl_platform_id platform,
    cl_dx9_device_source_intel dx9_device_source,
    void* dx9_object,
    cl_dx9_device_set_intel dx9_device_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices);

typedef cl_mem (CL_API_CALL* clCreateFromDX9MediaSurfaceINTEL_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    IDirect3DSurface9* resource,
    HANDLE sharedHandle,
    UINT plane,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clEnqueueAcquireDX9ObjectsINTEL_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueReleaseDX9ObjectsINTEL_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

#endif // defined(CLEXT_INCLUDE_DX9)

/* cl_intel_egl_image_yuv */

/* cl_intel_mem_channel_property */

/* cl_intel_mem_force_host_memory */

/* cl_intel_motion_estimation */

/* cl_intel_packed_yuv */

/* cl_intel_planar_yuv */

/* cl_intel_required_subgroup_size */

/* cl_intel_sharing_format_query */

#if defined(CLEXT_INCLUDE_D3D10)

/* cl_intel_sharing_format_query_d3d10 */

typedef cl_int (CL_API_CALL* clGetSupportedD3D10TextureFormatsINTEL_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint num_entries,
    DXGI_FORMAT* d3d10_formats,
    cl_uint* num_texture_formats);

#endif // defined(CLEXT_INCLUDE_D3D10)

#if defined(CLEXT_INCLUDE_D3D11)

/* cl_intel_sharing_format_query_d3d11 */

typedef cl_int (CL_API_CALL* clGetSupportedD3D11TextureFormatsINTEL_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint plane,
    cl_uint num_entries,
    DXGI_FORMAT* d3d11_formats,
    cl_uint* num_texture_formats);

#endif // defined(CLEXT_INCLUDE_D3D11)

#if defined(CLEXT_INCLUDE_DX9)

/* cl_intel_sharing_format_query_dx9 */

typedef cl_int (CL_API_CALL* clGetSupportedDX9MediaSurfaceFormatsINTEL_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint plane,
    cl_uint num_entries,
    D3DFORMAT* dx9_formats,
    cl_uint* num_surface_formats);

#endif // defined(CLEXT_INCLUDE_DX9)

#if defined(CLEXT_INCLUDE_GL)

/* cl_intel_sharing_format_query_gl */

typedef cl_int (CL_API_CALL* clGetSupportedGLTextureFormatsINTEL_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint num_entries,
    cl_GLenum* gl_formats,
    cl_uint* num_texture_formats);

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_VA_API)

/* cl_intel_sharing_format_query_va_api */

typedef cl_int (CL_API_CALL* clGetSupportedVA_APIMediaSurfaceFormatsINTEL_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint plane,
    cl_uint num_entries,
    VAImageFormat* va_api_formats,
    cl_uint* num_surface_formats);

#endif // defined(CLEXT_INCLUDE_VA_API)

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

#endif // defined(CL_VERSION_1_2)

#if defined(CLEXT_INCLUDE_VA_API)

/* cl_intel_va_api_media_sharing */

typedef cl_int (CL_API_CALL* clGetDeviceIDsFromVA_APIMediaAdapterINTEL_clextfn)(
    cl_platform_id platform,
    cl_va_api_device_source_intel media_adapter_type,
    void* media_adapter,
    cl_va_api_device_set_intel media_adapter_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices);

typedef cl_mem (CL_API_CALL* clCreateFromVA_APIMediaSurfaceINTEL_clextfn)(
    cl_context context,
    cl_mem_flags flags,
    VASurfaceID* surface,
    cl_uint plane,
    cl_int* errcode_ret);

typedef cl_int (CL_API_CALL* clEnqueueAcquireVA_APIMediaSurfacesINTEL_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

typedef cl_int (CL_API_CALL* clEnqueueReleaseVA_APIMediaSurfacesINTEL_clextfn)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event);

#endif // defined(CLEXT_INCLUDE_VA_API)

/* cl_nv_device_attribute_query */

/* cl_pocl_content_size */

typedef cl_int (CL_API_CALL* clSetContentSizeBufferPoCL_clextfn)(
    cl_mem buffer,
    cl_mem content_size_buffer);

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
    cl_platform_id platform;

    /* cl_khr_create_command_queue */
    clCreateCommandQueueWithPropertiesKHR_clextfn clCreateCommandQueueWithPropertiesKHR;

#if defined(CLEXT_INCLUDE_D3D10)

    /* cl_khr_d3d10_sharing */
    clGetDeviceIDsFromD3D10KHR_clextfn clGetDeviceIDsFromD3D10KHR;
    clCreateFromD3D10BufferKHR_clextfn clCreateFromD3D10BufferKHR;
    clCreateFromD3D10Texture2DKHR_clextfn clCreateFromD3D10Texture2DKHR;
    clCreateFromD3D10Texture3DKHR_clextfn clCreateFromD3D10Texture3DKHR;
    clEnqueueAcquireD3D10ObjectsKHR_clextfn clEnqueueAcquireD3D10ObjectsKHR;
    clEnqueueReleaseD3D10ObjectsKHR_clextfn clEnqueueReleaseD3D10ObjectsKHR;

#endif // defined(CLEXT_INCLUDE_D3D10)

#if defined(CLEXT_INCLUDE_D3D11)

    /* cl_khr_d3d11_sharing */
    clGetDeviceIDsFromD3D11KHR_clextfn clGetDeviceIDsFromD3D11KHR;
    clCreateFromD3D11BufferKHR_clextfn clCreateFromD3D11BufferKHR;
    clCreateFromD3D11Texture2DKHR_clextfn clCreateFromD3D11Texture2DKHR;
    clCreateFromD3D11Texture3DKHR_clextfn clCreateFromD3D11Texture3DKHR;
    clEnqueueAcquireD3D11ObjectsKHR_clextfn clEnqueueAcquireD3D11ObjectsKHR;
    clEnqueueReleaseD3D11ObjectsKHR_clextfn clEnqueueReleaseD3D11ObjectsKHR;

#endif // defined(CLEXT_INCLUDE_D3D11)

    /* cl_khr_depth_images */

    /* cl_khr_device_uuid */

#if defined(CLEXT_INCLUDE_DX9)

    /* cl_khr_dx9_media_sharing */
    clGetDeviceIDsFromDX9MediaAdapterKHR_clextfn clGetDeviceIDsFromDX9MediaAdapterKHR;
    clCreateFromDX9MediaSurfaceKHR_clextfn clCreateFromDX9MediaSurfaceKHR;
    clEnqueueAcquireDX9MediaSurfacesKHR_clextfn clEnqueueAcquireDX9MediaSurfacesKHR;
    clEnqueueReleaseDX9MediaSurfacesKHR_clextfn clEnqueueReleaseDX9MediaSurfacesKHR;

#endif // defined(CLEXT_INCLUDE_DX9)

#if defined(CLEXT_INCLUDE_EGL)

    /* cl_khr_egl_event */
    clCreateEventFromEGLSyncKHR_clextfn clCreateEventFromEGLSyncKHR;

#endif // defined(CLEXT_INCLUDE_EGL)

#if defined(CLEXT_INCLUDE_EGL)

    /* cl_khr_egl_image */
    clCreateFromEGLImageKHR_clextfn clCreateFromEGLImageKHR;
    clEnqueueAcquireEGLObjectsKHR_clextfn clEnqueueAcquireEGLObjectsKHR;
    clEnqueueReleaseEGLObjectsKHR_clextfn clEnqueueReleaseEGLObjectsKHR;

#endif // defined(CLEXT_INCLUDE_EGL)

    /* cl_khr_extended_versioning */

    /* cl_khr_external_memory */
    clEnqueueAcquireExternalMemObjectsKHR_clextfn clEnqueueAcquireExternalMemObjectsKHR;
    clEnqueueReleaseExternalMemObjectsKHR_clextfn clEnqueueReleaseExternalMemObjectsKHR;

    /* cl_khr_external_memory_dma_buf */

    /* cl_khr_external_memory_dx */

    /* cl_khr_external_memory_opaque_fd */

    /* cl_khr_external_memory_win32 */

    /* cl_khr_external_semaphore */
    clGetSemaphoreHandleForTypeKHR_clextfn clGetSemaphoreHandleForTypeKHR;

    /* cl_khr_external_semaphore_dx_fence */

    /* cl_khr_external_semaphore_opaque_fd */

    /* cl_khr_external_semaphore_sync_fd */

    /* cl_khr_external_semaphore_win32 */

    /* cl_khr_fp16 */

    /* cl_khr_fp64 */

#if defined(CLEXT_INCLUDE_GL)

    /* cl_khr_gl_depth_images */

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_GL)

    /* cl_khr_gl_event */
    clCreateEventFromGLsyncKHR_clextfn clCreateEventFromGLsyncKHR;

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_GL)

    /* cl_khr_gl_msaa_sharing */

#endif // defined(CLEXT_INCLUDE_GL)

    /* cl_khr_il_program */
    clCreateProgramWithILKHR_clextfn clCreateProgramWithILKHR;

    /* cl_khr_image2D_from_buffer */

    /* cl_khr_initialize_memory */

    /* cl_khr_integer_dot_product */

    /* cl_khr_mipmap_image */

    /* cl_khr_pci_bus_info */

    /* cl_khr_priority_hints */

    /* cl_khr_semaphore */
    clCreateSemaphoreWithPropertiesKHR_clextfn clCreateSemaphoreWithPropertiesKHR;
    clEnqueueWaitSemaphoresKHR_clextfn clEnqueueWaitSemaphoresKHR;
    clEnqueueSignalSemaphoresKHR_clextfn clEnqueueSignalSemaphoresKHR;
    clGetSemaphoreInfoKHR_clextfn clGetSemaphoreInfoKHR;
    clReleaseSemaphoreKHR_clextfn clReleaseSemaphoreKHR;
    clRetainSemaphoreKHR_clextfn clRetainSemaphoreKHR;

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

    /* cl_intel_device_attribute_query */

    /* cl_intel_device_partition_by_names */

    /* cl_intel_device_side_avc_motion_estimation */

    /* cl_intel_driver_diagnostics */

#if defined(CLEXT_INCLUDE_DX9)

    /* cl_intel_dx9_media_sharing */
    clGetDeviceIDsFromDX9INTEL_clextfn clGetDeviceIDsFromDX9INTEL;
    clCreateFromDX9MediaSurfaceINTEL_clextfn clCreateFromDX9MediaSurfaceINTEL;
    clEnqueueAcquireDX9ObjectsINTEL_clextfn clEnqueueAcquireDX9ObjectsINTEL;
    clEnqueueReleaseDX9ObjectsINTEL_clextfn clEnqueueReleaseDX9ObjectsINTEL;

#endif // defined(CLEXT_INCLUDE_DX9)

    /* cl_intel_egl_image_yuv */

    /* cl_intel_mem_channel_property */

    /* cl_intel_mem_force_host_memory */

    /* cl_intel_motion_estimation */

    /* cl_intel_packed_yuv */

    /* cl_intel_planar_yuv */

    /* cl_intel_required_subgroup_size */

    /* cl_intel_sharing_format_query */

#if defined(CLEXT_INCLUDE_D3D10)

    /* cl_intel_sharing_format_query_d3d10 */
    clGetSupportedD3D10TextureFormatsINTEL_clextfn clGetSupportedD3D10TextureFormatsINTEL;

#endif // defined(CLEXT_INCLUDE_D3D10)

#if defined(CLEXT_INCLUDE_D3D11)

    /* cl_intel_sharing_format_query_d3d11 */
    clGetSupportedD3D11TextureFormatsINTEL_clextfn clGetSupportedD3D11TextureFormatsINTEL;

#endif // defined(CLEXT_INCLUDE_D3D11)

#if defined(CLEXT_INCLUDE_DX9)

    /* cl_intel_sharing_format_query_dx9 */
    clGetSupportedDX9MediaSurfaceFormatsINTEL_clextfn clGetSupportedDX9MediaSurfaceFormatsINTEL;

#endif // defined(CLEXT_INCLUDE_DX9)

#if defined(CLEXT_INCLUDE_GL)

    /* cl_intel_sharing_format_query_gl */
    clGetSupportedGLTextureFormatsINTEL_clextfn clGetSupportedGLTextureFormatsINTEL;

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_VA_API)

    /* cl_intel_sharing_format_query_va_api */
    clGetSupportedVA_APIMediaSurfaceFormatsINTEL_clextfn clGetSupportedVA_APIMediaSurfaceFormatsINTEL;

#endif // defined(CLEXT_INCLUDE_VA_API)

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
#endif // defined(CL_VERSION_1_2)

#if defined(CLEXT_INCLUDE_VA_API)

    /* cl_intel_va_api_media_sharing */
    clGetDeviceIDsFromVA_APIMediaAdapterINTEL_clextfn clGetDeviceIDsFromVA_APIMediaAdapterINTEL;
    clCreateFromVA_APIMediaSurfaceINTEL_clextfn clCreateFromVA_APIMediaSurfaceINTEL;
    clEnqueueAcquireVA_APIMediaSurfacesINTEL_clextfn clEnqueueAcquireVA_APIMediaSurfacesINTEL;
    clEnqueueReleaseVA_APIMediaSurfacesINTEL_clextfn clEnqueueReleaseVA_APIMediaSurfacesINTEL;

#endif // defined(CLEXT_INCLUDE_VA_API)

    /* cl_nv_device_attribute_query */

    /* cl_pocl_content_size */
    clSetContentSizeBufferPoCL_clextfn clSetContentSizeBufferPoCL;

    /* cl_qcom_android_native_buffer_host_ptr */

    /* cl_qcom_ext_host_ptr */
    clGetDeviceImageInfoQCOM_clextfn clGetDeviceImageInfoQCOM;

    /* cl_qcom_ext_host_ptr_iocoherent */

    /* cl_qcom_ion_host_ptr */

};

/***************************************************************
* Dispatch Table Initialization
***************************************************************/

static void _init(cl_platform_id platform, openclext_dispatch_table* dispatch_ptr)
{
    dispatch_ptr->platform = platform;

#define CLEXT_GET_EXTENSION(_funcname)                                         \
    dispatch_ptr->_funcname =                                                  \
        (_funcname##_clextfn)clGetExtensionFunctionAddressForPlatform(         \
            platform, #_funcname);

    /* cl_khr_create_command_queue */
    CLEXT_GET_EXTENSION(clCreateCommandQueueWithPropertiesKHR);

#if defined(CLEXT_INCLUDE_D3D10)

    /* cl_khr_d3d10_sharing */
    CLEXT_GET_EXTENSION(clGetDeviceIDsFromD3D10KHR);
    CLEXT_GET_EXTENSION(clCreateFromD3D10BufferKHR);
    CLEXT_GET_EXTENSION(clCreateFromD3D10Texture2DKHR);
    CLEXT_GET_EXTENSION(clCreateFromD3D10Texture3DKHR);
    CLEXT_GET_EXTENSION(clEnqueueAcquireD3D10ObjectsKHR);
    CLEXT_GET_EXTENSION(clEnqueueReleaseD3D10ObjectsKHR);

#endif // defined(CLEXT_INCLUDE_D3D10)

#if defined(CLEXT_INCLUDE_D3D11)

    /* cl_khr_d3d11_sharing */
    CLEXT_GET_EXTENSION(clGetDeviceIDsFromD3D11KHR);
    CLEXT_GET_EXTENSION(clCreateFromD3D11BufferKHR);
    CLEXT_GET_EXTENSION(clCreateFromD3D11Texture2DKHR);
    CLEXT_GET_EXTENSION(clCreateFromD3D11Texture3DKHR);
    CLEXT_GET_EXTENSION(clEnqueueAcquireD3D11ObjectsKHR);
    CLEXT_GET_EXTENSION(clEnqueueReleaseD3D11ObjectsKHR);

#endif // defined(CLEXT_INCLUDE_D3D11)

    /* cl_khr_depth_images */

    /* cl_khr_device_uuid */

#if defined(CLEXT_INCLUDE_DX9)

    /* cl_khr_dx9_media_sharing */
    CLEXT_GET_EXTENSION(clGetDeviceIDsFromDX9MediaAdapterKHR);
    CLEXT_GET_EXTENSION(clCreateFromDX9MediaSurfaceKHR);
    CLEXT_GET_EXTENSION(clEnqueueAcquireDX9MediaSurfacesKHR);
    CLEXT_GET_EXTENSION(clEnqueueReleaseDX9MediaSurfacesKHR);

#endif // defined(CLEXT_INCLUDE_DX9)

#if defined(CLEXT_INCLUDE_EGL)

    /* cl_khr_egl_event */
    CLEXT_GET_EXTENSION(clCreateEventFromEGLSyncKHR);

#endif // defined(CLEXT_INCLUDE_EGL)

#if defined(CLEXT_INCLUDE_EGL)

    /* cl_khr_egl_image */
    CLEXT_GET_EXTENSION(clCreateFromEGLImageKHR);
    CLEXT_GET_EXTENSION(clEnqueueAcquireEGLObjectsKHR);
    CLEXT_GET_EXTENSION(clEnqueueReleaseEGLObjectsKHR);

#endif // defined(CLEXT_INCLUDE_EGL)

    /* cl_khr_extended_versioning */

    /* cl_khr_external_memory */
    CLEXT_GET_EXTENSION(clEnqueueAcquireExternalMemObjectsKHR);
    CLEXT_GET_EXTENSION(clEnqueueReleaseExternalMemObjectsKHR);

    /* cl_khr_external_memory_dma_buf */

    /* cl_khr_external_memory_dx */

    /* cl_khr_external_memory_opaque_fd */

    /* cl_khr_external_memory_win32 */

    /* cl_khr_external_semaphore */
    CLEXT_GET_EXTENSION(clGetSemaphoreHandleForTypeKHR);

    /* cl_khr_external_semaphore_dx_fence */

    /* cl_khr_external_semaphore_opaque_fd */

    /* cl_khr_external_semaphore_sync_fd */

    /* cl_khr_external_semaphore_win32 */

    /* cl_khr_fp16 */

    /* cl_khr_fp64 */

#if defined(CLEXT_INCLUDE_GL)

    /* cl_khr_gl_depth_images */

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_GL)

    /* cl_khr_gl_event */
    CLEXT_GET_EXTENSION(clCreateEventFromGLsyncKHR);

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_GL)

    /* cl_khr_gl_msaa_sharing */

#endif // defined(CLEXT_INCLUDE_GL)

    /* cl_khr_il_program */
    CLEXT_GET_EXTENSION(clCreateProgramWithILKHR);

    /* cl_khr_image2D_from_buffer */

    /* cl_khr_initialize_memory */

    /* cl_khr_integer_dot_product */

    /* cl_khr_mipmap_image */

    /* cl_khr_pci_bus_info */

    /* cl_khr_priority_hints */

    /* cl_khr_semaphore */
    CLEXT_GET_EXTENSION(clCreateSemaphoreWithPropertiesKHR);
    CLEXT_GET_EXTENSION(clEnqueueWaitSemaphoresKHR);
    CLEXT_GET_EXTENSION(clEnqueueSignalSemaphoresKHR);
    CLEXT_GET_EXTENSION(clGetSemaphoreInfoKHR);
    CLEXT_GET_EXTENSION(clReleaseSemaphoreKHR);
    CLEXT_GET_EXTENSION(clRetainSemaphoreKHR);

    /* cl_khr_spir */

    /* cl_khr_subgroup_named_barrier */

    /* cl_khr_subgroups */
    CLEXT_GET_EXTENSION(clGetKernelSubGroupInfoKHR);

    /* cl_khr_suggested_local_work_size */
    CLEXT_GET_EXTENSION(clGetKernelSuggestedLocalWorkSizeKHR);

    /* cl_khr_terminate_context */
    CLEXT_GET_EXTENSION(clTerminateContextKHR);

    /* cl_khr_throttle_hints */

    /* cl_ext_cxx_for_opencl */

    /* cl_ext_device_fission */
    CLEXT_GET_EXTENSION(clReleaseDeviceEXT);
    CLEXT_GET_EXTENSION(clRetainDeviceEXT);
    CLEXT_GET_EXTENSION(clCreateSubDevicesEXT);

    /* cl_ext_migrate_memobject */
    CLEXT_GET_EXTENSION(clEnqueueMigrateMemObjectEXT);

    /* cl_APPLE_SetMemObjectDestructor */
    CLEXT_GET_EXTENSION(clSetMemObjectDestructorAPPLE);

    /* cl_amd_device_attribute_query */

    /* cl_arm_controlled_kernel_termination */

    /* cl_arm_get_core_id */

    /* cl_arm_import_memory */
    CLEXT_GET_EXTENSION(clImportMemoryARM);

    /* cl_arm_printf */

    /* cl_arm_scheduling_controls */

    /* cl_arm_shared_virtual_memory */
    CLEXT_GET_EXTENSION(clSVMAllocARM);
    CLEXT_GET_EXTENSION(clSVMFreeARM);
    CLEXT_GET_EXTENSION(clEnqueueSVMFreeARM);
    CLEXT_GET_EXTENSION(clEnqueueSVMMemcpyARM);
    CLEXT_GET_EXTENSION(clEnqueueSVMMemFillARM);
    CLEXT_GET_EXTENSION(clEnqueueSVMMapARM);
    CLEXT_GET_EXTENSION(clEnqueueSVMUnmapARM);
    CLEXT_GET_EXTENSION(clSetKernelArgSVMPointerARM);
    CLEXT_GET_EXTENSION(clSetKernelExecInfoARM);

    /* cl_img_cached_allocations */

    /* cl_img_generate_mipmap */
    CLEXT_GET_EXTENSION(clEnqueueGenerateMipmapIMG);

    /* cl_img_mem_properties */

    /* cl_img_use_gralloc_ptr */
    CLEXT_GET_EXTENSION(clEnqueueAcquireGrallocObjectsIMG);
    CLEXT_GET_EXTENSION(clEnqueueReleaseGrallocObjectsIMG);

    /* cl_img_yuv_image */

    /* cl_intel_accelerator */
    CLEXT_GET_EXTENSION(clCreateAcceleratorINTEL);
    CLEXT_GET_EXTENSION(clGetAcceleratorInfoINTEL);
    CLEXT_GET_EXTENSION(clRetainAcceleratorINTEL);
    CLEXT_GET_EXTENSION(clReleaseAcceleratorINTEL);

    /* cl_intel_advanced_motion_estimation */

    /* cl_intel_command_queue_families */

    /* cl_intel_create_buffer_with_properties */
    CLEXT_GET_EXTENSION(clCreateBufferWithPropertiesINTEL);

    /* cl_intel_device_attribute_query */

    /* cl_intel_device_partition_by_names */

    /* cl_intel_device_side_avc_motion_estimation */

    /* cl_intel_driver_diagnostics */

#if defined(CLEXT_INCLUDE_DX9)

    /* cl_intel_dx9_media_sharing */
    CLEXT_GET_EXTENSION(clGetDeviceIDsFromDX9INTEL);
    CLEXT_GET_EXTENSION(clCreateFromDX9MediaSurfaceINTEL);
    CLEXT_GET_EXTENSION(clEnqueueAcquireDX9ObjectsINTEL);
    CLEXT_GET_EXTENSION(clEnqueueReleaseDX9ObjectsINTEL);

#endif // defined(CLEXT_INCLUDE_DX9)

    /* cl_intel_egl_image_yuv */

    /* cl_intel_mem_channel_property */

    /* cl_intel_mem_force_host_memory */

    /* cl_intel_motion_estimation */

    /* cl_intel_packed_yuv */

    /* cl_intel_planar_yuv */

    /* cl_intel_required_subgroup_size */

    /* cl_intel_sharing_format_query */

#if defined(CLEXT_INCLUDE_D3D10)

    /* cl_intel_sharing_format_query_d3d10 */
    CLEXT_GET_EXTENSION(clGetSupportedD3D10TextureFormatsINTEL);

#endif // defined(CLEXT_INCLUDE_D3D10)

#if defined(CLEXT_INCLUDE_D3D11)

    /* cl_intel_sharing_format_query_d3d11 */
    CLEXT_GET_EXTENSION(clGetSupportedD3D11TextureFormatsINTEL);

#endif // defined(CLEXT_INCLUDE_D3D11)

#if defined(CLEXT_INCLUDE_DX9)

    /* cl_intel_sharing_format_query_dx9 */
    CLEXT_GET_EXTENSION(clGetSupportedDX9MediaSurfaceFormatsINTEL);

#endif // defined(CLEXT_INCLUDE_DX9)

#if defined(CLEXT_INCLUDE_GL)

    /* cl_intel_sharing_format_query_gl */
    CLEXT_GET_EXTENSION(clGetSupportedGLTextureFormatsINTEL);

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_VA_API)

    /* cl_intel_sharing_format_query_va_api */
    CLEXT_GET_EXTENSION(clGetSupportedVA_APIMediaSurfaceFormatsINTEL);

#endif // defined(CLEXT_INCLUDE_VA_API)

    /* cl_intel_simultaneous_sharing */

    /* cl_intel_thread_local_exec */

    /* cl_intel_unified_shared_memory */
    CLEXT_GET_EXTENSION(clHostMemAllocINTEL);
    CLEXT_GET_EXTENSION(clDeviceMemAllocINTEL);
    CLEXT_GET_EXTENSION(clSharedMemAllocINTEL);
    CLEXT_GET_EXTENSION(clMemFreeINTEL);
    CLEXT_GET_EXTENSION(clMemBlockingFreeINTEL);
    CLEXT_GET_EXTENSION(clGetMemAllocInfoINTEL);
    CLEXT_GET_EXTENSION(clSetKernelArgMemPointerINTEL);
    CLEXT_GET_EXTENSION(clEnqueueMemsetINTEL);
    CLEXT_GET_EXTENSION(clEnqueueMemFillINTEL);
    CLEXT_GET_EXTENSION(clEnqueueMemcpyINTEL);
    CLEXT_GET_EXTENSION(clEnqueueMemAdviseINTEL);
#if defined(CL_VERSION_1_2)
    CLEXT_GET_EXTENSION(clEnqueueMigrateMemINTEL);
#endif // defined(CL_VERSION_1_2)

#if defined(CLEXT_INCLUDE_VA_API)

    /* cl_intel_va_api_media_sharing */
    CLEXT_GET_EXTENSION(clGetDeviceIDsFromVA_APIMediaAdapterINTEL);
    CLEXT_GET_EXTENSION(clCreateFromVA_APIMediaSurfaceINTEL);
    CLEXT_GET_EXTENSION(clEnqueueAcquireVA_APIMediaSurfacesINTEL);
    CLEXT_GET_EXTENSION(clEnqueueReleaseVA_APIMediaSurfacesINTEL);

#endif // defined(CLEXT_INCLUDE_VA_API)

    /* cl_nv_device_attribute_query */

    /* cl_pocl_content_size */
    CLEXT_GET_EXTENSION(clSetContentSizeBufferPoCL);

    /* cl_qcom_android_native_buffer_host_ptr */

    /* cl_qcom_ext_host_ptr */
    CLEXT_GET_EXTENSION(clGetDeviceImageInfoQCOM);

    /* cl_qcom_ext_host_ptr_iocoherent */

    /* cl_qcom_ion_host_ptr */

#undef CLEXT_GET_EXTENSION
}

#if defined(CLEXT_SINGLE_PLATFORM_ONLY)
static openclext_dispatch_table _dispatch = {0};
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
#else // defined(CLEXT_SINGLE_PLATFORM_ONLY)
static size_t _num_platforms = 0;
static openclext_dispatch_table* _dispatch_array = NULL;

template<typename T>
static openclext_dispatch_table* _get_dispatch(T object)
{
    if (_num_platforms == 0 && _dispatch_array == NULL) {
        cl_uint numPlatforms = 0;
        clGetPlatformIDs(0, NULL, &numPlatforms);

        openclext_dispatch_table* dispatch =
            (openclext_dispatch_table*)malloc(
                numPlatforms * sizeof(openclext_dispatch_table));
        if (dispatch == NULL) {
            return NULL;
        }

        std::vector<cl_platform_id> platforms(numPlatforms);
        clGetPlatformIDs(numPlatforms, platforms.data(), NULL);

        for (size_t i = 0; i < numPlatforms; i++) {
            _init(platforms[i], dispatch + i);
        }

        _num_platforms = numPlatforms;
        _dispatch_array = dispatch;
    }

    cl_platform_id platform = _get_platform(object);
    for (size_t i = 0; i < _num_platforms; i++) {
        openclext_dispatch_table* dispatch_ptr =
            _dispatch_array + i;
        if (dispatch_ptr->platform == platform) {
            return dispatch_ptr;
        }
    }

    return NULL;
}
#endif // defined(CLEXT_SINGLE_PLATFORM_ONLY)

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* Extension Functions
***************************************************************/

/* cl_khr_create_command_queue */

cl_command_queue CL_API_CALL clCreateCommandQueueWithPropertiesKHR(
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

#if defined(CLEXT_INCLUDE_D3D10)

/* cl_khr_d3d10_sharing */

cl_int CL_API_CALL clGetDeviceIDsFromD3D10KHR(
    cl_platform_id platform,
    cl_d3d10_device_source_khr d3d_device_source,
    void* d3d_object,
    cl_d3d10_device_set_khr d3d_device_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(platform);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetDeviceIDsFromD3D10KHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetDeviceIDsFromD3D10KHR(
        platform,
        d3d_device_source,
        d3d_object,
        d3d_device_set,
        num_entries,
        devices,
        num_devices);
}

cl_mem CL_API_CALL clCreateFromD3D10BufferKHR(
    cl_context context,
    cl_mem_flags flags,
    ID3D10Buffer* resource,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromD3D10BufferKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromD3D10BufferKHR(
        context,
        flags,
        resource,
        errcode_ret);
}

cl_mem CL_API_CALL clCreateFromD3D10Texture2DKHR(
    cl_context context,
    cl_mem_flags flags,
    ID3D10Texture2D* resource,
    UINT subresource,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromD3D10Texture2DKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromD3D10Texture2DKHR(
        context,
        flags,
        resource,
        subresource,
        errcode_ret);
}

cl_mem CL_API_CALL clCreateFromD3D10Texture3DKHR(
    cl_context context,
    cl_mem_flags flags,
    ID3D10Texture3D* resource,
    UINT subresource,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromD3D10Texture3DKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromD3D10Texture3DKHR(
        context,
        flags,
        resource,
        subresource,
        errcode_ret);
}

cl_int CL_API_CALL clEnqueueAcquireD3D10ObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueAcquireD3D10ObjectsKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueAcquireD3D10ObjectsKHR(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clEnqueueReleaseD3D10ObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueReleaseD3D10ObjectsKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueReleaseD3D10ObjectsKHR(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

#endif // defined(CLEXT_INCLUDE_D3D10)

#if defined(CLEXT_INCLUDE_D3D11)

/* cl_khr_d3d11_sharing */

cl_int CL_API_CALL clGetDeviceIDsFromD3D11KHR(
    cl_platform_id platform,
    cl_d3d11_device_source_khr d3d_device_source,
    void* d3d_object,
    cl_d3d11_device_set_khr d3d_device_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(platform);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetDeviceIDsFromD3D11KHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetDeviceIDsFromD3D11KHR(
        platform,
        d3d_device_source,
        d3d_object,
        d3d_device_set,
        num_entries,
        devices,
        num_devices);
}

cl_mem CL_API_CALL clCreateFromD3D11BufferKHR(
    cl_context context,
    cl_mem_flags flags,
    ID3D11Buffer* resource,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromD3D11BufferKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromD3D11BufferKHR(
        context,
        flags,
        resource,
        errcode_ret);
}

cl_mem CL_API_CALL clCreateFromD3D11Texture2DKHR(
    cl_context context,
    cl_mem_flags flags,
    ID3D11Texture2D* resource,
    UINT subresource,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromD3D11Texture2DKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromD3D11Texture2DKHR(
        context,
        flags,
        resource,
        subresource,
        errcode_ret);
}

cl_mem CL_API_CALL clCreateFromD3D11Texture3DKHR(
    cl_context context,
    cl_mem_flags flags,
    ID3D11Texture3D* resource,
    UINT subresource,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromD3D11Texture3DKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromD3D11Texture3DKHR(
        context,
        flags,
        resource,
        subresource,
        errcode_ret);
}

cl_int CL_API_CALL clEnqueueAcquireD3D11ObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueAcquireD3D11ObjectsKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueAcquireD3D11ObjectsKHR(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clEnqueueReleaseD3D11ObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueReleaseD3D11ObjectsKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueReleaseD3D11ObjectsKHR(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

#endif // defined(CLEXT_INCLUDE_D3D11)

/* cl_khr_depth_images */

/* cl_khr_device_uuid */

#if defined(CLEXT_INCLUDE_DX9)

/* cl_khr_dx9_media_sharing */

cl_int CL_API_CALL clGetDeviceIDsFromDX9MediaAdapterKHR(
    cl_platform_id platform,
    cl_uint num_media_adapters,
    cl_dx9_media_adapter_type_khr* media_adapter_type,
    void* media_adapters,
    cl_dx9_media_adapter_set_khr media_adapter_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(platform);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetDeviceIDsFromDX9MediaAdapterKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetDeviceIDsFromDX9MediaAdapterKHR(
        platform,
        num_media_adapters,
        media_adapter_type,
        media_adapters,
        media_adapter_set,
        num_entries,
        devices,
        num_devices);
}

cl_mem CL_API_CALL clCreateFromDX9MediaSurfaceKHR(
    cl_context context,
    cl_mem_flags flags,
    cl_dx9_media_adapter_type_khr adapter_type,
    void* surface_info,
    cl_uint plane,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromDX9MediaSurfaceKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromDX9MediaSurfaceKHR(
        context,
        flags,
        adapter_type,
        surface_info,
        plane,
        errcode_ret);
}

cl_int CL_API_CALL clEnqueueAcquireDX9MediaSurfacesKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueAcquireDX9MediaSurfacesKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueAcquireDX9MediaSurfacesKHR(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clEnqueueReleaseDX9MediaSurfacesKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueReleaseDX9MediaSurfacesKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueReleaseDX9MediaSurfacesKHR(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

#endif // defined(CLEXT_INCLUDE_DX9)

#if defined(CLEXT_INCLUDE_EGL)

/* cl_khr_egl_event */

cl_event CL_API_CALL clCreateEventFromEGLSyncKHR(
    cl_context context,
    CLeglSyncKHR sync,
    CLeglDisplayKHR display,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateEventFromEGLSyncKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateEventFromEGLSyncKHR(
        context,
        sync,
        display,
        errcode_ret);
}

#endif // defined(CLEXT_INCLUDE_EGL)

#if defined(CLEXT_INCLUDE_EGL)

/* cl_khr_egl_image */

cl_mem CL_API_CALL clCreateFromEGLImageKHR(
    cl_context context,
    CLeglDisplayKHR egldisplay,
    CLeglImageKHR eglimage,
    cl_mem_flags flags,
    const cl_egl_image_properties_khr* properties,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromEGLImageKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromEGLImageKHR(
        context,
        egldisplay,
        eglimage,
        flags,
        properties,
        errcode_ret);
}

cl_int CL_API_CALL clEnqueueAcquireEGLObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueAcquireEGLObjectsKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueAcquireEGLObjectsKHR(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clEnqueueReleaseEGLObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueReleaseEGLObjectsKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueReleaseEGLObjectsKHR(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

#endif // defined(CLEXT_INCLUDE_EGL)

/* cl_khr_extended_versioning */

/* cl_khr_external_memory */

cl_int CL_API_CALL clEnqueueAcquireExternalMemObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_mem_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueAcquireExternalMemObjectsKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueAcquireExternalMemObjectsKHR(
        command_queue,
        num_mem_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clEnqueueReleaseExternalMemObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_mem_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueReleaseExternalMemObjectsKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueReleaseExternalMemObjectsKHR(
        command_queue,
        num_mem_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

/* cl_khr_external_memory_dma_buf */

/* cl_khr_external_memory_dx */

/* cl_khr_external_memory_opaque_fd */

/* cl_khr_external_memory_win32 */

/* cl_khr_external_semaphore */

cl_int CL_API_CALL clGetSemaphoreHandleForTypeKHR(
    cl_semaphore_khr sema_object,
    cl_device_id device,
    cl_external_semaphore_handle_type_khr handle_type,
    size_t handle_size,
    void* handle_ptr,
    size_t* handle_size_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(sema_object);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetSemaphoreHandleForTypeKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetSemaphoreHandleForTypeKHR(
        sema_object,
        device,
        handle_type,
        handle_size,
        handle_ptr,
        handle_size_ret);
}

/* cl_khr_external_semaphore_dx_fence */

/* cl_khr_external_semaphore_opaque_fd */

/* cl_khr_external_semaphore_sync_fd */

/* cl_khr_external_semaphore_win32 */

/* cl_khr_fp16 */

/* cl_khr_fp64 */

#if defined(CLEXT_INCLUDE_GL)

/* cl_khr_gl_depth_images */

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_GL)

/* cl_khr_gl_event */

cl_event CL_API_CALL clCreateEventFromGLsyncKHR(
    cl_context context,
    cl_GLsync sync,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateEventFromGLsyncKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateEventFromGLsyncKHR(
        context,
        sync,
        errcode_ret);
}

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_GL)

/* cl_khr_gl_msaa_sharing */

#endif // defined(CLEXT_INCLUDE_GL)

/* cl_khr_il_program */

cl_program CL_API_CALL clCreateProgramWithILKHR(
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

/* cl_khr_semaphore */

cl_semaphore_khr CL_API_CALL clCreateSemaphoreWithPropertiesKHR(
    cl_context context,
    const cl_semaphore_properties_khr* sema_props,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateSemaphoreWithPropertiesKHR == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateSemaphoreWithPropertiesKHR(
        context,
        sema_props,
        errcode_ret);
}

cl_int CL_API_CALL clEnqueueWaitSemaphoresKHR(
    cl_command_queue command_queue,
    cl_uint num_sema_objects,
    const cl_semaphore_khr* sema_objects,
    const cl_semaphore_payload_khr* sema_payload_list,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueWaitSemaphoresKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueWaitSemaphoresKHR(
        command_queue,
        num_sema_objects,
        sema_objects,
        sema_payload_list,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clEnqueueSignalSemaphoresKHR(
    cl_command_queue command_queue,
    cl_uint num_sema_objects,
    const cl_semaphore_khr* sema_objects,
    const cl_semaphore_payload_khr* sema_payload_list,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueSignalSemaphoresKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueSignalSemaphoresKHR(
        command_queue,
        num_sema_objects,
        sema_objects,
        sema_payload_list,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clGetSemaphoreInfoKHR(
    cl_semaphore_khr sema_object,
    cl_semaphore_info_khr param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(sema_object);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetSemaphoreInfoKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetSemaphoreInfoKHR(
        sema_object,
        param_name,
        param_value_size,
        param_value,
        param_value_size_ret);
}

cl_int CL_API_CALL clReleaseSemaphoreKHR(
    cl_semaphore_khr sema_object)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(sema_object);
    if (dispatch_ptr == NULL || dispatch_ptr->clReleaseSemaphoreKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clReleaseSemaphoreKHR(
        sema_object);
}

cl_int CL_API_CALL clRetainSemaphoreKHR(
    cl_semaphore_khr sema_object)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(sema_object);
    if (dispatch_ptr == NULL || dispatch_ptr->clRetainSemaphoreKHR == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clRetainSemaphoreKHR(
        sema_object);
}

/* cl_khr_spir */

/* cl_khr_subgroup_named_barrier */

/* cl_khr_subgroups */

cl_int CL_API_CALL clGetKernelSubGroupInfoKHR(
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

cl_int CL_API_CALL clGetKernelSuggestedLocalWorkSizeKHR(
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

cl_int CL_API_CALL clTerminateContextKHR(
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

cl_int CL_API_CALL clReleaseDeviceEXT(
    cl_device_id device)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(device);
    if (dispatch_ptr == NULL || dispatch_ptr->clReleaseDeviceEXT == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clReleaseDeviceEXT(
        device);
}

cl_int CL_API_CALL clRetainDeviceEXT(
    cl_device_id device)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(device);
    if (dispatch_ptr == NULL || dispatch_ptr->clRetainDeviceEXT == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clRetainDeviceEXT(
        device);
}

cl_int CL_API_CALL clCreateSubDevicesEXT(
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

cl_int CL_API_CALL clEnqueueMigrateMemObjectEXT(
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

cl_int CL_API_CALL clSetMemObjectDestructorAPPLE(
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

cl_mem CL_API_CALL clImportMemoryARM(
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

void* CL_API_CALL clSVMAllocARM(
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

void CL_API_CALL clSVMFreeARM(
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

cl_int CL_API_CALL clEnqueueSVMFreeARM(
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

cl_int CL_API_CALL clEnqueueSVMMemcpyARM(
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

cl_int CL_API_CALL clEnqueueSVMMemFillARM(
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

cl_int CL_API_CALL clEnqueueSVMMapARM(
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

cl_int CL_API_CALL clEnqueueSVMUnmapARM(
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

cl_int CL_API_CALL clSetKernelArgSVMPointerARM(
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

cl_int CL_API_CALL clSetKernelExecInfoARM(
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

cl_int CL_API_CALL clEnqueueGenerateMipmapIMG(
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

cl_int CL_API_CALL clEnqueueAcquireGrallocObjectsIMG(
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

cl_int CL_API_CALL clEnqueueReleaseGrallocObjectsIMG(
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

cl_accelerator_intel CL_API_CALL clCreateAcceleratorINTEL(
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

cl_int CL_API_CALL clGetAcceleratorInfoINTEL(
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

cl_int CL_API_CALL clRetainAcceleratorINTEL(
    cl_accelerator_intel accelerator)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(accelerator);
    if (dispatch_ptr == NULL || dispatch_ptr->clRetainAcceleratorINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clRetainAcceleratorINTEL(
        accelerator);
}

cl_int CL_API_CALL clReleaseAcceleratorINTEL(
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

cl_mem CL_API_CALL clCreateBufferWithPropertiesINTEL(
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

/* cl_intel_device_attribute_query */

/* cl_intel_device_partition_by_names */

/* cl_intel_device_side_avc_motion_estimation */

/* cl_intel_driver_diagnostics */

#if defined(CLEXT_INCLUDE_DX9)

/* cl_intel_dx9_media_sharing */

cl_int CL_API_CALL clGetDeviceIDsFromDX9INTEL(
    cl_platform_id platform,
    cl_dx9_device_source_intel dx9_device_source,
    void* dx9_object,
    cl_dx9_device_set_intel dx9_device_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(platform);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetDeviceIDsFromDX9INTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetDeviceIDsFromDX9INTEL(
        platform,
        dx9_device_source,
        dx9_object,
        dx9_device_set,
        num_entries,
        devices,
        num_devices);
}

cl_mem CL_API_CALL clCreateFromDX9MediaSurfaceINTEL(
    cl_context context,
    cl_mem_flags flags,
    IDirect3DSurface9* resource,
    HANDLE sharedHandle,
    UINT plane,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromDX9MediaSurfaceINTEL == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromDX9MediaSurfaceINTEL(
        context,
        flags,
        resource,
        sharedHandle,
        plane,
        errcode_ret);
}

cl_int CL_API_CALL clEnqueueAcquireDX9ObjectsINTEL(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueAcquireDX9ObjectsINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueAcquireDX9ObjectsINTEL(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clEnqueueReleaseDX9ObjectsINTEL(
    cl_command_queue command_queue,
    cl_uint num_objects,
    cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueReleaseDX9ObjectsINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueReleaseDX9ObjectsINTEL(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

#endif // defined(CLEXT_INCLUDE_DX9)

/* cl_intel_egl_image_yuv */

/* cl_intel_mem_channel_property */

/* cl_intel_mem_force_host_memory */

/* cl_intel_motion_estimation */

/* cl_intel_packed_yuv */

/* cl_intel_planar_yuv */

/* cl_intel_required_subgroup_size */

/* cl_intel_sharing_format_query */

#if defined(CLEXT_INCLUDE_D3D10)

/* cl_intel_sharing_format_query_d3d10 */

cl_int CL_API_CALL clGetSupportedD3D10TextureFormatsINTEL(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint num_entries,
    DXGI_FORMAT* d3d10_formats,
    cl_uint* num_texture_formats)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetSupportedD3D10TextureFormatsINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetSupportedD3D10TextureFormatsINTEL(
        context,
        flags,
        image_type,
        num_entries,
        d3d10_formats,
        num_texture_formats);
}

#endif // defined(CLEXT_INCLUDE_D3D10)

#if defined(CLEXT_INCLUDE_D3D11)

/* cl_intel_sharing_format_query_d3d11 */

cl_int CL_API_CALL clGetSupportedD3D11TextureFormatsINTEL(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint plane,
    cl_uint num_entries,
    DXGI_FORMAT* d3d11_formats,
    cl_uint* num_texture_formats)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetSupportedD3D11TextureFormatsINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetSupportedD3D11TextureFormatsINTEL(
        context,
        flags,
        image_type,
        plane,
        num_entries,
        d3d11_formats,
        num_texture_formats);
}

#endif // defined(CLEXT_INCLUDE_D3D11)

#if defined(CLEXT_INCLUDE_DX9)

/* cl_intel_sharing_format_query_dx9 */

cl_int CL_API_CALL clGetSupportedDX9MediaSurfaceFormatsINTEL(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint plane,
    cl_uint num_entries,
    D3DFORMAT* dx9_formats,
    cl_uint* num_surface_formats)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetSupportedDX9MediaSurfaceFormatsINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetSupportedDX9MediaSurfaceFormatsINTEL(
        context,
        flags,
        image_type,
        plane,
        num_entries,
        dx9_formats,
        num_surface_formats);
}

#endif // defined(CLEXT_INCLUDE_DX9)

#if defined(CLEXT_INCLUDE_GL)

/* cl_intel_sharing_format_query_gl */

cl_int CL_API_CALL clGetSupportedGLTextureFormatsINTEL(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint num_entries,
    cl_GLenum* gl_formats,
    cl_uint* num_texture_formats)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetSupportedGLTextureFormatsINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetSupportedGLTextureFormatsINTEL(
        context,
        flags,
        image_type,
        num_entries,
        gl_formats,
        num_texture_formats);
}

#endif // defined(CLEXT_INCLUDE_GL)

#if defined(CLEXT_INCLUDE_VA_API)

/* cl_intel_sharing_format_query_va_api */

cl_int CL_API_CALL clGetSupportedVA_APIMediaSurfaceFormatsINTEL(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint plane,
    cl_uint num_entries,
    VAImageFormat* va_api_formats,
    cl_uint* num_surface_formats)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetSupportedVA_APIMediaSurfaceFormatsINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetSupportedVA_APIMediaSurfaceFormatsINTEL(
        context,
        flags,
        image_type,
        plane,
        num_entries,
        va_api_formats,
        num_surface_formats);
}

#endif // defined(CLEXT_INCLUDE_VA_API)

/* cl_intel_simultaneous_sharing */

/* cl_intel_thread_local_exec */

/* cl_intel_unified_shared_memory */

void* CL_API_CALL clHostMemAllocINTEL(
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

void* CL_API_CALL clDeviceMemAllocINTEL(
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

void* CL_API_CALL clSharedMemAllocINTEL(
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

cl_int CL_API_CALL clMemFreeINTEL(
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

cl_int CL_API_CALL clMemBlockingFreeINTEL(
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

cl_int CL_API_CALL clGetMemAllocInfoINTEL(
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

cl_int CL_API_CALL clSetKernelArgMemPointerINTEL(
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

cl_int CL_API_CALL clEnqueueMemsetINTEL(
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

cl_int CL_API_CALL clEnqueueMemFillINTEL(
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

cl_int CL_API_CALL clEnqueueMemcpyINTEL(
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

cl_int CL_API_CALL clEnqueueMemAdviseINTEL(
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

cl_int CL_API_CALL clEnqueueMigrateMemINTEL(
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

#endif // defined(CL_VERSION_1_2)

#if defined(CLEXT_INCLUDE_VA_API)

/* cl_intel_va_api_media_sharing */

cl_int CL_API_CALL clGetDeviceIDsFromVA_APIMediaAdapterINTEL(
    cl_platform_id platform,
    cl_va_api_device_source_intel media_adapter_type,
    void* media_adapter,
    cl_va_api_device_set_intel media_adapter_set,
    cl_uint num_entries,
    cl_device_id* devices,
    cl_uint* num_devices)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(platform);
    if (dispatch_ptr == NULL || dispatch_ptr->clGetDeviceIDsFromVA_APIMediaAdapterINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clGetDeviceIDsFromVA_APIMediaAdapterINTEL(
        platform,
        media_adapter_type,
        media_adapter,
        media_adapter_set,
        num_entries,
        devices,
        num_devices);
}

cl_mem CL_API_CALL clCreateFromVA_APIMediaSurfaceINTEL(
    cl_context context,
    cl_mem_flags flags,
    VASurfaceID* surface,
    cl_uint plane,
    cl_int* errcode_ret)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(context);
    if (dispatch_ptr == NULL || dispatch_ptr->clCreateFromVA_APIMediaSurfaceINTEL == NULL) {
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
    }
    return dispatch_ptr->clCreateFromVA_APIMediaSurfaceINTEL(
        context,
        flags,
        surface,
        plane,
        errcode_ret);
}

cl_int CL_API_CALL clEnqueueAcquireVA_APIMediaSurfacesINTEL(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueAcquireVA_APIMediaSurfacesINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueAcquireVA_APIMediaSurfacesINTEL(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

cl_int CL_API_CALL clEnqueueReleaseVA_APIMediaSurfacesINTEL(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem* mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list,
    cl_event* event)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(command_queue);
    if (dispatch_ptr == NULL || dispatch_ptr->clEnqueueReleaseVA_APIMediaSurfacesINTEL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clEnqueueReleaseVA_APIMediaSurfacesINTEL(
        command_queue,
        num_objects,
        mem_objects,
        num_events_in_wait_list,
        event_wait_list,
        event);
}

#endif // defined(CLEXT_INCLUDE_VA_API)

/* cl_nv_device_attribute_query */

/* cl_pocl_content_size */

cl_int CL_API_CALL clSetContentSizeBufferPoCL(
    cl_mem buffer,
    cl_mem content_size_buffer)
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(buffer);
    if (dispatch_ptr == NULL || dispatch_ptr->clSetContentSizeBufferPoCL == NULL) {
        return CL_INVALID_OPERATION;
    }
    return dispatch_ptr->clSetContentSizeBufferPoCL(
        buffer,
        content_size_buffer);
}

/* cl_qcom_android_native_buffer_host_ptr */

/* cl_qcom_ext_host_ptr */

cl_int CL_API_CALL clGetDeviceImageInfoQCOM(
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
