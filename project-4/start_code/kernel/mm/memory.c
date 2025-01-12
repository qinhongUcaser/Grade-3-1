#include "mm.h"
#include "sched.h"

#define phy_number 0x1000 //458752  
#define pte_number 0x80000 // 1048576 
// #define pte_number 0x200
int count = 0;

typedef struct page_table_entry {
    uint32_t virtual_number;
    uint32_t physical_number;
    int valid;
    uint32_t pid;
    // struct page_table_entry *next;
}PTE;

uint32_t phy_addr = 0x01000000;
PTE *page_table;

uint32_t get_cp0_context(void){
	uint32_t context;	
	asm(
		"mfc0 %0,$4\t\n"
		: "=r"(context)	
	);
	return context;
}

uint32_t get_cp0_index(void){
	uint32_t context;	
	asm(
		"mfc0 %0,$0\t\n"
		: "=r"(context)	
	);
	return context;
}

uint32_t get_cp0_badvaddr(void){
	uint32_t badvaddr;	
	asm(
		"mfc0 %0,$8\t\n"
		: "=r"(badvaddr)	
	);
	return badvaddr;
}

uint32_t get_cp0_entryhi(void){
	uint32_t badvaddr;	
	asm(
		"mfc0 %0,$10\t\n"
		: "=r"(badvaddr)	
	);
	return badvaddr;
}

uint32_t get_cp0_entrylo0(void){
	uint32_t badvaddr;	
	asm(
		"mfc0 %0,$2\t\n"
		: "=r"(badvaddr)	
	);
	return badvaddr;
}

void init_phy_table(void){

    page_table = (PTE*)0xa0900000;

    uint32_t i = 0;
    for(i = 0;i < pte_number;i++){
        page_table[i].virtual_number = 0x00000000+i*0x1000;
        page_table[i].pid = 0;
        page_table[i].valid = 0;
    }
    // debuginfo(page_table[i-1].virtual_number);
} 
//In task1&2, page table is initialized completely with address mapping, but only virtual pages in task3.


void init_TLB(void){
    int i = 0;
    int entryhi = 0x000000ff;
    int entrylo0 = 0x00000016;
    int entrylo1 = 0x00000056;
    for(i = 0;i < 32;i++){
        init_TLB_asm(i,entryhi,entrylo0,entrylo1);
        // entryhi = 0x2000 + entryhi;
        // entrylo0 = 0x40 + entrylo0;
        // entrylo1 = 0x40 + entrylo1;
    }
}
//TODO:Finish memory management functions here refer to mm.h and add any functions you need.

void do_TLBP(void){
    // debuginfo(1234);
    uint32_t context  = get_cp0_context();
    // debuginfo(context);
    TLBP((context<<9),current_running->pid);
    // debuginfo(1234);
}

void TLBrefill(void){

    uint32_t context = get_cp0_context();
    uint32_t index   = get_cp0_index();
    uint32_t entryhi = get_cp0_entryhi();
    uint32_t entrylo0 = get_cp0_entrylo0();
    uint32_t badvaddr = get_cp0_badvaddr();
    int i = 0;
    int j = 0;
    int k = 0;
    int temp = 0;
    int refill = 0;
    uint32_t asid = 0x000000ff & get_cp0_entryhi();
    uint32_t con_vir_addr = (context & 0x007ffff0)<<9;

    // debuginfo(1234);
    // printkf("\n%x \n%x \n%x \n%x \n%x",badvaddr,context,index,entryhi,entrylo0);
    for (i = 0;(i < pte_number)&&(refill != 1);i = i + 2){
        // printk("\n%d",i);
        if((con_vir_addr == page_table[i].virtual_number)&&(page_table[i].valid == 1)){
                refill = 1;
                i = i - 2;// debuginfo(i);
            if(asid != page_table[i].pid){
                // debuginfo(badvaddr);
                printk("address overload %x",context);
                // while(1){}
            }
        }
        // printk("\n%d",i);
    }

    if(refill == 0){
        count++;
        for(i = 0;i < pte_number;i = i + 2){
            if((page_table[i].valid == 0)&&(page_table[i].virtual_number == con_vir_addr)){
                // page_table[i].virtual_number = con_vir_addr;
                page_table[i].valid = 1;
                page_table[i+1].valid = 1;
                break;
            }
        }
        page_table[i].physical_number = phy_addr;
        page_table[i+1].physical_number = phy_addr + 0x1000;
        phy_addr = phy_addr + 0x2000;
        page_table[i].pid = (uint32_t)current_running->pid;
        page_table[i+1].pid = (uint32_t)current_running->pid;
    }
    // printk("%d",count);
    // printk("\n%x \n%d \n%x \n%x \n%x",con_vir_addr,current_running->pid,i,page_table[i].virtual_number,page_table[i+1].physical_number);
    // debuginfo(1234);
    set_TLBrefill(page_table[i].virtual_number,page_table[i].physical_number>>6,current_running->pid);
    TLB_back();
}

void TLBinvalid(void){

    uint32_t context = get_cp0_context();
    uint32_t index   = get_cp0_index();
    uint32_t entrylo0 = get_cp0_entrylo0();
    uint32_t entryhi = get_cp0_entryhi();
    int i = 0;
    int j = 0;
    int refill = 0;
    uint32_t asid = 0x000000ff & get_cp0_entryhi();
    uint32_t con_vir_addr = (context & 0x007ffff0)<<9;
    debuginfo(123456);
    
    // printk("\n%d \n%x \n%x \n%x",refill,i,page_table[i].virtual_number/2,page_table[i+1].physical_number);
    debuginfo(index);
    // set_TLBinvalid(page_table[i].virtual_number>>1,page_table[i].physical_number>>6,current_running->pid);
}