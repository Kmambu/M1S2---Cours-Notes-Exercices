#################################################################################
#	File : reset.s
#	Author : Alain Greiner
#	Date : 25/12/2011
#################################################################################
#       This is an improved boot code for a bi-processor architecture.
#	Depending on the proc_id, each processor
#       - initializes the interrupt vector.
#       - initializes the ICU MASK registers.
#       - initializes the TIMER .
#       - initializes the Status Register.
#       - initializes the stack pointer.
#       - initializes the EPC register, and jumps to the user code.
#################################################################################
		
	.section .reset,"ax",@progbits

	.extern	seg_stack_base
	.extern	seg_data_base
	.extern seg_icu_base

	.func	reset
	.type   reset, %function

reset:
       	.set noreorder

# get the processor id
        mfc0	$27,	$15,	1		# get the proc_id
        andi	$27,	$27,	0x1		# no more than 2 processors
        bne	$27,	$0,	proc1

proc0:
        # initialises interrupt vector entries for PROC[0]
	la	$26,	_interrupt_vector
	la	$27,	_isr_timer
	sw	$27,	8($26)			# TIMER[0] <-- _isr_timer
	la	$27,	_isr_tty_get
	sw	$27,	12($26)			# TTY[0] <-- _isr_tty_get

        #initializes the ICU[0] MASK register
	la	$26,	seg_icu_base		# $26 <= seg_icu_base[0]
	addiu	$27,	$0,	-1		# $27 <= 0xFFFF FFFF
	sw	$27,	0xC($26)		# ICU_MASK_RESET <= 0xFFFF FFFF
	ori	$27,	$0,	0xC		# TIMER[0] and TTY[0] IRQs on
	sw	$27,	0x8($26)		# ICU_MASK_SET <= 0b0000 1100

        # initializes TIMER[0] PERIOD and RUNNING registers
	la 	$26,	seg_timer_base
	ori	$27,	$0,	50000
	sw	$27,	0x8($26)		# TIMER_PERIOD[0] <= 50000 cy
	sw	$27,	0x4($26)		# TIMER_RUNNING <= true

        # initializes stack pointer for PROC[0]
	la	$29,	seg_stack_base
        li	$27,	0x10000			# stack size = 64K
	addu	$29,	$29,	$27    		# $29 <= seg_stack_base + 64K

        # initializes SR register for PROC[0]
       	li	$26,	0x0000FF13	
       	mtc0	$26,	$12			# SR <= 0x0000FF13

        # jump to main in user mode: main[0]
	la	$26,	seg_data_base
        lw	$26,	0($26)			# $26 <= main[0]
	mtc0	$26,	$14			# write it in EPC register
	eret

proc1:
        # initialises interrupt vector entries for PROC[1]
	la	$26,	_interrupt_vector
	la	$27,	_isr_timer
	sw	$27,	16($26)			# TIMER[1] <-- _isr_timer
	la	$27,	_isr_tty_get
	sw	$27,	20($26)			# TTY[1] <-- _isr_tty_get

        #initializes the ICU[1] MASK register
	la	$26,	seg_icu_base
	addiu	$26,	$26,	32		# $26 <= seg_icu_base[1]
	addiu	$27,	$0,	-1		# $27 <= 0xFFFFFFFF
	sw	$27,	0x0C($26)		# ICU_MASK_RESET <= 0xFFFFFFFF
	ori	$27,	$0,	0x30		# TIMER[1] and TTY[1] IRQs on
	sw	$27,	0x8($26)		# ICU_MASK_SET <= 0b0011 0000
	
        # initializes TIMER[1] PERIOD and RUNNING registers
	la 	$26,	seg_timer_base
	ori	$27,	$0,	1
	sll	$27,	$27,	4
	or	$26,	$26,	$27		# $26 <= seg_timer_base[procid]
	li	$27,	100000
	sw	$27,	0x8($26)		# TIMER_PERIOD[0] <= 100000 cy
	sw	$0,	0x4($26)		# TIMER_RUNNING <= false

        # initializes stack pointer for PROC[1]
	la	$29,	seg_stack_base
        li	$27,	0x20000			# stack size = 128K
	addu	$29,	$29,	$27    		# $29 <= seg_stack_base + 128K

        # initializes SR register for PROC[1]
       	li	$26,	0x0000FF13	
       	mtc0	$26,	$12			# SR <= 0x0000FF13

        # jump to main in user mode: main[1]
	la	$26,	seg_data_base
        lw	$26,	4($26)			# $26 <= main[1]
	mtc0	$26,	$14			# write it in EPC register
	eret

	.set reorder

	.set reorder

	.endfunc
	.size	reset, .-reset

