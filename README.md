# boiOS
#### I have implemented the basic functionalities of a kernel, including:
* IDT
* Paging
* Heap Driver
* Keyboard Driver
* Timer Driver
* A Simple Shell
##### When the kernel runs the user is greeted with this message:
![image](https://github.com/AhmadSameh/boiOS/assets/65093152/ee4aebad-eac6-4434-bd03-613cefd7f42f)
##### The timer was used for this printing just to emulate a booting process and is not the actual time needed to boot  

##### And this is when the shell runs
![image](https://github.com/AhmadSameh/boiOS/assets/65093152/c0189412-9bfc-4932-a911-96a8bc9d4f89)
##### Note that the only process I can call so far is an echo

## Future Plans
##### This is still a very basic kernel and I still need to learn how to improve it to be a better functioning OS, including
* A better virtual filesystem
* Implement cd, mkdir and other file manipulation processes
* Improve heap, paging and fat16 implementations
* Add more filesystems drivers
