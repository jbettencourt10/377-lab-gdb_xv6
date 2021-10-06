# COMPSCI 377 LAB: GDB & Xv6

## Purpose

This lab is designed to familiarize students with the Xv6 learning operating system and additionally familiarize students with the GDB debugging module. Please make sure that all of your answers to questions in these labs come from work done on the Edlab environment – otherwise, they may be inconsistent results and will not receive points.

Please submit your answers to this lab on Gradescope in the assignment marked “Lab #3’. All answers are due by the time specified on Gradescope. The TA present in your lab will do a brief explanation of the various parts of this lab, but you are expected to answer all questions by yourself. Please raise your hand if you have any questions during the lab section – TAs will be notified you are asking a question. Questions and Parts have a number of points marked next to them to signify their weight in this lab’s final grade. Labs are weighted equally, regardless of their total points.

Once you have logged in to Edlab, you can clone this lab repo using

```bash
git clone https://github.com/jbettencourt10/gdb_xv6.git
```

Then, clone the Xv6 repo into the 377-lab-gdb_xv6 using 

```bash
git clone https://github.com/mit-pdos/xv6-public.git
```

Finally, you can use `cd` to open the directory you just cloned:

```bash
cd 377-lab-gdb_xv6
```

Both folders include Makefiles that allow you to locally compile the lab executable and Xv6 OS and run all the sample code listed in this tutorial. You can compile them by running `make`. Feel free to modify the source files yourself, after making changes you can run `make` again to build new binaries from your modified files. You can also use `make clean` to remove all the built files, this command is usually used when something went wrong during the compilation so that you can start fresh.

## Part 1: Xv6 (5 Points)

Xv6 is a small operating system designed by MIT for students to learn how operating systems work. It includes several features we have or will learn about, like a scheduler, processes, file system, and system calls. For the sake of this lab, we will be investigating how Xv6 schedules processes, handles I/O, and makes systems calls by looking through the source code available to us. 

proc.c
```c++
//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
}

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
```

From the above files, and the files listed on gradescope, you should be able to answer the questions. Additionally, there exists a wide range of documentation about Xv6 on the internet, so feel free to look for that if you find it necessary.

## Part 2: GDB (5 Points)

We have gone over basic GDB before in lab, but today will be focused around utilizing GDB with threading to debug programs that use multiple threads. To run gdb on a process that uses multiple threads, simply open it with gdb like any other executable (gdb \_\_\_\_, where \_\_\_\_ is the name of the executable). When you do so, you should be able to see output as such:
 
 \[New Thread 0x7f85e5bce700 (LWP 364)]

As can be seen in the screenshot above, the memory address is shown for the created thread in the form 0x\_\_\_\_, and the LWP address for the thread is shown after LWP. The LWP address of a thread isn’t something we will work with in this class, but it useful to know that it is the identifier assigned to the process by the operating system. In the above image, the address of the thread is 0x7f85e5bce700, and the LWP of the thread is 364.

We can also find out more information about threads through our use of gdb. The ‘info threads’ command will list the info in the screenshot for all currently running threads from the process, as well as the id of its progenitor thread. The thread we are currently in will have an asterisk to its left. Threads that request time to wait will also show the amount of time they requested and need – the threads in threading.cpp will do this when run via gdb. Each of the threads also has a local ID listed on the right – using gdb, we can switch between these threads with the command ‘thread \_\_\_\_’, where \_\_\_\_ is the id of the thread we are trying to switch to.

## Part 3: Using Xv6 and GBD together (5 Points)

Threads can be extremely useful, but they can also encounter errors when they attempt to modify the same memory as one another. For example, if two threads each try to increment a value by 1, it may be that both operations occur at the same time, and the value is only incremented once instead of the two times it should be incremented instead. For this reason, we use various techniques to ensure that critical data components are not modified outside of their desired scope. One of these structures is called a lock, which is shared between threads and acts as per its name to prevent other threads from accessing sensitive data while it is locked. Please look at the modified threading.cpp below, called threading_lock.cpp:

threading_lock.cpp
```c++

```

As you can see when running this code, thread #1 will always finish printing out its 10 statements before thread #3 prints out any of its 10 statements. This is because when we lock mtx at the start of truth(), it prevents further calls of truth() to progress past that point until we call mtx.unlock() from the same thread that locked it. In essence, this allows us to stop any threads that rely on mtx, ensuring that certain pieces of code do not run at the same time as one another even if we want multiple processes to be running simultaneously.
