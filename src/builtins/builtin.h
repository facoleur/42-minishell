/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTIN_H
#define BUILTIN_H

#include "exec/stack.h"
#include "parser/node.h"

/* builtin.c */
char* builtin_pwd(void);
int	  builtin_cd(const char* path, char** env);
void  builtin_history(char** cmd, char** env);
int	  builtin_exit(t_node* node);
int	  builtin_echo(t_node* node);

/* dispatch.c */
int	 is_builtin(char** cmd);
int	 print_pwd(void);
int	 exe_builtin(t_node* node, char*** env);
int	 run_builtin(t_node* node, t_stack** pid_stack, char*** env);
void flag_builtin_fork(t_node* node);

#endif
