#include "Archetype.h"

template<typename ... _ComType>
Sun::Archetype<_ComType...>::Archetype() {
	ObjSizCaculator func;
	for_each_typelist<ComTypeList>(func);
	objectSize_ = func.obj_siz;
	//��entity��sizeҲ���ǽ�ȥ
	entityAlignSiz_ = TAlignSiz_v<Entity>;
	objectSize_ += entityAlignSiz_;
	if (objectSize_ > 0)
		capacity_ = SUN_CHUNK_SIZE / objectSize_;

	//����ƫ����
	size_t offset = entityAlignSiz_ * capacity_;

	SetOffset funcSet{ offset,capacity_,offsets_ };
	for_each_typelist< ComTypeList>(funcSet);
}

template<typename ... _Args>
Sun::Archetype<_Args...>::~Archetype() {
	ComDestructor func{ nullptr,0,offsets_ };
	for (auto& it : chunks_) {
		func.chunk = it;
		for (size_t i = 0; i < it->numEntity_; ++i) {
			func.index = i;
			for_each_typelist<ComTypeList>(func);
		}
	}
}

template<typename ... _Args>
template<class _ComType>
_ComType& Sun::Archetype<_Args...>::getComponent(const Sun::EntityAddress& addr) {
	Chunk* chunk = chunks_[addr.chunkIdx];
	constexpr size_t offsetIndex = TIndex_v<ComTypeList, _ComType, 0>;
	return *(_ComType*)(chunk->data_ + offsets_[offsetIndex] + addr.idxInChunk * TAlignSiz_v<_ComType>);
}

template<typename ... _Args>
template<class _ComType>
_ComType& Sun::Archetype<_Args...>::getComponent(const Sun::Entity& entity) {
	auto& addr = entityTable_[entity.index];
	Chunk* chunk = chunks_[addr.chunkIdx];
	constexpr size_t offsetIndex = TIndex_v<ComTypeList, _ComType,0>;
	return *(_ComType*)(chunk->data_ + offsets_[offsetIndex] + addr.idxInChunk * TAlignSiz_v<_ComType>);
}

template<typename ..._Args>
template<class _ComType>
constexpr bool Sun::Archetype<_Args...>::hasComponent() const {
	return TContain<_ComType, _Args...>::value;
}

template<typename ..._Args>
size_t Sun::Archetype<_Args...>::erase(const Entity& entity) {
	//��ʾ���ƶ���entity����
	size_t res = static_cast<size_t>(-1);

	EntityAddress addr = entityTable_[entity.index];

	Chunk* chunk = chunks_[addr.chunkIdx];
	//��chunk�ϵ�����������
	ComDestructor func{ chunk,addr.idxInChunk,offsets_ };
	for_each_typelist<ComTypeList>(func);
	//���ƶ���chunk
	Chunk* srcChunk = chunks_.back();
	//˵����ɾ����entity���� ���һ��chunk�����һ��entity ,����Ҫ�����ƶ�����
	if (addr.chunkIdx != chunks_.size() - 1 || addr.idxInChunk < chunk->numEntity_ - 1) {
		
		ComTypeMovement funcMove{ chunk,srcChunk,addr.idxInChunk,offsets_ };
		for_each_typelist<ComTypeList>(funcMove);
		//entity�ƶ�
		void* dst = chunk->data_ + entityOffset_ + addr.idxInChunk * entityAlignSiz_;
		void* src = srcChunk->data_ + entityOffset_ + (srcChunk->numEntity_ - 1) * entityAlignSiz_;
		moveEntity(dst, src);
		//��������
		Entity* dstEntity = (Entity*)(dst);
		auto& info = entityTable_[dstEntity->index];
		info.chunkIdx = addr.chunkIdx;
		info.idxInChunk = addr.idxInChunk;

		//��ֵ���ƶ���entity������
		res = dstEntity->index;
	}
	else {
	}
	--srcChunk->numEntity_;
	if (srcChunk->numEntity_ == 0) {
		freeChunk(srcChunk);
		chunks_.pop_back();
	}

	//ԭ�͵�entity����ȥ1
	--numEntity_;

	if (res != -1) {
		entityTable_[res].chunkIdx = addr.chunkIdx;
		entityTable_[res].idxInChunk = addr.idxInChunk;
	}

	recycleEntityEntry(entity);

	//���ر��ƶ���entity���������û�б��ƶ��򷵻�-1
	return res;

}

template<typename ..._Args>
void Sun::Archetype<_Args...>::clearAllEntities() {
	ComDestructor func{nullptr,0,offsets_ };
	for (Chunk* chunk : chunks_) {
		for (size_t idx = 0; idx < chunk->numEntity_; ++idx) {
			func.chunk = chunk; 
			func.index = idx;
			for_each_typelist<ComTypeList>(func);
		}
	}
	for (Chunk* chunk : chunks_) {
		freeChunk(chunk);
	}
	chunks_.clear();
	entityTable_.clear();
	freeEntries_.clear();
	numEntity_ = 0;
}

template<typename ..._Args>
void Sun::Archetype<_Args...>::assignObject(const EntityAddress& addr,const Entity& entity) {
	Chunk* chunk = chunks_[addr.chunkIdx];
	//�������
	ComTypeConstructor func{ chunk,addr.idxInChunk,offsets_ };
	for_each_typelist<ComTypeList>(func);
	//Entity����
	Entity* en = (Entity*)(chunk->data_ + entityOffset_ + addr.idxInChunk * entityAlignSiz_);
	*en = entity;
	//����ӳ��
	EntityInfo& info = entityTable_[entity.index];
	info.chunkIdx = addr.chunkIdx;
	info.idxInChunk = addr.idxInChunk;
}