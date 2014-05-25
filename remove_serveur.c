/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   remove_serveur.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/24 17:25:22 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/25 17:28:27 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "irc.h"
#include "libft.h"

void		remove_chan(t_chan *chan, t_irc *irc)
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

void		remove_user_chan(t_user *user, t_irc *irc)
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

void		remove_user(t_user *user, t_irc *irc)
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
	free(user);
	user = NULL;
}

int			quit_serv(char **arg, t_user *user, t_irc *irc)
{
	(void)arg;
	ft_putstr("User #");
	ft_putnbr(user->fd);
	ft_putstr(" is leaving\n");
	remove_user(user, irc);
	return (0);
}
