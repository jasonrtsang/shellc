
# CMPT 300 - Creating a shell that runs in a Linux shell... (shellception)

######Ref:
- http://cmpt-300.sfucloud.ca/cmpt-300/wp-content/uploads/2016/09/A1P2.pdf
- https://www.gnu.org/software/libc/manual/html_node

Notes:
- Internal commands reference the behaviour of the Ubuntu Terminal
- Jobs internal command is implemented without JOBSPEC
- bg and fg will return the most recent backgrounded process, else use job number to retrieve desired
- job flag (+) will show for the most recent process, if none exist (i.e. killed), bg and fg will return nothing

Source Code: myshell.c, myjobs.c | Header Files: myshell.h, myjobs.h | Executable Name: my_shell

For a fresh compile run "make clean" followed by "make all"

Testing Procedure:

    Testing bg and fg (with/ without arguements), jobs and the ability to kill and stop a process:
    - Load 3 sleep 100 proesses in the background. Stop, kill and let them run

    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ ./my_shell 
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ sleep 100 &
    [1]+ 2107
    my_shell > Jobs: 1 - /media/sf_cmpt_300/a1p2$ sleep 100 &
    [2]+ 2110
    my_shell > Jobs: 2 - /media/sf_cmpt_300/a1p2$ sleep 100 &
    [3]+ 2111
    my_shell > Jobs: 3 - /media/sf_cmpt_300/a1p2$ jobs
    [1]  2107    Running - B         sleep
    [2]  2110    Running - B         sleep
    [3]+ 2111    Running - B         sleep
    my_shell > Jobs: 3 - /media/sf_cmpt_300/a1p2$ ps
      PID TTY          TIME CMD
     1857 pts/17   00:00:00 bash
     2100 pts/17   00:00:02 my_shell
     2107 pts/17   00:00:00 sleep
     2110 pts/17   00:00:00 sleep
     2111 pts/17   00:00:00 sleep
     2112 pts/17   00:00:00 ps
    my_shell > Jobs: 3 - /media/sf_cmpt_300/a1p2$ fg 1
    ^Z
    [1]+ 2107    Stopped         sleep
    my_shell > Jobs: 3 - /media/sf_cmpt_300/a1p2$ jobs
    [1]+ 2107    Stopped         sleep
    [2]  2110    Running - B         sleep
    [3]  2111    Running - B         sleep
    my_shell > Jobs: 3 - /media/sf_cmpt_300/a1p2$ bg 1
    my_shell > Jobs: 3 - /media/sf_cmpt_300/a1p2$ jobs
    [1]+ 2107    Running - B         sleep
    [2]  2110    Running - B         sleep
    [3]  2111    Running - B         sleep
    my_shell > Jobs: 3 - /media/sf_cmpt_300/a1p2$ fg 2
    ^C
    [2]+ 2110    Killed          sleep
    my_shell > Jobs: 2 - /media/sf_cmpt_300/a1p2$ jobs
    [1]  2107    Running - B         sleep
    [3]  2111    Running - B         sleep
    my_shell > Jobs: 2 - /media/sf_cmpt_300/a1p2$ fg
    my_shell > Jobs: 2 - /media/sf_cmpt_300/a1p2$ fg 3
    ^Z
    [3]+ 2111    Stopped         sleep
    my_shell > Jobs: 2 - /media/sf_cmpt_300/a1p2$ jobs
    [1]  2107    Running - B         sleep
    [3]+ 2111    Stopped         sleep
    my_shell > Jobs: 2 - /media/sf_cmpt_300/a1p2$ bg
    my_shell > Jobs: 2 - /media/sf_cmpt_300/a1p2$ jobs
    [1]  2107    Running - B         sleep
    [3]+ 2111    Running - B         sleep
    my_shell > Jobs: 2 - /media/sf_cmpt_300/a1p2$ 
    [1]  2107    Done            sleep
    [3]+ 2111    Done            sleep
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ 

    Testing cd:
    - Go to previous directory and go back. Then try changing directory to an invalid file

    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ ./my_shell
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ pwd
    /media/sf_cmpt_300/a1p2
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ ls
    a1p2.sublime-workspace  Makefile  myjobs.c  myjobs.h  myjobs.o  my_shell  myshell.c  myshell.h  myshell.o  README.md  testfile
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ cd ..
    my_shell > Jobs: 0 - /media/sf_cmpt_300$ pwd
    /media/sf_cmpt_300
    my_shell > Jobs: 0 - /media/sf_cmpt_300$ ls 
    a1p1  a1p2  a1p2 2.zip  a1p2 3.zip  a1p2 4.zip  a1p2 5.zip  a1p2 6.zip  a1p2.zip
    my_shell > Jobs: 0 - /media/sf_cmpt_300$ cd a1p2
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ ls
    a1p2.sublime-workspace  Makefile  myjobs.c  myjobs.h  myjobs.o  my_shell  myshell.c  myshell.h  myshell.o  README.md  testfile
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ cd testfile
    cd: Not a directory
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ 

    Testing exit:
    - Input exit

    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ ./my_shell
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ exit
    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ 

    Testing exit with CTRL + D:
    - Input CTRL + C then CTRL + D

    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ ./my_shell
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ ^C
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ 
    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ 

    Testing Piping:
    - Grep for myjobs.h file in foreground, then background

    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ ./my_shell
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ ls -l
    total 108
    -rwxrwx--- 1 root vboxsf 12705 Oct  3 19:00 a1p2.sublime-workspace
    -rwxrwx--- 1 root vboxsf   236 Oct  2 15:58 Makefile
    -rwxrwx--- 1 root vboxsf  3955 Oct  5 23:10 myjobs.c
    -rwxrwx--- 1 root vboxsf  1793 Oct  5 22:47 myjobs.h
    -rwxrwx--- 1 root vboxsf  3808 Oct  7  2016 myjobs.o
    -rwxrwx--- 1 root vboxsf 19496 Oct  7  2016 my_shell
    -rwxrwx--- 1 root vboxsf 19579 Oct  7  2016 myshell.c
    -rwxrwx--- 1 root vboxsf  3338 Oct  7 14:38 myshell.h
    -rwxrwx--- 1 root vboxsf 14248 Oct  7  2016 myshell.o
    -rwxrwx--- 1 root vboxsf  8342 Oct  7  2016 README.md
    -rwxrwx--- 1 root vboxsf   210 Oct  7 15:16 testfile
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ ls -l | grep myjobs | grep .h
    -rwxrwx--- 1 root vboxsf  1793 Oct  5 22:47 myjobs.h
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ ls -l | grep myjobs | grep .h &
    [1]  3529
    [2]  3530
    [3]  3531
    my_shell > Jobs: 3 - /media/sf_cmpt_300/a1p2$ -rwxrwx--- 1 root vboxsf  1793 Oct  5 22:47 myjobs.h

    [2]  3530    Done            grep
    [3]  3531    Done            grep
    [1]  3529    Done            ls
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ 

    - Testing example piping command on assingment on Ubuntu Terminal vs my_shell

    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ cat testfile | gzip -c | gunzip -c | tail -n 10
    Testfile11
    Testfile12
    Testfile13
    Testfile14
    Testfile15
    Testfile16
    Testfile17
    Testfile18
    Testfile19
    Testfile20jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ ./my_shell 
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ cat testfile | gzip -c | gunzip -c | tail -n 10
    Testfile11
    Testfile12
    Testfile13
    Testfile14
    Testfile15
    Testfile16
    Testfile17
    Testfile18
    Testfile19
    Testfile20my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ 

    Testing Invalid inputs:
    - Enter blank as input, enter & and | with no commands

    jason@Ubuntu-VirtualBox:/media/sf_cmpt_300/a1p2$ ./my_shell 
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ 
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ &
    Error: Must input command before background process '&'
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ |
    Error: Must input command before '|' token for piping.
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ ls -l |
    Error: Must input command after '|' token for piping.
    my_shell > Jobs: 0 - /media/sf_cmpt_300/a1p2$ 

Testing Platforms:
- Ubuntu 16.04.2, gcc version 5.4.0
- SFU CSIL Linux machines
