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

/***************************************************************
* Function Pointer Typedefs
***************************************************************/

/* cl_khr_create_command_queue */

typedef cl_command_queue (CL_API_CALL* clCreateCommandQueueWithPropertiesKHR_clextfn)(
    cl_context context,
    cl_device_id device,
    const cl_queue_properties_khr* properties,
    cl_int* errcode_ret);

/* cl_khr_spir */

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


/***************************************************************
* Extension Function Pointer Dispatch Table
***************************************************************/

struct openclext_dispatch_table {
    /* cl_khr_create_command_queue */
    clCreateCommandQueueWithPropertiesKHR_clextfn clCreateCommandQueueWithPropertiesKHR;

    /* cl_khr_spir */

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

    /* cl_khr_spir */

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

/* cl_khr_spir */

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

#ifdef __cplusplus
}
#endif
