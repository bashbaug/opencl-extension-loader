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
    'cl_khr_icd',
    'cl_loader_layers',
    'cl_APPLE_ContextLoggingFunctions',
    'cl_intel_dx9_media_sharing',
    'cl_intel_va_api_media_sharing',
    'cl_intel_sharing_format_query_d3d10',
    'cl_intel_sharing_format_query_d3d11',
    'cl_intel_sharing_format_query_dx9',
    'cl_intel_sharing_format_query_gl',
    'cl_intel_sharing_format_query_va_api',
    }

defaultValueForType = {
    # Handle Types
    'cl_accelerator_intel'              : 'NULL',
    'cl_command_queue'                  : 'NULL',
    'cl_context'                        : 'NULL',
    'cl_device_id'                      : 'NULL',
    'cl_event'                          : 'NULL',
    'cl_kernel'                         : 'NULL',
    'cl_mem'                            : 'NULL',
    'cl_program'                        : 'NULL',
    'cl_sampler'                        : 'NULL',
    # Enum Types
    'cl_accelerator_info_intel'         : 'CL_ACCELERATOR_DESCRIPTOR_INTEL',
    'cl_accelerator_type_intel'         : '0',
    'cl_bool'                           : 'CL_FALSE',
    'cl_image_pitch_info_qcom'          : 'CL_IMAGE_ROW_ALIGNMENT_QCOM',
    'cl_kernel_exec_info_arm'           : 'CL_KERNEL_EXEC_INFO_SVM_PTRS_ARM',
    'cl_kernel_sub_group_info'          : 'CL_KERNEL_MAX_SUB_GROUP_SIZE_FOR_NDRANGE_KHR',
    'cl_mem_advice_intel'               : '0',
    'cl_map_flags'                      : 'CL_MAP_READ',
    'cl_mem_flags'                      : 'CL_MEM_READ_WRITE',
    'cl_mem_info_intel'                 : 'CL_MEM_ALLOC_TYPE_INTEL',
    'cl_mem_migration_flags'            : 'CL_MIGRATE_MEM_OBJECT_HOST',
    'cl_mem_migration_flags_ext'        : 'CL_MIGRATE_MEM_OBJECT_HOST_EXT',
    'cl_mipmap_filter_mode_img'         : 'CL_MIPMAP_FILTER_ANY_IMG',
    'cl_svm_mem_flags_arm'              : 'CL_MEM_SVM_FINE_GRAIN_BUFFER_ARM',
    # Integral Types
    'cl_int'                            : '0',
    'cl_uint'                           : '0',
    'size_t'                            : '0',
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
def getCallArgs(params):
    callstr = ""
    for i, param in enumerate(params):
        if i != 0:
            callstr += ', '
        if param.Type.endswith('*'):
            callstr += 'NULL'
        elif param.Type in defaultValueForType:
            callstr += defaultValueForType[param.Type]
        else:
            callstr += '???'
    return callstr

%>#define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_USE_DEPRECATED_OPENCL_2_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_2_APIS
#define CL_USE_DEPRECATED_OPENCL_3_0_APIS
#include "CL/cl.h"
%if includes:
${includes}
%endif

void call_all(void)
{
%for extension in sorted(spec.findall('extensions/extension'), key=getExtensionSortKey):
%  if shouldGenerate(extension.get('name')):
%for block in extension.findall('require'):
%  if shouldEmit(block):
#ifdef ${extension.get('name')}
%    if block.get('condition'):
#if ${block.get('condition')}
%    endif
%    for func in block.findall('command'):
<%
    api = apisigs[func.get('name')]
%>    ${api.Name}(${getCallArgs(api.Params)});
%    endfor
%    if block.get('condition'):
#endif // ${block.get('condition')}
%    endif
#endif // ${extension.get('name')}

%  endif
%endfor
%  endif
%endfor
}
