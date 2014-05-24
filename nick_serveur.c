/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick_serveur.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:24:05 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 17:31:59 by mle-roy          ###   ########.fr       */
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
