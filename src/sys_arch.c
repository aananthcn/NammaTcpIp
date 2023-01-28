/*
 * Created on Sat Jan 28 2023 9:49:06 AM
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 Aananth C N
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <arch/sys_arch.h>
#include <arch/cc.h>

#include <osek.h>
#include <os_api.h>
#include <sg_counter.h>


sys_prot_t sys_arch_protect(void) {
	sys_prot_t retc = 0;

	// enter critical section
	DisableAllInterrupts();

	return retc;
}


void sys_arch_unprotect(sys_prot_t pval)
{
	// unsed arg
	pval = pval;

	// exit critical section
	ResumeAllInterrupts();
}


u32_t sys_now(void) {
	return _GetOsTickCnt() * (OS_TICK_DURATION_ns / 1000000);
}