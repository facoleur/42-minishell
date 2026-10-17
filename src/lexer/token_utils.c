/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:31:29 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer/lexer.h"

int get_elem_count_tok(t_tokens** array)
{
	int i;

	i = 0;
	while (array[i])
	{
		i++;
	}
	return (i);
}

void free_tokens(t_tokens** tokens)
{
	int i;

	if (!tokens)
		return;
	i = 0;
	while (tokens[i])
	{
		if (tokens[i]->tok)
			free(tokens[i]->tok);
		free(tokens[i]);
		i++;
	}
	free(tokens);
}
