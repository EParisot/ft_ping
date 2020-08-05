/****************************************************************************/
/*                                                                          */
/*                                                       :::      ::::::::  */
/*  ft_ping_tools_2.c                                  :+:      :+:    :+:  */
/*                                                   +:+ +:+         +:+    */
/*  By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+       */
/*                                               +#+#+#+#+#+   +#+          */
/*  Created: 2020/08/05 23:08:57 by eparisot          #+#    #+#            */
/*  Updated: 2020/08/05 23:08:58 by eparisot         ###   ########.fr      */
/*                                                                          */
/****************************************************************************/

#include "../includes/ft_ping.h"

t_ping_pkt			*build_pkt(int msg_count)
{
	t_ping_pkt			*pkt;
	long unsigned int	i;

	i = 0;
	if ((pkt = (t_ping_pkt *)malloc(sizeof(t_ping_pkt))) == NULL)
		return(NULL);
	ft_memset(pkt, 0, sizeof(t_ping_pkt));
	pkt->header.type = ICMP_ECHO;
	pkt->header.un.echo.id = getpid();
	while (i < sizeof(pkt->msg) - 1)
		pkt->msg[i++] = 42;
	pkt->msg[i] = 0;
	pkt->header.un.echo.sequence = 0;
	pkt->header.un.echo.sequence = msg_count;
	pkt->header.checksum = calc_checksum(pkt, sizeof(*pkt));
	return(pkt);
}

struct msghdr		*build_msg(struct sockaddr *addr_struct)
{
	struct msghdr	*msg;
	struct iovec	*iov;
	char			*buffer;

	if ((msg = (struct msghdr *)malloc(sizeof(struct msghdr))) == NULL)
		return(NULL);
	if ((iov = (struct iovec *)malloc(sizeof(struct iovec))) == NULL)
		return(NULL);
	if ((buffer = malloc(BUFFER_MAX_SIZE)) == NULL)
		return(NULL);
	ft_memset(msg, 0, sizeof(struct msghdr));
	ft_memset(iov, 0, sizeof(struct iovec));
	ft_memset(buffer, 0, BUFFER_MAX_SIZE);
	iov->iov_base = buffer;
	iov->iov_len = BUFFER_MAX_SIZE;
	msg->msg_iov = iov;
	msg->msg_iovlen = 1;
	msg->msg_name = addr_struct;
	msg->msg_namelen = sizeof(struct sockaddr);
	return(msg);
}

void				sig_handler(int numSig)
{
	if (numSig == SIGINT)
		g_keyboard_interrupt = g_keyboard_interrupt + 0x10;
	if (numSig == SIGALRM)
		g_keyboard_interrupt = g_keyboard_interrupt + 0x01;
	return;
}

void				save_stats(t_ping_data *data, long int *delay)
{
	t_list			*new_lst;

	if (data->stats_list == NULL)
		data->stats_list = ft_lstnew(delay, sizeof(int));
	else
	{
		new_lst = ft_lstnew(delay, sizeof(int));
		ft_lstaddend(&data->stats_list, new_lst);
	}
}

void				del(void *addr, size_t size)
{
	(void)size;
	free(addr);
}
