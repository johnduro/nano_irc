/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chan_serveur.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:07:02 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 17:20:02 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "irc.h"
#include "libft.h"

t_chan		*chan_gen(char *name, t_irc *irc)
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

t_chan		*find_chan(char *name, t_chan *chan)
{
	while (chan)
	{
		if (!ft_strcmp(name, chan->name))
			return (chan);
		chan = chan->next;
	}
	return (NULL);
}

void		add_user_chan(t_user *user, t_chan *chan)
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

int			join_chan(char **arg, t_user *user, t_irc *irc)
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

int			leave_chan(char **arg, t_user *user, t_irc *irc)
{
	(void)arg;
	if (user->chan)
		remove_user_chan(user, irc);
	else
		add_to_write(user, L_CHAN);
	return (0);
}
