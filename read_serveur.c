/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_serveur.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:14:43 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 17:42:53 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "irc.h"
#include "libft.h"

int			find_read(char *str)
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

t_cmd		*get_list(void)
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

void		parse_command(t_user *user, t_irc *irc)
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

void		treat_command(t_user *user, t_irc *irc)
{
	char	*tmp;

	if (user->buf_read[0] == '/')
		parse_command(user, irc);
	else if ((ft_strlen(user->nick)) == 0)
		add_to_write(user, N_ERROR);
	else if (user->chan == NULL)
		add_to_write(user, C_ERROR);
	else
	{
		tmp = make_msg(user, user->buf_read, PUBLIC);
		send_to_chan(user->chan, tmp);
		free(tmp);
	}
}

void		read_client(t_user *user, t_irc *irc)
{
	int		ret;
	int		read;

	read = -1;
	if ((ret = recv(user->fd, user->buf_read, BUF_SIZE, MSG_PEEK)) > 0)
	{
		user->buf_read[ret] = '\0';
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
			ft_putstr(user->nick);
			write(1, ": ", 2);
			ft_putstr(user->buf_read);
			treat_command(user, irc);
		}
	}
	else if (ret <= 0)
		remove_user(user, irc);
}
