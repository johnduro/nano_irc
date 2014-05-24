/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/05/21 14:06:58 by mle-roy           #+#    #+#             */
/*   Updated: 2014/05/24 17:32:22 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_H
# define IRC_H

# include <sys/select.h>

# define BUF_SIZE 4096
# define NICK_LEN 9
# define CHAN_LEN 200
# define PUBLIC 1
# define PRIVATE 2
# define N_ERROR "IRC: you have to set a nickname (/nick <name>)\n"
# define N_LEN "IRC: your nickname is too long, no more than 9 !\n"
# define N_VALID "IRC: your nickname is not valid\n"
# define C_ERROR "IRC: please join a chan or send private message (/cmd)\n"
# define C_EMPTY "IRC: please specify a chan name (/join <chan>)\n"
# define C_LEN "IRC: your chan name is too long, no more than 200 !\n"
# define L_CHAN "IRC: your are not in a chan\n"
# define NO_MSG "IRC: wrong message format: /msg <nick> <msg>\n"
# define NO_USER "IRC: could not find user\n"
# define CMD "[/nick <name>][/join <chan>][/leave][/who][/msg <nick> <msg>]"
# define CMD2 "[/cmd][/chan][/quit]\n"

typedef struct s_chan	t_chan;

typedef struct			s_user
{
	int					fd;
	char				nick[NICK_LEN + 1];
	char				buf_read[BUF_SIZE + 1];
	char				buf_write[BUF_SIZE + 1];
	t_chan				*chan;
	struct s_user		*next;
	struct s_user		*prev;
	struct s_user		*next_chan;
	struct s_user		*prev_chan;
}						t_user;

struct					s_chan
{
	char				name[CHAN_LEN + 1];
	t_user				*user_first;
	t_user				*user_last;
	struct s_chan		*next;
	struct s_chan		*prev;
};

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

typedef int(*fn_ptr)(char **, t_user *, t_irc *);

typedef struct			s_cmd
{
	char				*name;
	fn_ptr				fn;
}						t_cmd;

int						error_serv(int code);
t_irc					*init_irc(int port);
t_user					*init_client(int fd);
t_chan					*chan_gen(char *name, t_irc *irc);
int						create_serv(int port);
void					main_loop(t_irc *irc);
void					init_fd(t_irc *irc);
void					check_fd(int ret, t_irc *irc);
void					accept_client(t_irc *irc);
void					write_client(t_user *user);
void					read_client(t_user *user, t_irc *irc);
void					remove_user(t_user *user, t_irc *irc);
void					remove_user_chan(t_user *user, t_irc *irc);
void					remove_chan(t_chan *chan, t_irc *irc);
void					treat_command(t_user *user, t_irc *irc);
void					send_to_chan(t_chan *chan, char *msg);
void					add_to_write(t_user *user, char *msg);
void					parse_command(t_user *user, t_irc *irc);
t_cmd					*get_list(void);
int						set_nick(char **arg, t_user *user, t_irc *irc);
int						nick_is_valid(char *nick, t_irc *irc);
int						join_chan(char **arg, t_user *user, t_irc *irc);
void					add_user_chan(t_user *user, t_chan *chan);
t_chan					*find_chan(char *name, t_chan *chan);
t_user					*find_user(char *name, t_user *user);
int						leave_chan(char **arg, t_user *user, t_irc *irc);
int						get_who(char **arg, t_user *user, t_irc *irc);
void					send_who(t_user *user, t_chan *chan);
char					*make_msg(t_user *user, char *msg, int flag);
int						send_msg(char **arg, t_user *user, t_irc *irc);
int						get_cmd(char **arg, t_user *user, t_irc *irc);
int						get_chan(char **arg, t_user *user, t_irc *irc);
int						quit_serv(char **arg, t_user *user, t_irc *irc);
void					send_chan(t_user *user, t_chan *chan);
int						check_nick(t_user *user);

#endif
