# Processor_Cache
COP290 Project

Right now it ignores most settings of i_cache in cfg because only one set of settings was declaredin cache.c, REMIND change this.  
No Write Allocate option in cfg file so we assumed Write Allocate in all cases.

Assumed Split I-D cache for Processor Simulator and neglected perfect option for now.

Assumed parameters of memory are powers of two and memory size is atleast one block.

Assumption - Only Access Memory once for one instruction load or data load or data store instead of four times (equivalent to the assumption that block size >= 4 always).
