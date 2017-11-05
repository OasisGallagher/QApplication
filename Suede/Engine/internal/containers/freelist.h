#pragma once
#include "tools/debug.h"
#include "tools/mathf.h"
#include "internal/memory/memory.h"

template <class T>
class free_list {
	struct Head {
		struct Block* nextBlock;
	};

	struct Block {
		Head head;
		T value;
	};

	struct Node {
		Block* block;
		Node* nextNode;
	};

	static const int HeadSize = sizeof(Head);

public:
	free_list(size_t capacity) : nodes_(nullptr), freeBlocks_(nullptr), busyBlocks_(nullptr) {
		capacity_ = grow_ = Mathf::NextPowerOfTwo(Mathf::Max(2, capacity));
		Grow(capacity_);
	}

	~free_list() {
		while (nodes_ != nullptr) {
			Node* next = nodes_->nextNode;
			Memory::Release(nodes_->block);
			Memory::Release(nodes_);
			nodes_ = next;
		}
	}

	T* pop() {
		if (freeBlocks_ == nullptr) {
			Grow(grow_ *= 2);
			capacity_ += grow_;
		}

		T* result = (T*)Advance(freeBlocks_, HeadSize);
		freeBlocks_ = freeBlocks_->head.nextBlock;
		return result;
	}

	void push(T* ptr) {
		Block* block = (Block*)Advance(ptr, -HeadSize);
		block->head.nextBlock = freeBlocks_;
		freeBlocks_ = block;
	}

private:
	void Grow(size_t size) {
		AssertX(freeBlocks_ == nullptr, "Grow is not necessary");

		Block* memory = Memory::CreateArray<Block>(grow_);
		for (size_t i = 0; i < grow_; ++i) {
			if (i >= 1)
				memory[i - 1].head.nextBlock = &memory[i];
		}
		memory[grow_ - 1].head.nextBlock = nullptr;

		Node* node = Memory::Create<Node>();
		node->block = memory;
		node->nextNode = nullptr;

		if (nodes_ == nullptr) {
			nodes_ = last_ = node;
		}
		else {
			last_->nextNode = node;
			last_ = node;
		}

		freeBlocks_ = memory;
	}

	template <class T>
	void* Advance(T* ptr, int off) {
		return (char*)ptr + off;
	}

	size_t grow_;

	size_t capacity_;

	Node* last_;
	Node* nodes_;

	Block* freeBlocks_;
};
