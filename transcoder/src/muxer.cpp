/*
 * muxer.c
 *
 *  Created on: Jul 1, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include "muxer.h"
#include "transcoder.h"
#include <stdexcept>


Muxer::Muxer()
:m_muxer(NULL)
{

}
Muxer::~Muxer()
{

}

void Muxer::Create(const char* dstfilepath, mediamuxer_output_format_e format)
{
	m_dstfilename = dstfilepath;
	AppTool::Iferror_throw(mediamuxer_create(&m_muxer), "fail to mediamuxer_create");
	int ret = MEDIAMUXER_ERROR_NONE;
	if((ret = mediamuxer_set_data_sink(m_muxer, const_cast<char*>(m_dstfilename.c_str()), format))!= MEDIAMUXER_ERROR_NONE)
	{
		mediamuxer_destroy(m_muxer);
		m_muxer = NULL;
		throw std::runtime_error("fail to mediamuxer_set_data_sinkr");
	}
}

void Muxer::Destroy()
{
	AppTool::Iferror_throw(mediamuxer_destroy(m_muxer), "fail to mediamuxer_destroy");
}

int Muxer::AddTrack(media_format_h media_format)
{
	int index = MEDIAMUXER_ERROR_INVALID_PARAMETER;
	AppTool::Iferror_throw(mediamuxer_add_track(m_muxer, media_format, &index), "fail to mediamuxer_add_track");
	return index;
}

void Muxer::CloseTrack(int track_index)
{
	AppTool::Iferror_throw(mediamuxer_close_track(m_muxer, track_index), "fail to mediamuxer_close_track");
}

void Muxer::Start()
{
	AppTool::Iferror_throw(mediamuxer_prepare(m_muxer), "fail to mediamuxer_prepare");
	int ret = MEDIAMUXER_ERROR_NONE;
	if((ret = mediamuxer_start(m_muxer))!= MEDIAMUXER_ERROR_NONE)
	{
		mediamuxer_unprepare(m_muxer);
		m_muxer = NULL;
		throw std::runtime_error("fail to mediamuxer_start");
	}
}
bool Muxer::WriteSample(int track_index, media_packet_h sample)
{
	int ret = MEDIAMUXER_ERROR_NONE;
	if((ret = mediamuxer_write_sample(m_muxer, track_index, sample))!= MEDIAMUXER_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, "Muxer", "fail to mediamuxer_write_sample:%d", ret);
		return false;
	}
	return true;
}
void Muxer::Stop()
{
	int ret = mediamuxer_stop(m_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
		dlog_print(DLOG_ERROR, "Muxer", "fail to mediamuxer_stop:%d", ret);
	ret = mediamuxer_unprepare(m_muxer);
	if(ret != MEDIAMUXER_ERROR_NONE)
		dlog_print(DLOG_ERROR, "Muxer", "fail to mediamuxer_unprepare:%d", ret);
}




