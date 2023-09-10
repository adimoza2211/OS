SimpleShell implements basic functionality of the bash terminal

Main:
    main function uses while loop to keep scanning for new terminal commands
    inside while loop, we check if the command has a piped element or not
    if yes we execute it differently from one without piped elements
    function exits if "exit" is input.

isPiped():
    checks if the entered command is piped or not

exec_command():
    1.executes non piped commands
    2.forks as per the code given in lecture 6 and 7
    3.if child process generated, 
      we tokenise the command using the " " as a delimiter so we can pass the arguments
      for the execvp()
    4.necessary error handling is done for errors in forks and execvp()
    5.all the while we wait for the child process to finish as in lecture 6,7 notes using
    the wait() and waitpid() syscall

exec_piped():
    declares number of pipes and assumes that the max number of pipes is 10
    2.tokenises the command using the "|" as a delimeter so we can execute the commands independently
    and feed the output of one into the input of another

    3.initialises file descriptors for the pipes
    
    4.iterates over the piped commands and calls the pipe() syscall and forks

    5.if child process is created,
        6.we redirect stdin to the file if it is not the first command
        7.redirect stdout to the current pipes write
        8.execute the command
        9.tokenise the individual command based on " " as done in non piped command
        10.call execvp()
        11.close file descriptors
        12.handle errors related to file descriptors, failed forks
    13.all the while we wait for the child process to finish as in lecture 6,7 notes using
     the wait() syscall
    
Some functions that won't work on this shell:
    sudo: The sudo command allows authorized users to execute 
    commands with superuser privileges. For example, running 
    system administration tasks or installing software that requires 
    root access.

    su: The su command allows switching to another user account, 
    including the superuser (root). 
    It's used to execute commands as a different user.

    mount: Mounting file systems and devices often requires root
    privileges. For example, mounting a USB drive or network share:

    ptables: Configuring firewall rules using iptables requires 
    superuser privileges.



Contributions:

Aditya Moza: Implemented the execution of the non-piped and & commands.
Aarya Khadelwal: Implemented the execution of piped commands.
The history , handling of CTRL C and general error handling was done by both.



Link to Private GitHUb repository: https://github.com/adimoza2211/OS.git 
