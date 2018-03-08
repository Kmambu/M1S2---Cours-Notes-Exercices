# TP-04 --- Mes premiers modules

## Exercice 1 : Mes premiers modules
### Question 1
Le fichier qui sera chargé dans le fil d'exécution du noyau est le fichier .ko.

### Question 2

### Question 3
On peut voir affiché les printk sur :
	* la console du kernel
	* la sortie de la commande dmesg
	* la sortie de la commande journalctl -xe
	* cat /proc/kmsg

### Question 4

### Question 5

### Question 6

## Exercice 2 : Modification d'une variable du noyau à l'aide d'un module
### Question 1
La variable __init_uts_ns__ est instanciée et exportée dans le fichier init/version.c.
Parce que cette variable ets exportée, elle peut bien être accedée par les autres modules du noyau.

### Question 2

### Question 3
??? Je ne comprend pas pourquoi restaurer le nom d'origine est "impératif" (hormis bien sûr en le sens qu'autrement le nom d'origine est perdu, et potentiellement un programme évalue sa compatibilité avec le système via le nom de noyau de ce dernier). le champ uts_namespace.uts_name.sysname est évoqué nul part en dehors de version.h, pour son initialisation.

## Les limites des modules
### #Question 1
Là, il y a un "petit" problème : iterate_supers() n'est pas par défaut exporté dans fs/super.c, donc impossible de l'appeller depuis notre module show_sb(). À partir de là, deux solutions sont possibles :
	1) Ajouter les incusions et le code de iterate_supers() dans le module (lol)
	2) Patcher le noyau en conséquence (ici plus envisageable)
*Comment créer un patch sous environement Linux?*
On veut patcher ici le fichier super.c, contenant la méthode iterate_blocks() :
	* copier le fichier source : 
	> cp super.c super.c.new
	* modifier la copie du fichier en conséquence de la maodification à apporter
	* créer un *diff* du fichier, ce fichier indiquera les modifications à apporter au fichier d'origine. Utiliser des diff est plus avantageux sur biens des points, notamment la portabilité de la solution.
	> diff -u super.c super.c.new > super.patch
	* Il ne reste plus qu'à aller dans le dossier source de Linux et appliquer le patch avec la commande :
	> patch < super.patch
Maintenant, on peut re-compiler notre noyau afin qu'il permettent au modules chargés dynamiquement d'utiliser iterate_supers() :D

### Question 2

