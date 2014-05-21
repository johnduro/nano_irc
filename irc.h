/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/21 14:06:58 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/21 18:33:09 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_H
# define IRC_H

# include <sys/select.h>

# define BUF_SIZE 4096
# define LOG_LEN 9
# define CHAN_LEN 200

typedef struct			s_user
{
	int					fd;
	char				login[LOG_LEN + 1];
	char				nick[LOG_LEN + 1];
	char				buf_read[BUF_SIZE + 1];
	char				buf_write[BUF_SIZE + 1];
	char				chan;
//	char				chan[CHAN_LEN + 1];
	struct s_user		*next;
	struct s_user		*prev;
	struct s_user		*next_in_chan;
	struct s_user		*prev_in_chan;
}						t_user;

typedef struct			s_chan
{
	char				name[CHAN_LEN + 1];
	t_user				*user_first;
	t_user				*user_last;
	struct s_chan		*next;
	struct s_chan		*prev;
}						t_chan;

typedef struct			s_irc
{
	int					sock;
	int					max_fd;
	int					max;
	t_chan				*chan;
	t_user				*user_first;
	t_user				*user_last;
	fd_set				fd_read;
	fd_set				fd_write;
}						t_irc;

int						error_serv(int code);
t_irc					*init_irc(int port);
t_chan					*chan_gen(char *name, t_chan *prev, t_chan *next);
int						create_serv(int port);
void					main_loop(t_irc *irc);
void					init_fd(t_irc *irc);
void					check_fd(int ret, t_irc *irc);
void					accept_client(t_irc *irc);

#endif
