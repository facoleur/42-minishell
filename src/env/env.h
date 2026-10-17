/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:53:55 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
#define ENV_H

#include "common/common.h"

/* env.c */
void print_env(char** tab);
int	 builtin_export(char*** env, char** var);
void unset_var(char** env, char* var);
void builtin_unset(char*** env, char** var);
void builtin_env(char** env);

/* env_utils.c */
char* ft_getenv(char* varname, char** env);
int	  does_var_exist(char** env, char* var);
void  export_var(char*** env, char* var);
int	  is_varname_valid(char* var);
int	  is_varvalue_valid(char* var);

/* env_copy.c */
char** copy_env(char** env, int size);
char** get_new_shlvl(char** env);

#endif
