//only for checking the system call that it is running fine or not
#include <stdio.h>
#include<unistd.h>
int main(){
int result = syscall(336);
if(result == 0)
printf("right\n");
else
printf("wrong\n");
}
