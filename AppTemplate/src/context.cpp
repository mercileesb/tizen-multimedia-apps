/*
 * context.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Hotak
 */
#include "context.h"
#include <dlog.h>

Context::Context()
{
	const ALCchar * m_defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	m_device = alcOpenDevice(m_defaultDeviceName);
	m_contextID = alcCreateContext(m_device, NULL);
	alcMakeContextCurrent(m_contextID);

}

Context::~Context()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_contextID);
	alcCloseDevice(m_device);
}

void Context::convertVecToArr(ALuint* arr)
{
	for(int i = 0 ; i < m_ImportSourceIdx.size() ; i++)
	{
		arr[i] = m_ImportSourceIdx[i]->GetSourceId();
	}
}

void Context::ResetSource()
{
	m_ImportSourceIdx.clear();
}

void Context::setSourcePos(ALuint source, int x, int y, int z)
{
	alSource3i(source, AL_POSITION, x, y, z);
	dlog_print(DLOG_DEBUG, "ALContext", "source:%d, (%d, %d, %d)", source, x, y, z);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "ALContext", "alSource3i error:%d", ret);
	}
}

void Context::setListenerPos(int x, int y, int z)
{
	alListener3i(AL_POSITION, x, y, z);
	dlog_print(DLOG_DEBUG, "ALContext", "listener:(%d, %d, %d)", x, y, z);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "ALContext", "alListener3i error:%d", ret);
	}
}

void Context::Play()
{
	ALuint source[MAXNUM];
	convertVecToArr(source);
	alSourcePlayv(m_ImportSourceIdx.size(), source);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "ALContext", "alSourcePlayv error:%d", ret);
	}
}

void Context::Stop()
{
	ALuint source[MAXNUM];
	convertVecToArr(source);
	alSourceStopv(m_ImportSourceIdx.size(), source);
	ALenum ret = alGetError();
	if (ret != AL_NO_ERROR)
	{
		dlog_print(DLOG_FATAL, "ALContext", "alSourceStopv error:%d", ret);
	}
}

void Context::Push(Source* source)
{
	m_ImportSourceIdx.push_back(source);
}

void Context::Pop(Source* source)
{
	for(int i = 0 ; i < m_ImportSourceIdx.size() ; i++)
	{
		if(m_ImportSourceIdx[i] == source)
		{
			m_ImportSourceIdx.erase(m_ImportSourceIdx.begin() + i);
			break;
		}
	}
}
