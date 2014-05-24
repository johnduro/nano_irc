/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who_serveur.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:26:32 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 17:26:57 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "irc.h"
#include "libft.h"

void		send_who(t_user *user, t_chan *chan)
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
		bwsus = bwsus->next_chan;
	}
	add_to_write(user, ret);
	free(ret);
}

int			get_who(char **arg, t_user *user, t_irc *irc)
{
	(void)arg;
	(void)irc;
	if (user->chan)
		send_who(user, user->chan);
	else
		add_to_write(user, L_CHAN);
	return (0);
}
