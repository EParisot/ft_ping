/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping_tools_3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/01 17:09:25 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/01 17:09:25 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_ping.h"

void                print_stats(t_ping_data *data)
{
    t_list      *tmp_lst;
    int         tot;
    int         success;
    int         tot_time;
    float       prop;

    tot_time = 0;
    tot = 0;
    success = 0;
    prop = 0;
    tmp_lst = data->stats_list;
    while (tmp_lst->next)
    {
        ++tot;
        if (*(int*)(tmp_lst->content) != 0)
        {
            ++success;
            tot_time += *(int*)tmp_lst->content;
        }
        tmp_lst = tmp_lst->next;
    }
    if (success)
        prop = 100 - (float)success / (float)tot * 100;
    printf("\n%d packets transmitted, %d received, %.0f%% packet loss, time %dms", \
        tot, success, prop, tot_time);
    printf("\nrtt min/avg/max/mdev = /// ms\n");
}