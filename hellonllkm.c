#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>

#define NETLINK_TEST_PROTOCOL 31

static void nlmsg_dump(struct nlmsghdr *nlh)
{
    if(nlh == NULL) return;
    
    printk(KERN_INFO "Netlink message header: \'start dump\'...\n");
    printk("\t\'nlmsg_len\': %d\n", nlh->nlmsg_len);
    printk("\t\'nlmsg_type\': %d\n", nlh->nlmsg_type);
    printk("\t\'nlmsg_flags\': %d\n",nlh->nlmsg_flags);
    printk("\t\'nlmsg_seq\': %d\n", nlh->nlmsg_seq);
    printk("\t\'nlmsg_pid\': %d\n", nlh->nlmsg_pid);
    printk("...end dump\n"); 
}

void hellonllkm_rcv_msg(struct sk_buff *skb)
{
    
}

static struct netlink_kernel_cfg hellonlcfg = {
    .input = hellonllkm_rcv_msg,
};

static struct sock *nl_sk = NULL;

static int __init hellonllkm_start(void)
{
    printk(KERN_INFO "\'hellonllkm\' kernel module inserted now we can greet with userspace %d :)\n", 200);
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST_PROTOCOL, &hellonlcfg);
    
    if(!nl_sk)
    {
        printk(KERN_INFO "Failedto create hellonllkm netlink socket. No memory!!\n");
    	return -ENOMEM;
    }
    return 0;
}

static void __exit hellonllkm_exit(void)
{
    netlink_kernel_release(nl_sk);
    nl_sk = NULL;
    printk(KERN_INFO "\'hellonllkm\' kernel module removed from kernel.\n");
}

module_init(hellonllkm_start);
module_exit(hellonllkm_exit);

#define AUTHOR_NAME "Bodhayan Nandi"
#define MODULE_DESC "Netlink kernel module application to greet with user space"

MODULE_AUTHOR(AUTHOR_NAME);
MODULE_DESCRIPTION(MODULE_DESC);
MODULE_LICENSE("GPL");

