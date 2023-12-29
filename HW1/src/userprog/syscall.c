#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "devices/input.h"



static void syscall_handler (struct intr_frame *);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


static void
syscall_handler (struct intr_frame *f) 
{
	pid_t pid;

  int sys_num = *(uint32_t *)(f->esp);

  if(sys_num <0){
    exit(-1);
  }

  switch(sys_num){
      case SYS_HALT:
	      halt();	
     	  break;

      case SYS_EXIT:
	      validation_check(f->esp+4);
	      exit(*(uint32_t*)(f->esp + 4));
     	  break;

      case SYS_EXEC:
	      validation_check(f->esp+4);
	      f->eax = exec((const char*)*(uint32_t*)(f->esp + 4));
     	  break;

      case SYS_WAIT:
	      validation_check(f->esp+4);
	      pid=*(pid_t*)(f->esp + 4);
	      f->eax = wait((pid_t)*(uint32_t *)(f->esp +4));
     	  break;


      // this is 

      case SYS_READ:
	      validation_check(f->esp+20); validation_check(f->esp+24); validation_check(f->esp+28);
        f->eax = read((int)*(uint32_t*)(f->esp+20), (void*)*(uint32_t*)(f->esp+24),(unsigned)*(
uint32_t*)(f->esp+28));
	      break;

    case SYS_WRITE:
	      validation_check(f->esp+20); validation_check(f->esp+24); validation_check(f->esp+28);
	      f->eax = write((int)*(uint32_t*)(f->esp+20), (void*)*(uint32_t*)(f->esp+24),(unsigned)*(uint32_t*)(f->esp+28));
	      break;

    //this is

    case SYS_FIBO:
	      validation_check(f->esp+28);
	      f->eax=fibonacci((int)*(uint32_t*)(f->esp+28));
	      break;

    case SYS_MAX:
	      validation_check(f->esp+32); validation_check(f->esp+36); validation_check(f->esp+40); validation_check(f->esp+44);
	      f->eax=max_of_four_int((int)*(uint32_t*)(f->esp+32), (int)*(uint32_t*)(f->esp+36), (int)*(uint32_t*)(f->esp+40),(int)*(uint32_t*)(f->esp+44));
	      break;

    default:
	      break;	
  }
}





// halt
void halt(){
  shutdown_power_off();
}
// validation_check
void validation_check(void * addr)
{
  if(!is_user_vaddr(addr))
  {
    exit(-1);
  }
  if(is_kernel_vaddr(addr))
  {
    exit(-1);
  }

  return;
}
// exit
void
exit(int status)
{
  printf("%s: exit(%d)\n",thread_name(),status);
  thread_current()->exit_status = status;
  thread_exit();
}
// exec
pid_t
exec(const char *file)
{
  return process_execute(file);
}
// wait
int wait(pid_t pid)
{
  return process_wait(pid);
}
// read

int read(int fd,void * buffer, unsigned length)
{
  if(fd == 0)
  {
    return input_getc();
  }
  return -1;
}

// write
int
write(int fd, const void *buffer, unsigned length)
{
  if(fd ==1)
  {
    putbuf(buffer,length);
    return length;
  }
  else{
    return -1;
  }
  

}

// fibonacci
int 
fibonacci(int num)
{
  int now, before, temp;
  before = 0; now = 1;
  if(num<2)
  {
    return num;
  }
  else
  {
    for(int i=1;i<num;i++)
    {
      temp = now;
      now += before;
      before = temp;
    }
    return now;
  }

}
// max_of_four_int
int max_of_four_int(int n1, int n2, int n3, int n4)
{
    int ret;
    if(n1>n2)
    {
      ret = n1;
    }
    else{
      ret = n2;
    }
    ret = (ret > n3) ? ret : n3;
    ret = (ret > n4) ? ret : n4;

    return ret;
}
//
