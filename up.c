#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"


int main(int argc, char *argv[])
{
    shm_open(1, 2, 0);
    fork();
    exit();
    return 0;
}
