/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANSION_H
#define EXPANSION_H

#include "lexer/lexer.h"

/* expansion.c */
char* expand_token(char* token, int status, char** env);
void  handle_single_quotes(char* token, int* i, char** res, int* j);
void  handle_others(char* token, int* i, char** res, int* j);
void  handle_expansion(char* token, char** res, char** env, int* nbr);

/* expansion_utils.c */
void  expand_env_vars(char** tokens, int status, char** env, int* expanded);
char* get_var_name(char* token);
char* get_var_value(char* var_name, int status, char** env);
int	  get_nbr_of_vars(char* token);

#endif
