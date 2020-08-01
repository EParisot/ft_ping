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

static void set_addr_info_struct(struct addrinfo *hints)
{
    ft_memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints->ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints->ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints->ai_protocol = 0;          /* Any protocol */
    hints->ai_canonname = NULL;
    hints->ai_addr = NULL;
    hints->ai_next = NULL;
}

static char *get_str_addr(t_ping_data *data)
{
    struct addrinfo     hints;
    struct addrinfo     *result;
    struct sockaddr_in  *addr_in;
    struct sockaddr_in6 *addr_in6;
    char                *str_addr;
    int                 ret;            // !!! pass this as parameter to satisfty normipute

    str_addr = NULL;
    set_addr_info_struct(&hints);
    if ((ret = getaddrinfo(data->target, NULL, &hints, &result)) != 0)
        fprintf(stderr, "ft_ping: %s: %s\n", data->target, gai_strerror(ret));
    else if (result->ai_addr->sa_family == AF_INET) 
    {
        addr_in = (struct sockaddr_in *)result->ai_addr;
        str_addr = (char *)malloc(INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(addr_in->sin_addr), str_addr, INET_ADDRSTRLEN);
    } 
    else if (result->ai_addr->sa_family == AF_INET6)
    {
        addr_in6 = (struct sockaddr_in6 *)result->ai_addr;
        str_addr = (char *)malloc(INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(addr_in6->sin6_addr), str_addr, INET6_ADDRSTRLEN);
    }
    freeaddrinfo(result);
    return(str_addr);
}

int ft_ping(t_ping_data *data)
{
    char        *str_addr;

    if ((str_addr = get_str_addr(data)) == NULL)
        return(2);
    // TODO Build ICMP package
    printf("FT_PING %s (%s)\n", data->target, str_addr);
    // TODO open socket
    // TODO send ICMP package
    // ...
    // TODO close socket
    free(str_addr);
    return(0);
}