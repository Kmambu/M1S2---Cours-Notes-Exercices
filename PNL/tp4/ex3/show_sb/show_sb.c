#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_DESCRIPTION("Prints to the Kernel message console a list of the superblocks");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");
static void print_sb(struct super_block *sb, void* unused)
{
	u8 *b = sb->s_uuid;

	pr_info("uuid=%.2x%.2x%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x type=%s\n",
			b[0],b[1],b[2],b[3],
			b[4],b[5],
			b[6],b[7],
			b[8],b[9],
			b[10],b[11],b[12],b[13],b[14],b[15],
			(sb->s_type)->name);
}
static int __init hello_init(void)
{
	iterate_supers(print_sb, NULL);
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	pr_info("Goodbye, cruel world\n");
}
module_exit(hello_exit);

