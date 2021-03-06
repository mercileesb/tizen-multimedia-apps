/*
 * codecbase.cpp
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "codecbase.h"
#include <stdexcept>


#define throw_error_and_destroy_codec(codec, msg, error_code)\
	{\
		if(codec)\
		{\
			mediacodec_destroy(codec);\
			codec = NULL;\
		}\
		throw std::runtime_error(std::string(msg)+to_string(error_code));\
	}\

CodecBase::CodecBase()
:m_mediacodec(NULL), m_in_count(0), m_format(NULL)
{

}
CodecBase::~CodecBase()
{
	Destroy();
}

void CodecBase::Create(const CodecInfo& codecinfo)
{
	/*
	 * A codec object is first created by calling mediacodec_create and the handle is received
	 */
	iferror_throw(mediacodec_create(&m_mediacodec), "fail to create mediacodec_create");

	/*
	 *create and create_format are pure functions so they should be implemented in derived classes
	 *an audio decoder object is configured with mediacodec_set_adec_info
	 *whereas an audio encoder is set with mediacodec_set_aenc_info.
	 *a video decoder and a video encoder are set using mediacodec_set_vdec_info
	 *and mediacodec_set_venc_info respectively
	 */
	if(create(m_mediacodec, codecinfo) == false)//codec info setting
		throw_error_and_destroy_codec(m_mediacodec, "fail to create specific codec", 0)
	if((m_format = create_format(codecinfo)) == NULL)//create format info
		throw_error_and_destroy_codec(m_mediacodec, "fail to create format", 0)

	/*
	 * Essential callback functions are configured as follows.
	 * First, mediacodec_set_eos_cb and mediacodec_set_error_cb are set up in order to take notifications for EOS and errors.
	 * The callback function mediacodec_set_input_buffer_used_cb is called
	 * when all of the input packets have been exhausted.
	 * Once a set of packets have been all used, the packets are destroyed for efficient memory use.
	 * The encoding/decoding results are received through the callback function specified with mediacodec_set_output_buffer_available_cb.
	 * Note that the packet passed through the callback function is not the output packet,
	 * and the output has to be collected using mediacodec_get_output at the point when the callback function is called.
	 */
	int ret = MEDIACODEC_ERROR_NONE;
	if((ret = mediacodec_set_eos_cb(m_mediacodec, mc_eos_cb, (void*)this)) != MEDIACODEC_ERROR_NONE)
		throw_error_and_destroy_codec(m_mediacodec, "fail to mediacodec_set_eos_cb", ret);
	if((ret = mediacodec_set_error_cb(m_mediacodec, mc_error_cb, (void*)this)) != MEDIACODEC_ERROR_NONE)
		throw_error_and_destroy_codec(m_mediacodec, "fail to mediacodec_set_error_cb", ret);
	if((ret = mediacodec_set_input_buffer_used_cb(m_mediacodec, mc_input_buffer_used_cb, (void*)this)) != MEDIACODEC_ERROR_NONE)
		throw_error_and_destroy_codec(m_mediacodec, "fail to mediacodec_set_input_buffer_used_cb", ret);
	if((ret = mediacodec_set_output_buffer_available_cb(m_mediacodec, mc_output_buffer_available_cb, (void*)this)) != MEDIACODEC_ERROR_NONE)
		throw_error_and_destroy_codec(m_mediacodec, "fail to mediacodec_set_output_buffer_available_cb", ret);

	/*
	 * After call prepare, packet encoding/decoding is ready for execution.
	 */
	if((ret = mediacodec_prepare(m_mediacodec)) != MEDIACODEC_ERROR_NONE)
		throw_error_and_destroy_codec(m_mediacodec, "fail to mediacodec_prepare", ret);
	m_out.queue.SetName(getname());
}

void CodecBase::Destroy()
{
	if(m_mediacodec == NULL)
		return;
	dlog_print(DLOG_DEBUG, "CodecBase", "enter into destroy");

	if(m_format)
	{
		media_format_unref(m_format);
		m_format = NULL;
	}

	int ret = mediacodec_unprepare(m_mediacodec);
	if(ret != MEDIAMUXER_ERROR_NONE)
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_unprepare:%d", ret);

	ret = mediacodec_destroy(m_mediacodec);
	if(ret != MEDIAMUXER_ERROR_NONE)
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_destroy:%d", ret);

	m_mediacodec = NULL;
	m_in_count = 0;
	m_out.count = 0;
	m_out.eos = false;
	m_out.queue.ClearAll();
	dlog_print(DLOG_DEBUG, "CodecBase", "exit from destroy");
}

media_format_h CodecBase::GetMediaFormat()
{
	return m_format;
}
bool CodecBase::GetPacket(media_packet_h& packet)
{
	if(m_out.queue.IsEmpty())
		return false;
	packet = m_out.queue.Get();
	return true;
}

/*
 * Packets to encode/decode are pushed in sequence using mediacodec_process_input
 */
bool CodecBase::InsertPacket(media_packet_h packet)
{
	int ret = mediacodec_process_input(m_mediacodec, packet, 1000);
	if(ret != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_process_input %d [%s]", ret, getname());
		return false;
	}
	m_in_count++;
	dlog_print(DLOG_DEBUG, "CodecBase", "input %d th packet [%s]",m_in_count, getname());
	return true;
}

bool CodecBase::IsEoS()
{
	if(m_out.queue.IsEmpty() && m_out.eos)
		return true;
	return false;
}

bool CodecBase::pushpacket_to_outputqueue(const media_packet_h& packet)
{
	packet_created_dbg(packet);
	if(m_out.queue.IsFull())
	{
		dlog_print(DLOG_ERROR, "CodecBase", "unhandled case[%s] queue is full", getname());
		return false;
	}
	m_out.queue.Push(packet);
	++m_out.count;
	dlog_print(DLOG_DEBUG, "CodecBase", "mediacodec[%s]_get_output %d", getname(),m_out.count);
	return true;
}

/*
 * the callback funciton mc_input_buffer_used_cb is called
 * when the input packets are no longer used in the codec.
 * After the callback funciton is called, the existing packets are destroyed
 */
void CodecBase::handle_input_buffer_used(media_packet_h pkt)
{
	media_packet_destroy_dbg(pkt);
}

/*
 * The callback function mc_output_buffer_available_cb is called
 * after the output packets have been generated,
 * which are ready for exploitation.
 * In our sample code, the packets are turned to a file and inserted to an output queue
 */
void CodecBase::handle_output_buffer_available(media_packet_h pkt)
{
	media_packet_h output_buf = NULL;
	int ret = mediacodec_get_output(m_mediacodec, &output_buf, 0);
	if(ret != MEDIACODEC_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "CodecBase", "fail to mediacodec_get_output %d [%s]", ret, getname());
		return;
	}
	pushpacket_to_outputqueue(output_buf);
}

void CodecBase::handle_error(mediacodec_error_e error)
{
	//TODO:: how to handle this error??
	dlog_print(DLOG_ERROR, "CodecBase", "media codec error %d [%s]", (int)error, getname());
}

void CodecBase::handle_eos()
{
	m_out.eos = true;
	m_out.queue.SetBackToEoS();
	dlog_print(DLOG_DEBUG, "CodecBase", "EOS callback [%s]", getname());
}

void CodecBase::mc_input_buffer_used_cb(media_packet_h pkt, void *user_data)
{
	CodecBase* codec = (CodecBase*)user_data;
	codec->handle_input_buffer_used(pkt);
}
void CodecBase::mc_output_buffer_available_cb(media_packet_h pkt, void *user_data)
{
	CodecBase* codec = (CodecBase*)user_data;
	codec->handle_output_buffer_available(pkt);
}
void CodecBase::mc_error_cb(mediacodec_error_e error, void *user_data)
{
	CodecBase* codec = (CodecBase*)user_data;
	codec->handle_error(error);
}
void CodecBase::mc_eos_cb(void *user_data)
{

	CodecBase* codec = (CodecBase*)user_data;
	codec->handle_eos();
}


#if 0
void transform_completed_cb(media_packet_h *dst, int error_code, void *user_data)
{
	dlog_print(DLOG_DEBUG, "CodecBase", "============transform_completed_cb is called[%p] with error:%d", dst, error_code);
	media_packet_h pkt = *dst;
	media_format_h fmt;
	media_packet_get_format(pkt, &fmt);
	media_format_mimetype_e mimetype;
	int width, height, avg_bps, max_bps;
	int ret = media_format_get_video_info(fmt, &mimetype, &width, &height, &avg_bps, &max_bps);
	dlog_print(DLOG_DEBUG, "CodecBase", "video format of output pkt[%d]: %x, %d, %d, %d, %d", ret, (int)mimetype,width, height, avg_bps, max_bps);
	media_format_unref(fmt);

	bool hastbm = true;
	ret = media_packet_has_tbm_surface_buffer(pkt, &hastbm);
	dlog_print(DLOG_DEBUG, "CodecBase", "has tbm[%d]?: %d", ret, (int)hastbm);

	uint32_t numplanes = 0;
	ret = media_packet_get_number_of_video_planes(pkt, &numplanes);
	dlog_print(DLOG_DEBUG, "CodecBase", "num of video planes[%d]?: %d", ret, (int)numplanes);

	for(int i=0; i < numplanes; ++i)
	{
		int stride_width,stride_height;
		ret = media_packet_get_video_stride_width(pkt, i, &stride_width);
		dlog_print(DLOG_DEBUG, "CodecBase", "stride width of %dth video planes[%d]?: %d", i, ret, stride_width);
		ret = media_packet_get_video_stride_height(pkt, i, &stride_height);
		dlog_print(DLOG_DEBUG, "CodecBase", "stride height of %dth video planes[%d]?: %d", i, ret, stride_height);
	}
	dlog_print(DLOG_DEBUG, "CodecBase", "============end of transform_completed_cb");
	media_packet_destroy(pkt);
	bool* bcomplete = (bool*)user_data;
	*bcomplete=1;
}

void CodecBase::analyze_packet_for_debug(media_packet_h pkt)
{
	int ret = 0;


	bool isaudio = true;
	ret = media_packet_is_audio(pkt, &isaudio);
	dlog_print(DLOG_DEBUG, "CodecBase", "is audio[%d]?: %d", ret, (int)isaudio);

	bool isencoded = true;
	ret = media_packet_is_encoded(pkt, &isencoded);
	dlog_print(DLOG_DEBUG, "CodecBase", "is encoded[%d]?: %d", ret, (int)isencoded);

	bool israw = true;
	ret = media_packet_is_raw(pkt, &israw);
	dlog_print(DLOG_DEBUG, "CodecBase", "is raw[%d]?: %d", ret, (int)israw);



	media_format_h fmt;
	media_packet_get_format(pkt, &fmt);
	media_format_mimetype_e mimetype;
	if(isaudio)
	{
		int channel, samplerate, bit, avg_bps;
		ret = media_format_get_audio_info(fmt, &mimetype, &channel, &samplerate, &bit, &avg_bps);
		dlog_print(DLOG_DEBUG, "CodecBase", "audio format of output pkt[%d]: %x, %d, %d, %d, %d", ret, (int)mimetype,channel, samplerate, bit, avg_bps);
		media_format_unref(fmt);
	}
	else
	{
		int width, height, avg_bps, max_bps;
		ret = media_format_get_video_info(fmt, &mimetype, &width, &height, &avg_bps, &max_bps);
		dlog_print(DLOG_DEBUG, "CodecBase", "video format of output pkt[%d]: %x, %d, %d, %d, %d", ret, (int)mimetype,width, height, avg_bps, max_bps);
		media_format_unref(fmt);

		bool hastbm = true;
		ret = media_packet_has_tbm_surface_buffer(pkt, &hastbm);
		dlog_print(DLOG_DEBUG, "CodecBase", "has tbm[%d]?: %d", ret, (int)hastbm);

		uint32_t numplanes = 0;
		ret = media_packet_get_number_of_video_planes(pkt, &numplanes);
		dlog_print(DLOG_DEBUG, "CodecBase", "num of video planes[%d]?: %d", ret, (int)numplanes);

		for(int i=0; i < numplanes; ++i)
		{
			int stride_width,stride_height;
			ret = media_packet_get_video_stride_width(pkt, i, &stride_width);
			dlog_print(DLOG_DEBUG, "CodecBase", "stride width of %dth video planes[%d]?: %d", i, ret, stride_width);
			ret = media_packet_get_video_stride_height(pkt, i, &stride_height);
			dlog_print(DLOG_DEBUG, "CodecBase", "stride height of %dth video planes[%d]?: %d", i, ret, stride_height);
		}

		transformation_h handle;
		ret =image_util_transform_create(&handle);
		ret = image_util_transform_set_resolution(handle, 640, 360);
		bool bcomplete = false;
		ret = image_util_transform_run(handle, pkt, transform_completed_cb, (void*)&bcomplete);
		dlog_print(DLOG_DEBUG, "CodecBase", "image_util_transform_run[%d]", ret);

		while(bcomplete == false)
			sleep(1);

		ret = image_util_transform_destroy(handle);

	}
}
#endif
