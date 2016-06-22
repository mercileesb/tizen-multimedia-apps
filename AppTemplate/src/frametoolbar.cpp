/*
 * frametoolbar.cpp
 *
 *  Created on: May 27, 2016
 *      Author: Jason
 */

#include "frametoolbar.h"
#include <stdexcept>

frametoolbar::frametoolbar()
	:m_toolbar(NULL)
{

}
frametoolbar::~frametoolbar()
{

}

void frametoolbar::Create(Evas_Object* framewnd, Evas_Object* conformant)
{
	Evas_Object* toolbar = elm_toolbar_add(framewnd);
	if(toolbar == NULL)
	{
		throw std::runtime_error("fail to create toolbar in framewnd");
	}
	elm_box_pack_end(framewnd, toolbar);
	evas_object_show(toolbar);
	elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_NONE);
	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);
	elm_object_item_part_content_set(framewnd, "toolbar", toolbar);

	m_btn[FRMAE_TOOLBAR_LEFT_BTN] = elm_toolbar_item_append(toolbar, NULL, "Left", on_leftbtn_cb, this);
	m_btn[FRMAE_TOOLBAR_CENTER_BTN] = elm_toolbar_item_append(toolbar, NULL, "Center", on_centerbtn_cb, this);
	m_btn[FRMAE_TOOLBAR_RIGHT_BTN] = elm_toolbar_item_append(toolbar, NULL, "Right", on_rightbtn_cb, this);
	m_toolbar = toolbar;
}

void frametoolbar::handlebuttonclick(int buttonid)
{
	switch(buttonid)
	{
		case FRMAE_TOOLBAR_LEFT_BTN:
			break;
		case FRMAE_TOOLBAR_CENTER_BTN:
			break;
		case FRMAE_TOOLBAR_RIGHT_BTN:
			break;
	}
}

void frametoolbar::on_leftbtn_cb(void *data, Evas_Object *obj, void *event_info)
{
	frametoolbar* ptoolbar = (frametoolbar*)data;
	ptoolbar->handlebuttonclick(FRMAE_TOOLBAR_LEFT_BTN);
}
void frametoolbar::on_centerbtn_cb(void *data, Evas_Object *obj, void *event_info)
{
	frametoolbar* ptoolbar = (frametoolbar*)data;
	ptoolbar->handlebuttonclick(FRMAE_TOOLBAR_CENTER_BTN);
}
void frametoolbar::on_rightbtn_cb(void *data, Evas_Object *obj, void *event_info)
{
	frametoolbar* ptoolbar = (frametoolbar*)data;
	ptoolbar->handlebuttonclick(FRMAE_TOOLBAR_RIGHT_BTN);
}

