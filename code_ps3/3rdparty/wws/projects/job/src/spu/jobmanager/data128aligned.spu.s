/*
 * Copyright (c) 2003-2006 Sony Computer Entertainment.
 * Use and distribution without consent strictly prohibited.
 */

.section .data128,"aw"

.align 7	//g_jobHeaderCache should ideally be 128 byte aligned
//If this was in the same file as the rest of the data, the whole thing
//would be padded out to taking a multiple of 128 bytes

.global g_jobHeaderCache
g_jobHeaderCache: .skip 0x80
