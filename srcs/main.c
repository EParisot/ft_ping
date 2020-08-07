/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/05 23:09:42 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/05 23:11:02 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_ping.h"

static void			print_usage(void)
{
	printf("usage: ping [-v] [target]\n");
}

static void			clean_data(t_ping_data *data)
{
	if (data && data->target)
		free(data->target);
	if (data && data->target_addr)
		free(data->target_addr);
	if (data && data->stats_list)
		ft_lstdel(&data->stats_list, del);
	free(data);
}

static int			get_inputs(int ac, char **av, t_ping_data *data)
{
	int				i;

	i = 0;
	while (++i < ac)
		if (av[i] && ft_strlen(av[i]))
		{
			if (ft_strcmp(av[i], "-h") == 0)
				;
			else if (ft_strcmp(av[i], "-v") == 0)
				data->verbose = 1;
			else
			{
				if (!(data->target = (char *)malloc(ft_strlen(av[i]) + 1)))
					return (-1);
				ft_strcpy(data->target, av[i]);
			}
		}
	if (data->target == NULL)
	{
		print_usage();
		return (-1);
	}
	return (0);
}

static t_ping_data	*check_inputs(int ac, char **av)
{
	t_ping_data	*data;

	data = NULL;
	if (ac <= 1)
	{
		print_usage();
		return (NULL);
	}
	if ((data = (t_ping_data*)malloc(sizeof(t_ping_data))) == NULL)
		return (NULL);
	data->verbose = 0;
	data->target = NULL;
	data->target_addr = NULL;
	data->errors = 0;
	data->success = 0;
	data->msg_count = 0;
	data->last_ttl = 0;
	get_inputs(ac, av, data);
	return (data);
}

int					main(int ac, char **av)
{
	int				ret;
	t_ping_data		*data;

	data = check_inputs(ac, av);
	if (data == NULL || data->target == NULL)
	{
		clean_data(data);
		return (-1);
	}
	ret = ft_ping(data);
	clean_data(data);
	return (ret);
}
