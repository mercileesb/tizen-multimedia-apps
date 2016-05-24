/*
 * view.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <Elementary.h>

class View
{
public:
	View();
	~View();

	bool CreateView(Evas_Object* framewnd, Evas_Object* conformant);

protected:
//	virtual bool decorateview(Evas_Object* layout) = 0;

private:
	Evas_Object* createlayout(Evas_Object* framewnd, Evas_Object* conformant);
	void getresource(const char *edj_file_in, char *edj_path_out, int edj_path_max);

	void handlelayoutevent(Evas_Object *obj, void *event_info);
	//for callback
	static void layout_back_cb(void *data, Evas_Object *obj, void *event_info);

private:
	Evas_Object* m_layout;
	Evas_Object* m_framewnd;
	Evas_Object* m_conformant;
};


#endif /* VIEW_H_ */
