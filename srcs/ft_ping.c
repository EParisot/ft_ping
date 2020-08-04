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

static int      _check_and_wait(struct msghdr *msg)
{
    int             ret;

    ret = 0;
    if (((struct icmphdr *)(msg->msg_iov->iov_base + \
        ((struct iphdr *)msg->msg_iov->iov_base)->ihl * \
        sizeof(unsigned int)))->un.echo.id != getpid())
    {
            fprintf(stderr, "Someone else packet showed up!\n");
            ret = -1;
    }
    while (g_keyboard_interrupt << 1 == 0);
    g_keyboard_interrupt = g_keyboard_interrupt & 0x10;
    return(ret);
}

static void     _send_and_receive(t_ping_data *data, \
                struct msghdr *msg, struct sockaddr *addr_struct)
{
    int             received_size;
    struct timeval  start;
    struct timeval  end;
    static int      msg_count;
    t_ping_pkt      *pkt;

    pkt = NULL;
    received_size = 0;
    if ((pkt = build_pkt(msg_count++)) == NULL)
    {
        fprintf(stderr, "ft_ping: Error building pkt!\n");
        return;
    }
    alarm(TIMEOUT);
    gettimeofday(&start, NULL);
    if (sendto(data->sockfd, pkt, sizeof(t_ping_pkt), 0, addr_struct, \
        sizeof(struct sockaddr)) <= 0)
    {
        fprintf(stderr, "ft_ping: Packet Sending Failed!\n");
        free(pkt);
        return;
    }
    received_size = recvmsg(data->sockfd, msg, 0);
    gettimeofday(&end, NULL);
    if (_check_and_wait(msg) == 0)
        printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%ld\n", \
        received_size, data->target, data->target_addr, msg_count, TTL_VAL, \
        ((end.tv_sec * 1000000 + end.tv_usec) - \
        (start.tv_sec * 1000000 + start.tv_usec)));
    free(pkt);
}

static int      _ping_loop(t_ping_data *data, struct sockaddr *addr_struct)
{    
    struct msghdr   *msg;

    msg = NULL;
    g_keyboard_interrupt = 0;
    signal(SIGINT, sig_handler);
    signal(SIGALRM, sig_handler);
    while (g_keyboard_interrupt == 0)
    {
        if ((msg = build_msg(addr_struct)) == NULL)
        {
            fprintf(stderr, "ft_ping: Error building msg!\n");
            return(-1);
        }
        _send_and_receive(data, msg, addr_struct);
        free(msg->msg_iov->iov_base);
        free(msg->msg_iov);
        free(msg);
    }
    return(0);
}

int             exec_ping(t_ping_data *data)
{
    int             ttl_val;
    struct sockaddr addr_struct;
    struct timeval  tv_out;
    
    ttl_val = TTL_VAL;
    tv_out.tv_sec = TIMEOUT; 
    tv_out.tv_usec = 0;
    ft_memset(&addr_struct, 0, sizeof(struct sockaddr));
    addr_struct.sa_family = data->ip_version;
    ft_memcpy(addr_struct.sa_data, data->sock_addr, 14);
    if (setsockopt(data->sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)))
    { 
        fprintf(stderr, "Setting socket options to TTL failed!\n");
        return(-1);
    }
    if (setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, \
        (const char*)&tv_out, sizeof(tv_out)) != 0)
    {
        fprintf(stderr, "Setting socket recv options failed!\n");
        return(-1);
    }
    if (_ping_loop(data, &addr_struct) != 0)
        return(-1);
    printf("Stats :\n");
    return(0);
}

int             ft_ping(t_ping_data *data)
{
    t_ping_pkt      *pkt;

    pkt = NULL;
    if ((int)getuid() != 0)
    {
        fprintf(stderr, "ft_ping: Insufficient Permissions\n");
        return(2);
    }
    if (dns_lookup(data) == -1 || data->target_addr == NULL)
    {
        fprintf(stderr, "ft_ping: DNS lookup failed! Could not resolve \
            hostname %s\n", data->target);
        return(2);
    }
    if ((data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        fprintf(stderr, "ft_ping: Socket file descriptor not received\n");
        return(2);
    }
    printf("FT_PING %s (%s) %ld(%ld) bytes of data.\n\n", \
        data->target, data->target_addr, sizeof(pkt->msg), \
        sizeof(*pkt) + sizeof(struct sockaddr));
    exec_ping(data);
    close(data->sockfd);
    return(0);
}