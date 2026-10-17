/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STACK_H
#define STACK_H

#include "common/common.h"

typedef struct s_stack
{
	int				value;
	struct s_stack* next;
} t_stack;

t_stack* stack_new(int content);
void	 stack_add(t_stack** lst, int value);
void	 stack_drop(t_stack** lst);

#endif
