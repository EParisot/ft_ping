# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eparisot <eparisot@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/08/01 17:01:33 by eparisot          #+#    #+#              #
#    Updated: 2020/08/01 17:01:33 by eparisot         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	ft_ping

SRCS	=	srcs/main.c \
			srcs/ft_ping.c \
			srcs/ft_ping_tools.c \

INC		=	includes/ft_ping.h

OBJS	=	$(SRCS:.c=.o)

LIBS	=	libft/libft.a \

CFLAGS	=	-Wall -Wextra -Werror -g3

RM		=	rm -f

all		:	$(LIBS) $(NAME)

$(NAME)	:	$(OBJS) $(INC)
	gcc $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)

$(LIBS)	:
	@$(MAKE) -C libft

clean	:
	$(RM) $(OBJS) && $(MAKE) clean -C libft

fclean	:	clean
	$(RM) $(NAME) libft/libft.a

re		:	fclean all

.PHONY : all re clean fclean
