/*
 * audioencoder.h
 *
 *  Created on: Jul 2, 2016
 *      Author: Jason
 */

#ifndef AUDIOENCODER_H_
#define AUDIOENCODER_H_


#include "codecbase.h"

class AudioEncoder : public CodecBase
{
public:
	AudioEncoder();
	~AudioEncoder();
	media_format_h GetMediaFormat();

protected:
	bool create(mediacodec_h mediacodec, const CodecInfo& codecinfo);
	void destroy();
	const char* getname();

private:
	bool create_format(const CodecInfo& codecinfo);
	media_format_h m_format;



};


#endif /* AUDIOENCODER_H_ */
