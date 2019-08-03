/*
 * Copyright (c) 2003-2006 Sony Computer Entertainment.
 * Use and distribution without consent strictly prohibited.
 */

.section .data256,"aw"

.align 8	//Note that g_auditBlocks requires 256 byte alignment
//If this was in the same file as the rest of the data, the whole thing
//would be padded out to taking a multiple of 256 bytes

.global g_auditBlocks
g_auditBlocks: .skip 0x100

.global g_WwsJob_loadCommands
g_WwsJob_loadCommands: .skip 0x100
