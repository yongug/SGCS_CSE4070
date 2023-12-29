#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "filesys/file.h"
#include "threads/synch.h"
#include "filesys/filesys.h"

struct file{
  struct inode *inode;
  off_t pos;
  bool deny_write;
};

struct lock filesys_lock;
static void syscall_handler (struct intr_frame *);


void
syscall_init (void) 
{
  lock_init(&filesys_lock);
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

    		// Project 2 
		case SYS_CREATE:
      		validation_check(f->esp+4); validation_check(f->esp+8);
			f->eax = create((const char *)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
			break;

		case SYS_REMOVE:
      		validation_check(f->esp+4);
			f->eax = remove((const char *)*(uint32_t *)(f->esp + 4));
			break;

		case SYS_OPEN:
			validation_check(f->esp+4);
			f->eax = open((const char *)*(uint32_t *)(f->esp + 4));
			break;

		case SYS_CLOSE:
			validation_check(f->esp+4);
			close((int)*(uint32_t *)(f->esp + 4));
			break;

		case SYS_FILESIZE:
			validation_check(f->esp+4);
			f->eax = filesize((int)*(uint32_t *)(f->esp + 4));
			break;

		// For SYS_READ and SYS_WRITE, modify the contents of Project 1
		case SYS_SEEK:
			validation_check(f->esp+4); validation_check(f->esp+8);
			seek((int)*(uint32_t *)(f->esp + 4), (unsigned)*(uint32_t *)(f->esp + 8));
			break;

		case SYS_TELL:
			validation_check(f->esp+4);
			f->eax = tell((int)*(uint32_t *)(f->esp + 4));
			break;

    default:
	      break;	
  }
}
//





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
  for(int i=3;i<128;i++)
  {
    if(thread_current() ->fd[i] != NULL)
    {
      close(i);
    }
  }
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
  int i = 0;
	
	validation_check(buffer);
	
	lock_acquire(&filesys_lock);

	if(fd == 0) {
		for(i=0; (unsigned int)i<length; i++) {
			if((((char *)buffer)[i] = input_getc()) == '\0') {
				break;
			}
		}
		lock_release(&filesys_lock);
		return i;
	}
  else if(fd<=1 || fd>=128)
  {
    lock_release(&filesys_lock);
    return -1;
  }
	else if(fd >= 3) {
    struct file *file = thread_current()-> fd[fd];
		if(file == NULL){
			lock_release(&filesys_lock);
			exit(-1);
		}
		lock_release(&filesys_lock);
		return file_read(file, buffer, length);
	}

	else {
		lock_release(&filesys_lock);
		return 0;
	}
}

// write
int
write(int fd, const void *buffer, unsigned length)
{
  validation_check(buffer);

	lock_acquire(&filesys_lock);

	if(fd == 1) {
		putbuf(buffer, length);
		lock_release(&filesys_lock);
		return length;
	}
  else if(fd<=0 || fd >=128)
  {
    lock_release(&filesys_lock);
    return 0;
  }
	else if(fd >= 3) {
    struct file *file = thread_current()-> fd[fd];
		if(file == NULL) {
			lock_release(&filesys_lock);
			exit(-1);
		}
		if(file->deny_write) {
			file_deny_write(file);
		}
		lock_release(&filesys_lock);
		return file_write(file, buffer, length);
	}
	
	else {
		lock_release(&filesys_lock);
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

// ------------------------------------------------------------------------//

bool 
create (const char *file, unsigned initial_size)
{
	if(file == NULL)
  {
    exit(-1);
  }
  validation_check(file);

	return filesys_create(file, initial_size);
}

bool 
remove (const char *file)
{
	if(file == NULL)
  {
    exit(-1);
  }
  validation_check(file);

	return filesys_remove(file);
}

int 
open (const char *file)
{
	if(file == NULL)
  {
    exit(-1);
  }
  validation_check(file);

	lock_acquire(&filesys_lock);
	struct file *fp = filesys_open(file);
	
	if(fp == NULL) {
		lock_release(&filesys_lock);
		return -1;
	}

	else {
		for(int i=3; i < 128; i++) {
			if(thread_current()->fd[i] == NULL) {
				if(strcmp(thread_current()->name, file)==0) {
					file_deny_write(fp);
				}

				thread_current()->fd[i] = fp;
				lock_release(&filesys_lock);
				return i;
			}
		}
	}

	lock_release(&filesys_lock);
	return -1;
}

void
close (int fd)
{
  struct file* file = thread_current()->fd[fd];

	if(file == NULL)
		exit(-1);

	file = NULL;
	
	return file_close(file);
}

int 
filesize (int fd)
{
  struct file* file = thread_current()->fd[fd];
	if(file == NULL)
		exit(-1);

	return file_length(file);
}

void
seek (int fd, unsigned position)
{
	  struct file* file = thread_current()->fd[fd];
	if(file == NULL)
			exit(-1);
	
	file_seek(file, position);
	}

unsigned
tell (int fd)
{
	    struct file* file = thread_current()->fd[fd];
	if(file == NULL)
			exit(-1);
	
		return file_tell(file);
}