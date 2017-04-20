# Processor_Cache
##COP290 Project

1. Compute DRAM access Latency properly.
2. Add checks that Cache parameters are powers of two and Cache Size > Block Size.

3. Right now it ignores most settings of i_cache in cfg because only one set of settings was declaredin cache.c, REMIND change this.  
4. No Write Allocate option in cfg file so we assumed Write Allocate in all cases.

5. Assumed Split I-D cache for Processor Simulator and neglected perfect option for now.

6. Assumed parameters of memory are powers of two and memory size is atleast one block.

7. Assumption - Only Access Memory once for one instruction load or data load or data store instead of four times (equivalent to the assumption that block size >= 4 always).
