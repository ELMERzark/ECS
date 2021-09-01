/*****************************************************************************
* @brief : Chunk
* ��ArchetypeMgr����������һ���ڴ�����
* @author : acedtang
* @date : 2021/8/19
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_ECS_CHUNK_H
#define __SUN_ECS_CHUNK_H

#include <cstdint>

namespace Sun {

#define SUN_CHUNK_SIZE 16384

	struct Chunk {
		//��Chunk����ʵ��object����
		size_t numEntity_ = 0;
		//�洢object����
		uint8_t data_[SUN_CHUNK_SIZE];
	};
}

#endif