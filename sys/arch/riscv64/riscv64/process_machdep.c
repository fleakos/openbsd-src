/*	$OpenBSD: process_machdep.c,v 1.4 2021/05/14 06:48:52 jsg Exp $	*/

/*
 * Copyright (c) 2014 Patrick Wildt <patrick@blueri.se>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file may seem a bit stylized, but that so that it's easier to port.
 * Functions to be implemented here are:
 *
 * process_read_regs(proc, regs)
 *	Get the current user-visible register set from the process
 *	and copy it into the regs structure (<machine/reg.h>).
 *	The process is stopped at the time read_regs is called.
 *
 * process_write_regs(proc, regs)
 *	Update the current register set from the passed in regs
 *	structure.  Take care to avoid clobbering special CPU
 *	registers or privileged bits in the PSL.
 *	The process is stopped at the time write_regs is called.
 *
 * process_sstep(proc, sstep)
 *	Arrange for the process to trap or not trap depending on sstep
 *	after executing a single instruction.
 *
 * process_set_pc(proc)
 *	Set the process's program counter.
 */

#include <sys/param.h>

#include <sys/proc.h>
#include <sys/ptrace.h>
#include <sys/systm.h>
#include <sys/user.h>

#include <machine/pcb.h>
#include <machine/reg.h>

int
process_read_regs(struct proc *p, struct reg *regs)
{
	struct trapframe *tf = p->p_addr->u_pcb.pcb_tf;

	memcpy(&regs->r_t[0], &tf->tf_t[0], sizeof(regs->r_t));
	memcpy(&regs->r_s[0], &tf->tf_s[0], sizeof(regs->r_s));
	memcpy(&regs->r_a[0], &tf->tf_a[0], sizeof(regs->r_a));
	regs->r_ra = tf->tf_ra;
	regs->r_sp = tf->tf_sp;
	regs->r_gp = tf->tf_gp;
	regs->r_tp = tf->tf_tp;//following Freebsd
	//regs->r_tp = (uint64_t)p->p_addr->u_pcb.pcb_tcb;//XXX why?
	//XXX freebsd adds the following two fields so we just follow.
	regs->r_sepc = tf->tf_sepc;
	regs->r_sstatus = tf->tf_sstatus;

	return(0);
}

#if 0
int
process_read_fpregs(struct proc *p, struct fpreg *regs)
{
	if (p->p_addr->u_pcb.pcb_flags & PCB_FPU)
		memcpy(regs, &p->p_addr->u_pcb.pcb_fpstate, sizeof(*regs));
	else
		memset(regs, 0, sizeof(*regs));

	return(0);
}
#endif
#ifdef	PTRACE

int
process_write_regs(struct proc *p, struct reg *regs)
{
	struct trapframe *tf = p->p_addr->u_pcb.pcb_tf;

	memcpy(&tf->tf_t[0], &regs->r_t[0], sizeof(tf->tf_t));
	memcpy(&tf->tf_s[0], &regs->r_s[0], sizeof(tf->tf_s));
	memcpy(&tf->tf_a[0], &regs->r_a[0], sizeof(tf->tf_a));
	tf->tf_ra = regs->r_ra;
	tf->tf_sp = regs->r_sp;
	tf->tf_gp = regs->r_gp;
	tf->tf_tp = regs->r_tp; //XXX
	tf->tf_sepc = regs->r_sepc;
	//p->p_addr->u_pcb.pcb_tcb = (void *)regs->r_tp;//XXX why? freebsd just copied r_tp to tf_tp
	//XXX should we add r_sepc and sstatus also?
	return(0);
}

#if 0
int
process_write_fpregs(struct proc *p,  struct fpreg *regs)
{
	p->p_addr->u_pcb.pcb_flags |= PCB_FPU;
	memcpy(&p->p_addr->u_pcb.pcb_fpstate, regs,
	    sizeof(p->p_addr->u_pcb.pcb_fpstate));
	return(0);
}
#endif

int
process_sstep(struct proc *p, int sstep)
{
#if 0
	//XXX TODO
	struct trapframe *tf = p->p_addr->u_pcb.pcb_tf;

	if (sstep) {
		p->p_addr->u_pcb.pcb_flags |= PCB_SINGLESTEP;
		tf->tf_spsr |= PSR_SS;
	} else {
		p->p_addr->u_pcb.pcb_flags &= ~(PCB_SINGLESTEP);
		tf->tf_spsr &= ~PSR_SS;
	}
	return 0;
#endif
	return (EOPNOTSUPP);
}

int
process_set_pc(struct proc *p, caddr_t addr)
{
	struct trapframe *tf = p->p_addr->u_pcb.pcb_tf;
	tf->tf_sepc = (uint64_t)addr;
	return (0);
}

#endif	/* PTRACE */
