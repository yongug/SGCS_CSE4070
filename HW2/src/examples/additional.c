#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
    int i;
    int arr[4];
    bool success_flag = true;

    if(argc==5)
    {
        for(int i=0;i<argc-1;i++)
        {
            arr[i] = atoi(argv[i+1]);
        }
        int fibo = fibonacci(arr[0]);
        int max = max_of_four_int(arg[0],arg[1],arg[2],arg[3]);

        if(max == -1 || fibo == -1)
        {
            printf("additional failed : invaild return\n");
            success_flag = false;
        }
        else
        {
            printf("%d %d\n", fibo,max);
        }

    }
    else
    {
        printf("additional failed : not proper argument number\n")
        success_flag = false;
    }
    
    if(success_flag == true)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}