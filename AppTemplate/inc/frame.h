/*
 * frame.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <Elementary.h>
class FrameWindow
{
public:
	FrameWindow();
	~FrameWindow();

	bool CreateBaseFrame();
	void Show();


private:
	Evas_Object* createframewindow();
	Evas_Object* createconformation(Evas_Object* parentwnd);
	void handledeleterequest(Evas_Object *obj, void *event_info);

	//for callback
	static void delete_request_cb(void *data, Evas_Object *obj, void *event_info);



private:
	Evas_Object* m_framewnd;
	Evas_Object* m_conformant;


};


#endif /* FRAME_H_ */
