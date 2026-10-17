/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:31:29 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer/lexer.h"

/**
 * @brief check if a character in a string is within single or double quotes.
 *
 * @param input string
 * @param index character index to check
 * @return 1 if within quotes,
	0 if not. Returns 1 even if the quote is not closed.
 */
int is_in_quotes(char* input, int index)
{
	int in_double_quote;
	int in_single_quote;
	int i;

	i				= -1;
	in_double_quote = false;
	in_single_quote = false;
	while (input[++i] && i <= index)
	{
		if (input[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (input[i] == '\"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		if (i == index)
			break;
	}
	if (in_single_quote)
		return (SINGLE_QUOTE);
	if (in_double_quote)
		return (DOUBLE_QUOTE);
	else
		return (0);
}

int is_quote(char c)
{
	if (c == '\'')
		return (SINGLE_QUOTE);
	if (c == '\"')
		return (DOUBLE_QUOTE);
	else
		return (0);
}

void all_quotes_remover_loop(char* string, char** token, char prev_quote)
{
	int i;
	int j;
	int is_in_quote;

	i			= -1;
	j			= 0;
	is_in_quote = false;
	while (string[++i])
	{
		if (is_quote(string[i]))
		{
			if (!is_in_quote)
				prev_quote = string[i];
			if (!is_in_quote)
				is_in_quote = true;
			else if (prev_quote != string[i])
				(*token)[j++] = string[i];
			else if (prev_quote == string[i])
				is_in_quote = false;
		}
		else
			(*token)[j++] = string[i];
	}
	(*token)[j] = 0;
}

char* all_quotes_remover(char* string)
{
	char* token;
	char  prev_quote;

	prev_quote = 0;
	token	   = palloc(ft_strlen(string) + 1, sizeof(char));
	if (!token)
		return (0);
	all_quotes_remover_loop(string, &token, prev_quote);
	return (token);
}

char* around_quotes_remover(char* string)
{
	char* res;
	int	  i;

	if (!is_quote(string[0]))
		return (ft_strdup(string));
	res = palloc(ft_strlen(string) + 1, sizeof(char));
	if (!res)
		return (0);
	i = 0;
	while (string[i])
	{
		res[i] = string[i + 1];
		i++;
	}
	res[i - 2] = 0;
	return (res);
}
