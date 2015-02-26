#include <obs-module.h>

OBS_DECLARE_MODULE()

OBS_MODULE_USE_DEFAULT_LOCALE("obs-filters", "en-US")

extern struct obs_source_info mask_filter;

bool obs_module_load(void)
{
	obs_register_source(&mask_filter);
	return true;
}
