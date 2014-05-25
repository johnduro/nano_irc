/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/21 14:10:09 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/25 17:58:30 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "irc.h"
#include "libft.h"

int			client_error(int code)
{
	if (code == -1)
		ft_putstr("Usage ./client <addr> <port>\n");
	if (code == -3)
		ft_putstr("CLIENT: malloc error\n");
	exit(code);
}

int			create_client(char *addr, int port)
{
	int						sock;
	struct protoent			*proto;
	struct sockaddr_in		sin;
	struct hostent			*host;
	char					*tmp;

	proto = getprotobyname("tcp");
	if (proto == 0)
		return (-1);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	host = gethostbyname(addr);
	tmp = inet_ntoa(*((struct in_addr *)host->h_addr_list[0]));
	sin.sin_addr.s_addr = inet_addr(tmp);
	if (connect(sock, (const struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		ft_putstr_fd("ERROR: Connection\n", 2);
		return (-1);
	}
	ft_putstr("SUCCESS: Connection\n");
	return (sock);
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
