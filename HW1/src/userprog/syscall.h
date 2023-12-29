#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

typedef int pid_t;

void syscall_init (void);
void halt (void);
void exit(int status);
pid_t exec(const char* file);
int wait(pid_t pid);
int read(int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
int fibonacci(int num);
int max_of_four_int(int n1, int n2, int n3, int n4);

#endif /* userprog/syscall.h */