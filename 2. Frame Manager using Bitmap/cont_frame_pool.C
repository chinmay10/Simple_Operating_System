/*
 File: ContFramePool.C
 
 Author:Chinmay Ajit Sawkar		
 Date  :2022-09-16
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

#define MB * (0x1 << 20)
#define KB * (0x1 << 10)

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

ContFramePool* ContFramePool::frame_pool_head;
ContFramePool* ContFramePool::frame_pool_list;

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    //Console::puts("ContframePool::Constructor not implemented!\n");
    // assert(false);
    
    base_frame_no = _base_frame_no;
    nframes = _n_frames;
    nFreeFrames = _n_frames;
    info_frame_no = _info_frame_no;
    
    if(info_frame_no == 0) 
    {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE); 
    } 
    else 
    {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }
    
    assert ((nframes % 8 ) == 0);
    
    int i=0;
    while(i*8<_n_frames*2){
    	bitmap[i] = 0x0;
    	i++;
    }
    

    if(_info_frame_no == 0) {
        bitmap[0] = 0x40;
        nFreeFrames--;
    }

    if (ContFramePool::frame_pool_head == NULL) {
        ContFramePool::frame_pool_head = this;
        ContFramePool::frame_pool_list = this;
    } else {
        ContFramePool::frame_pool_list->frame_pool_next = this;
        ContFramePool::frame_pool_list = this;
    }
    frame_pool_next = NULL;
 
    Console::puts("ContframePool::Constructor is implemented.\n");
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::get_frames not implemented!\n");
    // assert(false);
    
    unsigned int frames_req = _n_frames;
    unsigned int frame_no = base_frame_no;
    int search = 0;
    int found = 0;
    int idx_p = 0;
    int idx_q = 0;

    if(_n_frames > nFreeFrames) {
        Console::puts("Not enough frames available");

    }



    for (unsigned int i = 0; i<nframes/4; i++) {
        unsigned char value = bitmap[i];
        unsigned char mask = 0xC0;
        for (int j = 0; j < 4; j++) {
            if((bitmap[i] & mask) == 0) {
                if(search == 1) {
                    frames_req--;
                } else {
                    
                    search = 1;
                    frame_no += i*4 + j;
                    idx_p = i;
                    idx_q = j;
                    frames_req--;
                }
            } else {
                if(search == 1) {
                    
                    frame_no = base_frame_no;
                    frames_req = _n_frames;
                    idx_p = 0;
                    idx_q = 0;
                    search = 0;
                }
                
            }
            mask = mask>>2;
            if (frames_req == 0) {
                
                found = 1;
                break;
            }
        } 
        if (frames_req == 0) {
            found = 1;
            break;
        }
    }  

    
    if (found == 0 ) {
        Console::puts("No free frame found for length: ");Console::puti(_n_frames);Console::puts("\n");
        return 0;
    }

    // setting allocated frames with head pointer
    int Tot_frame = _n_frames;
    unsigned char head_mask = 0x40;
    unsigned char mask2 = 0xC0;
    head_mask = head_mask>>(idx_q*2);
    mask2 = mask2>>(idx_q*2);
    
    bitmap[idx_p] = (bitmap[idx_p] & ~mask2)| head_mask; 
    
    idx_q++;
    Tot_frame--;

    unsigned char mask1 = 0xC0;
    mask1 = mask1>>(idx_q*2);
    while(Tot_frame > 0 && idx_q < 4) {
        bitmap[idx_p] = bitmap[idx_p] | mask1;
        mask1 = mask1>>2;
        Tot_frame--;
        idx_q++;
    }
    
    for(int i = idx_p + 1; i< nframes/4; i++) {
        mask1 = 0xC0;
        for (int j = 0; j< 4 ; j++) {
            if (Tot_frame == 0) {
                break;
            }
            bitmap[i] = bitmap[i] | mask1;

            mask1 = mask1>>2;
            Tot_frame--;
        }
        if (Tot_frame ==0){
            break;
        }
    }



    if (search == 1) {

        nFreeFrames -= _n_frames;
        return frame_no;
    } else {
        Console::puts("No free frame found for length: ");Console::puti(_n_frames);Console::puts("\n");
        return 0;
    }
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::mark_inaccessible not implemented!\n");
    // assert(false);
    
    
    if (_base_frame_no < base_frame_no || base_frame_no + nframes < _base_frame_no + _n_frames) {
        Console::puts("range out of index, cannot mark inaccessible \n");
    } else {
        
        nFreeFrames -= _n_frames;
        int bf_no_dif = (_base_frame_no - base_frame_no)*2;
        int idx_p = bf_no_dif / 8;
        int idx_q = (bf_no_dif % 8) /2;

         

        int Tot_frame = _n_frames;
        unsigned char mask1 = 0x80;
        unsigned char mask2 = 0xC0;
        
        mask1 = mask1>>(idx_q*2);
        mask2 = mask2>>(idx_q*2);
        while(Tot_frame > 0 && idx_q < 4) {
            bitmap[idx_p] = (bitmap[idx_p] & ~mask2) | mask1; 
            mask1 = mask1>>2;
            mask2 = mask2>>2;
            Tot_frame--;
            idx_q++;
        }

        for(int i = idx_p + 1; i< idx_p + _n_frames/4; i++) {
            mask1 = 0xC0;
            mask2 = 0xC0;
            for (int j = 0; j< 4 ; j++) {
                if (Tot_frame == 0) {
                    break;
                }
                bitmap[i] = (bitmap[i] & ~mask2)| mask1;
                mask1 = mask1>>2;
                mask2 = mask2>>2;
                Tot_frame--;
            }
            if (Tot_frame ==0){
                break;
            }
        }
        
    }

}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    //Console::puts("ContframePool::release_frames not implemented!\n");
    //assert(false);
    
    
    
    ContFramePool* current_pool = ContFramePool::frame_pool_head;
    while ( (current_pool->base_frame_no > _first_frame_no || current_pool->base_frame_no + current_pool->nframes <= _first_frame_no) ) {
        if (current_pool->frame_pool_next == NULL) {
            Console::puts("Cannot not find the frame!! \n");
            return;
        } else {
            current_pool = current_pool->frame_pool_next;
        }
    }


    unsigned char* bitmap_ptr = current_pool->bitmap;
    int bf_no_dif = (_first_frame_no - current_pool->base_frame_no)*2;
    int idx_p = bf_no_dif / 8;
    int idx_q = (bf_no_dif % 8) /2;

    unsigned char head_mask = 0x80;
    unsigned char mask1 = 0xC0;
    head_mask = head_mask>>idx_q*2;
    mask1 = mask1>>idx_q*2;
    if (((bitmap_ptr[idx_p]^head_mask)&mask1 ) == mask1) {
        
        bitmap_ptr[idx_p] = bitmap_ptr[idx_p] & (~mask1); 
        idx_q++;
        mask1 = mask1>>2;
        current_pool->nFreeFrames++;

        while (idx_q < 4) {
            if ((bitmap_ptr[idx_p] & mask1) == mask1) {
                bitmap_ptr[idx_p] = bitmap_ptr[idx_p] & (~mask1);
                idx_q++;
                mask1 = mask1>>2;
                current_pool->nFreeFrames++;
            } else {

                return; 
            }
        } 

        for(int i = idx_p+1; i < (current_pool->base_frame_no + current_pool->nframes)/4; i++ ) {
            mask1 = 0xC0;
            for (int j = 0; j < 4 ;j++) {
                if ((bitmap_ptr[i] & mask1) == mask1) {
                    bitmap_ptr[i] = bitmap_ptr[i] & (~mask1);
                    mask1 = mask1>>2;
                    current_pool->nFreeFrames++;
                } else {
                   

                    return; 
                }
            }
        } 


    } else {
        Console::puts("Cannot release as frame is not the head in the sequence \n");
    }
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    //Console::puts("ContframePool::need_info_frames not implemented!\n");
    //assert(false);
    
    if ( (_n_frames*2)/(8*4 KB) + (_n_frames*2) % (8*4 KB) > 0 ) {
    	return 1;
    } else {
    	return 0;
    }		  	
    
    
}
