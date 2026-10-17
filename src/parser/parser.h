/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
#define PARSER_H

#include "lexer/lexer.h"
#include "parser/node.h"

/* parser.c */
t_node* parser(t_tokens** tok);
char**	consolidate_cmd(t_tokens** tokens, int i, int* arg_count);
void	parse_heredoc(char* token, t_node** lst);
void	parse_cmd(t_tokens** tokens, t_node** lst, int* i, int* arg_count);

int is_metachar(char c);

#endif
