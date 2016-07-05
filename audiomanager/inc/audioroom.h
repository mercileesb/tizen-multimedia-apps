/*
 * audioroom.h
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */

#ifndef AUDIOROOM_H_
#define AUDIOROOM_H_

#include <Elementary.h>
#include "sourceinroom.h"
#include "listenerinroom.h"
#include <vector>


#define ROOM_OBJECT_MAX ROOM_SOURCE_MAX+1
#define LISTENER_INDEX 5

typedef struct _ObjectPos
{
	_ObjectPos(int _objidx, const Pos& _pos)
	:objidx(_objidx), pos(_pos)
	{

	}
	int objidx;
	Pos pos;
}ObjectPos;

class AudioRoom
{
public:
	AudioRoom();
	~AudioRoom();

	void Create(Evas_Object* box);
	void Destroy();

	void HideObjects();
	void ShowObjectsIfinRoom();

	void ConfigureSources(const std::vector<RoomSourceProperty>& sources);
	void ToggleSource(int index);
	const char* GetSourceIconPath(int index);
	Pos GetListenerPos();
	void GetPosofSourcesinRoom(std::vector<ObjectPos>& positions);
	EvasCoordRect GetRect();




private:
	SourceinRoom* findsourcebyindex(int index);
	void handledownevent(Evas_Coord x, Evas_Coord y);
	void handleupevent(Evas_Coord x, Evas_Coord y);
	void handlemoveevent(Evas_Coord x, Evas_Coord y);



	static void mousedown_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
	static void mousemove_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
	static void mouseup_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);



private:
	ObjectinRoom* m_objects[ROOM_OBJECT_MAX];
	Evas_Object* m_eventrect;
};


#endif /* AUDIOROOM_H_ */