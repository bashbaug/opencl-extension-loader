<%
# Extensions to skip - they are in dedicated files.
skipExtensions = {
    'cl_khr_d3d10_sharing',
    'cl_khr_d3d11_sharing',
    'cl_khr_dx9_media_sharing',
    'cl_khr_egl_event',
    'cl_khr_egl_image',
    'cl_khr_gl_depth_images',
    'cl_khr_gl_event',
    'cl_khr_gl_msaa_sharing',
    'cl_khr_gl_sharing',
    'cl_loader_layers',
    'cl_intel_dx9_media_sharing',
    'cl_intel_va_api_media_sharing',
    }

# Extensions to include in this file:
def shouldGenerate(extension):
    if extension in genExtensions:
        return True
    elif not genExtensions and not extension in skipExtensions:
        return True
    return False

# XML blocks with funcitons to include:
def shouldEmit(block):
    for func in block.findall('command'):
        return True
    return False

# Order the extensions should be emitted in the headers.
# KHR -> EXT -> Vendor Extensions
def getExtensionSortKey(item):
    name = item.get('name')
    if name.startswith('cl_khr'):
        return 0, name
    if name.startswith('cl_ext'):
        return 1, name
    return 99, name

# Gets C function parameter strings for the specified API params:
def getCParameterStrings(params):
    strings = []
    if len(params) == 0:
        strings.append("void")
    else:
        for param in params:
            paramstr = param.Type + ' ' + param.Name + param.TypeEnd
            strings.append(paramstr)
    return strings

%>/*******************************************************************************
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
%if includes:
${includes}
%endif

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

%for extension in sorted(spec.findall('extensions/extension'), key=getExtensionSortKey):
%  if shouldGenerate(extension.get('name')):
<%
    name = extension.get('name')
%>/* ${name} */
%for block in extension.findall('require'):
%  if shouldEmit(block):
%    if block.get('condition'):
#if ${block.get('condition')}
%    endif
%    for func in block.findall('command'):
<%
    api = apisigs[func.get('name')]
%>
typedef ${api.RetType} (CL_API_CALL* ${api.Name}_clextfn)(
%      for i, paramStr in enumerate(getCParameterStrings(api.Params)):
%        if i < len(api.Params)-1:
    ${paramStr},
%        else:
    ${paramStr});
%        endif
%      endfor
%    endfor
%    if block.get('condition'):
#endif
%    endif
%  endif
%endfor

%  endif
%endfor

/***************************************************************
* Extension Function Pointer Dispatch Table
***************************************************************/

struct openclext_dispatch_table {
%for extension in sorted(spec.findall('extensions/extension'), key=getExtensionSortKey):
%  if shouldGenerate(extension.get('name')):
<%
    name = extension.get('name')
%>    /* ${name} */
%for block in extension.findall('require'):
%  if shouldEmit(block):
%    if block.get('condition'):
#if ${block.get('condition')}
%    endif
%    for func in block.findall('command'):
<%
    api = apisigs[func.get('name')]
%>    ${api.Name}_clextfn ${api.Name};
%    endfor
%    if block.get('condition'):
#endif
%    endif
%  endif
%endfor

%  endif
%endfor
};

/***************************************************************
* Dispatch Table Initialization (TODO)
***************************************************************/

static openclext_dispatch_table _dispatch = {0};

static void _init(cl_platform_id platform, openclext_dispatch_table* dispatch_ptr)
{
#define GET_EXTENSION(_funcname)                                               ${"\\"}
    dispatch_ptr->##_funcname =                                                ${"\\"}
        (_funcname##_fn)clGetExtensionFunctionAddressForPlatform(platform,     ${"\\"}
                                                                 #_funcname);

%for extension in sorted(spec.findall('extensions/extension'), key=getExtensionSortKey):
%  if shouldGenerate(extension.get('name')):
<%
    name = extension.get('name')
%>    /* ${name} */
%for block in extension.findall('require'):
%  if shouldEmit(block):
%    if block.get('condition'):
#if ${block.get('condition')}
%    endif
%    for func in block.findall('command'):
<%
    api = apisigs[func.get('name')]
%>    GET_EXTENSION(${api.Name});
%    endfor
%    if block.get('condition'):
#endif
%    endif
%  endif
%endfor

%  endif
%endfor
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

%for extension in sorted(spec.findall('extensions/extension'), key=getExtensionSortKey):
%  if shouldGenerate(extension.get('name')):
<%
    name = extension.get('name')
%>/* ${name} */
%for block in extension.findall('require'):
%  if shouldEmit(block):
%    if block.get('condition'):
#if ${block.get('condition')}
%    endif
%    for func in block.findall('command'):
<%
    api = apisigs[func.get('name')]
%>
${api.RetType} ${api.Name}(
%      for i, paramStr in enumerate(getCParameterStrings(api.Params)):
%        if i < len(api.Params)-1:
    ${paramStr},
%        else:
    ${paramStr})
%        endif
%      endfor
{
    struct openclext_dispatch_table* dispatch_ptr = _get_dispatch(${api.Params[0].Name});
    if (dispatch_ptr == NULL || dispatch_ptr->${api.Name} == NULL) {
%      if api.RetType == "cl_int":
        return CL_INVALID_OPERATION;
%      elif api.Params[len(api.Params)-1].Name == "errcode_ret":
        if (errcode_ret) *errcode_ret = CL_INVALID_OPERATION;
        return NULL;
%      else:
        // not sure how to return an error in this case!
%      endif
    }
%      if api.RetType == "void":
    dispatch_ptr->${api.Name}(
%      else:
    return dispatch_ptr->${api.Name}(
%      endif
%      for i, arg in enumerate(api.Params):
%        if i < len(api.Params)-1:
        ${arg.Name},
%        else:
        ${arg.Name});
%        endif
%      endfor
}
%    endfor
%    if block.get('condition'):
#endif
%    endif
%  endif
%endfor

%  endif
%endfor
#ifdef __cplusplus
}
#endif
