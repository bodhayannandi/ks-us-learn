#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>

#define NETLINK_TEST_PROTOCOL 31

static struct sock *nl_sk = NULL;

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
    struct nlmsghdr *nlh_recv, *nlh_reply;
    int user_space_process_port_id;
    char *user_space_data;
    int user_space_data_len;
    char *kernel_reply[256];
    struct sk_buff *skb_out;
    
    printk(KERN_INFO "%s invoked\n",__FUNCTION__);
    
    nlh_recv = (struct nlmsghdr*) skb;
    user_space_process_port_id = nlh_recv->nlmsg_pid;
    
    nlmsg_dump(nlh_recv);
    
    printk(KERN_INFO "%s(%d): port id of the sending user space process = %u\n",__FUNCTION__,__LINE__,user_space_process_port_id);
    
    user_space_data = (char*)nlmsg_data(nlh_recv);
    user_space_data_len = skb->len;
    
    printk(KERN_INFO "%s(%d) user space has sent message: %s, skb->len=%d, nlh_recv->nlmsg_len=%d\n", __FUNCTION__, __LINE__, user_space_data, user_space_data_len, nlh_recv->nlmsg_len);
    
    if(nlh_recv->nlmsg_flags & NLM_F_ACK)
    {
        memset(kernel_reply,0,sizeof(kernel_reply));
        snprintf(kernel_reply, sizeof(kernel_reply), "Msg from process %d has been processed by kernel", nlh_recv->nlmsg_pid);
        skb_out = nlmsg_new(sizeof(kernel_reply), 0);
        nlh_reply = nlmsg_put(skb_out, 
        		0, //Port ID of kernal, is always 0
        		nlh_recv->nlmsg_seq, //sequence no of reply must match with seq no of receive header
        		NLMSG_DONE,
        		sizeof(kernel_msg), //Size of message can be exact size, but we are sending max size
        		0 //Flag field for now we can set it to 0
        		);
        strncpy(nlmsg_data(nlh_reply), kernel_reply, sizeof(kernel_reply);
        res = nlmsg_unicast(nl_sk, skb_out, user_space_process_port_id);
        if(res < 0)
        {
            printk(KERN_INFO "Error while sending data back to user-space\n");
            kfree_skb(skb_out);
        }
    }
    
}

static struct netlink_kernel_cfg hellonlcfg = {
    .input = hellonllkm_rcv_msg,
};

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

