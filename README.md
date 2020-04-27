# Project 8

## Readings
- [p8.pdf](https://github.com/sklaw/enee447project8_hw_template_Shuangqi_sessions/blob/master/p8.pdf)
  - Pay attention to the following points:
    - There're multiple formats for a page entry. In this project we will _**only**_ use the `section` format.
    
- [official ARM doc](https://static.docs.arm.com/ddi0406/c/DDI0406C_C_arm_architecture_reference_manual.pdf)

## Places that require implementations
- https://github.com/sklaw/enee447project8_hw_template_Shuangqi_sessions/blob/master/threads.c#L109
- https://github.com/sklaw/enee447project8_hw_template_Shuangqi_sessions/blob/master/vm.c#L20
- https://github.com/sklaw/enee447project8_hw_template_Shuangqi_sessions/blob/master/vm.c#L29
- https://github.com/sklaw/enee447project8_hw_template_Shuangqi_sessions/blob/master/vm.c#L40
- https://github.com/sklaw/enee447project8_hw_template_Shuangqi_sessions/blob/master/vm.c#L79
  
## NOTE
- Since our application binaries are all smaller than 1 MB, they can all be placed within just a page. So no page fault will happen. And that's why we didn't implement page-fault handling in this code.
- Permission bits in page table entries: we don't use them. So you don't need to initialize them.
