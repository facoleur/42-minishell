/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HISTORY_H
#define HISTORY_H

#include "common/common.h"

/* history.c */
void ft_history(char* prompt, char** env);
void add_to_history(char* line);
void print_hist(char** env);

/* history_file.c */
void  remove_end_newline(char* line);
char* get_history_path(char** env);
void  ft_read_history_loop(char** line, int hist_fd);
void  ft_read_history(char** env);
int	  ft_write_history_file(char* line, char** env);

#endif
