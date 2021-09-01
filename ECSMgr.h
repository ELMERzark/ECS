/*****************************************************************************
* @brief : ECSMgr
* ECS�ܹ��������࣬����������Ϣʵ�壬�����ԭ�ͣ�ϵͳ��
* ecs�ܹ�˵����
* ��Ҫ��ɲ���:
* 1,ECSMgr 2,Archetype 3,Chunk, 4,Entity 
* ECSMgr��������һ��
* ���а������ArchetypeMgr,һ��ArchetypeMgr������ͬԭ�͵�һ��Chunk
* ÿ��ArchetypeMgr��Ӧ���Chunk,Chunk�����洢�������
* ECSMgr�д���һ��EntityTable,�����¼��ÿ��Entity��Ӧ��Archtype*��Chunk������
* ͨ��������������ҵ������������Ϣ��
* ��Entity������Կ���һ���������洢��EntityTable��������
* Entity��������EntityTable�Ľڵ㣬��ͨ���ýڵ������������Ϣ ��2������)
* (���ö���������ԭ����ECS�ܹ���������ܷ����ƶ����Ӷ�����1������ʧЧ)
* 
* ʾ��ͼ:
* EntityTable:[entityInfo0 ,entityInfo1,..........]
* ArchetypeMgr0 ,   ArchetypeMgr1,...
* ArchetypeMgr0 : Chunk0  Chunk1, ... ,Chunk_n...
* Chunk:[                         ] a fix length buffer   component are in chunk
* with a SOA format
* entityInfo save a pointer to  ArchetypeMgr
* @author : acedtang
* @date : 2021/8/19
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_ECS_MGR_H
#define __SUN_ECS_MGR_H

#include <functional>
#include <type_traits>
#include <cstdint>
#include <algorithm>
#include <typeinfo>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Archetype.h"
#include "Entity.h"
#include "ArchetypeIter.h"
#include "TaskPackage.h"
#include "ECSGlobal.h"
#include "Util/Traits/Traits.h"
#include "ECSMemoryPool.h"
#include "ThirdParty/taskflow/taskflow.hpp"

namespace Sun {

	class Entity;

	template<typename ... _Archetyeps>
	class ECSMgr;

	class ECSMgrBase {
	public:
		ECSMgrBase() {
			if (memoryPool_ == nullptr) {
				memoryPool_.reset(new ECSMemoryPool());
			}
		}
		~ECSMgrBase() {
		}
		friend class ArchetypeBase;
	
		ECS_HEAD void runAllTasks();
		ECS_HEAD void addPackage(const TaskPackage& package);//FIXME, should be protected,do not call directly
	protected:
		template <typename C, std::enable_if_t<tf::is_condition_task_v<C>, void>*>
		ECS_HEAD tf::Task emplaceTask(C&& task);
	protected:
		/*ECS_HEAD std::vector<EntityInfo> entityTable_;
		ECS_HEAD std::vector<std::size_t> freeEntries_;*/
		ECS_HEAD std::unique_ptr<ECSMemoryPool> memoryPool_;
		ECS_HEAD tf::Taskflow taskFlow_;
		ECS_HEAD tf::Executor executor_;
		ECS_HEAD std::map<const Chunk*, SVector<TaskPackage>> packages_;
	};

	//��ȡecsmgr������״̬����Ҫ���ڲ���
	template<typename ... _Archetypes>
	struct ESMgrState {
		size_t totalEntityNum = 0;
		size_t totalArchetypeNum = 0;
		std::tuple<_Archetypes...> archetypes;
	};

	template<typename _OriArchetype, typename _TargetArchetype>
	struct ComTypeMove {
		template<typename _ComType>
		void operator()() {
			if (TContain_v <_OriArchetype, _ComType> == false) {
				auto dst = &(dstMgr.getComponent <_ComType>(dstAddr));
				construct<_ComType>(dst);
				return;
			}
			auto src = &(srcMgr.getComponent<_ComType>(srcAddr));
			auto dst = &(dstMgr.getComponent <_ComType>(dstAddr));
			if constexpr (std::is_move_constructible_v<_ComType> == true) {
				move_construct<_ComType>(dst, src);
			}
			else if constexpr (std::is_copy_constructible_v<_ComType> == true) {
				copy_construct<_ComType>(dst, src);
			}
			
		}
		_OriArchetype& srcMgr;
		_TargetArchetype& dstMgr;
		EntityAddress srcAddr;
		EntityAddress dstAddr;
	};

	struct GetTotalEntityNum {
		template<typename _Archetype>
		void operator()(_Archetype& arch) {
			siz_ += arch.getEntityNum();
		}
		size_t siz_ = 0;
	};

	struct ClearAll {
		template<typename _Archetype>
		void operator()(_Archetype& arch) {
			arch.clearAllEntities();
		}
	};

	template<typename _List, typename _Archetype>
	struct ArchetypeFilterBinaryOp {
		static constexpr bool value = TContainList<_Archetype, _List>::value;
	};

	template<typename ... _Archetypes>
	struct Functor {
		//����ģ���һ��ʵ�ּ���
		template<typename ..._MComType>
		struct FunctorInternal {
			template<typename _Archetype>
			void operator()() {
				auto& tuple = mgr_->getArchetypeTuple();
				auto& archetype = std::get<_Archetype>(tuple);

				SVector<Chunk*>& chunks = archetype.getChunks();
				ArchetypeIter<_Archetype> iter1 = mgr_->begin<_Archetype>();
				for (const Chunk* chunk : chunks) {
					size_t numEntity = chunk->numEntity_;
					auto tmpFunc = func_; //TODO how to Optimize?
					std::function<void()> wrapperfunc = [iter1, numEntity, tmpFunc]() {
						auto iter2 = iter1 + numEntity;
						for (auto iter = iter1; iter != iter2; ++iter) {
							tmpFunc(iter.get<std::decay_t<_MComType>>()...);
						}
					};

					TaskPackage package;
					package.chunk = chunk;
					package.func = wrapperfunc;

					mgr_->addPackage(package);

					iter1 += numEntity;
				}
			}
			ECSMgr<_Archetypes...>* mgr_ = nullptr;
			std::function<void(_MComType...)> func_;
		};
	};

	template<typename ... _Archetypes>
	class ECSMgr : public ECSMgrBase
	{
	public:
		static_assert(TIsUnique_v<TypeList<_Archetypes...>,TEqualAsSet> == true);
		using ArchetypeTuple = std::tuple<_Archetypes...>;

		ECSMgr() {
			for_each_tuple([this](ArchetypeBase& base) {
				base.setECSMgr(this);
				}, archeTypeTuple_);
		}
		~ECSMgr() {
		}

		template<typename _Archetype>
		ECS_HEAD bool exist(const TEntity<_Archetype>& e) ECS_TAIL {
			const _Archetype& ref = getArchetype<_Archetype>();
			const auto& table = ref.getEntityTable();
			return e.index >= 0 && e.index < table.size() && e.version == table[e.index].version;
		}

		template<typename _Archetype>
		ECS_HEAD _Archetype& getArchetype() {
			static_assert(TContain_v< ArchetypeTuple, _Archetype> == true);
			return std::get<_Archetype>(archeTypeTuple_);
		}

		template<typename _Archetype>
		ECS_HEAD const _Archetype& getArchetype() const {
			static_assert(TContain_v< ArchetypeTuple, _Archetype> == true);
			return std::get<_Archetype>(archeTypeTuple_);
		}

		template<typename _Archetype>
		ECS_HEAD Entity requestFreeEntity();

		//ͨ��ԭ�ʹ���Entity
		template<typename _Archetype>
		ECS_HEAD TEntity<_Archetype> createEntity();

		//��entityΪԭ�ʹ���entity
		template<typename _TargetArchetype,typename _Archetype>
		ECS_HEAD TEntity<_TargetArchetype> createEntity(const TEntity<_Archetype>& en);

		//��һ��entityתΪΪ��һ�ֵķ���������ͨ�����ַ�ʽ����ӣ�ɾ�������ԭentity��ʧЧ
		template<typename _TargetArchetype,typename _Archetype>
		ECS_HEAD TEntity<_TargetArchetype> createEntity(TEntity<_Archetype>&& en);

		//����entity
		template<typename _Archetype>
		ECS_HEAD void eraseEntity(const TEntity<_Archetype>& entity);

		ECS_HEAD void clearAllEntities();

		template<typename _ComType,typename _Archetype>
		ECS_HEAD _ComType& getComponent(const TEntity<_Archetype>& entity);

		//the same as getComponent
		template<typename _ComType, typename _Archetype>
		ECS_HEAD _ComType& get(const TEntity<_Archetype>& entity);

		template<typename _ComType,typename _Archetype>
		ECS_HEAD bool hasComponent(const _ComType& cmpt, const TEntity<_Archetype>& entity) ECS_TAIL;

		template<typename _Archetype>
		ECS_HEAD size_t getArchetypeEntityNum() ECS_TAIL;

		template<typename _Archetype>
		ECS_HEAD ArchetypeIter<_Archetype> begin();

		template<typename _Archetype>
		ECS_HEAD ArchetypeIter<_Archetype> end();

		ECS_HEAD size_t getTotalEntityNum() ECS_TAIL;

		template<typename _Archetype>
		ECS_HEAD constexpr bool hasArchetype() ECS_TAIL;

		ECS_HEAD constexpr size_t getArchetypeNum() ECS_TAIL;

		template<typename ... _MComType>
		ECS_HEAD void entity_for_each(std::function<void(_MComType...)> func);
		//void entity_for_each(void(_ComType*...) func);

		//SVector<ArchetypeMgr*> filterArchetype(const ArchetypeFilter& filter) const;

		ECS_HEAD ArchetypeTuple& getArchetypeTuple();

		//for test
		ECS_HEAD bool checkValidate() const;
	protected:
		ECS_HEAD ArchetypeTuple archeTypeTuple_;
	};

}

#include "ECSMgr.inl"
#include "Entity.inl"
#include "Archetype.inl"

#endif