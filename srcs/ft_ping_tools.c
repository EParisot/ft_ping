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

void set_addr_info_struct(struct addrinfo *hints)
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

void free_addr_info(struct addrinfo *result)
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

void dns_lookup(t_ping_data *data)
{
    struct addrinfo     hints;
    struct addrinfo     *result;
    struct sockaddr_in  *addr_in;
    struct sockaddr_in6 *addr_in6;
    char                *str_addr;

    str_addr = NULL;
    set_addr_info_struct(&hints);
    if (getaddrinfo(data->target, NULL, &hints, &result) != 0)
        fprintf(stderr, "ft_ping: %s: No address associated with hostname\n", data->target);
    else if (result->ai_addr->sa_family == AF_INET) 
    {
        addr_in = (struct sockaddr_in *)result->ai_addr;
        str_addr = (char *)malloc(INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(addr_in->sin_addr), str_addr, INET_ADDRSTRLEN);
        data->ip_version = AF_INET;
    } 
    else if (result->ai_addr->sa_family == AF_INET6)
    {
        addr_in6 = (struct sockaddr_in6 *)result->ai_addr;
        str_addr = (char *)malloc(INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), str_addr, INET6_ADDRSTRLEN);
        data->ip_version = AF_INET6;
    }
    ft_memcpy(data->sock_addr, result->ai_addr->sa_data, 14);
    free_addr_info(result);
    data->target_addr = str_addr;
}
