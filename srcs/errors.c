/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/05 23:09:17 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/05 23:09:18 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_ping.h"

void		count_success(t_ping_data *data)
{
	t_list	*tmp_lst;

	tmp_lst = data->stats_list;
	while (tmp_lst)
	{
		if (*(int*)(tmp_lst->content) != 0)
			data->success++;
		tmp_lst = tmp_lst->next;
	}
}

int			dns_err(t_ping_data *data, struct addrinfo *hints, \
												struct addrinfo **result)
{
	int err;

	err = 0;
	if ((err = getaddrinfo(data->target, NULL, hints, result)) != 0)
	{
		if (err != -5 && err != -2)
			fprintf(stderr, \
		"ft_ping: %s: Temporary failure in name resolution\n", data->target);
		else if (err == -5)
			fprintf(stderr, \
		"ft_ping: %s: No address associated with hostname!\n", data->target);
		else if (err == -2)
			fprintf(stderr, "ft_ping: %s: Name or service not known\n",\
				data->target);
		return (-1);
	}
	return (0);
}

void		ping_err(t_ping_data *data)
{
	printf("From %s icmp_seq=%d Destination Host Unreachable\n", \
		data->target, data->msg_count);
	data->errors++;
}
