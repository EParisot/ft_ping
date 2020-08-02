/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/01 17:09:25 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/01 17:09:25 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H

# define FT_PING_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include "../libft/libft.h"
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/ip_icmp.h>
# include <sys/time.h>

#define PING_PKT_S 64

typedef struct		s_ping_data
{
    int             verbose;
    int             sockfd;
    int             ip_version;
    char            *target;
    char            *target_addr;
}					t_ping_data;

typedef struct      s_ping_pkt 
{ 
    struct icmphdr  header; 
    char            msg[PING_PKT_S - sizeof(struct icmphdr)];
}                   t_ping_pkt;

int ft_ping(t_ping_data *data);
void set_addr_info_struct(struct addrinfo *hints);
void free_addr_info(struct addrinfo *result);
void dns_lookup(t_ping_data *data);


# endif