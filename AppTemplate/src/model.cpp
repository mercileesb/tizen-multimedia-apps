/*
 * model.cpp

 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "base.h"
#include "model.h"
#include <assert.h>

Model::Model()
:m_created(false)
{

}


Model::~Model()
{
	baseassert(m_created==false);
}

bool Model::Create()
{
//	m_created = creatspecificmodel();
	m_created = true;
	return m_created;
}

void Model::Destroy()
{
//	destroyspecificmodel();
	m_created = false;
}

//serialize
bool Model::Serialize()
{
	return true;
}

bool Model::Load()
{
	return true;
}


