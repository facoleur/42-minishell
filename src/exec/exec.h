/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
#define EXEC_H

#include "exec/stack.h"
#include "parser/node.h"

/* exec.c */
int	 exe_prompt(t_node* list, char*** env, int* status);
int	 run_cmd(t_node* node, t_stack** pid_stack, char*** env);
int	 check_executable(t_node* node);
void wait_pid(t_stack* pid_stack, int* status);
int	 max_pipe(t_node* node);

/* cmd_path.c */
char*  get_cmd_path(t_node* node, char** path);
char** get_path(char* env[]);
int	   cmd_do_not_include_path(char* cmd);
void   deal_with_multi_cmd(t_node* node);
void   deal_cout(t_node* node, t_node* prev_cmd, int i, int ii);

/* pipe.c */
int init_pipe(t_node* node);
int close_pipe(t_node* node);
int stdin_occupied(t_node* node);
int set_pipe(t_node* node);
int check_pipe(t_node* node);

#endif
