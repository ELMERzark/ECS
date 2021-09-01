#include "Archetype.h"
#include "ECSMgr.h"

namespace Sun {

	Chunk* ArchetypeBase::applyChunk() {
		return ecsMgr_->memoryPool_->mallocChunk();
	}

	void ArchetypeBase::freeChunk(Chunk* chunk) {
		return ecsMgr_->memoryPool_->freeChunk(chunk);
	}

	EntityAddress ArchetypeBase::requestEntry() {
		EntityAddress res;
		Chunk* chunk = nullptr;
		//ֻ�п������һ��chunk����������䲻�����������
		if (chunks_.size() > 0 && chunks_.back()->numEntity_ < capacity_) {
			chunk = chunks_.back();
			res.chunkIdx = chunks_.size() - 1;
		}
		//����chunk���Ѿ�����
		else {
			chunk = applyChunk();
			chunks_.emplace_back(chunk);
			res.chunkIdx = chunks_.size() - 1;
		}
		res.idxInChunk = chunk->numEntity_;
		chunk->numEntity_ += 1;

		numEntity_ += 1;

		return res;
	}
}