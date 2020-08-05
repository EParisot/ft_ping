/****************************************************************************/
/*                                                                          */
/*                                                       :::      ::::::::  */
/*  ft_ping_tools.c                                    :+:      :+:    :+:  */
/*                                                   +:+ +:+         +:+    */
/*  By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+       */
/*                                               +#+#+#+#+#+   +#+          */
/*  Created: 2020/08/05 23:08:42 by eparisot          #+#    #+#            */
/*  Updated: 2020/08/05 23:08:45 by eparisot         ###   ########.fr      */
/*                                                                          */
/****************************************************************************/

#include "../includes/ft_ping.h"

unsigned short		calc_checksum(void *msg, int msg_size)
{
	unsigned int	sum;
	unsigned short	res;
	unsigned short	*buf;

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

void				set_addr_info_struct(struct addrinfo *hints)
{
	ft_memset(hints, 0, sizeof(struct addrinfo));
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = SOCK_DGRAM;
	hints->ai_flags = AI_PASSIVE | AI_CANONNAME;
	hints->ai_protocol = 0;
	hints->ai_canonname = NULL;
	hints->ai_addr = NULL;
	hints->ai_next = NULL;
}

void				free_addr_info(struct addrinfo *result)
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

static char			*_dns_lookup(t_ping_data *data, struct addrinfo *result)
{
	struct sockaddr_in	*addr_in;
	struct sockaddr_in6	*addr_in6;
	char				*str_addr;

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
		inet_ntop(AF_INET6, &addr_in6->sin6_addr, str_addr, INET6_ADDRSTRLEN);
		data->ip_version = AF_INET6;
	}
	return(str_addr);
}

int					dns_lookup(t_ping_data *data)
{
	struct addrinfo hints;
	struct addrinfo *result;
	int				err;

	result = NULL;
	err = 0;
	set_addr_info_struct(&hints);
	if ((err = getaddrinfo(data->target, NULL, &hints, &result)) != 0)
	{
		if (err != -5 && err != -2)
			fprintf(stderr, "ft_ping: %s: Temporary failure in name resolution\n", \
				data->target);
		else if (err == -5 && data->verbose)
			fprintf(stderr, "ft_ping: %s: No address associated with hostname!\n",\
				data->target);
		else if (err == -2 && data->verbose)
			fprintf(stderr, "ft_ping: %s: Name or service not known\n",\
				data->target);
		return(-1);
	}
	if (result)
	{
		if ((data->target_addr = _dns_lookup(data, result)) == NULL)
		{
			fprintf(stderr, "ft_ping: dns_lookup Malloc failed!\n");
			return(-1);
		}
		ft_memcpy(data->sock_addr, result->ai_addr->sa_data, 14);
	}
	free_addr_info(result);
	return(0);
}
