//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"
#include "syscall.h"

extern void set_void_argument(int argument_number, int system_call_number);
extern void set_int_argument(int value, int argument_number,
        int system_call_number);

void
swap_syscalls(struct system_call_status* s1, struct system_call_status* s2)
{
	struct system_call_status* s3	= s1;
	*s1 = *s2;
	*s2 = *s3;
}

void
config_new_syscall_status(struct system_call_status* new_element,
        int syscall_id, int pid)
{
    // Add new syscall struct to the end of sorted_syscalls by time
    sorted_syscalls.items[sorted_syscalls.number_of_calls] = new_element;

    new_element->index_in_sorted_syscalls_by_time =
            sorted_syscalls.number_of_calls;
    new_element->syscall_number = syscall_id;
    new_element->pid = pid;

    sorted_syscalls.number_of_calls++;
    sorted_syscalls.number_of_calls %= NPROC * MAX_SYS_CALL_NUMBERS;
}

void
reorder_sorted_syscall_by_syscall_num(int pid, int syscall_id)
{
//    int i, j;
	
    struct system_call_status* system_call_status_struct =
            process_system_calls[pid].system_calls;
    int number_of_calls = process_system_calls[pid].number_of_calls;

    struct system_call_status* last_element =
            &system_call_status_struct[number_of_calls];
    config_new_syscall_status(last_element, syscall_id, pid);

//    for(i = ONE; i <= number_of_calls; i++)
//    {
//	if (system_call_status_struct[i].syscall_number >
//	        last_element->syscall_number)
//	    break;
//    }

//    for(j = number_of_calls; j > i; j--)
//    {
//	swap_syscalls(&system_call_status_struct[j],
//	        &system_call_status_struct[j - 1]);
//	// Correct index
//	sorted_syscalls.items[system_call_status_struct[j].
//	        index_in_sorted_syscalls_by_time] =
//	        &system_call_status_struct[j];
//	sorted_syscalls.items[system_call_status_struct[j - 1].
//	        index_in_sorted_syscalls_by_time] =
//	        &system_call_status_struct[j - 1];
//    }
}

void
set_pointer_argument(uint value, int argument_number, int system_call_number)
{
    struct proc* curproc = myproc();
    int pid = curproc->pid;

    struct system_call* system_call_struct = &process_system_calls[pid];
    int number_of_calls = (*system_call_struct).number_of_calls;

    (*system_call_struct).system_calls[number_of_calls].number_of_arguments++;
    (*system_call_struct).system_calls[number_of_calls].
            arguments[argument_number].type = POINTER;

    (*system_call_struct).system_calls[number_of_calls].
            arguments[argument_number].p_value = value;
}

void
set_charp_argument(char* value, int argument_number, int system_call_number)
{
    struct proc* curproc = myproc();
    int pid = curproc->pid;
    int i;

    struct system_call* system_call_struct = &process_system_calls[pid];
    int number_of_calls = (*system_call_struct).number_of_calls;

    (*system_call_struct).system_calls[number_of_calls].number_of_arguments++;
    (*system_call_struct).system_calls[number_of_calls].
            arguments[argument_number].type = CHARP;

    for (i = 0; i < MAX_CHARP_SIZE; ++i)
    {
		(*system_call_struct).system_calls[number_of_calls].
				arguments[argument_number].charp_value[i] = value[i];
		if (value[i] == '\0')
			break;
    }
}

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int n, int *pfd, struct file **pf)
{
	int fd;
	struct file *f;

	if(argint(n, &fd) < 0)
		return -1;
	if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
		return -1;
	if(pfd)
		*pfd = fd;
	if(pf)
		*pf = f;
	return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
	int fd;
	struct proc *curproc = myproc();

	for(fd = 0; fd < NOFILE; fd++){
		if(curproc->ofile[fd] == 0){
			curproc->ofile[fd] = f;
			return fd;
		}
	}
	return -1;
}

int
sys_dup(void)
{
	struct file *f;
	int fd;

	if(argfd(0, 0, &f) < 0)
		return -1;
	if((fd=fdalloc(f)) < 0)
		return -1;

	set_pointer_argument((uint)f, FIRST, SYS_dup);
	filedup(f);
	return fd;
}

int
sys_read(void)
{
	struct file *f;
	int n;
	char *p;

	if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
		return -1;

	set_pointer_argument((uint)f, FIRST, SYS_write);
	set_charp_argument(p, SECOND, SYS_write);
	set_int_argument(n, THIRD, SYS_write);

	return fileread(f, p, n);
}

int
sys_write(void)
{
	struct file *f;
	int n;
	char *p;

	if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
		return -1;

	set_pointer_argument((uint)f, FIRST, SYS_write);
	set_charp_argument(p, SECOND, SYS_write);
	set_int_argument(n, THIRD, SYS_write);

	return filewrite(f, p, n);
}

int
sys_close(void)
{
	int fd;
	struct file *f;

	if(argfd(0, &fd, &f) < 0)
	        return -1;
	myproc()->ofile[fd] = 0;

	set_pointer_argument((uint)f, FIRST, SYS_close);
	fileclose(f);
	return 0;
}

int
sys_fstat(void)
{
	struct file *f;
	struct stat *st;

	if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
		return -1;

	set_pointer_argument((uint)f, FIRST, SYS_fstat);
	set_pointer_argument((uint)st, SECOND, SYS_fstat);
	return filestat(f, st);
}

// Create the path new as a link to the same inode as old.
int
sys_link(void)
{
	char name[DIRSIZ], *new, *old;
	struct inode *dp, *ip;

	if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
		return -1;

	set_charp_argument(old, FIRST, SYS_link);
	set_charp_argument(new, SECOND, SYS_link);

	begin_op();
	if((ip = namei(old)) == 0){
		end_op();
		return -1;
	}

	ilock(ip);
	if(ip->type == T_DIR){
		iunlockput(ip);
		end_op();
		return -1;
	}

	ip->nlink++;
	iupdate(ip);
	iunlock(ip);

	if((dp = nameiparent(new, name)) == 0)
		goto bad;
	ilock(dp);
	if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
		iunlockput(dp);
		goto bad;
	}
	iunlockput(dp);
	iput(ip);

	end_op();

	return 0;

bad:
	ilock(ip);
	ip->nlink--;
	iupdate(ip);
	iunlockput(ip);
	end_op();
	return -1;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
	int off;
	struct dirent de;

	for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
		if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
			panic("isdirempty: readi");
		if(de.inum != 0)
			return 0;
	}
	return 1;
}

//PAGEBREAK!
int
sys_unlink(void)
{
	struct inode *ip, *dp;
	struct dirent de;
	char name[DIRSIZ], *path;
	uint off;

	if(argstr(0, &path) < 0)
		return -1;

	set_charp_argument(path, FIRST, SYS_unlink);

	begin_op();
	if((dp = nameiparent(path, name)) == 0){
		end_op();
		return -1;
	}

	ilock(dp);

	// Cannot unlink "." or "..".
	if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
		goto bad;

	if((ip = dirlookup(dp, name, &off)) == 0)
		goto bad;
	ilock(ip);

	if(ip->nlink < 1)
		panic("unlink: nlink < 1");
	if(ip->type == T_DIR && !isdirempty(ip)){
		iunlockput(ip);
		goto bad;
	}

	memset(&de, 0, sizeof(de));
	if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
		panic("unlink: writei");
	if(ip->type == T_DIR){
		dp->nlink--;
		iupdate(dp);
	}
	iunlockput(dp);

	ip->nlink--;
	iupdate(ip);
	iunlockput(ip);

	end_op();

	return 0;

bad:
	iunlockput(dp);
	end_op();
	return -1;
}

static struct inode*
create(char *path, short type, short major, short minor)
{
	uint off;
	struct inode *ip, *dp;
	char name[DIRSIZ];

	if((dp = nameiparent(path, name)) == 0)
		return 0;
	ilock(dp);

	if((ip = dirlookup(dp, name, &off)) != 0){
		iunlockput(dp);
		ilock(ip);
		if(type == T_FILE && ip->type == T_FILE)
			return ip;
		iunlockput(ip);
		return 0;
	}

	if((ip = ialloc(dp->dev, type)) == 0)
		panic("create: ialloc");

	ilock(ip);
	ip->major = major;
	ip->minor = minor;
	ip->nlink = 1;
	iupdate(ip);

	if(type == T_DIR){	// Create . and .. entries.
		dp->nlink++;	// for ".."
		iupdate(dp);
		// No ip->nlink++ for ".": avoid cyclic ref count.
		if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
			panic("create dots");
	}

	if(dirlink(dp, name, ip->inum) < 0)
		panic("create: dirlink");

	iunlockput(dp);

	return ip;
}

int
sys_open(void)
{
	char *path;
	int fd, omode;
	struct file *f;
	struct inode *ip;

	if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
		return -1;

	begin_op();

	if(omode & O_CREATE){
		ip = create(path, T_FILE, 0, 0);
		if(ip == 0){
			end_op();
			return -1;
		}
	} else {
		if((ip = namei(path)) == 0){
			end_op();
			return -1;
		}
		ilock(ip);
		if(ip->type == T_DIR && omode != O_RDONLY){
			iunlockput(ip);
			end_op();
			return -1;
		}
	}

	if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
		if(f)
			fileclose(f);
		iunlockput(ip);
		end_op();
		return -1;
	}
	iunlock(ip);
	end_op();

	f->type = FD_INODE;
	f->ip = ip;
	f->off = 0;
	f->readable = !(omode & O_WRONLY);
	f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

	set_charp_argument(path, FIRST, SYS_open);
	set_int_argument(omode, SECOND, SYS_open);

	return fd;
}

int
sys_mkdir(void)
{
	char *path;
	struct inode *ip;

	begin_op();
	if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0)) == 0){
		end_op();
		return -1;
	}
	iunlockput(ip);
	end_op();
	set_charp_argument(path, FIRST, SYS_mkdir);

	return 0;
}

int
sys_mknod(void)
{
	struct inode *ip;
	char *path;
	int major, minor;

	begin_op();
	if((argstr(0, &path)) < 0 ||
		 argint(1, &major) < 0 ||
		 argint(2, &minor) < 0 ||
		 (ip = create(path, T_DEV, major, minor)) == 0){
		end_op();
		return -1;
	}
	iunlockput(ip);
	end_op();
	set_charp_argument(path, FIRST, SYS_mknod);
	set_int_argument(major, SECOND, SYS_mknod);
	set_int_argument(minor, THIRD, SYS_mknod);

	return 0;
}

int
sys_chdir(void)
{
	char *path;
	struct inode *ip;
	struct proc *curproc = myproc();
	
	begin_op();
	if(argstr(0, &path) < 0 || (ip = namei(path)) == 0){
		end_op();
		return -1;
	}
	ilock(ip);
	if(ip->type != T_DIR){
		iunlockput(ip);
		end_op();
		return -1;
	}
	iunlock(ip);
	iput(curproc->cwd);
	end_op();
	curproc->cwd = ip;
	set_charp_argument(path, FIRST, SYS_chdir);

	return 0;
}

int
sys_exec(void)
{
	char *path, *argv[MAXARG];
	int i;
	uint uargv, uarg;

	if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
		return -1;
	}
	memset(argv, 0, sizeof(argv));
	for(i=0;; i++){
		if(i >= NELEM(argv))
			return -1;
		if(fetchint(uargv+4*i, (int*)&uarg) < 0)
			return -1;
		if(uarg == 0){
			argv[i] = 0;
			break;
		}
		if(fetchstr(uarg, &argv[i]) < 0)
			return -1;
	}

	set_charp_argument(path, FIRST, SYS_exec);
	set_pointer_argument((uint)argv, SECOND, SYS_exec);
	return exec(path, argv);
}

int
sys_pipe(void)
{
	int *fd;
	struct file *rf, *wf;
	int fd0, fd1;

	if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
		return -1;

	set_pointer_argument((uint)fd, FIRST, SYS_pipe);

	if(pipealloc(&rf, &wf) < 0)
		return -1;
	fd0 = -1;
	if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
		if(fd0 >= 0)
			myproc()->ofile[fd0] = 0;
		fileclose(rf);
		fileclose(wf);
		return -1;
	}
	fd[0] = fd0;
	fd[1] = fd1;
	return 0;
}
