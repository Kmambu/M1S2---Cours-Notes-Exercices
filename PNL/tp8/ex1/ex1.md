# Question 1

Ce patch modifie la librairie kernel dcache, plus spécifiquement :
> Le patch exporte la variable d_hash_shift et dentry_hashtable.

# Question 2

En fait, le patch était sensé exporter uniquement dentry_hashtable, et il fallait alors ecrire un nouveau patch pour également exporter d_hash_shift ^^'

# Question 3


