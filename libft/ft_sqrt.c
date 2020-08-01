/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sqrt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/08 15:41:47 by eparisot          #+#    #+#             */
/*   Updated: 2017/11/08 15:43:12 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int		ft_sqrt(int nb)
{
	int r;

	r = 1;
	if (nb < 0)
		return (0);
	while (r * r < nb)
		r++;
	if (r * r == nb)
		return (r);
	else
		return (0);
}
