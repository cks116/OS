#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>

asmlinkage int sys_my_new(int isStart, int pid, unsigned long __user *start_sec, unsigned long __user *start_nsec)
{
	struct timespec t;	

	unsigned long *kstart_sec = &t.tv_sec, *kstart_nsec = &t.tv_nsec;

	getnstimeofday(&t);

	if (isStart) 
	{
		kstart_sec = &t.tv_sec;
		kstart_nsec = &t.tv_nsec;
		copy_to_user(start_sec, kstart_sec, sizeof(t.tv_sec));
		copy_to_user(start_nsec, kstart_nsec, sizeof(t.tv_nsec));
	}
	else
	{
		unsigned long end_sec, end_nsec;
		end_sec = t.tv_sec;
		end_nsec = t.tv_nsec;
		copy_from_user(kstart_sec, start_sec, sizeof(t.tv_sec));
		copy_from_user(kstart_nsec, start_nsec, sizeof(t.tv_nsec));	
				
		printk("[Project1] %d %lu.%09lu %lu.%09lu\n", pid, *kstart_sec, *kstart_nsec, end_sec, end_nsec);
 	}

	return 0;
}
