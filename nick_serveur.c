/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick_serveur.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:24:05 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/25 17:28:16 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "irc.h"
#include "libft.h"

int			check_nick(t_user *user)
{
	if ((ft_strlen(user->nick)) <= 0)
	{
		add_to_write(user, N_ERROR);
		return (-1);
	}
	return (0);
}

int			nick_is_valid(char *nick, t_irc *irc)
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

int			set_nick(char **arg, t_user *user, t_irc *irc)
{
	arg++;
	if (!(*arg))
	{
		add_to_write(user, N_ERROR);
		return (0);
	}
	if (ft_strlen(*arg) > NICK_LEN)
		add_to_write(user, N_LEN);
	else if (nick_is_valid(*arg, irc))
		ft_strcpy(user->nick, *arg);
	else
		add_to_write(user, N_VALID);
	return (0);
}
