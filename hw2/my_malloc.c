/* 
Clarence Jiang, yjia279
THIS  CODE  WAS MY OWN WORK , IT WAS  WRITTEN  WITHOUT  CONSULTING  ANYSOURCES  OUTSIDE  OF  THOSE  APPROVED  BY THE  INSTRUCTOR. 
Clarence Jiang  */

//
//  my_malloc.c
//  Lab1: Malloc
//
//

#include "my_malloc.h"
#include <unistd.h>

MyErrorNo my_errno=MYNOERROR;
FreeListNode free_list = NULL;
uint32_t magic_number = 2081101;

void node_insertion(FreeListNode node){
    if (free_list == NULL){
        //If the free list itself is empty, just simply assign the pointer freed to it 
        free_list = node;  
        free_list->flink = NULL;
        return;
    }

    if (node < free_list){
        FreeListNode temp = free_list;
        free_list = node;
        node ->flink = temp;
        return;
    }
    else{
        FreeListNode temp_pointer = free_list;
        while (temp_pointer->flink != NULL){
            if (temp_pointer->flink < node){
                temp_pointer = temp_pointer->flink;
            }
            else
            {
                //This is basically inserting the node in the memory place where it is greater than node
                node->flink = temp_pointer->flink;
                temp_pointer->flink = node; 
                return;
            }
        }
        // If the flink eventually reaches the very end, it means 
        // the address is greater than the current end of the 
        // free list, just attach it at the very end
        temp_pointer->flink = node;
        node->flink = NULL;
        return;
    }
}

void node_removal(FreeListNode node){
    FreeListNode pointer = free_list;

    if(pointer == node){
        free_list = pointer->flink;
        return;
    }
    else{
        //We have to iterate two pointers together, then we can know both 
        //the node in front of us and the node we connect to
        FreeListNode pointer2 = pointer->flink;
        while (pointer2!= NULL){
            if (pointer2 == node){
                pointer->flink = pointer2->flink;
                return;
            }
            else{
                pointer = pointer->flink;
                pointer2 = pointer2->flink;

            }   
        }
        return;
    }
}

void* deal_with_oversize(void * heap_head, uint32_t chunk_size){
    
    if (sbrk(chunk_size) == (void *) -1){
        my_errno = MYENOMEM;
        return NULL;
    }
    uint32_t * pointer_head = (uint32_t *) heap_head;
    *pointer_head++ = chunk_size;
    *pointer_head = magic_number;

    return heap_head;
    
}

void *my_malloc(uint32_t size){
    if (size == 0){
        my_errno=MYNOERROR;
        return NULL;
    }

    void * heap_head = NULL;
    uint32_t chunk_size = 0;

    //First regardless of which situation falls in, it never harms to 
    //calculate the chunk size + padding 
    if (size % 8 == 0){
            chunk_size = CHUNKHEADERSIZE + size;
        }
    else{
            chunk_size = CHUNKHEADERSIZE + size + (8-(size % 8));
        }

    // Then the first case is when we have a free list to work with
    // If there is free node memory that can be used within the free list
    if (free_list != NULL){
        FreeListNode pointer = free_list;
        //Keep iterating 
        while(pointer != NULL){
            if (pointer->size >= chunk_size){
                void * pointer_copy = pointer;
                uint32_t node_size = pointer->size;
                //If there is space left, do splitting
                if (node_size-chunk_size >= 16) {
                    void * remaining_space_head = pointer_copy + chunk_size;
                    FreeListNode remaining_node = (FreeListNode) remaining_space_head;
                    remaining_node->flink = NULL;
                    remaining_node->size = node_size - chunk_size;
                    node_removal(pointer);
                    node_insertion(remaining_node);
                }
                //The node_size exactly equals to the chunk size, or the splitting
                //remaing space is less than 16, the only case is 8, because we want 8-aligment. 
                else{
                    if (node_size - chunk_size == 8){
                        node_removal(pointer);
                        chunk_size += 8;
                    }
                    else{
                        node_removal(pointer);
                    }
                    
                }
                uint32_t * pointer_head = (uint32_t *) pointer;
                *pointer_head++ = chunk_size;
                *pointer_head = magic_number;
                my_errno=MYNOERROR;

                return pointer_copy+CHUNKHEADERSIZE;

            }
            else{
                pointer = pointer->flink;
            }

        }
    }    

    //The second case is when there is no free list, which means
    // we have to call sbrk for more spaces. This should be what will happen
    // when we have no free_list (initizalation step) and also when there
    // is no proper node for memory allocation. 
    // Also a subcase of oversize is also considered. 
    
    // Get the top of the heap 
    heap_head = sbrk(0); 

        //Test if the normal 8192 bytes is not enough
    if (chunk_size > 8192){
        heap_head = deal_with_oversize(heap_head, chunk_size);
        my_errno = MYNOERROR;
        return heap_head+CHUNKHEADERSIZE;
    }
    
    // Test if the 8k bytes got exceed all the memory the system can give
    if (sbrk(8192) == (void *) -1) {
        my_errno = MYENOMEM;
        return NULL;
    }
    
    uint32_t * pointer_head = (uint32_t *) heap_head;
    *pointer_head++ = chunk_size;
    *pointer_head = magic_number;
    
    if (8192-chunk_size >= 16){
        void * free_node_head = (char *)heap_head + chunk_size;
        FreeListNode new_node = (FreeListNode) free_node_head;
        new_node -> flink = NULL;
        new_node -> size =  8192 - chunk_size;
        node_insertion(new_node);
    }
    my_errno=MYNOERROR;
    return heap_head+CHUNKHEADERSIZE;
    
}
      
void my_free(void *ptr){
    if (ptr == free_list_begin() || ptr> sbrk(0)){
        my_errno = MYBADFREEPTR;
        return;
    }

     if (ptr == NULL) {
        my_errno = MYBADFREEPTR;
        return;
    }
    //Suppose ptr is acting as the return of malloc, it should be the address under chunk header
    ptr-=CHUNKHEADERSIZE; 
    //Get the size of memory space 
    uint32_t * pointer_size = (uint32_t *) ptr; 
    if (*++pointer_size != magic_number) {
        my_errno = MYBADFREEPTR;
        return;
    }
    //Define a new free list node, since this previously allocated space is 
    uint32_t sizeGet = *(uint32_t *)ptr;
    FreeListNode node = (FreeListNode) ptr;
    node -> size = sizeGet;
    node->flink = NULL;
    node_insertion(node);
    my_errno = MYNOERROR;
}

FreeListNode free_list_begin(){
    return free_list;
}

void coalesce_free_list(){
    FreeListNode node = free_list;
    while (node != NULL) {
        uint32_t node_size = node->size;
        if ((void *)node + node_size == node->flink) {
            uint32_t next_node_size = node->flink->size;
            node->size += next_node_size;
            node->flink = node->flink->flink;
        } else {
            node = node->flink;
        }
    }
}
