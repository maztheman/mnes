#ifndef __MSOUND_H__
#define __MSOUND_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined _MSC_VER
#	if !defined DLL_EXPORT && defined MSND_STATIC
#		define DLL_EXPORT
#	endif
#	if defined DLL_EXPORT
#		define MSND_EXPORT __declspec(dllexport)
#	else
#		define MSND_EXPORT __declspec(dllimport)
#	endif
#endif
	typedef void*	device_list_t;
	typedef void*	device_t;
	typedef unsigned int uint;
	typedef unsigned char uchar;

	typedef struct device_format_t
	{
		int channels;
		int hertz;
		int bits_per_sample;

	} device_format_t;

	MSND_EXPORT void			msound_initialize(void);
	MSND_EXPORT void			msound_finalized(void);


	MSND_EXPORT device_list_t	msound_device_list(void);
	MSND_EXPORT uint			msound_device_list_get_count(device_list_t device_list);
	MSND_EXPORT device_t		msound_device_list_get_item(device_list_t device_list, uint index);
	MSND_EXPORT void			msound_device_list_destroy(device_list_t list);

	MSND_EXPORT int				msound_device_initialize(device_t device, device_format_t* format);
	MSND_EXPORT uint			msound_device_get_buffer_size(device_t device);
	MSND_EXPORT void			msound_device_start(device_t device);
	MSND_EXPORT void			msound_device_stop(device_t device);
	MSND_EXPORT void			msound_device_set_buffer(device_t device, uchar* data, uint length);
	MSND_EXPORT void			msound_device_destroy(device_t device);

#undef MSND_EXPORT

#ifdef __cplusplus
}
#endif

#endif