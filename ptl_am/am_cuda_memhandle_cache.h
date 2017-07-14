/*

  This file is provided under a dual BSD/GPLv2 license.  When using or
  redistributing this file, you may do so under either license.

  GPL LICENSE SUMMARY

  Copyright(c) 2016 Intel Corporation.

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  Contact Information:
  Intel Corporation, www.intel.com

  BSD LICENSE

  Copyright(c) 2016 Intel Corporation.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifdef PSM_CUDA

#ifndef _AM_CUDA_MEMHANDLE_CACHE_H
#define _AM_CUDA_MEMHANDLE_CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

struct _cl_map_item;

typedef struct
{
	unsigned long		start;		 /* start virtual address */
	cudaIpcMemHandle_t      cuda_ipc_handle; /* cuda ipc mem handle */
	void*			cuda_ipc_dev_ptr;/* Cuda device pointer */
	uint16_t		length;	 /* length*/
	psm2_epid_t             epid;
	struct _cl_map_item*	i_prev;	 /* idle queue previous */
	struct _cl_map_item*	i_next;	 /* idle queue next */
}__attribute__ ((aligned (128))) rbtree_cuda_memhandle_cache_mapitem_pl_t;

typedef struct {
	uint32_t		nelems;	/* number of elements in the cache */
} rbtree_cuda_memhandle_cache_map_pl_t;

#define RBTREE_MI_PL  rbtree_cuda_memhandle_cache_mapitem_pl_t
#define RBTREE_MAP_PL rbtree_cuda_memhandle_cache_map_pl_t

#include "rbtree.h"

cl_qmap_t cuda_memhandle_cachemap; /* Global cache */
uint8_t cuda_memhandle_cache_enabled;
mpool_t cuda_memhandle_mpool;
uint32_t cuda_memhandle_cache_size;
#define CUDA_MEMHANDLE_CACHE_SIZE 64

/*
 * Macro definition for easy programming.
 */

#define NELEMS			cuda_memhandle_cachemap.payload.nelems

/*
 * Macro for idle queue management.
 */
#define IHEAD			cuda_memhandle_cachemap.root
#define LAST			IHEAD->payload.i_prev
#define FIRST			IHEAD->payload.i_next
#define INEXT(x)		x->payload.i_next
#define IPREV(x)		x->payload.i_prev


psm2_error_t am_cuda_memhandle_mpool_init(uint32_t memcache_size);

psm2_error_t am_cuda_memhandle_cache_map_init();

void*
am_cuda_memhandle_acquire(uintptr_t sbuf, cudaIpcMemHandle_t* handle,
				uint32_t length, psm2_epid_t epid);
void
am_cuda_memhandle_release(void* cuda_ipc_dev_ptr);

void psmi_cuda_memhandle_cache_alloc_func(int is_alloc, void* context, void* obj);

void am_cuda_memhandle_cache_map_fini();

#endif

#endif