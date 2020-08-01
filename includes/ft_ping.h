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
# include "../libft/libft.h"
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>

typedef struct		s_ping_data
{
    int             verbose;
    char            *target;
}					t_ping_data;

int ft_ping(t_ping_data *data);

# endif