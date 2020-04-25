# Project 8


## Readings
- [p8.pdf](https://github.com/sklaw/enee447project8_hw_template_Shuangqi_sessions/blob/master/p8.pdf)
  - Pay attention to the following points:
    - There're multiple formats for a page entry. In this project we will _**only**_ use the `section` format.
    
- [official ARM doc](https://static.docs.arm.com/ddi0406/c/DDI0406C_C_arm_architecture_reference_manual.pdf)

## The VM configuration used in this project
- Short-descriptor translation table format:
  - We use (PROCID, ASID, virtual page number) to identify a cached page table entry in [TLB](https://en.wikipedia.org/wiki/Translation_lookaside_buffer)
  - **NOTE**: in this project, we assume all threads have the same PROCID, 0. And we let ASID equal to thread id. 
- Section-based page entry: each entry of the page table provides a mapping from a 1MB virtual page to a 1MB physical page.
- A page table in this project has 4096 entries. Each entry is a 32-bit value.

## ARM VM setup
- Initialize `TTBCR` such that
  - we use the 32-bit translation system, with the Short-descriptor translation table format
  - We use only `TTBR0` to point to page table. Read [B3.5.4 in official ARM doc](https://static.docs.arm.com/ddi0406/c/DDI0406C_C_arm_architecture_reference_manual.pdf) to figure out how to achieve this by initializing `TTBCR` properly.
  
- Initialize `SCTLR` such that
  - MMU is enabled. Read [B4.1.130 in official ARM doc](https://static.docs.arm.com/ddi0406/c/DDI0406C_C_arm_architecture_reference_manual.pdf) to figure out how to achieve this by initializing `SCTLR` properly.

## NOTE
- Since our application binaries are all smaller than 1 MB, they can all be placed within just a page. So no page fault will happen. And that's why we didn't implement page-fault handling in this code.
