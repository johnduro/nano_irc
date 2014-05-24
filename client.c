/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/21 14:10:09 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 20:51:16 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "irc.h"
#include "libft.h"

int			client_error(int code)
{
	if (code == -1)
		ft_putstr("Usage ./client <addr> <port>\n");
	if (code == -2)
		ft_putstr("ERROR: please use me right... \"put _FILE_\"\n");
	if (code == -3)
		ft_putstr("CLIENT: malloc error\n");
	if (code == -4)
		ft_putstr("ERROR: please use me right... \"get _FILE_\"\n");
	return (code);
}

int			create_client(char *addr, int port)
{
	int						sock;
	struct protoent			*proto;
	struct sockaddr_in		sin;

	proto = getprotobyname("tcp");
	if (proto == 0)
		return (-1);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(addr);
	if (connect(sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		ft_putstr_fd("ERROR: Connection\n", 2);
		return (-1);
	}
	ft_putstr("SUCCESS: Connection\n");
	return (sock);
}

t_client	*init_t_client(int sock)
{
	t_client		*new;

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

int			find_read_client(char *str)
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

void		treat_recv(t_client *client)
{
	char	flag;
	int		len_w;
	int		len_r;

	flag = 0;
	len_w = ft_strlen(client->buf_write);
	len_r = ft_strlen(client->buf_read);
	if (!ft_strncmp("IRC: ", client->buf_read, 5))
		flag = 17;
	if ((BUF_SIZE - (len_w + len_r + flag)) >= 0)
	{
		if (flag)
			ft_putstr("\033[2;32m");
//			ft_strcat(client->buf_write, "\033[2;32m");
		ft_putstr(client->buf_read);
//		ft_strcat(client->buf_write, client->buf_read);
		if (flag)
			ft_putstr("\033[0m");
//			ft_strcat(client->buf_write, "\033[0m");
		ft_strclr(client->buf_read);
	}
}

void		recv_in_client(t_client *client)
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

void		read_in_client(t_client *client)
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

void		send_in_client(t_client *client)
{
	send(client->sock, client->buf_send, ft_strlen(client->buf_send), 0);
	ft_strclr(client->buf_send);
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
	int				ret;
	t_client		*cl;
	struct timeval	tv;

	cl = init_t_client(sock);
	while (42)
	{
		init_fd_client(cl);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		ret = select(cl->sock + 1, &cl->fd_read, &cl->fd_write, NULL, &tv);
		if (ret)
			check_fd_client(cl);
	}
	return (0);
}


int			main(int ac, char **av)
{
	int		port;
	int		sock;

	if (ac != 3)
		return (client_error(-1));
	port = ft_atoi(av[2]);
	sock = create_client(av[1], port);
	if (sock == -1)
		return (-1);
	make_client(sock);
	return (0);
}
