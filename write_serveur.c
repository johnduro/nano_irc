
#include "irc.h"
#include "libft.h"

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

void	add_to_write(t_user *user, char *msg)
{
	int	len_msg;
	int	len_wr;

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
	t_user	*bwsus;

	bwsus = chan->user_first;
	while (bwsus)
	{
		add_to_write(bwsus, msg);
		bwsus = bwsus->next;
	}
}
