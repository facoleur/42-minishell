/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
#define ERROR_H

#include "lexer/lexer.h"
#include "parser/node.h"

#define ERR_CMD_CANT_EXE 126
#define ERR_CMD_NOT_FOUND 127

#define ERR_UNCLOSED_QUOTE 1
#define ERR_UNEXPECTED_TOKEN 2

/* error.c */
void write_err(char* str);
int	 check_error(t_node* lst);
void ft_dup2(int fd1, int fd2);

/* error_handling.c */
int errror_msg(int type, int c);
int token_error(t_tokens* token, int* op_flag, int* f_op_flag);
int parsing_error(t_tokens** tokens);

/* error_handling_utils.c */
int is_operator(char* token);
int is_file_operator(char* token);
int is_unhandled_operator(char* tokens);

#endif
