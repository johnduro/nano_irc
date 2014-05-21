/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serveur.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/21 14:06:14 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/21 19:12:38 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/resource.h>
#include <sys/socket.h>
#include <netdb.h>
#include "irc.h"
#include "libft.h"

int		error_serv(int code)
{
	if (code == -1)
		ft_putendl_fd("Usage: ./serveur <port>", 2);
	if (code == -2)
		ft_putendl_fd("Wrong port number", 2);
	if (code == -3)
		ft_putendl_fd("Malloc error", 2);
	if (code == -4)
		ft_putendl_fd("Bind error", 2);
	exit(code);
}

int		create_serv(int port)
{
	int					sock;
	struct sockaddr_in	sin;
	struct protoent		*pe;

	pe = getprotobyname("tcp");
	sock = socket(PF_INET, SOCK_STREAM, pe->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	if ((bind(sock, (struct sockaddr*)&sin, sizeof(sin))) == -1)
		error_serv(-4);
	listen(sock, 42);
	return (sock);
}

t_chan	*chan_gen(char *name, t_chan *prev, t_chan *next)
{
	t_chan		*new;

	if ((new = (t_chan *)malloc(sizeof(*new))) == NULL)
		error_serv(-3);
	ft_strcpy(new->name, name);
	new->user_first = NULL;
	new->user_last = NULL;
	new->next = next;
	new->prev = prev;
	return (new);
}

t_irc	*init_irc(int port)
{
	struct r_limit	rlp;
	t_irc			*new;

	if ((new = (t_irc *)malloc(sizeof(*new))) == NULL)
		error_serv(-3);
	getrlimit(RLIMIT_NOFILE, &rlp);
	new->max_fd = rlp.rlim_cur;
	new->sock = create_serv(port);
	new->chan = chan_gen("&general", NULL, NULL);
	new->user_first = NULL;
	new->user_last = NULL;
	return (new);
}

void	init_fd(t_irc *irc)
{
	t_user		*bwsus;

	irc->max = irc->sock;
	FD_ZERO(&irc->fd_read);
	FD_ZERO(&irc->fd_write);
	FD_SET(irc->sock, &irc->fd_read);
	bwsus = irc->user_first;
	while (bwsus)
	{
		FD_SET(bwsus->fd, &irc->fd_read);
		if (ft_strlen(bwsus->buf_write) > 0)
			FD_SET(bwsus->fd, &irc->fd_write);
		irc->max = ft_isgreater(irc->max, bwsus->fd);
		bwsus = bwsus->next;
	}
}


void	print_new_client(int cs, char *addr, int port)
{
	ft_putstr("New client #");
	ft_putnbr(cs);
	ft_putstr(" from ");
	ft_putstr(addr);
	write(1, ":", 1);
	ft_putnbr(port);
	write(1, "\n", 1);
}

void	init_client(int fd)
{
	t_user		*new;

	if ((new = (t_user *)malloc(sizeof(*new))) == NULL)
		error_serv(-3);
	new->fd = fd;
	new->login[0] = '\0';
	new->nick[0] = '\0';
	new->chan = 0;
	new->next = NULL;
	new->prev = NULL;
	new->next_in_chan = NULL;
	new->prev_in_chan = NULL;
	return (new);
}

void	accept_client(t_irc *irc)
{
	int						cs;
	struct sockaddr_in		csin;
	socklen_t				csin_len;
	t_user					*user;

	csin_len = sizeof(csin);
	if ((cs = accept(s, (struct sockaddr*)&csin, &csin_len)) == -1)
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

void	check_fd(int ret, t_irc *irc)
{
	t_user		*bwsus;

	if (FD_ISSET(irc->sock, &irc->fd_read))
		accept_client(irc);
	bwsus = irc->user_first;
	while (bwsus && ret)
	{
		if (FD_ISSET(bwsus->fd, &irc->fd_read))
			read_client(bwsus); // a faire
		if (FD_ISSET(bwsus->fd, &irc->fd_write))
			write_client(bwsus); // a faire
		if (FD_ISSET(bwsus->fd, &irc->fd_write)
			|| FD_ISSET(bwsus->fd, &irc->fd_read))
			ret--;
		bwsus = bwsus->next;
	}
}

void	main_loop(t_irc *irc)
{
	int					ret;
	struct timeval		tv;

	ret = 0;
	while (42)
	{
		init_fd(irc);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		ret = select(irc->max + 1, &irc->fd_read, &irc->fd_write, NULL, &tv);
		check_fd(ret, irc);
	}
}

int		main(int argc, char **argv)
{
	t_irc		*irc;
	int			port;

	if (argc != 2)
		error_serv(-1);
	port = ft_atoi(argv[1]);
	if (port <= 1024)
		error_serv(-2);
	irc = init_irc(port);
	main_loop(irc);
	return (0);
}
