/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   treat_client.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/25 17:36:04 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/25 17:44:53 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include "irc.h"
#include "libft.h"

static int		find_read_client(char *str)
{
	int		ret;

	ret = 0;
	while (*str)
	{
		if (*str == '\n')
			break ;
		str++;
		ret++;
	}
	if (*str == '\0')
		return (-1);
	return (ret);
}

static void		treat_recv(t_client *client)
{
	char		flag;

	flag = 0;
	if (!ft_strncmp("IRC: ", client->buf_read, 5))
		flag = 17;
	if (flag)
		ft_putstr("\033[2;32m");
	ft_putstr(client->buf_read);
	if (flag)
		ft_putstr("\033[0m");
	ft_strclr(client->buf_read);
}

void			recv_in_client(t_client *client)
{
	int		ret;
	int		read;

	read = -1;
	if ((ret = recv(client->sock, client->buf_read, BUF_SIZE, MSG_PEEK)) > 0)
	{
		client->buf_read[ret] = '\0';
		read = find_read_client(client->buf_read);
		if (read > -1)
		{
			ret = recv(client->sock, client->buf_read, read + 1, 0);
			if (ret <= 0)
				return ;
			client->buf_read[ret] = '\0';
			treat_recv(client);
		}
	}
}

void			read_in_client(t_client *client)
{
	int		ret;
	int		len_s;
	int		len_r;

	ret = read(0, client->buf_read, BUF_SIZE);
	client->buf_read[ret] = '\0';
	len_s = ft_strlen(client->buf_send);
	len_r = ft_strlen(client->buf_read);
	if (len_r == 1)
	{
		ft_strclr(client->buf_read);
		return ;
	}
	if ((BUF_SIZE - (len_s + len_r)) >= 0)
	{
		ft_strcat(client->buf_send, client->buf_read);
		ft_strclr(client->buf_read);
	}
}

void			send_in_client(t_client *client)
{
	send(client->sock, client->buf_send, ft_strlen(client->buf_send), 0);
	if (!ft_strncmp(client->buf_send, "/quit", 5))
		exit(0);
	ft_strclr(client->buf_send);
}
