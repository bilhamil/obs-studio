#include <obs-module.h>
#include <graphics/vec4.h>
#include <util/dstr.h>

#define SETTING_MASK_PATH              "mask_path"
#define SETTING_COLOR                  "color"
#define SETTING_OPACITY                "opacity"

#define TEXT_MASK_PATH                 obs_module_text("Path")
#define TEXT_COLOR                     obs_module_text("Color")
#define TEXT_OPACITY                   obs_module_text("Opacity")
#define TEXT_PATH_IMAGES               obs_module_text("BrowsePath.Images")
#define TEXT_PATH_ALL_FILES            obs_module_text("BrowsePath.AllFiles")

struct mask_filter_data {
	obs_source_t                   *context;
	gs_effect_t                    *effect;

	gs_texture_t                   *mask;
	struct vec4                    color;
};

static const char *mask_filter_get_name(void)
{
	return obs_module_text("MaskFilter.Name");
}

static void mask_filter_update(void *data, obs_data_t *settings)
{
	struct mask_filter_data *filter = data;

	const char *path = obs_data_get_string(settings, SETTING_MASK_PATH);
	uint32_t color = (uint32_t)obs_data_get_int(settings, SETTING_COLOR);
	int opacity = (int)obs_data_get_int(settings, SETTING_OPACITY);

	color |= (uint32_t)(((double)opacity) * 2.55) << 24;

	vec4_from_rgba(&filter->color, color);

	obs_enter_graphics();
	gs_texture_destroy(filter->mask);
	filter->mask = (path) ? gs_texture_create_from_file(path) : NULL;
	obs_leave_graphics();
}

static void mask_filter_defaults(obs_data_t *settings)
{
	obs_data_set_default_int(settings, SETTING_COLOR, 0xFFFFFF);
	obs_data_set_default_int(settings, SETTING_OPACITY, 100);
}

#define IMAGE_FILTER_EXTENSIONS \
	" (*.bmp *.jpg *.jpeg *.tga *.gif *.png)"

static obs_properties_t *mask_filter_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();
	struct dstr filter_str = {0};

	dstr_copy(&filter_str, TEXT_PATH_IMAGES);
	dstr_cat(&filter_str, IMAGE_FILTER_EXTENSIONS ";;");
	dstr_cat(&filter_str, TEXT_PATH_ALL_FILES);
	dstr_cat(&filter_str, " (*.*)");

	obs_properties_add_path(props, SETTING_MASK_PATH, TEXT_MASK_PATH,
			OBS_PATH_FILE, filter_str.array, NULL);
	obs_properties_add_color(props, SETTING_COLOR, TEXT_COLOR);
	obs_properties_add_int(props, SETTING_OPACITY, TEXT_OPACITY, 0, 100, 1);

	dstr_free(&filter_str);

	UNUSED_PARAMETER(data);
	return props;
}

static void *mask_filter_create(obs_data_t *settings, obs_source_t *context)
{
	struct mask_filter_data *filter =
		bzalloc(sizeof(struct mask_filter_data));
	char *effect_path = obs_module_file("mask_filter.effect");

	filter->context = context;

	obs_enter_graphics();
	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	obs_leave_graphics();

	bfree(effect_path);

	obs_source_update(context, settings);
	return filter;
}

static void mask_filter_destroy(void *data)
{
	struct mask_filter_data *filter = data;

	obs_enter_graphics();
	gs_effect_destroy(filter->effect);
	gs_texture_destroy(filter->mask);
	obs_leave_graphics();

	bfree(filter);
}

static void mask_filter_render(void *data, gs_effect_t *effect)
{
	struct mask_filter_data *filter = data;
	gs_eparam_t *param;

	if (!filter->mask) {
		obs_source_skip_video_filter(filter->context);
		return;
	}

	param = gs_effect_get_param_by_name(filter->effect, "mask");
	gs_effect_set_texture(param, filter->mask);

	param = gs_effect_get_param_by_name(filter->effect, "color");
	gs_effect_set_vec4(param, &filter->color);

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
	.update                        = mask_filter_update,
	.get_defaults                  = mask_filter_defaults,
	.get_properties                = mask_filter_properties,
	.video_render                  = mask_filter_render
};
