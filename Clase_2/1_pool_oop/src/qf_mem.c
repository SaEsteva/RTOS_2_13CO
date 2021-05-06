/**
* @file
* @ingroup qf
* @brief ::QMPool implementatin (Memory Pool)
* @cond
******************************************************************************
* Last updated for version 6.2.0
* Last updated on  2018-03-13
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2018 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* https://www.state-machine.com
* mailto:info@state-machine.com
******************************************************************************
* @endcond
*/

#include "qmpool.h"      /* QMPOOL */
#include "FreeRTOS.h"   /* FreeRTOS */
#include "task.h"

/****************************************************************************/
/**
* @description
* Initialize a fixed block-size memory pool by providing it with the pool
* memory to manage, size of this memory, and the block size.
*
* @param[in,out] me       pointer (see @ref oop)
* @param[in]     poolSto  pointer to the memory buffer for pool storage
* @param[in]     poolSize size of the storage buffer in bytes
* @param[in]     blockSize fixed-size of the memory blocks in bytes
*
* @attention
* The caller of QMPool::init() must make sure that the @p poolSto
* pointer is properly **aligned**. In particular, it must be possible to
* efficiently store a pointer at the location pointed to by @p poolSto.
* Internally, the QMPool_init() function rounds up the block size
* @p blockSize so that it can fit an integer number of pointers.
* This is done to achieve proper alignment of the blocks within the pool.
*
* @note
* Due to the rounding of block size the actual capacity of the pool might
* be less than (@p poolSize / @p blockSize). You can check the capacity
* of the pool by calling the QF_getPoolMin() function.
*
* @note
* This function is **not** protected by a critical section, because
* it is intended to be called only during the initialization of the system,
* when interrupts are not allowed yet.
*
* @note
* Many QF ports use memory pools to implement the event pools.
*
* @usage
* The following example illustrates how to invoke QMPool_init():
* @include qmp_init.c
*/
void QMPool_init( QMPool * const me, void * const poolSto,
		unsigned int poolSize, unsigned short blockSize )
{
    QFreeBlock *fb;
    unsigned short nblocks;

    me->free_head = poolSto;

    /* round up the blockSize to fit an integer # free blocks, no division */
    me->blockSize = ( QMPoolSize )sizeof( QFreeBlock ); /* start with just one */
    nblocks = ( unsigned short )1; /* #free blocks that fit in one memory block */
    while ( me->blockSize < ( QMPoolSize )blockSize )
    {
        me->blockSize += ( QMPoolSize )sizeof( QFreeBlock );
        ++nblocks;
    }
    blockSize = ( unsigned short )me->blockSize; /* round-up to nearest block */

    /* chain all blocks together in a free-list... */
    poolSize -= ( unsigned int )blockSize; /* don't count the last block */
    me->nTot  = ( QMPoolCtr )1;  /* the last block already in the pool */
    fb = ( QFreeBlock * )me->free_head; /* start at the head of the free list */

    /* chain all blocks together in a free-list... */
    while ( poolSize >= ( unsigned int )blockSize )
    {
        fb->next = &QF_PTR_AT_( fb, nblocks ); /*point next link to next block */
        fb = fb->next;           /* advance to the next block */
        poolSize -= ( unsigned int )blockSize; /* reduce available pool size */
        ++me->nTot;              /* increment the number of blocks so far */
    }

    fb->next  = ( QFreeBlock * )0; /* the last link points to NULL */
    me->nFree = me->nTot;        /* all blocks are free */
    me->nMin  = me->nTot;        /* the minimum number of free blocks */
    me->start = poolSto;         /* the original start this pool buffer */
    me->end   = fb;              /* the last block in this pool */
}

/****************************************************************************/
/**
* @description
* Recycle a memory block to the fixed block-size memory pool.
*
* @param[in,out] me   pointer (see @ref oop)
* @param[in]     b    pointer to the memory block that is being recycled
*
* @attention
* The recycled block must be allocated from the **same** memory pool
* to which it is returned.
*
* @note
* This function can be called from any task level or ISR level.
*
* @sa
* QMPool_get()
*
* @usage
* The following example illustrates how to use QMPool_put():
* @include qmp_use.c
*/
void QMPool_put( QMPool * const me, void *b )
{

    //UBaseType_t uxSavedInterruptStatus;
    /** @pre # free blocks cannot exceed the total # blocks and
    * the block pointer must be from this pool.
    */

    portENTER_CRITICAL(); //Enter on critical section
    //uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    ( ( QFreeBlock * )b )->next = ( QFreeBlock * )me->free_head; /* link into list */
    me->free_head = b;      /* set as new head of the free list */
    ++me->nFree;            /* one more free block in this pool */
    //taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

    portEXIT_CRITICAL(); //Exit from critical section
}

/****************************************************************************/
/**
* @description
* The function allocates a memory block from the pool and returns a pointer
* to the block back to the caller.
*
* @param[in,out] me      pointer (see @ref oop)
* @param[in]     margin  the minimum number of unused blocks still available
*                        in the pool after the allocation.
*
* @note
* This function can be called from any task level or ISR level.
*
* @note
* The memory pool @p me must be initialized before any events can
* be requested from it. Also, the QMPool_get() function uses internally a
* QF critical section, so you should be careful not to call it from within
* a critical section when nesting of critical section is not supported.
*
* @attention
* An allocated block must be later returned back to the same pool
* from which it has been allocated.
*
* @sa QMPool_put()
*
* @usage
* The following example illustrates how to use QMPool_get():
* @include qmp_use.c
*/
void *QMPool_get( QMPool * const me, unsigned short const margin )
{
    QFreeBlock *fb;

    portENTER_CRITICAL(); //Enter on critical section

    /* have more free blocks than the requested margin? */
    if ( me->nFree > ( QMPoolCtr )margin )
    {
        void *fb_next;
        fb = ( QFreeBlock * )me->free_head; /* get a free block */

        fb_next = fb->next; /* put volatile to a temporary to avoid UB */

        /* is the pool becoming empty? */
        --me->nFree; /* one less free block */
        if ( me->nFree == ( QMPoolCtr )0 )
        {

            me->nMin = ( QMPoolCtr )0; /* remember that the pool got empty */

        }
        else
        {
            /* pool is not empty, so the next free block must be in range
            *
            * NOTE: the next free block pointer can fall out of range
            * when the client code writes past the memory block, thus
            * corrupting the next block.
            */

            /* is the number of free blocks the new minimum so far? */
            if ( me->nMin > me->nFree )
            {
                me->nMin = me->nFree; /* remember the new minimum */
            }
        }

        me->free_head = fb_next; /* set the head to the next free block */

    }
    /* don't have enough free blocks at this point */
    else
    {

        fb = ( QFreeBlock * )0;

    }

    portEXIT_CRITICAL(); //Exit from critical section

    return fb;  /* return the block or NULL pointer to the caller */
}

/****************************************************************************/
/**
* @description
* This function obtains the minimum number of free blocks in the given
* event pool since this pool has been initialized by a call to QF_poolInit().
*
* @param[in,out] me      pointer (see @ref oop)
*
* @returns
* the minimum number of unused blocks in the given event pool.
*/
unsigned short QMPool_getMin( QMPool * const me )
{
	unsigned short min;

    portENTER_CRITICAL(); //Enter on critical section

    min = me->nMin;

    portEXIT_CRITICAL(); //Exit from critical section

    return min;
}

