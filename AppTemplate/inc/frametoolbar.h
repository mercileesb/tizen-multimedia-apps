/*
 * frametoolbar.h
 *
 *  Created on: May 27, 2016
 *      Author: Jason
 */

#ifndef FRAMETOOLBAR_H_
#define FRAMETOOLBAR_H_

#include <Elementary.h>


class frametoolbar
{
public:
	enum
	{
		FRMAE_TOOLBAR_LEFT_BTN = 0,
		FRMAE_TOOLBAR_CENTER_BTN,
		FRMAE_TOOLBAR_RIGHT_BTN,
		FRMAE_TOOLBAR_MAX,
	};
	frametoolbar();
	~frametoolbar();

	void Create(Evas_Object* framewnd, Evas_Object* conformant);

private:
	void handlebuttonclick(int buttonid);

	static void on_leftbtn_cb(void *data, Evas_Object *obj, void *event_info);
	static void on_centerbtn_cb(void *data, Evas_Object *obj, void *event_info);
	static void on_rightbtn_cb(void *data, Evas_Object *obj, void *event_info);

private:
	Evas_Object* m_toolbar;
	Elm_Object_Item* m_btn[FRMAE_TOOLBAR_MAX];

};




#endif /* FRAMETOOLBAR_H_ */
