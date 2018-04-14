#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list_bl.h>
#include <linux/dcache.h>

#define BUFSIZE 64

MODULE_DESCRIPTION("Monitors a task");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");

MODULE_PARM_DESC(target, "Displays the address and size of the Dentry Table");

/******************************************************/
/***** module initialization and cleanup methodes *****/
/******************************************************/
int init_module(void)
{
	struct dentry *dentry;
	struct hlist_bl_head *head = dentry_hashtable;
	struct hlist_bl_node *node;
	int i=0, num=0, size = (1 << d_hash_shift);
	pr_warn("[weasel] Loading weasel module\n");
	pr_warn("[weasel] | dentry_hashtable=%p\n", head);
	pr_warn("[weasel] | size = %d\n", size);
	hlist_bl_lock(dentry_hashtable);
	for(i=0; i<d_hash_shift; i++)
	{
		head = dentry_hashtable + ((unsigned int)i >> (32 - d_hash_shift));
		hlist_bl_for_each_entry(dentry, node, head,d_u.d_in_lookup_hash)
		{
			num++;
		}
	}
	hlist_bl_unlock(dentry_hashtable);
	pr_warn("[weasel] | number of elements=%d\n", i);
	return 0;
}

void cleanup_module(void)
{
	pr_warn("[weasel] Unloading weasel module\n");
}
