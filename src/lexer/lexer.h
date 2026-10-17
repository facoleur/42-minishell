/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
#define LEXER_H

#include "common/common.h"

#define SINGLE_QUOTE 1
#define DOUBLE_QUOTE 2
#define UNQUOTED 0

typedef struct s_tokens
{
	char* tok;
	int	  quote;
} t_tokens;

typedef struct s_tokenzied
{
	char*	normalized;
	char**	space_tok;
	char*** op_tok;
	char**	tok;
} t_tokenized;

/* lexer.c : point d'entree du domaine */
t_tokens** tokenizer(char* string, int status, char** env);
char*	   input_normalizer(char* input);
void	   normalize_loop(char* input, int i, char** output);

/* tokenizer.c */
char*	   op_tokenzier_loop(char* string, int* i, int* op_flag, int* start);
char**	   op_tokenizer(char* string);
char**	   sp_tokenizer(char* string, char c);
t_tokens** quotes_tokenizer(char** tokens, int* expanded);

/* operators.c */
int is_single_op(char* string, int i);
int is_double_op(char* string, int i);
int count_op(char* string);
int count_quotes(char* string);
int is_quotes_opened(char* string);

/* quotes.c */
int	  is_in_quotes(char* input, int index);
int	  is_quote(char c);
char* all_quotes_remover(char* string);
char* around_quotes_remover(char* string);
void  all_quotes_remover_loop(char* string, char** token, char prev_quote);

/* token_utils.c */
int	 get_elem_count_tok(t_tokens** array);
void free_tokens(t_tokens** tokens);

#endif
