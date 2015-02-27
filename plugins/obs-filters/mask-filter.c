#include <obs-module.h>

struct mask_filter_data {
	obs_source_t *context;

	gs_texture_t *mask;
	gs_effect_t *effect;
	char *mask_path;
};

static const char *mask_filter_get_name(void)
{
	return obs_module_text("MaskFilter.Name");
}

static void *mask_filter_create(obs_data_t *settings, obs_source_t *context)
{
	struct mask_filter_data *filter =
		bzalloc(sizeof(struct mask_filter_data));
	char *effect_path = obs_module_file("mask_filter.effect");

	filter->mask_path = bstrdup("D:\\heart.png");
	filter->context = context;

	obs_enter_graphics();
	filter->mask = gs_texture_create_from_file(filter->mask_path);
	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	obs_leave_graphics();

	bfree(effect_path);

	UNUSED_PARAMETER(settings);
	return filter;
}

static void mask_filter_destroy(void *data)
{
	struct mask_filter_data *filter = data;

	obs_enter_graphics();
	gs_effect_destroy(filter->effect);
	gs_texture_destroy(filter->mask);
	obs_leave_graphics();

	bfree(filter->mask_path);
	bfree(filter);
}

static void mask_filter_render(void *data, gs_effect_t *effect)
{
	struct mask_filter_data *filter = data;
	gs_eparam_t *param;
	struct vec4 color;

	vec4_set(&color, 1.0f, 1.0f, 1.0f, 1.0f);

	param = gs_effect_get_param_by_name(filter->effect, "mask");
	gs_effect_set_texture(param, filter->mask);

	param = gs_effect_get_param_by_name(filter->effect, "color");
	gs_effect_set_vec4(param, &color);

	obs_source_process_filter(filter->context, filter->effect,
			0, 0, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING);

	UNUSED_PARAMETER(effect);
}

struct obs_source_info mask_filter = {
	.id                            = "mask_filter",
	.type                          = OBS_SOURCE_TYPE_FILTER,
	.output_flags                  = OBS_SOURCE_VIDEO,
	.get_name                      = mask_filter_get_name,
	.create                        = mask_filter_create,
	.destroy                       = mask_filter_destroy,
	.video_render                  = mask_filter_render
};
