/*****************************************************************************
* @brief : Archetype
* ��ʾECS�е�ԭ��
* @author : acedtang
* @date : 2021/8/19
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_ECS_ARCHETYPE_H
#define __SUN_ECS_ARCHETYPE_H

#include <functional>
#include "Util/Traits/Traits.h"
#include "Chunk.h"
#include "ECSGlobal.h"
#include "Entity.h"

namespace Sun {

	class ECSMgrBase;

	//��ģ����ʽ�洢ArchetypeһЩ��ģ���޹صĳ�Ա�뺯��
	class ArchetypeBase {
	public:
		friend class ECSMgrBase;
		void setECSMgr(ECSMgrBase* base) {
			ecsMgr_ = base;
		}
		SVector<Chunk*>& getChunks() {
			return chunks_;
		}
		Chunk* applyChunk();
		void freeChunk(Chunk* chunk);
		size_t getEntityNum() const {
			return numEntity_;
		}
		size_t getCapacity() const {
			return capacity_;
		}
		bool validation(const EntityAddress& addr) const {
			return true;
		}
		Entity* getEntity(const Entity& entity) {
			auto& addr = entityTable_[entity.index];
			return (Entity*)(chunks_[addr.chunkIdx]->data_ + entityOffset_ + addr.idxInChunk * entityAlignSiz_);
		}
		const Entity* getEntity(const Entity& entity) const {
			const auto& addr = entityTable_[entity.index];
			return (const Entity*)(chunks_[addr.chunkIdx]->data_ + entityOffset_ + addr.idxInChunk * entityAlignSiz_);
		}
		void moveEntity(void* dst, void* src) {
			Entity* ds = (Entity*)(dst);
			Entity* sr = (Entity*)(src);
			*ds = *sr;
			sr->~Entity();
		}
		EntityAddress requestEntry();

		bool exist(const Entity& entity) {
			return entity.index >= 0 && entity.index < entityTable_.size()
				&& entityTable_[entity.index].chunkIdx != static_cast<size_t>(-1) && entity.version ==entityTable_[entity.index].version;
		}
		void recycleEntityEntry(const Entity& entity) {
			assert(exist(entity));

			auto& entry = entityTable_[entity.index];
			entry.chunkIdx = static_cast<std::size_t>(-1);
			entry.idxInChunk = static_cast<std::size_t>(-1);
			++entry.version;

			freeEntries_.push_back(entity.index);
		}
	protected:
		ECSMgrBase* ecsMgr_ = nullptr;
		int numEntity_ = 0;
		//ʼ��ָ����С������δ��chunk����������ڣ���Ϊ-1
		int indexToChunk_ = -1;
		SVector<Chunk*> chunks_;
		//��ԭ��������chunk�������ɵ����object����
		size_t capacity_ = 0;
		//��ԭ���е���objectռ�õ��ֽ���(��align)
		size_t objectSize_ = 0;
		//entity ƫ����ʼ��Ϊ0
		const size_t entityOffset_ = 0;
		//eneity align size
		size_t entityAlignSiz_ = 0;

		SVector<EntityInfo> entityTable_;
		SVector<size_t> freeEntries_;
	};

	//forward declare
	template<typename ... _Args>
	class Archetype;

	struct ObjSizCaculator {
		template<typename _ComType>
		void operator()() {
			obj_siz += TAlignSiz_v<_ComType>;
		}
		size_t obj_siz = 0;
	};

	template<typename ... _Args>
	class Archetype : public ArchetypeBase {
	public:
		using ComTypeList = TUnique_t<TypeList<_Args...>>;
		using _My = Archetype<_Args...>;
		static constexpr size_t ComTypeNum = TLength_v<ComTypeList>;

		static_assert(TIsUnique_v<TypeList<_Args...>> == true ,"component type should be unique!");

		template<typename _ComType>
		using TComOffsetInfo = TDerived<_ComType, size_t>;

		struct SetOffset {
			template<typename _ComType>
			void operator()() {
				constexpr size_t offsetIndex = TFind_v<typename Archetype::ComTypeList, _ComType>;
				offsets[offsetIndex] = offset;
				offset += TAlignSiz_v<_ComType> *capacity;
			}
			size_t offset = 0;
			size_t capacity = 0;
			std::array<size_t, ComTypeNum>& offsets;
		};

		struct ComDestructor {
			template<class _ComType>
			void operator()() {
				constexpr size_t offsetIndex = TFind_v<typename Archetype::ComTypeList, _ComType>;
				void* addr = chunk->data_ + offsets[offsetIndex] + index * TAlignSiz_v<_ComType>;
				destruct<_ComType>(addr);
			}
			Chunk* chunk = nullptr;
			size_t index = 0;
			std::array<size_t, ComTypeNum>& offsets;
		};
		struct ComTypeMovement {
			template<typename _ComType>
			void operator()() {
				constexpr size_t index = TFind_v<Archetype::ComTypeList, _ComType>;
				void* dst = chunk->data_ + offsets[index] + idxInChunk * TAlignSiz_v<_ComType>;
				void* src = srcChunk->data_ + offsets[index] + (srcChunk->numEntity_ - 1) * TAlignSiz_v<_ComType>;
				destruct<_ComType>(dst);
				move_construct<_ComType>(dst, src);
				//copy_construct<_ComType>(dst, src);
				destruct<_ComType>(src);
			}
			Chunk* chunk = nullptr;
			Chunk* srcChunk = nullptr;
			size_t idxInChunk = 0;
			std::array<size_t, ComTypeNum>& offsets;
		};
		struct ComTypeConstructor {
			template<typename _ComType>
			void operator()() {
				constexpr size_t offsetIndex = TFind_v<typename Archetype::ComTypeList, _ComType>;
				construct<_ComType>(chunk->data_ + offsets[offsetIndex] + TAlignSiz_v<_ComType>*index);
			}
			Chunk* chunk = nullptr;
			size_t index = 0;
			std::array<size_t, ComTypeNum>& offsets;
		};

		Archetype();

		~Archetype();

		template<class _T>
		_T& getComponent(const EntityAddress& entity);
		template<class _T>
		_T& getComponent(const Entity& entity);

		template<class _T>
		constexpr bool hasComponent() const;

		EntityAddress createObject(const Entity& entity);

		size_t erase(const Entity& entity);

		void clearAllEntities();

		std::array<size_t, ComTypeNum>& getOffsets() {
			return offsets_;
		}

		SVector<EntityInfo>& getEntityTable() {
			return entityTable_;
		}

		const SVector<EntityInfo>& getEntityTable() const {
			return entityTable_;
		}

		size_t getEntityNum() const {
			size_t siz = entityTable_.size() - freeEntries_.size();
			assert(siz == numEntity_);
			return siz;
		}

		Entity requestFreeEntity() {
			Entity entity;
			if (freeEntries_.empty()) {
				std::size_t index = entityTable_.size();
				entityTable_.emplace_back();
				entity.index = index;
				entity.version = entityTable_[index].version;
				return entity;
			}
			std::size_t entry = freeEntries_.back();
			freeEntries_.pop_back();

			entity.index = entry;
			entity.version = entityTable_[entry].version;

			return entity;
		}

		Entity createEntity() {
			Entity entity = requestFreeEntity();
			EntityAddress res = requestEntry();
			//��chunk�Ĳ�λ�Ϸ������
			assignObject(res, entity);

			return entity;
		}
		//just for test
		bool checkValidate() const {
			bool allok = true;
			Entity en;
			size_t emptySiz = 0;
			for (size_t i = 0; i < entityTable_.size(); ++i) {
				auto& addr = entityTable_[i];
				if (entityTable_[i].chunkIdx != static_cast<size_t>(-1)) {
					en.index = i;
					const Entity* ptr = getEntity(en);
					if (ptr->index != i) {
						allok = false;
						break;
					}
				}
				else {
					emptySiz += 1;
				}
			}
			for (size_t i = 0; i < freeEntries_.size(); ++i) {
				assert(entityTable_[freeEntries_[i]].chunkIdx == static_cast<size_t>(-1));
			}
			assert(emptySiz == freeEntries_.size());
			assert(allok == true);
			return allok;
		}
	protected:
		void assignObject(const EntityAddress& addr,const Entity& entity);
	protected:
		std::array<size_t, ComTypeNum>  offsets_;
	};

	
}


#endif