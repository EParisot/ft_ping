/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping_tools_4.c                                  :+:      :+:    :+:   */
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
			++data->success;
		tmp_lst = tmp_lst->next;
	}
}
