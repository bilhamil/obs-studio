# Once done these will be defined:
#
#  WEBRTC_FOUND
#  WEBRTC_INCLUDE_DIRS
#  WEBRTC_LIBRARIES
#
# For use in OBS: 
#
#  WEBRTC_INCLUDE_DIR

find_package(PkgConfig QUIET)
pkg_check_modules(PC_WEBRTC QUIET)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(_lib_suffix 64)
	set(_dir_suffix _x64)
else()
	set(_lib_suffix 32)
	set(_dir_suffix)
endif()

find_path(WEBRTC_INCLUDE_DIR
	NAMES 
		"webrtc/common_types.h"
	HINTS
		ENV WebRTCPath
		${PC_WEBRTC_INCLUDE_DIRS}
	PATHS
		/usr/include /usr/local/include /opt/local/include /sw/include
	PATH_SUFFIXES
		src/)

find_path(WEBRTC_LIBRARY_DEBUG_DIR
	NAMES 
		libyuv.lib
	HINTS
		ENV WebRTCPath
		${PC_WEBRTC_LIBRARY_DEBUG_DIR}
	PATH_SUFFIXES
		src/out/Debug${_dir_suffix})

find_path(WEBRTC_LIBRARY_RELEASE_DIR
	NAMES 
		libyuv.lib
	HINTS
		ENV WebRTCPath
		${PC_WEBRTC_LIBRARY_RELEASE_DIR}
	PATHS
		/usr/lib /usr/local/lib /opt/local/lib /sw/lib
	PATH_SUFFIXES
		src/out/Release${_dir_suffix})

if(WIN32)
	set(LIBJINGLE_SYSTEM_LIBS 
		wininet
		dnsapi
		version
		msimg32
		ws2_32
		usp10
		psapi
		dbghelp
		winmm
		shlwapi
		kernel32
		gdi32
		winspool
		comdlg32
		advapi32
		shell32
		ole32
		oleaut32
		user32
		uuid
		odbc32
		odbccp32
		delayimp
		credui
		netapi32
		crypt32
		iphlpapi
		secur32
		dmoguids
		wmcodecdspuuid
		amstrmid
		msdmo
		Strmiids
	)
else()
	set(LIBJINGLE_SYSTEM_LIBS)
endif()

macro(set_webrtc_libs VARNAME LIBDIRECTORY)
	set(${VARNAME}
		${LIBJINGLE_SYSTEM_LIBS}
		${LIBDIRECTORY}/obj/webrtc/modules/audioproc_debug_proto.lib
		${LIBDIRECTORY}/obj/webrtc/modules/audio_coding_module.lib
		${LIBDIRECTORY}/obj/webrtc/modules/audio_conference_mixer.lib
		${LIBDIRECTORY}/obj/webrtc/modules/audio_decoder_interface.lib
		${LIBDIRECTORY}/obj/webrtc/modules/audio_device.lib
		${LIBDIRECTORY}/obj/webrtc/modules/audio_encoder_interface.lib
		${LIBDIRECTORY}/obj/webrtc/modules/audio_processing.lib
		${LIBDIRECTORY}/obj/webrtc/modules/audio_processing_sse2.lib
		${LIBDIRECTORY}/obj/webrtc/modules/bitrate_controller.lib
		${LIBDIRECTORY}/obj/third_party/boringssl/boringssl.lib
		${LIBDIRECTORY}/obj/webrtc/modules/CNG.lib
		${LIBDIRECTORY}/obj/webrtc/common_audio/common_audio.lib
		${LIBDIRECTORY}/obj/webrtc/common_audio/common_audio_sse2.lib
		${LIBDIRECTORY}/obj/webrtc/common_video/common_video.lib
		${LIBDIRECTORY}/obj/third_party/winsdk_samples/directshow_baseclasses.lib
		${LIBDIRECTORY}/obj/webrtc/test/field_trial.lib
		${LIBDIRECTORY}/obj/webrtc/modules/G711.lib
		${LIBDIRECTORY}/obj/webrtc/modules/G722.lib
		${LIBDIRECTORY}/obj/webrtc/modules/iLBC.lib
		${LIBDIRECTORY}/obj/webrtc/modules/iSAC.lib
		${LIBDIRECTORY}/obj/third_party/jsoncpp/jsoncpp.lib
		${LIBDIRECTORY}/obj/talk/libjingle_media.lib
		${LIBDIRECTORY}/obj/talk/libjingle_p2p.lib
		${LIBDIRECTORY}/obj/talk/libjingle_peerconnection.lib
		${LIBDIRECTORY}/obj/third_party/libjpeg_turbo/libjpeg.lib
		${LIBDIRECTORY}/obj/third_party/libsrtp/libsrtp.lib
		${LIBDIRECTORY}/obj/third_party/libvpx/libvpx.lib
		${LIBDIRECTORY}/obj/third_party/libvpx/libvpx_intrinsics_avx2.lib
		${LIBDIRECTORY}/obj/third_party/libvpx/libvpx_intrinsics_mmx.lib
		${LIBDIRECTORY}/obj/third_party/libvpx/libvpx_intrinsics_sse2.lib
		${LIBDIRECTORY}/obj/third_party/libvpx/libvpx_intrinsics_sse4_1.lib
		${LIBDIRECTORY}/obj/third_party/libvpx/libvpx_intrinsics_ssse3.lib
		${LIBDIRECTORY}/libyuv.lib
		${LIBDIRECTORY}/obj/webrtc/modules/media_file.lib
		${LIBDIRECTORY}/obj/webrtc/system_wrappers/metrics_default.lib
		${LIBDIRECTORY}/obj/webrtc/modules/neteq.lib
		${LIBDIRECTORY}/obj/third_party/openmax_dl/dl/openmax_dl.lib
		${LIBDIRECTORY}/obj/third_party/opus/opus.lib
		${LIBDIRECTORY}/obj/webrtc/modules/paced_sender.lib
		${LIBDIRECTORY}/obj/webrtc/modules/PCM16B.lib
		${LIBDIRECTORY}/obj/third_party/protobuf/protobuf_full_do_not_use.lib
		${LIBDIRECTORY}/obj/webrtc/modules/red.lib
		${LIBDIRECTORY}/obj/webrtc/modules/remote_bitrate_estimator.lib
		${LIBDIRECTORY}/obj/webrtc/base/rtc_base.lib
		${LIBDIRECTORY}/obj/webrtc/base/rtc_base_approved.lib
		${LIBDIRECTORY}/obj/webrtc/p2p/rtc_p2p.lib
		${LIBDIRECTORY}/obj/webrtc/modules/rtp_rtcp.lib
		${LIBDIRECTORY}/obj/webrtc/system_wrappers/system_wrappers.lib
		${LIBDIRECTORY}/obj/third_party/usrsctp/usrsctplib.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_capture_module.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_capture_module_internal_impl.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_coding/utility/video_coding_utility.lib
		${LIBDIRECTORY}/obj/webrtc/video_engine/video_engine_core.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_processing.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_processing_sse2.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_render_module.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_render_module_internal_impl.lib
		${LIBDIRECTORY}/obj/webrtc/voice_engine/voice_engine.lib
		${LIBDIRECTORY}/obj/webrtc/modules/webrtc_i420.lib
		${LIBDIRECTORY}/obj/webrtc/modules/webrtc_opus.lib
		${LIBDIRECTORY}/obj/webrtc/modules/webrtc_utility.lib
		${LIBDIRECTORY}/obj/webrtc/modules/webrtc_video_coding.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_coding/codecs/vp8/webrtc_vp8.lib
		${LIBDIRECTORY}/obj/webrtc/modules/video_coding/codecs/vp9/webrtc_vp9.lib
	)
endmacro()

if(WEBRTC_LIBRARY_RELEASE_DIR)
	set_webrtc_libs(WEBRTC_LIBRARIES_RELEASE WEBRTC_LIBRARY_RELEASE_DIR)
endif()

if(WEBRTC_LIBRARY_DEBUG_DIR)
	set_webrtc_libs(WEBRTC_LIBRARIES_DEBUG WEBRTC_LIBRARY_DEBUG_DIR)
endif()

if((CMAKE_C_FLAGS_DEBUG OR CMAKE_CXX_FLAGS_DEBUG) AND WEBRTC_LIBRARY_DEBUG_DIR)
	set_webrtc_libs(WEBRTC_LIBRARIES ${WEBRTC_LIBRARY_DEBUG_DIR})
elseif(WEBRTC_LIBRARY_RELEASE_DIR)
	set_webrtc_libs(WEBRTC_LIBRARIES ${WEBRTC_LIBRARY_RELEASE_DIR})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WebRTC DEFAULT_MSG WEBRTC_LIBRARIES WEBRTC_INCLUDE_DIR)
mark_as_advanced(WEBRTC_LIBRARIES_RELEASE WEBRTC_LIBRARIES_DEBUG WEBRTC_LIBRARIES)

if(WEBRTC_FOUND)
	set(WEBRTC_INCLUDE_DIRS ${WEBRTC_INCLUDE_DIR})
endif()
