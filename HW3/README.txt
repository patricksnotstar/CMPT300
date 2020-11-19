To run the program do: make, then do ./OSSim

Design decisions I made:
* Init process will be blocked from doing P and V operations since they can only run when no other processes can so they do not need semaphore to go into critical sections. Plus init process cannot be blocked so it's pointless to do P and V on it.

* I allow the Init process to do send/receive operations without ever being blocked because the reason why other prcesses are blocked when doing send/receive is for synchronization between 2 processes but the Init process can only run when no other processes can so it doesn't need any synchronization. By doing it this way, I can make my program simpler.

* Calling quantum on Init does not do anything because no other processes can run. Same goes for when there is only 1 processes available because Init should not run when there is another process able to run.

* I used a list to store messaged SENT to each process so potentially each process can receive multple messages at once but they can only receive the most recent REPLY. The reason being a process can only be blocked by the send operation once and so it only needs 1 reply to be unblocked, no reason for me to store many replies which would require more overhead and memory to now keep track of which message is a reply and which one is a send. There are many reasons why someone might use reply to send a message but that would be a decision the user has to make not the OS.

* When a process is being killed or exited and it owns a semaphore, I made it so that it automatically release its semaphore. This way the user does not have to and I don't think there's a risk to this because if the user wants to kill a process, it will stop execution anyway so it doesn't need the semaphore anymore. If this is an actual running process then I would make the process undo everything it has done in the critical section before it releases its semaphore. The user has to be sure it is okay to kill a process. 

* I prevent a process from owning multiple semaphores because when a process is executing in one critical section, it's not executing in the other critical section so this would violate condition number 2 of the solution to critical section problem. and if there are 2 semaphores for the same critical section then it's a user fault.

* I prevent a process from doing a blocking operation like send and receive when it owns a semaphore because when it is blocked and other process cannot get in the critical section, that would also violate condition number 2 of the solution to critical section problem. I make the user to call V first instead of doing it automatically because the process is still alive in this case so I cannot arbitrarily release its semaphore so this would be a decision the user has to make.

* When calling TotalInfo, I only display the pid stored in each queue because I think that's enough. If a process is in ready queue 0 then you know it has priority 0 and in READY state. This simulation does not allow for changing priority so I think this is sufficient enough. Similarly, if a process is in send queue, you know its state is BLOCK so I just need to display the priority.

* Apart from the commands given in the assignment description, I also added command 'm' to display the menu again.
