# TP4 : Serveur Web minimaliste pour la commande des LEDs et la lecture du bouton poussoir

# Objectif

Le but de ce TME est de créer un site web consultable par un browser web permettant de changer l'état des leds. Le serveur web est écrit en Python, à la réception des requêtes du client, il exécute des scripts CGI (Common Gateway Interface) écrit également en Python pour produire des pages HTML dynamiques. Les scripts CGI devront communiquer avec le programme écrit en C contrôlant les LEDs. La communication entre les scripts et le programme de contrôle se fera par fifo UNIX. Le programme en C sera : soit la version permettant le contrôle depuis une application utilisateur (avec les droits de root), soit par une application utilisateur qui communique avec un driver. 

Pour ce faire, nous allons procéder en deux temps.
	1) Nous allons faire communiquer un programme python avec un programme C par FIFO.
	2) Nous allons créer un serveur local sur le PC de développement et le faire communiquer avec le programme C.
	3) Nous allons mettre le serveur sur une raspberry PI et communiquer avec le programme C
	4) Nous allons remplacer le programme C par le programme de contrôle des LEDs.

## 1. Commumication par FIFO
### *writer.py*
	* La FIFO est créée dans le répertoire /tmp, à l'aide de la fonction mkfifo(). Il ne faut pas confondre cet appel, qui permet de créer la FIFO sans l'ouvrir, et open(), qui prend en argument le chemin vers le fichier à ouvrir (ici la FIFO) et un mode d'ouverture.
	* Plusieurs raisons à tester l'existence du fichier :
		1) Il est possible que la commande mkfifo() ait échoué et on doit s'assurer de la bonne exécution de cette méthode
		2) Parce qu'un bon programmeur teste les valeurs de retours de ses méthodes ('-_-)
	* La fonciton flush() force l'écriture des données en tampon dans le fichier désigné par le descripteur de fichier, puis vide ledit tampon.
	* Le producteur ne ferme pas la FIFO parce qu'il est possible que le consommateur n'aie pas encore fini de consommer. Il vaut alors mieux laisser au consommateur le soin de fermer la ressource.

### *reader.py*
	* readline() permet de lire depuis un fichier une ligne
	* L'écrivain est chargé d'écrire dans la FIFO tandis que le lecteur est chargé de lire dans la FIFO et d'afficher dans la sortie standard les valeurs de la FIFO. L'écrivain seul n'apporte aucun feedback.

## 2. Création d'un serveur fake

