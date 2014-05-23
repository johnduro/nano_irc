/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serveur.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/21 14:06:14 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/22 20:53:32 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <sys/resource.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
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

t_chan	*chan_gen(char *name, t_irc *irc)
{
	t_chan		*bwschan;
	t_chan		*new;

	if ((new = (t_chan *)malloc(sizeof(*new))) == NULL)
		error_serv(-3);
	ft_strcpy(new->name, name);
	new->user_first = NULL;
	new->user_last = NULL;
	new->next = NULL;
	if (irc->chan == NULL)
	{
		irc->chan = new;
		new->prev = NULL;
	}
	else
	{
		bwschan = irc->chan;
		while (bwschan->next)
			bwschan = bwschan->next;
		bwschan->next = new;
		new->prev = bwschan;
	}
	return (new);
}

t_irc	*init_irc(int port)
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

t_user	*init_client(int fd)
{
	t_user		*new;

	if ((new = (t_user *)malloc(sizeof(*new))) == NULL)
		error_serv(-3);
	new->fd = fd;
//	new->login[0] = '\0';
	new->nick[0] = '\0';
	new->chan = NULL;
	new->next = NULL;
	new->prev = NULL;
	new->next_chan = NULL;
	new->prev_chan = NULL;
	return (new);
}

void	accept_client(t_irc *irc)
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

void	write_client(t_user *user)
{
	send(user->fd, user->buf_write, ft_strlen(user->buf_write), 0);
	ft_strclr(user->buf_write);
}

void	remove_chan(t_chan *chan, t_irc *irc)
{
	if (chan->prev == NULL && chan->next == NULL)
		irc->chan = NULL;
	else if (chan->prev && chan->next == NULL)
		chan->prev->next = NULL;
	else if (chan->prev == NULL && chan->next)
	{
		irc->chan = chan->next;
		chan->next->prev = NULL;
	}
	else if (chan->prev && chan->next)
	{
		chan->prev->next = chan->next;
		chan->next->prev = chan->prev;
	}
	free(chan);
}

void	remove_user_chan(t_user *user, t_irc *irc)
{
	t_chan	*chan;

	chan = user->chan;
	if (user->prev_chan == NULL && user->next_chan == NULL)
		remove_chan(chan, irc);
	else if (user->prev_chan == NULL && user->next_chan)
	{
		chan->user_first = user->next_chan;
		user->next_chan->prev_chan = NULL;
	}
	else if (user->prev_chan && user->next_chan == NULL)
	{
		chan->user_last = user->prev_chan;
		user->prev_chan->next_chan = NULL;
	}
	else if (user->prev_chan && user->next_chan)
	{
		user->prev_chan->next_chan = user->next_chan;
		user->next_chan->prev_chan = user->prev_chan;
	}
	user->chan = NULL;
}

void	remove_user(t_user *user, t_irc *irc)
{
	if (user->chan)
		remove_user_chan(user, irc);
	if (user->prev == NULL && user->next == NULL)
	{
		irc->user_first = NULL;
		irc->user_last = NULL;
	}
	else if (user->prev == NULL && user->next)
	{
		irc->user_first = user->next;
		user->next->prev = NULL;
	}
	else if (user->prev && user->next == NULL)
	{
		irc->user_last = user->prev;
		user->prev->next = NULL;
	}
	else if (user->prev && user->next)
	{
		user->prev->next = user->next;
		user->next->prev = user->prev;
	}
	close(user->fd);
	free(user); //mettre a NULL apres ?
}

int		find_read(char *str)
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

void	add_to_write(t_user *user, char *msg)
{
	int		len_msg;
	int		len_wr;

	len_msg = ft_strlen(msg);
	len_wr = ft_strlen(user->buf_write);
	if ((BUF_SIZE - (len_msg + len_wr)) >= 0)
	{
		ft_strcat(user->buf_write, msg);
		if (msg[len_msg - 1] != '\n')
			ft_strcat(user->buf_write, "\n");
	}
}

void	send_to_chan(t_chan *chan, char *msg)
{
	t_user		*bwsus;

	bwsus = chan->user_first;
	while (bwsus)
	{
		add_to_write(bwsus, msg);
		bwsus = bwsus->next;
	}
}

int		nick_is_valid(char *nick, t_irc *irc)
{
	char	*hold;
	t_user	*bwsus;

	hold = nick;
	while (*nick)
	{
		if (!ft_isalnum(*nick))
			return (0);
		nick++;
	}
	bwsus = irc->user_first;
	while (bwsus)
	{
		if (!ft_strcmp(bwsus->nick, hold))
			return (0);
		bwsus = bwsus->next;
	}
	return (1);
}

int		set_nick(char **arg, t_user *user, t_irc *irc)
{
	char	*trim;

	arg++;
	if (!(*arg))
	{
		add_to_write(user, N_ERROR);
		return (0);
	}
	trim = ft_strtrim(*arg); //plus besoin ?
	if (ft_strlen(trim) > NICK_LEN)
		add_to_write(user, N_LEN);
	else if (nick_is_valid(trim, irc))
		ft_strcpy(user->nick, trim);
	else
		add_to_write(user, N_VALID);
	free(trim);
	return (0);
}

t_chan	*find_chan(char *name, t_chan *chan)
{
	while (chan)
	{
		if (!ft_strcmp(name, chan->name))
			return (chan);
		chan = chan->next;
	}
	return (NULL);
}

void	add_user_chan(t_user *user, t_chan *chan)
{
	if (chan->user_last == NULL)
	{
		chan->user_first = user;
		chan->user_last = user;
		user->prev_chan = NULL;
	}
	else
	{
		chan->user_last->next_chan = user;
		user->prev_chan = chan->user_last;
	}
	user->next_chan = NULL;
	user->chan = chan;
}

int		join_chan(char **arg, t_user *user, t_irc *irc)
{
	t_chan		*chan;

	arg++;
	if (!(*arg))
		add_to_write(user, C_EMPTY);
	else if (ft_strlen(*arg) > CHAN_LEN)
		add_to_write(user, C_LEN);
	else
	{
		if (user->chan)
			remove_user_chan(user, irc);
		if ((chan = find_chan(*arg, irc->chan)) == NULL)
			chan = chan_gen(*arg, irc);
		add_user_chan(user, chan);
	}
	return (0);
}

int		leave_chan(char **arg, t_user *user, t_irc *irc)
{
	(void)arg;
	if (user->chan)
		remove_user_chan(user, irc);
	else
		add_to_write(user, L_CHAN);
	return (0);
}

void	send_who(t_user *user, t_chan *chan)
{
	t_user	*bwsus;
	char	*tmp;
	char	*ret;

	bwsus = chan->user_first;
	ret = ft_strdup("User in chan:");
	while (bwsus)
	{
		tmp = ft_strjoinwsep(ret, bwsus->nick, ' ');
		free(ret);
		ret = tmp;
		bwsus = bwsus->next;
	}
	add_to_write(user, ret);
	free(ret);
}

int		get_who(char **arg, t_user *user, t_irc *irc)
{
	(void)arg;
	(void)irc;
	if (user->chan)
		send_who(user, user->chan);
	else
		add_to_write(user, L_CHAN);
	return (0);
}

char	*make_msg(t_user *user, char *msg, int flag)
{
	char	*tmp;

	tmp = ft_strnew(ft_strlen(user->nick) + 11 + ft_strlen(msg));
	ft_strcat(tmp, user->nick);
	if (flag == PRIVATE)
		ft_strcat(tmp, "(private): ");
	else
		ft_strcat(tmp, ": ");
	ft_strcat(tmp, msg);
	return (tmp);
}

t_user	*find_user(char *name, t_user *user)
{
	while (user)
	{
		if (!ft_strcmp(name, user->nick))
			return (user);
		user = user->next;
	}
	return (NULL);

}

int		send_msg(char **arg, t_user *user, t_irc *irc)
{
	char	*trim;
	t_user	*recip;
	char	*msg;

	arg++;
	if ((ft_strlen(user->nick)) <= 0)
	{
		add_to_write(user, N_ERROR);
		return (0);
	}
	if (!(*arg))
	{
		add_to_write(user, NO_MSG);
		return (0);
	}
	trim = ft_strtrim(*arg);
	if ((recip = find_user(trim, irc->user_first)) == NULL)
		add_to_write(user, NO_USER);
	else
	{
		arg++;
		if (!(*arg))
			add_to_write(user, NO_MSG);
		else
		{
			msg = make_msg(user, *arg, PRIVATE);
			add_to_write(recip, msg);
			free(msg);
		}
	}
	free(trim);
	return (0);
}

int		get_cmd(char **arg, t_user *user, t_irc *irc)
{
	char	*tmp;

	(void)arg;
	(void)irc;
	tmp = ft_strjoin(CMD, CMD2);
	add_to_write(user, tmp);
	free(tmp);
	return (0);
}

void	send_chan(t_user *user, t_chan *chan)
{
	t_chan	*bwschan;
	char	*tmp;
	char	*ret;

	bwschan = chan;
	ret = ft_strdup("Chan on server:");
	while (bwschan)
	{
		tmp = ft_strjoinwsep(ret, bwschan->name, ' ');
		free(ret);
		ret = tmp;
		if (bwschan == user->chan)
		{
			tmp = ft_strjoin(ret, "(*)");
			free(ret);
			ret = tmp;
		}
		bwschan = bwschan->next;
	}
	add_to_write(user, ret);
	free(ret);
}

int		get_chan(char **arg, t_user *user, t_irc *irc)
{
	(void)arg;
	send_chan(user, irc->chan);
	return (0);
}

int		quit_serv(char **arg, t_user *user, t_irc *irc)
{
	(void)arg;
	ft_putstr("User #");
	ft_putnbr(user->fd);
	ft_putstr(" is leaving\n");
	remove_user(user, irc);
	return (0);
}

t_cmd	*get_list(void)
{
	static t_cmd	cmd[] =

	{
	{"/nick", &set_nick},
	{"/join", &join_chan},
	{"/leave", &leave_chan},
	{"/who", &get_who},
	{"/msg", &send_msg},
	{"/cmd", &get_cmd},
	{"/chan", &get_chan},
	{"/quit", &quit_serv},
	{NULL, NULL}
	};
	return (cmd);
}

void	parse_command(t_user *user, t_irc *irc)
{
	char	**cmd;
	char	*trim;
	t_cmd	*list;
	int		i;

	trim = ft_strtrim(user->buf_read);
	cmd = ft_strsplit(trim, ' ');
	free(trim);
	list = get_list();
	i = 0;
	while (list[i].name)
	{
		if (!ft_strcmp(list[i].name, *cmd))
		{
			list[i].fn(cmd, user, irc);
			break ;
		}
		i++;
	}
	ft_tabfree(&cmd);
}

void	treat_command(t_user *user, t_irc *irc)
{
	char		*tmp;

	if (user->buf_read[0] == '/')
		parse_command(user, irc); //a faire attention au nick
	else if ((ft_strlen(user->nick)) == 0)
		add_to_write(user, N_ERROR);
	else if (user->chan == NULL)
		add_to_write(user, C_ERROR);
	else
	{
/*		tmp = ft_strnew(ft_strlen(user->nick) + 3 + ft_strlen(user->buf_read));
		ft_strcat(tmp, user->nick);
		ft_strcat(tmp, ": ");
		ft_strcat(tmp, user->buf_read);*/
		tmp = make_msg(user, user->buf_read, PUBLIC);
		send_to_chan(user->chan, tmp);
		free(tmp);
	}
}

void	read_client(t_user *user, t_irc *irc)
{
	int		ret;
	int		read;

	if (!user)
		return ;
	read = -1;
	if ((ret = recv(user->fd, user->buf_read, BUF_SIZE, MSG_PEEK)) > 0)
	{
		user->buf_read[ret] = '\0';
		printf("RECU: %s<\n", user->buf_read); //a enlever
		read = find_read(user->buf_read);
		if (read > -1)
		{
			ret = recv(user->fd, user->buf_read, read + 1, 0);
			if (ret <= 0)
			{
				remove_user(user, irc);
				return ;
			}
			user->buf_read[ret] = '\0';
			printf("TRAITE: %s<\n", user->buf_read); //a enlever
			treat_command(user, irc);
		}
	}
	else if (ret <= 0)
		remove_user(user, irc);
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
			read_client(bwsus, irc);
		if (FD_ISSET(bwsus->fd, &irc->fd_write))
			write_client(bwsus);
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
