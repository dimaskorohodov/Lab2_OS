#include <cstdlib>
#include <cmath>
#include <stdio.h>

#define MAXIMUM_MEMORY_SIZE	64
#define LENGTH_OF_THE_LIST	7
#define MAXIMUM_HEADER_INDEX	6
struct BlockOfMemory {
	BlockOfMemory* previus;
	BlockOfMemory* next;
	int power;
	int tag;
};

typedef struct BuddyHead {
	BlockOfMemory* head;
	int sizeOfTheBlock;
}FreeList[LENGTH_OF_THE_LIST];

static BlockOfMemory* freeSpace = NULL;
static BlockOfMemory* userSpace[MAXIMUM_MEMORY_SIZE] = { NULL };
static int userCount = 0;

void initFreeList(FreeList list) {
	for (int i = 0; i < LENGTH_OF_THE_LIST; i++) {
		list[i].head = NULL;
		list[i].sizeOfTheBlock = (int)pow(2.0, i);
	}


	BlockOfMemory* p = (BlockOfMemory*)malloc(sizeof(BlockOfMemory) * MAXIMUM_MEMORY_SIZE);
	if (p != NULL) {
		p->power = MAXIMUM_HEADER_INDEX;
		p->tag = 0;
		p->next = p->previus = p;
	}
	freeSpace = list[MAXIMUM_HEADER_INDEX].head = p;
}

void destroy_list_of_blocks(FreeList list) {
	for (int i = 0; i < LENGTH_OF_THE_LIST; i++) {
		list[i].head = NULL;
	}
	free(freeSpace);
}

BlockOfMemory* allocateBuddy(FreeList list, int size) {
	bool isFirst = true;
	int k = -1;
	int check = -1;

	for (int i = 0; i < LENGTH_OF_THE_LIST; i++) {
		BuddyHead head = list[i];
		if (head.sizeOfTheBlock >= size) {
			if (isFirst) {
				check = i;
				isFirst = false;
			}
			if (head.head != NULL) {
				k = i;
				break;
			}
		}
	}
	BlockOfMemory* freeNode = NULL;
	if (k == -1) {
		return NULL;
	}
	else {

		freeNode = list[k].head;
		freeNode->power = check;
		freeNode->tag = 1;
		list[k].head = freeNode->next;
		if (freeNode->previus == freeNode->next) {
			list[k].head = NULL;
		}
		else {
			freeNode->previus->next = freeNode->next;
			freeNode->next->previus = freeNode->previus;
		}

		for (int i = check; i < k; i++) {
			int index = (int)pow(2.0, i);
			BlockOfMemory* p = freeNode + index;
			p->power = i;
			p->tag = 0;
			BlockOfMemory* head = list[i].head;
			if (head != NULL) {
				p->previus = head->previus;
				p->next = head;
				p->previus->next = head->previus = p;
			}
			else {
				p->previus = p->next = p;
			}
			list[i].head = p;
		}
	}
	userSpace[userCount++] = freeNode;
	return freeNode;
}

void free_user_space(BlockOfMemory* node) {
	for (int i = 0; i < userCount; i++) {
		if (userSpace[i] == node) {
			userSpace[i] = NULL;
		}
	}
}


void regenerate_buddy(FreeList list, BlockOfMemory* node) {
	free_user_space(node);
	while (node->power < MAXIMUM_HEADER_INDEX) {
		int sub = node - freeSpace;
		BlockOfMemory* buddy = NULL;
		int i = (int)pow(2.0, node->power + 1);
		bool isNext = true;
		if (sub % i == 0) {
			buddy = node + i / 2;
		}
		else {
			buddy = node - i / 2;
			isNext = false;
		}
		if (buddy->tag == 0) {

			if (buddy->previus == buddy->next) {
				list[buddy->power].head = NULL;
			}
			else {
				buddy->previus->next = buddy->next;
				buddy->next->previus = buddy->previus;
				list[buddy->power].head = buddy->next;
			}
			if (isNext == false) {
				node = buddy;
			}
			node->power++;
		}
		else {
			break;
		}
	}
	BlockOfMemory* head = list[node->power].head;
	node->tag = 0;
	if (head == NULL) {
		node->previus = node->next = node;
	}
	else {
		node->next = head;
		node->previus = head->previus;
		node->previus->next = head->previus = node;
	}
	list[node->power].head = node;
}

void make_output(FreeList list) {
	printf("------------------------printing list of the buddies-------------------\n");
	for (int i = 0; i < LENGTH_OF_THE_LIST; i++) {
		BlockOfMemory* head = list[i].head;
		if (head) {
      printf("The first address space is： %p ", head, "\n");
      printf("Table precursor： %p ", head->previus, "\n");
      printf("Successor： %p ", head->next, "\n");
      printf("The block size is 2 to the power of：%i ", head->power, "\n");

		}
	}
	printf("------------------------printing user space-------------------\n");
	for (int i = 0; i < userCount; i++) {
		BlockOfMemory* userSpaceBlock = userSpace[i];
		if (userSpaceBlock != NULL) {
      printf("The first address space is: %p, The block size is two in the power %i \n", userSpaceBlock, userSpaceBlock->power);

		//	printf("he first address space is：%p,The block size is 2 to the power of %i\n", us, us->power);
		}
	}
	printf("\n");
}



int main()
{
	FreeList list;
	initFreeList(list);
	make_output(list);
	printf("Let`s allocate one space\n");
	BlockOfMemory* s1_1 = allocateBuddy(list, 1);
	make_output(list);
	printf("Let`s allocate 2 spaces\n");
	BlockOfMemory* s1_2 = allocateBuddy(list, 1);
	make_output(list);
	printf("Let`s allocate 17 spaces\n");
	BlockOfMemory* s29 = allocateBuddy(list, 17);
	make_output(list);
	printf("Let`s release 2 space\n");
	regenerate_buddy(list, s1_2);
	make_output(list);
	printf("Let`s release 1 space\n");
	regenerate_buddy(list, s1_1);
	make_output(list);
	destroy_list_of_blocks(list);
	return 0;
}
