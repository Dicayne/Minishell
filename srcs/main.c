/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hthomas <hthomas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/15 19:21:43 by hthomas           #+#    #+#             */
/*   Updated: 2020/11/24 16:29:33 by hthomas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*exec_cmd(t_list_cmd *cmd, t_list *env)
{
	if (!cmd)
		return (NULL);
	else if (!ft_strcmp(cmd->str, "echo"))
		return (ft_echo(cmd->next));
	else if (!ft_strcmp(cmd->str, "cd"))
		return (ft_cd(cmd->next, env));
	else if (!ft_strcmp(cmd->str, "pwd"))
		return (ft_pwd());
	else if (!ft_strcmp(cmd->str, "export"))
		return (ft_export(cmd->next, env));
	else if (!ft_strcmp(cmd->str, "unset"))
		return (ft_unset(cmd->next, env));
	else if (!ft_strcmp(cmd->str, "env"))
		return (ft_env(env));
	else if (!ft_strcmp(cmd->str, "exit"))
		return (ft_exit(cmd->next, env));
	else if (!search_command(cmd, env))
		not_found(cmd->str);
	return (NULL);
}


int		filename_redir(t_list_cmd *cmd, char **filename)
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

	if (filename_redir(cmd, &filename))
		return (FAILURE);
	//else if (cmd->flags & F_INPUT)
	// lst_line->input = filename; //! todo
	else if (cmd->flags & F_APPEND)
		lst_line->output = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (cmd->flags & F_OUTPUT)
		lst_line->output = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (cmd->flags & F_INPUT)
		dup2(lst_line->input, STDIN);
	if (cmd->flags & F_OUTPUT)
		dup2(lst_line->output, STDOUT);
	if (lst_line->output < 0)
		ft_putstr_fd("error open\n", STDERR);
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
	while (cmd)
	{
		if (cmd->next && cmd->next->flags & F_REDIRS)
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

/*
** add flags together, for example 011 + 001 = 011 (and not 100 like addition)
*/

int 	bits_per_bits_or(int flags1, int flags2)
{
	int	sum;
	int	max;
	int	size;

	size = 0;
	max = ft_max_int(flags1, flags2);
	while (max)
	{
		max = max >> 1;
		size++;
	}
	sum = 0;
	while (size + 1)
	{
		sum += (flags1 >> size == 1 | flags2 >> size == 1) << size;
		size--;
	}
	return (sum);
}

void	fusion_cmd(t_list_cmd *cmd)
{
	while (cmd)
	{
		while (cmd->flags & F_NO_SP_AFTER && cmd->next && !(cmd->next->flags & F_SPECIALS))
		{
			if (!(cmd->next->flags & F_NO_SP_AFTER))
				cmd->flags -= F_NO_SP_AFTER;
			cmd->str = ft_strjoin_free(cmd->str, cmd->next->str);
			cmd->flags = bits_per_bits_or(cmd->flags, cmd->next->flags);
			c_lst_remove_next_one(cmd);
		}
		cmd = cmd->next;
	}
}

t_list_cmd	*split_add_back(t_list_cmd *cmd, void (*del)(t_list_cmd *), t_list_cmd *to_del)
{
	t_list_cmd	*next;
	int			flags;
	char		**tab;
	int			i;

	next = cmd->next;
	flags = cmd->flags - F_VAR_ENV;
	tab = ft_split_set(cmd->str, WSP);
	del(to_del);
	cmd = NULL;
	i = 0;
	if (!tab[i])
		c_lst_add_back(&cmd, c_lst_new("", F_VAR_PARSED + flags));
	while (tab[i])
		c_lst_add_back(&cmd, c_lst_new(tab[i++], F_VAR_PARSED + flags));
	c_lst_add_back(&cmd, next);
	ft_free_tab(tab);
	return (cmd);
}

t_list_cmd	*reparse_var_env(t_list_cmd *cmd)
{
	t_list_cmd	*start;

	start = NULL;
	while (cmd)
	{
		if (cmd->flags & F_VAR_ENV)
			cmd = split_add_back(cmd, c_lst_free_one, cmd);
		if (cmd->next && cmd->next->flags & F_VAR_ENV)
			cmd->next = split_add_back(cmd->next, c_lst_remove_next_one, cmd);
		if (!start)
			start = cmd;
		cmd = cmd->next;
	}
	return (start);
}

int		make_and_exec_cmd(t_list_line *lst_line, t_list *env, char **ret)
{
	replace_all_var_env(lst_line->cmd, env);
	fusion_cmd(lst_line->cmd);
	lst_line->cmd = reparse_var_env(lst_line->cmd);
	if (delete_backslashes(lst_line->cmd, env))
		return (FAILURE);
				// ft_printf("--------1--------\n");
				// t_list_cmd *copy = lst_line->cmd;
				// while (copy)
				// {
				// 	ft_printf("F:%d\t%s\n", copy->flags, copy->str);
				// 	copy = copy->next;
				// }
	if (redirections(lst_line))
		return (FAILURE);
	*ret = exec_cmd(lst_line->cmd, env);
	if (lst_line->output > 2 && close(lst_line->output) < 0)
		ft_putstr_fd("error close\n", STDERR);
	return (SUCCESS);
}

void	create_pipes_and_semicolon(t_list_line *lst_line, t_list *env)
{
	t_list_cmd	*cmd;
	char		*ret;
	t_list_line	*start;

	// while (lst_line)
	// {
	// 	if (lst_line->pipe)
	// 	{
	// 		int tmp = lst_line->output;
	// 		lst_line->output = lst_line->next->input;
	// 		lst_line->next->input = tmp;
	// 		ft_printf("output:%d\n", lst_line->output);
	// 		ft_printf("input:%d\n\n", lst_line->next->input);
	// 	}
	// 	lst_line = lst_line->next;
	// }

	start = lst_line;
	while (lst_line)
	{
		if (lst_line->pipe)
		{
			int		fdpipe[2]; // Used to store two ends of first pipe
			pid_t	p;

			if (pipe(fdpipe) == -1) //error
			{
				ft_putstr_fd("pipe: pipe failed\n", STDERR);
				return ;
			}
			// do something ?
			p = fork();
			if (p < 0) //error
			{
				ft_putstr_fd("pipe: fork failed\n", STDERR);
				return ;
			}
			else if (p > 0) //parent process
			{
				close(fdpipe[1]);
				char	*line;
				wait(NULL);
						ft_printf("***Parent\n");
						if (get_next_line(&line, fdpipe[0]) == -1)
							return ;
						ft_printf("***|%s|\n", line);
						free(line);
				close(fdpipe[0]);
			}
			else // child process
			{
				close(fdpipe[0]);
				// lst_line->output = fdpipe[1];
				if (!make_and_exec_cmd(lst_line, env, &ret))
				{
					write(fdpipe[1], ret, strlen(ret) + 1);
					free(ret);
				}
				else
					g_glob.exit = 127;
				close(fdpipe[1]);
				ft_printf("***End child\n");
				exit(0);
			}
			lst_line = lst_line->next; // maybe useless
			break;
		}
		lst_line = lst_line->next;
	}
}

void	exec_line(t_list_line *lst_line, t_list *env)
{
	char		*ret;
	t_list_line	*start;

	int		fd_outold;
	int		fd_inold;
	fd_outold = dup(STDOUT);
	fd_inold = dup(STDIN);
	create_pipes_and_semicolon(lst_line, env);
	start = lst_line;
	while (lst_line)
	{
		if (make_and_exec_cmd(lst_line, env, &ret))
			break;
		if (ret)
		{
			ft_putstr_fd(ret, lst_line->output);
			free(ret);
		}
		dup2(fd_outold, STDOUT);
		dup2(fd_inold, STDIN);
		lst_line = lst_line->next;
	}
	l_lst_clear(start);
}

void	fill_env(t_list **env)
{
	char	*keyval;
	char	*pwd;

	keyval = ft_strdup("SHLVL=0");
	ft_lstadd_back(env, ft_lstnew(keyval));
	pwd = getcwd(NULL, 0);
	keyval = ft_strjoin("PWD=", pwd);
	free(pwd);
	ft_lstadd_back(env, ft_lstnew(keyval));
	keyval = ft_strdup("OLDPWD=");
	ft_lstadd_back(env, ft_lstnew(keyval));
}

void	set_env(char **envp, t_list **env)
{
	int		i;
	char	*keyval;

	i = 0;
	*env = NULL;
	if (envp[i])
	{
		while (envp[i])
		{
			keyval = ft_strdup(envp[i]);
			ft_lstadd_back(env, ft_lstnew(keyval));
			i++;
		}
	}
	else
		fill_env(env);
}

void	increment_shlvl(t_list *env)
{
	t_list_cmd	*args;
	char		*tmp;
	int			sh_lvl;

	args = c_lst_new("$SHLVL", F_VAR_ENV);
	replace_all_var_env(args, env);
	sh_lvl = ft_atoi(args->str);
	c_lst_clear(args);
	args = c_lst_new("SHLVL", F_NOTHING);
	tmp = ft_itoa(sh_lvl + 1);
	args->str = ft_strjoin_free(args->str, "=");
	args->str = ft_strjoin_free(args->str, tmp);
	free(tmp);
	tmp = ft_export(args, env);
	free(tmp);
	c_lst_clear(args);
}

void	sighandler(int signum)
{
	if (signum == SIGINT)
	{
		ft_putstr_fd("\n", STDOUT);
		print_prompt();
		g_glob.exit = 130;
	}
	else if (signum == SIGQUIT)
	{
		ft_putstr_fd("Quit (core dumped)\n", STDOUT);
		print_prompt();
		g_glob.exit = 131;
	}
}

int		main(const int argc, char *argv[], char *envp[])
{
	char		*input;
	t_list_line	*lst_line;
	t_list		*env;

	if (argc != 1)
	{
		ft_putstr_fd("ERROR: Too many argument\n", STDERR);
		return (FAILURE);
	}
	g_glob.exit = 0;
	g_glob.path = getcwd(NULL, 0);
	signal(SIGINT, sighandler);
	signal(SIGQUIT, sighandler);
	set_env(envp, &env);
	ft_putstr(WELCOME_MSG);
	increment_shlvl(env);
	print_prompt();
	while (get_next_line(&input, 0) > 0)
	{
		lst_line = NULL;
		if (parse_input(input, &lst_line, env))
		{
			parse_error(input, lst_line);
			continue;
		}
		exec_line(lst_line, env);
		free(input);
		print_prompt();
	}
	free(g_glob.path);
	clear_env_lst(env);
	free(input);
	ft_printf("\n");
	return (g_glob.exit);
}
