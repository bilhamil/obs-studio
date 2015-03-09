#include <obs-module.h>
#include <obs-avc.h>
#include <obs-properties.h>
#include <util/platform.h>
#include <util/circlebuf.h>
#include <util/dstr.h>
#include <util/threading.h>
#include <inttypes.h>

struct obs_output_info webrtc_output_info;

struct WebRTCStream {
	obs_output_t     *output;
};

static const char *WebRTCStreamGetName(void)
{
	return "WebRTC Output";
}

static void *WebRTCStreamCreate(obs_data_t *settings, obs_output_t *output)
{
	struct WebRTCStream *stream = new WebRTCStream();
	stream->output = output;

	UNUSED_PARAMETER(settings);
	return stream;

fail:
	WebRTCStreamDestroy(stream);
	return NULL;
}

static bool WebRTCStreamStart(void *data)
{
	struct WebRTCStream *stream = reinterpret_cast<WebRTCStream*>(data);
	return true;
}

void WebRTCStreamRawVideo(void *data, struct video_data *frame)
{
	struct WebRTCStream *stream = reinterpret_cast<WebRTCStream*>(data);
}

void WebRTCStreamRawAudio(void *data, struct audio_data *frames)
{
	struct WebRTCStream *stream = reinterpret_cast<WebRTCStream*>(data);
}

static void WebRTCStreamStop(void *data)
{
	struct WebRTCStream *stream = reinterpret_cast<WebRTCStream*>(data);
}

static void WebRTCStreamDestroy(void *data)
{
	struct WebRTCStream *stream = reinterpret_cast<WebRTCStream*>(data);

}

static obs_properties_t *WebRTCStreamProperties(void *data)
{
	UNUSED_PARAMETER(data);
}

static void WebRTCStreamDefaults(obs_data_t *defaults)
{
	//unused for now
}

void setup_webrc_output_info()
{
	webrtc_output_info.id = "webrtc_output";
	webrtc_output_info.flags = OBS_OUTPUT_AV |
	OBS_OUTPUT_SERVICE |
	OBS_OUTPUT_MULTI_TRACK,
	webrtc_output_info.get_name = WebRTCStreamGetName;
	webrtc_output_info.create = WebRTCStreamCreate;
	webrtc_output_info.destroy = WebRTCStreamDestroy;
	webrtc_output_info.start = WebRTCStreamStart;
	webrtc_output_info.stop = WebRTCStreamStop;
	webrtc_output_info.raw_video = WebRTCStreamRawVideo;
	webrtc_output_info.raw_audio = WebRTCStreamRawAudio;
	webrtc_output_info.get_defaults = WebRTCStreamDefaults;
	webrtc_output_info.get_properties = WebRTCStreamProperties;
	//webrtc_output_info.get_dropped_frames = rtmp_stream_dropped_frames;
}