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

static float    calc_mean(t_list *lst)
{
	float	mean;
    int     size;

	mean = 0;
    size = 0;
	while (lst && lst->content)
	{
		mean += (*(int*)lst->content);
		size += 1;
        lst = lst->next;
	}
    mean /= size;
	return (mean / 1000);
}

float           calc_std(t_list *lst, float mean)
{
    float           std;
    int             size; 
    float           tmp;

    std = 0;
    size = 0;
    tmp = 0;
    while (lst && lst->content)
	{
        tmp = (*(int*)lst->content / 1000) - mean;
        if (tmp < 0)
            tmp *= -1;
        std += tmp;
        size += 1;
        lst = lst->next;
	}
    std /= size;
    return(std);
}

void            print_pkt_stats(t_ping_data *data, int received_size, \
                                int msg_count, int delay)
{
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n", \
            (int)(received_size - sizeof(struct iphdr)), data->target, \
            msg_count, TTL_VAL, (float)(delay) / 1000);
}

static void     _print_stats_2(t_ping_data *data)
{
    float mean;

    mean = calc_mean(data->stats_list);
    printf("\nrtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", \
        (float)(ft_lstmin(data->stats_list)) / 1000, \
        mean, \
        (float)(ft_lstmax(data->stats_list)) / 1000, \
        calc_std(data->stats_list, mean));
}

void            print_stats(t_ping_data *data, int msg_count, int delay)
{
    float       prop;

    count_success(data);
    prop = (msg_count) ? 100 - (float)data->success / \
                            (float)msg_count * 100 : 0;
    printf("\n--- %s ping statistics ---", data->target);
    if (data->errors == 0)
        printf("\n%d packets transmitted, %d received, %.0f%% packet loss, time %dms", \
            msg_count, data->success, prop, delay);
    else
        printf("\n%d packets transmitted, %d received, +%d errors, %.0f%% packet loss, time %dms", \
            msg_count, data->success, data->errors, prop, delay);
    if (data->success > 0)
        _print_stats_2(data);
}