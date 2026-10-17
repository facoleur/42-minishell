/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "parser/node.h"

/* redirection.c */
int	 exe_infile(t_node* node);
int	 exe_outfile(t_node* node);
int	 exe_outfile_append(t_node* node);
int	 exe_heredoc(t_node* node);
void heredoc_child(t_node* node);

/* dispatch.c */
int run_redirection_file(t_node* node);
int run_outfile(t_node* node_tmp, t_node* node);

#endif
