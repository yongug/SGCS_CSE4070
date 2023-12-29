#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "userprog/process.h"
#include "devices/shutdown.h"
#include "lib/user/syscall.h"

typedef int pid_t;

/* Project 1*/
void syscall_init (void);
void halt (void);
void exit(int status);
pid_t exec(const char* file);
int wait(pid_t pid);
int read(int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
int fibonacci(int num);
int max_of_four_int(int n1, int n2, int n3, int n4);


/* Project 2*/
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int filesize(int fd);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);
int open(const char *file);
#endif /* userprog/syscall.h */