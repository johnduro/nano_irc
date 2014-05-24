/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_serveur.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:20:11 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 17:21:14 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netdb.h>
#include "libft.h"
#include "irc.h"

int			error_serv(int code)
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

int			create_serv(int port)
{
	int						sock;
	struct sockaddr_in		sin;
	struct protoent			*pe;

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

t_irc		*init_irc(int port)
{
	struct rlimit	rlp;
	t_irc			*new;

	if ((new = (t_irc *)malloc(sizeof(*new))) == NULL)
		error_serv(-3);
	getrlimit(RLIMIT_NOFILE, &rlp);
	new->max_fd = rlp.rlim_cur;
	new->sock = create_serv(port);
	new->chan = NULL;
	new->user_first = NULL;
	new->user_last = NULL;
	return (new);
}

int			main(int argc, char **argv)
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
