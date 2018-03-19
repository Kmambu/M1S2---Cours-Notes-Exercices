# Quesiton 1
Le rôle de la _struct pid_ est d'identifier les processus ayant utilisé un même PID, y compris le processus courant ayant été assigné dit PID.

# Question 2
*utime* et *stime* sont tous les deux de types u64

# Question 3
* struct pid possède un réference sur une struct task_struct
* struct task_struct possède une struct pid
