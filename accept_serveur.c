/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   accept_serveur.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:18:01 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 17:19:04 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "irc.h"
#include "libft.h"

void		print_new_client(int cs, char *addr, int port)
{
	ft_putstr("New client #");
	ft_putnbr(cs);
	ft_putstr(" from ");
	ft_putstr(addr);
	write(1, ":", 1);
	ft_putnbr(port);
	write(1, "\n", 1);
}

t_user		*init_client(int fd)
{
	t_user	*new;

	if ((new = (t_user *)malloc(sizeof(*new))) == NULL)
		error_serv(-3);
	new->fd = fd;
	new->nick[0] = '\0';
	new->chan = NULL;
	new->next = NULL;
	new->prev = NULL;
	new->next_chan = NULL;
	new->prev_chan = NULL;
	return (new);
}

void		accept_client(t_irc *irc)
{
	int						cs;
	struct sockaddr_in		csin;
	socklen_t				csin_len;
	t_user					*user;

	csin_len = sizeof(csin);
	if ((cs = accept(irc->sock, (struct sockaddr*)&csin, &csin_len)) == -1)
		return ;
	print_new_client(cs, inet_ntoa(csin.sin_addr), ntohs(csin.sin_port));
	user = init_client(cs);
	if (irc->user_last == NULL)
	{
		irc->user_first = user;
		irc->user_last = user;
	}
	else
	{
		irc->user_last->next = user;
		user->prev = irc->user_last;
		irc->user_last = user;
	}
}
