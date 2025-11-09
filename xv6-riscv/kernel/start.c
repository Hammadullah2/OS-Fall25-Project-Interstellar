#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

void main();
void timerinit();

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// entry.S jumps here in machine mode on stack0.
void
start()
{
  // set M Previous Privilege mode to Supervisor, for mret.
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);

  // set M Exception Program Counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_mepc((uint64)main);

  // disable paging for now.
  w_satp(0);

  // delegate all interrupts and exceptions to supervisor mode.
  w_medeleg(0xffff);
  w_mideleg(0xffff);
  w_sie(r_sie() | SIE_SEIE | SIE_STIE);

  // configure Physical Memory Protection to give supervisor mode
  // access to all of physical memory.
  w_pmpaddr0(0x3fffffffffffffull);
  w_pmpcfg0(0xf);

  // ask for clock interrupts.
  timerinit();

  // keep each CPU's hartid in its tp register, for cpuid().
  int id = r_mhartid();
  w_tp(id);

  //2a-start
  // --- Begin: enable FPU for supervisor/user ---
  //
  // Set the SSTATUS FS field to Initial so supervisor/user mode can
  // execute floating-point instructions without immediately trapping.
  //
  // FS (two bits) is at bit positions 13..14 in sstatus (mask 3<<13).
  // We set FS = 01 (Initial). We also clear fcsr to a known state.
  //
  {
    const unsigned long SSTATUS_FS = (3UL << 13);
    const unsigned long SSTATUS_FS_INITIAL = (1UL << 13);

    unsigned long s = r_sstatus();
    s &= ~SSTATUS_FS;                // clear current FS bits
    s |= SSTATUS_FS_INITIAL;         // set FS = 01 (Initial)
    w_sstatus(s);

    // Clear fcsr (floating-point control/status) so rounding mode and flags are zeroed.
    // Using inline CSR write here; r_fcsr/w_fcsr were added in riscv.h for convenience.
    asm volatile("csrw fcsr, zero");
  }
  // --- End: enable FPU for supervisor/user ---
  //2a-end

  // switch to supervisor mode and jump to main().
  asm volatile("mret");
}

// ask each hart to generate timer interrupts.
void
timerinit()
{
  // enable supervisor-mode timer interrupts.
  w_mie(r_mie() | MIE_STIE);
  
  // enable the sstc extension (i.e. stimecmp).
  w_menvcfg(r_menvcfg() | (1L << 63)); 
  
  // allow supervisor to use stimecmp and time.
  w_mcounteren(r_mcounteren() | 2);
  
  // ask for the very first timer interrupt.
  w_stimecmp(r_time() + 1000000);
}
