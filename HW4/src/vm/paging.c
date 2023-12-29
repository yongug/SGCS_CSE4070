#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "userprog/pagedir.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "page.h"
#include "threads/vaddr.h"

void stack_growth_func(struct thread *curr, const void* fault_addr,bool bl){
	
	bool is_set_page =false;

	void * func_addr;
	func_addr = fault_addr;
	
	void *us_page = pg_round_down(func_addr);
	
	void *ke_page = palloc_get_page(PAL_USER);

	//
	//
	if(bl==true)
	{
		pagedir_get_page(curr->pagedir, us_page);
		is_set_page = true;
		if(is_set_page)
		{
			pagedir_set_page(curr->pagedir, us_page,ke_page, bl);
		}
		is_set_page = false;
	}		
		
}