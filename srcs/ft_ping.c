/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/05 23:08:24 by eparisot          #+#    #+#             */
/*   Updated: 2020/08/05 23:08:29 by eparisot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_ping.h"

static int		check_and_wait(t_ping_data *data, struct msghdr *msg)
{
	int			ret;
	char		*str_addr;

	ret = 0;
	if ((str_addr = (char *)malloc(INET_ADDRSTRLEN)) == NULL)
		return (-1);
	inet_ntop(AF_INET, &(((struct iphdr *)msg->msg_iov->iov_base)->saddr), \
												str_addr, INET_ADDRSTRLEN);
	if (ft_strcmp(str_addr, data->target_addr))
		ret = -1;
	data->last_ttl = ((struct iphdr *)msg->msg_iov->iov_base)->ttl;
	if (ret == 0 && ((struct icmphdr *)(msg->msg_iov->iov_base + \
		((struct iphdr *)msg->msg_iov->iov_base)->ihl * \
		sizeof(unsigned int)))->un.echo.id != getpid() && data->verbose)
	{
		fprintf(stderr, "ft_ping: Someone else packet showed up!\n");
		ret = -1;
	}
	while (g_keyboard_interrupt << 1 == 0)
		;
	g_keyboard_interrupt = g_keyboard_interrupt & 0x10;
	free(str_addr);
	return (ret);
}

static int		send_and_receive(t_ping_data *data, struct msghdr *msg, \
				struct sockaddr *addr_struct, long int delay)
{
	int				received_size;
	struct timeval	start;
	struct timeval	end;
	t_ping_pkt		*pkt;

	pkt = NULL;
	received_size = 0;
	if ((pkt = build_pkt(data)) == NULL)
		return (-1);
	gettimeofday(&start, NULL);
	if (sendto(data->sockfd, pkt, sizeof(t_ping_pkt), 0, addr_struct, \
						sizeof(struct sockaddr)) <= 0)
		return (-1);
	received_size = recvmsg(data->sockfd, msg, 0);
	gettimeofday(&end, NULL);
	delay = (received_size > -1) ? ((end.tv_sec * 1000000 + end.tv_usec) - \
			(start.tv_sec * 1000000 + start.tv_usec)) : 0;
	if (check_and_wait(data, msg) == 0 && g_keyboard_interrupt < 10)
		print_pkt_stats(data, received_size, delay);
	else if (g_keyboard_interrupt < 10 && data->msg_count % 3 == 0)
		ping_err(data);
	if (delay / 1000 > TIMEOUT * 1000 && data->verbose)
		fprintf(stderr, "Request timeout for icmp_seq %hu\n", data->msg_count);
	free(pkt);
	return (0);
}

static int		ping_loop(t_ping_data *data, struct sockaddr *addr_struct)
{
	struct msghdr	*msg;

	msg = NULL;
	g_keyboard_interrupt = 1;
	data->stats_list = NULL;
	signal(SIGINT, sig_handler);
	signal(SIGALRM, sig_handler);
	while (g_keyboard_interrupt < 10)
	{
		if ((msg = build_msg(addr_struct)) == NULL)
		{
			fprintf(stderr, "ft_ping: Error building msg!\n");
			return (-1);
		}
		alarm(TIMEOUT);
		if (send_and_receive(data, msg, addr_struct, 0) == -1)
			fprintf(stderr, "ft_ping: Error sending pkt\n");
		free(msg->msg_iov->iov_base);
		free(msg->msg_iov);
		free(msg);
	}
	return (0);
}

int				exec_ping(t_ping_data *data)
{
	int				ttl_val;
	struct sockaddr	addr_struct;
	struct timeval	tv_out;
	struct timeval	start;
	struct timeval	end;

	ttl_val = TTL_VAL;
	tv_out.tv_sec = TIMEOUT;
	tv_out.tv_usec = 0;
	ft_memset(&addr_struct, 0, sizeof(struct sockaddr));
	addr_struct.sa_family = AF_INET;
	ft_memcpy(addr_struct.sa_data, data->sock_addr, 14);
	if (setsockopt(data->sockfd, SOL_IP, IP_TTL, &ttl_val, \
										sizeof(ttl_val)))
		return (-1);
	if (setsockopt(data->sockfd, SOL_SOCKET, SO_RCVTIMEO, \
		(const char*)&tv_out, sizeof(tv_out)) != 0)
		return (-1);
	gettimeofday(&start, NULL);
	if (ping_loop(data, &addr_struct) < 0)
		return (-1);
	gettimeofday(&end, NULL);
	print_stats(data, ((end.tv_sec * 1000000 + end.tv_usec) - \
			(start.tv_sec * 1000000 + start.tv_usec)) / 1000);
	return (0);
}

int				ft_ping(t_ping_data *data)
{
	t_ping_pkt	*pkt;

	pkt = NULL;
	if ((int)getuid() != 0)
	{
		if (data->verbose)
			fprintf(stderr, "ft_ping: socket: Insufficient Permissions\n");
		return (-1);
	}
	if (dns_lookup(data) != 0)
		return (-1);
	if ((data->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		if (data->verbose)
			fprintf(stderr, "ft_ping: Socket file descriptor not received\n");
		return (-1);
	}
	printf("FT_PING %s (%s) %ld(%ld) bytes of data.\n", data->target, \
		data->target_addr, sizeof(pkt->msg), sizeof(*pkt) + \
		sizeof(struct sockaddr) + 4);
	exec_ping(data);
	close(data->sockfd);
	return (0);
}
