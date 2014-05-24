
#include <stdlib.h>
#include "irc.h"
#include "libft.h"

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

int	send_msg(char **arg, t_user *user, t_irc *irc)
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
