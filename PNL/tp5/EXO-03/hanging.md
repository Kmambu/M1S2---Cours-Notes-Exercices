# Mon premier bug

## Question 1 : Explication de hanging.c
- hanging.ko est un module qui crée un kthread hanging_thread, utilisant la fonction my_hanging_fn.
my_hanging_fn rend la tache non-interruptible puis attend via un appel à la fonctioon schedule_timeout() une durée de 60 secondes avant d'afficher dans la Kconsole "done waiting", puis retourne 0.

## Question 2 : Explication de la situation
Après le timeout d'une minute le noyau ne crashe pas, néanmoins une stsck trace est malgré tout imprimée. Cela pourrait être dû au fait que le module est fait dormir le noyau plus de 30 secondes

## Quesiton 3
Kernel Hacking
	-> Debug Lockups and Hangs
		-> Panic (Reboot) On Hung Tasks

## Question 4
Il ne s'agit de la backtrace du module mais celui dû au Kernel Panic

## Question 5
    [0]kdb> ps
    82 sleeping system daemon (state M) processes suppressed,
    use 'ps A' to see all.
    Task Addr               Pid   Parent [*] cpu State Thread             Command
    0xffff880007223200       15        2  1    0   R  0xffff880007223b80 *khungtaskd
    
    0xffff880007110000        1        0  0    0   S  0xffff880007110980  systemd
    0xffff880007220000       11        2  0    0   R  0xffff880007220980  watchdog/0
    0xffff880007223200       15        2  1    0   R  0xffff880007223b80 *khungtaskd
    0xffff8800072dcb00       27        2  0    0   R  0xffff8800072dd480  kworker/0:1
    0xffff880006455780      166        2  0    0   R  0xffff880006456100  kpktgend_0
    0xffff880006464b00      200        1  0    0   R  0xffff880006465480  systemd-journal
    0xffff880006466400      205        1  0    0   S  0xffff880006466d80  systemd-udevd
    0xffff880005d38000      265        1  0    0   S  0xffff880005d38980  systemd-logind
    0xffff880003ce0000      266        1  0    0   S  0xffff880003ce0980  dbus-daemon
    0xffff880003ce0c80      267        1  0    0   S  0xffff880003ce1600  agetty
    0xffff880003ce1900      268        1  0    0   S  0xffff880003ce2280  login
    0xffff880003ce2580      269        1  0    0   S  0xffff880003ce2f00  systemd
    0xffff880003ce5780      273      269  0    0   S  0xffff880003ce6100  (sd-pam)
    0xffff880004ef4b00      283      268  0    0   S  0xffff880004ef5480  bash
    0xffff880004ef0000      293        2  0    0   D  0xffff880004ef0980  my_hanging_fn
    [0]kdb> btp 293
    Stack traceback for pid 293
    0xffff880004ef0000      293        2  0    0   D  0xffff880004ef0980  my_hanging_fn
     ffff880003eb0780 ffff880003eb0780 ffff880004ef0000 ffff880004c18000
     ffff880007c19600 ffffc90000747df0 ffffffff81b3d6c2 ffffffff81b4d135
     00ffffff81b4d125 ffff880007c19600 ffffc90000747e30 00000000ffffe17f
    Call Trace:
     [<ffffffff81b3d6c2>] ? __schedule+0x242/0x700
     [<ffffffff81b4d135>] ? __clear_rsb+0x25/0x3d
     [<ffffffff81b3dbac>] schedule+0x2c/0x80
     [<ffffffff81b412d2>] schedule_timeout+0x82/0x450
     [<ffffffff811aa2c0>] ? del_timer_sync+0x50/0x50
     [<ffffffffa0000000>] ? 0xffffffffa0000000
     [<ffffffff81144914>] ? SyS_exit_group+0x14/0x20
     [<ffffffffa0000038>] my_hanging_fn+0x38/0x50 [hanging]
     [<ffffffff8115e2f9>] kthread+0xd9/0xf0
     [<ffffffff81b4d135>] ? __clear_rsb+0x25/0x3d
     [<ffffffff8115e220>] ? kthread_park+0x60/0x60
     [<ffffffff81003ac6>] ? do_syscall_64+0x76/0xe0
     [<ffffffff81b42b27>] ret_from_fork+0x57/0x70
    [0]kdb>

## Question 6
    Module                  Size  modstruct     Used by
    hanging                 1436  0xffffffffa0000180    0  (Live) 0xffffffffa0000000 [ ]

* La première adresse correspond à l'adresse mémoire de la _struct module_ du mondule hanging
* Le numéro et la seconde adresse sont respectivement le nombre de modules dépendant de hanging et l'adresse du KObject associé au module hanging

## Question 7
1) mettre un timeout plus restreint dans hanging.c
2) mettre un delai limite de hanging plus long dans le Kernel
