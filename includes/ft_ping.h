#ifndef FT_PING_H

# define FT_PING_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include "../libft/libft.h"
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/ip_icmp.h>
# include <sys/time.h>
# include <signal.h>

#define PING_PKT_S 64
#define BUFFER_MAX_SIZE 1024
#define TTL_VAL 51
#define TIMEOUT 1

int		g_keyboard_interrupt;

typedef struct		s_ping_data
{
	int				verbose;
	int				sockfd;
	int				ip_version;
	char			*target;
	char			*target_addr;
	char			sock_addr[16];
	t_list			*stats_list;
	int				errors;
	int				success;
}					t_ping_data;

typedef struct		s_ping_pkt 
{
	struct icmphdr	header;
	char			msg[PING_PKT_S - sizeof(struct icmphdr)];
}					t_ping_pkt;

int				ft_ping(t_ping_data *data);
unsigned short	calc_checksum(void *msg, int msg_size);
void			set_addr_info_struct(struct addrinfo *hints);
void			free_addr_info(struct addrinfo *result);
int				dns_lookup(t_ping_data *data);
t_ping_pkt		*build_pkt(int msg_count);
struct msghdr	*build_msg(struct sockaddr *addr_struct);
void			sig_handler(int numSig);
void			save_stats(t_ping_data *data, long int *delay);
void			count_success(t_ping_data *data);
void			print_stats(t_ping_data *data, int msg_count, int delay);
void			print_pkt_stats(t_ping_data *data, int received_size, \
								int msg_count, int delay);
void			del(void *addr, size_t size);

# endif