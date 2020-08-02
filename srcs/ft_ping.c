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

unsigned short calc_checksum(void *msg, int msg_size)
{
    unsigned short res;
    unsigned short *buf;

    res = 0;
    buf = (unsigned short *)msg;
    while (msg_size > 1)
    {
        res += *buf;
        buf++;
        msg_size -= 2;
    }
    if (msg_size == 1)
        res += *(unsigned char *)buf;
    res = (res >> 16) + (res & 0xFFFF);
    res += (res >> 16);
    return(~res);
}

static t_ping_pkt *build_pkt()
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
    pkt->header.checksum = calc_checksum(pkt, sizeof(t_ping_pkt));
    return(pkt);
}

static struct msghdr *build_msg(struct sockaddr *addr_struct)
{
    struct msghdr   *msg;

    if ((msg = (struct msghdr *)malloc(sizeof(struct msghdr))) == NULL)
        return(NULL);
    ft_memset(msg, 0, sizeof(struct msghdr));
    if ((msg->msg_iov = malloc(PING_PKT_S)) == NULL)
        return(NULL);
    ft_memset(msg->msg_iov, 0, PING_PKT_S);
    msg->msg_name = addr_struct;
    msg->msg_namelen = sizeof(struct sockaddr);
    return(msg);
}

int exec_ping(t_ping_data *data, t_ping_pkt *pkt)
{
    struct timeval  start;
    struct timeval  end;
    int             ttl_val;
    struct sockaddr addr_struct;
    struct timeval  tv_out;
    struct msghdr   *msg;
    int             received_size;

    msg = NULL;
    ttl_val = 64;
    tv_out.tv_sec = 1; 
    tv_out.tv_usec = 0;
    ft_memset(&addr_struct, 0, sizeof(struct sockaddr));
    addr_struct.sa_family = data->ip_version;
    if (inet_pton(data->ip_version, data->target_addr, &addr_struct.sa_data) != 1)
    {
        fprintf(stderr, "Inet_pton failed\n");
        return(-1);
    }
    if (setsockopt(data->sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0)
    { 
        fprintf(stderr, "Setting socket options to TTL failed!\n");
        return(-1);
    }
    gettimeofday(&start, NULL);
    if (sendto(data->sockfd, pkt, sizeof(t_ping_pkt), 0, &addr_struct, sizeof(struct sockaddr)) <= 0)
    {
        fprintf(stderr, "Packet Sending Failed!\n");
    }
    // setting timeout of recv setting 
    if (setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof(tv_out)) != 0)
    {
        fprintf(stderr, "Setting socket recv options failed!\n");
        return(-1);
    }
    if ((msg = build_msg(&addr_struct)) == NULL)
    {
        fprintf(stderr, "Error building msg buffer!\n");
        return(-1);
    }
    if ((received_size = recvmsg(data->sockfd, msg, MSG_DONTWAIT)) <= 0)
    {
        free(msg->msg_iov);
        free(msg);
        fprintf(stderr, "Socket recv failed! %s\n", strerror(errno));
        return(-1);
    }
    free(msg->msg_iov);
    free(msg);
    gettimeofday(&end, NULL);
    printf("received %d\n", received_size);
    printf("delay %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
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
    printf("FT_PING %s (%s)\n", data->target, data->target_addr);

    pkt = build_pkt();
    exec_ping(data, pkt);
    free(pkt);

    // close socket fd
    close(data->sockfd);
    return(0);
}