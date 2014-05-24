/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msg_serveur.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:23:40 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 17:39:40 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "irc.h"
#include "libft.h"

t_user			*find_user(char *name, t_user *user)
{
	while (user)
	{
		if (!ft_strcmp(name, user->nick))
			return (user);
		user = user->next;
	}
	return (NULL);
}

static void		go_msg(t_user *user, t_user *recip, char *msg)
{
	char	*final;

	final = make_msg(user, msg, PRIVATE);
	add_to_write(recip, final);
	free(final);
}

int				send_msg(char **arg, t_user *user, t_irc *irc)
{
	char	*trim;
	t_user	*recip;

	arg++;
	if (check_nick(user))
		return (0);
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
			go_msg(user, recip, *arg);
	}
	free(trim);
	return (0);
}
