
#include <stdlib.h>
#include "irc.h"
#include "libft.h"

int	get_cmd(char **arg, t_user *user, t_irc *irc)
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

int	get_chan(char **arg, t_user *user, t_irc *irc)
{
	(void)arg;
	send_chan(user, irc->chan);
	return (0);
}
