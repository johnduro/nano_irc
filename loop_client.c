/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop_client.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/25 17:31:19 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/25 17:43:06 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <sys/select.h>
#include "irc.h"
#include "libft.h"

t_client	*init_t_client(int sock)
{
	t_client	*new;

	if ((new = (t_client *)malloc(sizeof(*new))) == NULL)
		exit(client_error(-3));
	new->sock = sock;
	return (new);
}

void		init_fd_client(t_client *client)
{
	FD_ZERO(&client->fd_read);
	FD_ZERO(&client->fd_write);
	FD_SET(client->sock, &client->fd_read);
	FD_SET(0, &client->fd_read);
	if (ft_strlen(client->buf_send) > 0)
		FD_SET(client->sock, &client->fd_write);
}

void		check_fd_client(t_client *client)
{
	if (FD_ISSET(client->sock, &client->fd_read))
		recv_in_client(client);
	if (FD_ISSET(0, &client->fd_read))
		read_in_client(client);
	if (FD_ISSET(client->sock, &client->fd_write))
		send_in_client(client);
}

int			make_client(int sock)
{
	int					ret;
	t_client			*cl;
	struct timeval		tv;

	cl = init_t_client(sock);
	while (42)
	{
		init_fd_client(cl);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		ret = select(cl->sock + 1, &cl->fd_read, &cl->fd_write, NULL, &tv);
		if (ret == -1)
			exit(-1);
		if (ret)
			check_fd_client(cl);
	}
	return (0);
}
