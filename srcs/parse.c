/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hthomas <hthomas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/17 15:52:09 by hthomas           #+#    #+#             */
/*   Updated: 2020/10/11 11:29:01 by hthomas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	replace_all_var_env(t_list_cmd *cmd, char **envp, int i)
{
	int		azerty;
	int		size;

	azerty = 0;
	while (envp[azerty])
	{
		if (!ft_strncmp(envp[azerty], &(cmd->str[i + 1]), ft_strlen(&(cmd->str[i])) - 1) && envp[azerty][ft_strlen(&(cmd->str[i])) - 1] == '=')
		{
			size = ft_strlen(&(cmd->str[i]));
			cmd->str[i] = '\0';// wil maybe cause some leaks later ¯\_(ツ)_/
			cmd->str = ft_strjoin_free(cmd->str, &(envp[azerty][size]));
			return ;
		}
		azerty++;
	}
	cmd->str[i] = '\0';
}

void	err_code(t_list_cmd *cmd, char **envp)
{
	ft_putstr("err_code\n");
	//!to do
	return ;
}

// int 	deal_backslash(t_list_command *cmd, char **envp)
// {
// 	int		i;

// 	while (cmd)
// 	{
// 		i = 0;
// 		while (cmd->str && cmd->str[i])
// 		{
// 			if (cmd->str[i] == '\\' && ...)
// 			{
// 				//? i++;
// 				//! to do
// 			}
// 			i++;
// 		}
// 		cmd = cmd->next;
// 	}
// 	return (0);
// }

int		replace_dollar_and_tild(t_list_cmd *cmd, char **envp)
{
	int		i;
	char	*tmp;

	while (cmd)
	{
		i = 0;
		while (cmd->str && cmd->str[i])
		{
			if (cmd->str[i] == '$' && !(cmd->flags & F_SIMPLE_QUOTES) && cmd->str[i + 1] > 32)
			{
				if (cmd->str[i + 1] == '?')
					err_code(cmd, envp);
				else
					replace_all_var_env(cmd, envp, i);
			}
			else if (cmd->str[i] == '~' && !in_quotes(cmd) && (!cmd->str[i + 1] || cmd->str[i + 1] == '/'))
			{
				tmp = cmd->str;
				cmd->str = ft_strdup(&find_var_env(envp, "HOME=")[5]);
				free(tmp);
			}
			i++;
		}
		cmd = cmd->next;
	}
	return (0);
}

void	delete_empty_elements(t_list_cmd *cmd)
{
	t_list_cmd	*tmp;

	while (cmd)
	{
		if (cmd->next)
		{
			if (!ft_strlen(cmd->next->str) && !in_quotes(cmd->next))
				c_lst_remove_next_one(cmd);
		}
		cmd = cmd->next;
	}
}

// void	ancien_parsing_a_supprimer(char *input, t_list_command **cmd)
// {
// 	char **tmp;
// 	int i;
// 	tmp = ft_split_set(input, WHITESPACES);
// 	i = 0;
// 	if (tmp[i])
// 		*cmd = c_lst_new(tmp[i++], '?'); // create fisrt element of the list
// 	while (tmp[i])
// 		c_lst_add_back(cmd, c_lst_new(tmp[i++], '?')); // fill the list
// 	ft_free_tab(tmp);
// }

int		input_to_command(char *input, t_list_cmd **cmd)
{
	t_parse		par;

	init_par(&par);
	while (input[par.i])
	{
		if (input[par.i] == '\'' && !par.in_double && !escaped(input, par.i))
			simple_quotes(input, cmd, &par);
		else if (input[par.i] == '\"' && !par.in_simple && !escaped(input, par.i))
			double_quotes(input, cmd, &par);
		else if (is_separator(&input[par.i]) && !escaped(input, par.i) && !par.in_simple && !par.in_double)
			separator(input, cmd, &par);
		// si je suis sur un mot et hors de quotes
		else if (!ft_in_charset(input[par.i], WHITESPACES) && !par.in_simple && !par.in_double)
			end_word(input, cmd, &par);
		par.i++;
	}
	return (par.in_simple || par.in_double);
}

int		parse_input(char *input, t_list_line **lst_line, char **envp)
{
	t_list_cmd	*cmd;

	cmd = NULL;
	if (input_to_command(input, &cmd))
		return (ERR);
	// deal_backslash((**lst_line)->cmd, envp);
	replace_dollar_and_tild(cmd, envp);
	// ancien_parsing_a_supprimer(input,( *lst_line)->cmd);
	delete_empty_elements(cmd);










	if (cmd)
		l_lst_add_back(lst_line, l_lst_copy_all(cmd, get_separator(cmd->str)));
	while (cmd)
	{
		// ft_putstr("\nnew while: ");
		// ft_putstr(cmd->str);
		// ft_putstr("\n");
		if (cmd->flags & F_SEPARATOR/*is_separator((cmd)->str)*/)
		{
			// ft_putstr("blocked?\n");
			// ft_putstr("add new cmd sep\n");
			l_lst_add_back(lst_line, l_lst_copy_all(cmd->next, get_separator(cmd->str)));
			// ft_putstr("not\n");
			// c_lst_clear(tmp);
			cmd->next = NULL;
		}
		t_list_cmd	*tmp = cmd;
		cmd = cmd->next;
		// c_lst_del_one(tmp);
	}
















	
	ft_putstr("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	t_list_line **copy = lst_line;
	while(*copy)
	{
		ft_putstr("--------------\n");
		cmd = (*copy)->cmd;
		while (cmd)
		{
			ft_putstr(cmd->str);
			ft_putstr("\n");
			cmd = cmd->next;
		}
		*copy = (*copy)->next;
	}
	// c_lst_clear(cmd);
	return (OK);
}
