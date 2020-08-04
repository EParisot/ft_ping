/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping_tools.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/01 17:09:25 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/01 17:09:25 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_ping.h"

unsigned short      calc_checksum(void *msg, int msg_size)
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

void                set_addr_info_struct(struct addrinfo *hints)
{
    ft_memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints->ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints->ai_flags = AI_PASSIVE | AI_CANONNAME;
    hints->ai_protocol = 0;          /* Any protocol */
    hints->ai_canonname = NULL;
    hints->ai_addr = NULL;
    hints->ai_next = NULL;
}

void                free_addr_info(struct addrinfo *result)
{
    struct addrinfo *tmp;

    while (result)
    {
        tmp = result;
        result = result->ai_next;
        free(tmp->ai_canonname);
        free(tmp);
    }
}

static char         *_dns_lookup(t_ping_data *data, struct addrinfo *result)
{
    struct sockaddr_in  *addr_in;
    struct sockaddr_in6 *addr_in6;
    char                *str_addr;

    str_addr = NULL;
    if (result->ai_addr->sa_family == AF_INET) 
    {
        addr_in = (struct sockaddr_in *)result->ai_addr;
        if ((str_addr = (char *)malloc(INET_ADDRSTRLEN)) == NULL)
            return(NULL);
        inet_ntop(AF_INET, &(addr_in->sin_addr), str_addr, INET_ADDRSTRLEN);
        data->ip_version = AF_INET;
    } 
    else if (result->ai_addr->sa_family == AF_INET6)
    {
        addr_in6 = (struct sockaddr_in6 *)result->ai_addr;
        if ((str_addr = (char *)malloc(INET6_ADDRSTRLEN)) == NULL)
            return(NULL);
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), str_addr, INET6_ADDRSTRLEN);
        data->ip_version = AF_INET6;
    }
    return(str_addr);
}

int                 dns_lookup(t_ping_data *data)
{
    struct addrinfo hints;
    struct addrinfo *result;

    set_addr_info_struct(&hints);
    if (getaddrinfo(data->target, NULL, &hints, &result) != 0)
    {
        fprintf(stderr, "ft_ping: %s: No address associated with hostname!\n", data->target);
        return(-1);
    }
    if ((data->target_addr = _dns_lookup(data, result)) == NULL)
    {
        fprintf(stderr, "ft_ping: Malloc failed!\n");
        return(-1);
    }
    ft_memcpy(data->sock_addr, result->ai_addr->sa_data, 14);
    free_addr_info(result);
    return(0);
}
