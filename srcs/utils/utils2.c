/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hthomas <hthomas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 15:13:24 by vmoreau           #+#    #+#             */
/*   Updated: 2020/12/07 20:01:37 by hthomas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int		get_flags(char *str)
{
	int		flags;

	flags = 0;
	if (*str == ';')
		flags += F_SEMICOLON;
	else if (*str == '|')
		flags += F_PIPE;
	else if (*str == '<')
		flags += F_INPUT;
	else if (*str == '>')
	{
		flags += F_OUTPUT;
		if (str[1] == '>')
			flags += F_APPEND;
	}
	return (flags);
}

void	cmd_plusplus_free(t_list_cmd **cmd)
{
	t_list_cmd	*tmp;

	tmp = *cmd;
	*cmd = (*cmd)->next;
	c_lst_free_one(tmp);
}

char	**cmd_to_strs(t_list_cmd *cmd)
{
	char	**argv;
	int		i;

	if (!(argv = malloc(sizeof(*argv) * (c_lst_size(cmd) + 1))))
		return (NULL);
	i = 0;
	while (cmd)
	{
		argv[i++] = ft_strdup(cmd->str);
		cmd = cmd->next;
	}
	argv[i] = NULL;
	return (argv);
}

void	remove_double_char(char *str, char *charset)
{
	int	i;

	i = 0;
	if (!str[i] || !str[i + 1])
		return ;
	while (str[i])
	{
		if (ft_in_charset(str[i], charset) &&
			ft_in_charset(str[i + 1], charset))
		{
			ft_strcpy(&str[i], &str[i + 1]);
			continue;
		}
		i++;
	}
}

void	print_welcome_msg(void)
{
	ft_putstr_fd("            ▄███████████████▄           \n", STDERR);
	ft_putstr_fd("         ██████▀   ██   ▀███████        \n", STDERR);
	ft_putstr_fd("      ███   ▀███   ██   ███▀   ███      \n", STDERR);
	ft_putstr_fd("     ██████   ██   ██   ██   ██████     \n", STDERR);
	ft_putstr_fd("     ██   ██   ██  ██  ██   ██   ██     \n", STDERR);
	ft_putstr_fd("    ███    ██  ██  ██  ██  ██    ███    \n", STDERR);
	ft_putstr_fd("    ██ ██   ██  █  ██  █  ██   ██ ██    \n", STDERR);
	ft_putstr_fd("    ██  ███  ██ ██ ██ ██ ██  ███  ██    \n", STDERR);
	ft_putstr_fd("    ██    ██  ██ █ ██ █ ██  ██    ██    \n", STDERR);
	ft_putstr_fd("    ████▄   ██ █  █  █  █ ██   ▄████    \n", STDERR);
	ft_putstr_fd("       ████   █          █   ████       \n", STDERR);
	ft_putstr_fd("          ██                ██          \n", STDERR);
	ft_putstr_fd("          ████████▄  ▄████████          \n", STDERR);
	ft_putstr_fd("                  ▀██▀                  \n", STDERR);
	ft_putstr_fd("           _       _     _          _ _\n", STDERR);
	ft_putstr_fd(" _ __ ___ (_)_ __ (_)___| |__   ___| | |\n", STDERR);
	ft_putstr_fd("| '_ ` _ \\| | '_ \\| / __| '_ \\ / _ \\ | |\n", STDERR);
	ft_putstr_fd("| | | | | | | | | | \\__ \\ | | |  __/ | |\n", STDERR);
	ft_putstr_fd("|_| |_| |_|_|_| |_|_|___/_| |_|\\___|_|_|\n\n", STDERR);
}
