/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *         The kernel's entry, where most of the initialization work is done.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#include "irq.h"
#include "test.h"
#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "syscall.h"

queue_t ready_queue;
queue_t block_queue;

void debuginfo(uint32_t info)
{
	printk("%x ", info);	
	//while (1) {}
}

static void init_pcb()
{
	int i,total;
	//printk("test_begin\n");
	queue_init(&ready_queue);
	queue_init(&block_queue);
	total = num_sched1_tasks;//printk("test_init_queue\n");
	for(i=0;i<total+1;i++){
		pcb[i].status = TASK_READY;
		pcb[i].user_context.regs[29] = 0xa0f00000 + i*0x10000;
		pcb[i].kernel_context.regs[29] = 0xa0f00000 + (i+16)*0x10000; 
		pcb[i].pid = i;
		pcb[i].type = USER_PROCESS;
		pcb[i].user_context.pc = 0;
		pcb[i].kernel_context.pc = 0;//printk("test_init_pcb\n");
	}

	for(i=1;i<num_sched1_tasks+1;i++){
		pcb[i].user_context.regs[31] = sched1_tasks[i-1]->entry_point;
		pcb[i].kernel_context.regs[31] = (uint32_t)&backtouser;
		queue_push(&ready_queue, &pcb[i]);
		//pcb[i].kernel_context.cp0_epc = sched1_tasks[i-1]->entry_point;
		pcb[i].user_context.cp0_epc = sched1_tasks[i-1]->entry_point;
                pcb[i].kernel_context.cp0_status = 0x10008002;
		pcb[i].user_context.cp0_status = 0x10008002;
	}

	/*for(i=num_sched1_tasks+1;i<total+1;i++){
		pcb[i].user_context.regs[31] = lock_tasks[i-num_sched1_tasks-1]->entry_point;
		queue_push(&ready_queue, &pcb[i]);
	}//printk("test_init_push\n");*/
	
	pcb[0].status = TASK_EXITED;
	pcb[0].user_context.regs[31] = sched1_tasks[0]->entry_point;
	pcb[0].kernel_context.regs[31] = (uint32_t)&reset;
	pcb[0].pid = 0;	
	pcb[0].kernel_context.cp0_status = 0x10008002;
	pcb[0].user_context.cp0_status = 0x10008003;
	
	current_running = &pcb[0];
}

static void init_exception_handler()
{
}

static void init_exception()
{
	//debuginfo(1);// 1. Get CP0_STATUS
	disable_interrupt();// 2. Disable all interrupt		
	memcpy((void*)0x80000180,(void*)exception_handler_entry,exception_handler_end-exception_handler_begin);// 3. Copy the level 2 exception handling code to 0x80000180	 
}

static void init_syscall(void)
{
	// init system call table.
}

// jump from bootloader.
// The beginning of everything >_< ~~~~~~~~~~~~~~
void __attribute__((section(".entry_function"))) _start(void)
{
	// Close the cache, no longer refresh the cache 
	//printk("test_start\n");// when making the exception vector entry copy
	asm_start();

	// init interrupt (^_^)
	init_exception();
	printk("> [INIT] Interrupt processing initialization succeeded.\n");
	// init system call table (0_0)
	init_syscall();
	printk("> [INIT] System call initialized successfully.\n");

	// init Process Control Block (-_-!)
	init_pcb();
	printk("> [lINIT] PCB initialization succeeded.\n");

	// init screen (QAQ)
	init_screen();
	printk("> [INIT] SCREEN initialization succeeded.\n");

	
	enable_interrupt();
	reset();//4. reset CP0_COMPARE & CP0_COUNT register// TODO Enable interrupt

	while (1)
	{
		//debuginfo(current_running->pid);// (QAQQQQQQQQQQQ)
		// If you do non-preemptive scheduling, you need to use it to surrender control	
		// do_scheduler();
	};
	return;
}
