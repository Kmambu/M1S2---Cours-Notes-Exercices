# Le noyau et la gestion de la mémoire

* Question : je lui demande de la mémoire, comment le lui faire allouer?
* Réalité : la majorité de l'allocaiton mémoire par le noyau est faite -indépendamment du processus-

Problématique :
* comment libérer les ressources en s'assurant de ne pas faire de fuite mémoire
* Comment faire face aux problémes de fragmentation 

## Algorithmes de nettoyage (Garbage Collectors)

* Mark and Sweep : marquer les objets comme inateignables puis les supprimer
* Compteur de réferences :
	* Varier le nombre d'accès à la ressource
	* ref = 0 -> Ressource destructible
	* struct Kref (avant -> __atomic_t count__)
		* la gestion des types atomiques est spécifiques aux architecures matérielles.

> Kref\_get (struct Kref \*r)

> Kref\_put (struct Kref \*r, void(\*release)(struct Kref \*))
