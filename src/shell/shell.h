/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
#define SHELL_H

#include "common/common.h"

typedef struct s_fd
{
	int out;
	int in;
} t_fd;

/* main.c */
void  setback_fd(t_fd* fd);
char* ft_readline(char** prompt);
void  process_input_loop(char*** env_copy, int* status);

/* terminal.c */
void set_signal(int primary);

/* signals.c */
void sigint_handler(int sig);
void sigint_handler_p(int sig);
void sigint_handler_heredoc(int sig);
void sigquit_handler(int signo);
void sigquit_handler_p(int signo);

#endif
