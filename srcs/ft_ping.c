/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/01 17:09:25 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/01 17:09:25 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_ping.h"
#include <errno.h>

int     g_keyboard_interrupt;

unsigned short calc_checksum(void *msg, int msg_size)
{
    unsigned int    sum;
    unsigned short  res;
    unsigned short  *buf;

    sum = 0;
    buf = (unsigned short *)msg;
    while (msg_size > 1)
    {
        sum += *buf++;
        msg_size -= 2;
    }
    if (msg_size == 1)
        sum += *(unsigned char *)buf;
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);
    sum += (sum >> 16);
    res = ~sum;
    return(res);
}

static t_ping_pkt *build_pkt(int msg_count)
{
    t_ping_pkt          *pkt;
    long unsigned int   i;

    i = 0;
    if ((pkt = (t_ping_pkt *)malloc(sizeof(t_ping_pkt))) == NULL)
        return(NULL);
    ft_memset(pkt, 0, sizeof(t_ping_pkt));
    pkt->header.type = ICMP_ECHO;
    pkt->header.un.echo.id = getpid();
    while (i < sizeof(pkt->msg) - 1)
        pkt->msg[i++] = 42;
    pkt->msg[i] = 0;
    pkt->header.un.echo.sequence = 0;
    pkt->header.un.echo.sequence = msg_count;
    pkt->header.checksum = calc_checksum(pkt, sizeof(*pkt));
    return(pkt);
}

static struct msghdr *build_msg(struct sockaddr *addr_struct)
{
    struct msghdr   *msg;
    struct iovec    *iov;
    char            *buffer;

    if ((msg = (struct msghdr *)malloc(sizeof(struct msghdr))) == NULL)
        return(NULL);
    if ((iov = (struct iovec *)malloc(sizeof(struct iovec))) == NULL)
        return(NULL);
    if ((buffer = malloc(BUFFER_MAX_SIZE)) == NULL)
        return(NULL);
    ft_memset(msg, 0, sizeof(struct msghdr));
    ft_memset(iov, 0, sizeof(struct iovec));
    ft_memset(buffer, 0, BUFFER_MAX_SIZE);
    iov->iov_base = buffer;
    iov->iov_len = BUFFER_MAX_SIZE;
    msg->msg_iov = iov;
    msg->msg_iovlen = 1;
    msg->msg_name = addr_struct;
    msg->msg_namelen = sizeof(struct sockaddr);
    return(msg);
}

void sig_handler(int numSig)
{
    if (numSig == SIGINT)
        g_keyboard_interrupt = g_keyboard_interrupt + 0x10;
    if (numSig == SIGALRM)
        g_keyboard_interrupt = g_keyboard_interrupt + 0x01;
    return;
}

int exec_ping(t_ping_data *data)
{
    struct timeval  start;
    struct timeval  end;
    int             ttl_val;
    struct sockaddr addr_struct;
    struct timeval  tv_out;
    struct msghdr   *msg;
    int             msg_count;
    int             received_size;
    t_ping_pkt      *pkt;

    msg = NULL;
    msg_count = 0;
    ttl_val = TTL_VAL;
    tv_out.tv_sec = TIMEOUT; 
    tv_out.tv_usec = 0;
    ft_memset(&addr_struct, 0, sizeof(struct sockaddr));
    addr_struct.sa_family = data->ip_version;
    ft_memcpy(addr_struct.sa_data, data->sock_addr, 14);
    if (setsockopt(data->sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0)
    { 
        fprintf(stderr, "Setting socket options to TTL failed!\n");
        return(-1);
    }
    if (setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof(tv_out)) != 0)
    {
        fprintf(stderr, "Setting socket recv options failed!\n");
        return(-1);
    }
    signal(SIGINT, sig_handler);
    signal(SIGALRM, sig_handler);
    g_keyboard_interrupt = 0;
    while (g_keyboard_interrupt == 0)
    {
        pkt = build_pkt(msg_count++);
        gettimeofday(&start, NULL);
        if (sendto(data->sockfd, pkt, sizeof(t_ping_pkt), 0, &addr_struct, sizeof(struct sockaddr)) <= 0)
        {
            fprintf(stderr, "Packet Sending Failed!\n");
            return(-1);
        }
        if ((msg = build_msg(&addr_struct)) == NULL)
        {
            fprintf(stderr, "Error building msg buffer!\n");
            return(-1);
        }
        alarm(TIMEOUT);
        received_size = recvmsg(data->sockfd, msg, 0);
        gettimeofday(&end, NULL);
        // check pid
        if (((struct icmphdr *)(msg->msg_iov->iov_base + ((struct iphdr *)msg->msg_iov->iov_base)->ihl * sizeof(unsigned int)))->un.echo.id != getpid())
            fprintf(stderr, "Someone else packet showed up!\n");
        while (g_keyboard_interrupt << 1 == 0);
        g_keyboard_interrupt = g_keyboard_interrupt & 0x10;
        if (received_size == -1)
            fprintf(stderr, "Receive Error!\n");
        printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%ld\n", received_size, data->target, data->target_addr, msg_count, ttl_val, ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
        free(msg->msg_iov->iov_base);
        free(msg->msg_iov);
        free(msg);
        free(pkt);
    }
    printf("Stats :\n");
    return(0);
}

int ft_ping(t_ping_data *data)
{
    t_ping_pkt  *pkt;

    pkt = NULL;
    // check user rights
    if ((int)getuid() != 0)
    {
        fprintf(stderr, "ft_ping: Insufficient Permissions\n");
        return(2);
    }
    // DNS Lookup
    dns_lookup(data);
    if (data->target_addr == NULL)
    {
        fprintf(stderr, "ft_ping: DNS lookup failed! Could not resolve hostname %s\n", data->target);
        return(2);
    }
    // open socket
    if ((data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        fprintf(stderr, "ft_ping: Socket file descriptor not received\n");
        return(2);
    }
    printf("FT_PING %s (%s) %ld(%ld) bytes of data.\n\n", data->target, data->target_addr, sizeof(pkt->msg), sizeof(*pkt) + sizeof(struct sockaddr));
    exec_ping(data);
    // close socket fd
    close(data->sockfd);
    return(0);
}