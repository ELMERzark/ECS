/*****************************************************************************
* @brief :Entity
* ECS�е�ʵ�壬���Կ���һ�������������object�Լ��������Ϣ
* @author : acedtang
* @date : 2021/8/19
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_ECS_ENTITY_H
#define __SUN_ECS_ENTITY_H

#include <cstdint>
#include <cassert>


namespace Sun {

	class Entity {
	public:
		std::uint32_t index = -1;
		std::uint32_t version = -1;
	private:
	};

	template<typename _Archetype>
	class TEntity : public Entity{
	public:
		TEntity() {

		}
		explicit TEntity(const Entity& entity):Entity(entity) {

		}
		using type = _Archetype;
	};

}

//#include "Entity.inl"

#endif