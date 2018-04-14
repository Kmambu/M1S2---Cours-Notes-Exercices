# Question 1

```c
static inline struct hlist_bl_head *d_hash(unsigned int hash)
{
	return dentry_hashtable + (hash >> d_hash_shift);
}
```

La taille de la table est de 2^(32-d_hash_shift)
