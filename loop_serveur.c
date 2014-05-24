
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h> //nonon
#include "irc.h"
#include "libft.h"

void	write_client(t_user *user)
{
	send(user->fd, user->buf_write, ft_strlen(user->buf_write), 0);
	ft_strclr(user->buf_write);
}

void	check_fd(int ret, t_irc *irc)
{
	t_user	*bwsus;
	t_user	*keep;

	if (FD_ISSET(irc->sock, &irc->fd_read))
		accept_client(irc);
	bwsus = irc->user_first;
	while (bwsus && ret)
	{
		keep = bwsus->next;
		printf("la 1\n"); //nonon
		if (FD_ISSET(bwsus->fd, &irc->fd_read))
			read_client(bwsus, irc);
 		printf("la 2\n"); //nonon
		if (bwsus && FD_ISSET(bwsus->fd, &irc->fd_write))
			write_client(bwsus);
		printf("la 3\n"); //nonon
		if (bwsus && (FD_ISSET(bwsus->fd, &irc->fd_write)
					|| FD_ISSET(bwsus->fd, &irc->fd_read)))
			ret--;
		bwsus = keep;
	}
}

void	init_fd(t_irc *irc)
{
	t_user	*bwsus;

	irc->max = irc->sock;
	FD_ZERO(&irc->fd_read);
	FD_ZERO(&irc->fd_write);
	FD_SET(irc->sock, &irc->fd_read);
	bwsus = irc->user_first;
	while (bwsus)
	{
		FD_SET(bwsus->fd, &irc->fd_read);
		if (ft_strlen(bwsus->buf_write) > 0)
			FD_SET(bwsus->fd, &irc->fd_write);
		irc->max = ft_isgreater(irc->max, bwsus->fd);
		bwsus = bwsus->next;
	}
}

void	main_loop(t_irc *irc)
{
	int	ret;
	struct timeval	tv;

	ret = 0;
	while (42)
	{
		init_fd(irc);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		ret = select(irc->max + 1, &irc->fd_read, &irc->fd_write, NULL, &tv);
		check_fd(ret, irc);
	}
}
