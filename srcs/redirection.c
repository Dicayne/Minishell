/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hthomas <hthomas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/24 16:39:14 by hthomas           #+#    #+#             */
/*   Updated: 2020/11/29 11:02:41 by hthomas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int		get_filename_redir(t_list_cmd *cmd, char **filename)
{
	if (!cmd->next)
	{
		ft_putstr_fd("minishell: syntax error\n", STDERR);
		g_glob.exit = 2;
		return (FAILURE);
	}
	*filename = cmd->next->str;
	if (!*filename)
	{
		ft_putstr_fd("minishell: : No such file or directory\n", STDERR);
		g_glob.exit = 1;
	}
	else if (cmd->next->flags & F_VAR_PARSED)
	{
		if (cmd->next->flags & F_DOUBLE_QUOTE)
		{
			ft_putstr_fd("minishell: : No such file or directory\n", STDERR);
			g_glob.exit = 1;
			return (FAILURE);
		}
		else if (!(cmd->next->flags & F_SIMPLE_QUOTE))
		{
			ft_putstr_fd("minishell: ambiguous redirect\n", STDERR);
			g_glob.exit = 1;
			return (FAILURE);
		}
	}
	return (SUCCESS);
}

int		open_fd(t_list_line *lst_line, t_list_cmd *cmd)
{
	char	*filename;

	if (get_filename_redir(cmd, &filename))
		return (FAILURE);
	else if (cmd->flags & F_INPUT)
		lst_line->input = open(filename, O_RDONLY); //! todo
	else if (cmd->flags & F_APPEND)
		lst_line->output = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (cmd->flags & F_OUTPUT)
		lst_line->output = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (lst_line->input < 0)
	{
		ft_putstr_fd("minishell: ", STDERR);
		ft_putstr_fd(filename, STDERR);
		ft_putstr_fd(": No such file or directory\n", STDERR);
		g_glob.exit = 1;
		return (FAILURE);
	}
	if (lst_line->output < 0)
		ft_putstr_fd("error open output\n", STDERR);
	if (cmd->flags & F_INPUT)
		dup2(lst_line->input, STDIN);
	if (cmd->flags & F_OUTPUT)
		dup2(lst_line->output, STDOUT);
	c_lst_remove_next_one(cmd);
	return (SUCCESS);
}

int		redirections(t_list_line *lst_line)
{
	t_list_cmd	*cmd;
	t_list_cmd	*tmp;

	cmd = lst_line->cmd;
	if (cmd->flags & F_REDIRS)
	{
		if (open_fd(lst_line, cmd))
			return (FAILURE);
		tmp = cmd;
		lst_line->cmd = cmd->next;
		cmd = lst_line->cmd;
		c_lst_free_one(tmp);
	}
	while (cmd && cmd->next)
	{
		if (cmd->next->flags & F_REDIRS)
		{
			if (open_fd(lst_line, cmd->next))
				return (FAILURE);
			c_lst_remove_next_one(cmd);
			continue ;
		}
		cmd = cmd->next;
	}
	return (SUCCESS);
}
