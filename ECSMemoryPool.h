/*****************************************************************************
* @brief :ECSMemoryPool
* ECSר���ڴ�أ���������ECS�ڴ����
* @author : acedtang
* @date : 2021/8/19
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_ECS_MEMORY_POOL_H
#define __SUN_ECS_MEMORY_POOL_H

namespace Sun {

	struct Chunk;

	class ECSMemoryPool {
	public:
		Chunk* mallocChunk();
		void freeChunk(Chunk* chunk);
	};
}


#endif