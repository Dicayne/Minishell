/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoreau <vmoreau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/20 18:23:14 by hthomas           #+#    #+#             */
/*   Updated: 2020/12/16 10:59:37 by vmoreau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	print_prompt(void)
{
	char	*pwd;

	if (g_glob.exit == 0)
		ft_putstr_fd("\033[1;32m➜ \033[1;36m", STDERR);
	else
		ft_putstr_fd("\033[1;31m➜ \033[1;36m", STDERR);
	pwd = getcwd(NULL, 0);
	if (!pwd)
		ft_putstr_fd(g_glob.path, STDERR);
	else
		ft_putstr_fd(pwd, STDERR);
	free(pwd);
	ft_putstr_fd("\033[0m$ ", STDERR);
}

void	not_found(t_list_line *lst_line, t_list *env)
{
	char	*ret;

	ret = ft_strdup(lst_line->cmd->str);
	ret = ft_strjoin_free(ret, ": command not found\n");
	g_glob.exit = CMD_NOT_FOUND;
	ft_putstr_fd(ret, STDERR);
	free(ret);
	clear_env_lst(env);
	l_lst_clear(lst_line);
	free(g_glob.path);
	exit(CMD_NOT_FOUND);
}

int		print_synt_err(void)
{
	g_glob.exit = 2;
	ft_putstr_fd("minishell: syntax error\n", STDERR);
	return (-1);
}

void	parse_error(char *input, t_list_line *lst_line)
{
	print_synt_err();
	print_prompt();
	l_lst_clear(lst_line);
	free(input);
}
