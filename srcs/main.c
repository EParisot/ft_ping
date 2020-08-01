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

static void print_usage()
{
    printf("usage: ping [-v] [target]\n");
}

static void clean_data(t_ping_data *data)
{
    free(data->target);
    free(data);
}

static t_ping_data *get_inputs(int ac, char **av, t_ping_data *data)
{
    int         i;

    i = 1;
    while (i < ac)
    {
        if (av[i] && ft_strlen(av[i]))
        {
            if (ft_strcmp(av[i], "-h") == 0)
            {
                print_usage();
                return(NULL);
            } 
            else if (ft_strcmp(av[i], "-v") == 0)
            {
                data->verbose = 1;
            } 
            else
            {
                if ((data->target = (char *)malloc(ft_strlen(av[i] + 1))) == NULL)
                    return(NULL);
                ft_strcpy(data->target, av[i]);
            }
        }
        ++i;
    }
    return(data);
}

static t_ping_data *check_inputs(int ac, char **av)
{
    t_ping_data *data;

    if (ac <= 1)
    {
        print_usage();
        return(NULL);
    }
    if ((data = (t_ping_data*)malloc(sizeof(t_ping_data))) == NULL)
        return(NULL);
    data->verbose = 0;
    data->target = NULL;
    data = get_inputs(ac, av, data);
    return(data);
}

int main(int ac, char **av)
{
    t_ping_data *data;

    if ((data = check_inputs(ac, av)) == NULL)
    {
        clean_data(data);
        return(-1);
    }
    ft_ping(data);
    clean_data(data);
    return(0);
}