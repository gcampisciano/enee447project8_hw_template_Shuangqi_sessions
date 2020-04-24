# Project 8


- Raspberry pi 3b+ has a ARMv8 CPU
- According to this website, when we are using 32-bit mode of the CPU, we need to look at MMU documentation of ARMv7
  - http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0438c/CACFEBDD.html:
  > In AArch32 state, the Armv8 address translation system resembles the Armv7 address translation system ......

- How to configure MMU
  - https://stackoverflow.com/questions/19544694/understanding-mrc-on-arm7
  - http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0360f/CHDGIJFB.html
- TLB
  - https://en.wikipedia.org/wiki/Translation_lookaside_buffer
  - http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0425/BABDHCBD.html
