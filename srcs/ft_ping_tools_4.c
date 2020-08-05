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