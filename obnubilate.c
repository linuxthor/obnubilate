#include <net/ip.h>
#include <net/tcp.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/crypto.h>
#include <crypto/aes.h>

struct packet_type net_if_proto;

void decrypt(char *buf, u8 *key)
{
    struct crypto_cipher *tfm;
    int    i, count, div, mod;
    char  dst[AES_BLOCK_SIZE];

    div = strlen(buf) / AES_BLOCK_SIZE;
    mod = strlen(buf) % AES_BLOCK_SIZE;
    if(mod > 0)
    {
        div++;
    }
    count = div;
    tfm   = crypto_alloc_cipher("aes", 0, 16);
    crypto_cipher_setkey(tfm, key, 16);
    for (i=0; i<count; i++)
    {
        crypto_cipher_decrypt_one(tfm,dst,buf);
        memcpy(buf,dst,AES_BLOCK_SIZE);
        buf = buf+AES_BLOCK_SIZE;
    }
}

void process_packet(struct sk_buff *skb)
{
    struct ethhdr      *eth;
    struct iphdr       *iph;
    struct tcphdr      *tph;
    unsigned char     *data;
    int src_port, dest_port;
    eth = eth_hdr(skb);
    iph = ip_hdr(skb);

    if(iph->protocol == 6)  // tcp
    {
        tph = (struct tcphdr*) (((char*) iph) + iph->ihl * 4);
        src_port  = htons(tph->source);
        dest_port = htons(tph->dest);
        data = (unsigned char *)((unsigned char *)tph + (tph->doff * 4));

        if(src_port == 42420 && tph->window == htons(128) && dest_port == 444)
        {
            // hping3 -V -S -p 444 -s 42420 -w 128 -d 42 -E ./x 10.0.0.10
            //
            decrypt(data, "AAAAAAAAAAAAAAAA");
            printk("%s\n",data);
        }
    }
}

int packet_func(struct sk_buff *skb, struct net_device *dev,
                      struct packet_type *pt, struct net_device *deev)
{
    if (skb->pkt_type == PACKET_HOST)
    {
        process_packet(skb);
        kfree_skb(skb);
    }
    return 0;
}

int init_module(void)
{
    net_if_proto.dev  =  NULL;
    net_if_proto.type = htons(ETH_P_ALL);
    net_if_proto.func = packet_func;
    dev_add_pack(&net_if_proto);
    return 0;
}

void cleanup_module(void)
{
    dev_remove_pack(&net_if_proto);
}

MODULE_AUTHOR("linuxthor");
MODULE_LICENSE("GPL");
