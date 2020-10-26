/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoreau <vmoreau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/24 13:04:47 by hthomas           #+#    #+#             */
/*   Updated: 2020/10/26 20:42:15 by vmoreau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	**get_paths(char **envp)
{
	int		i;
	char	**path;

	i = 0;
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5))
		i++;
	path = ft_split(&envp[i][5], ':');
	return (path);
}

void	binary_not_found(char *path, int *ret)
{
	struct stat	buf;
	int			dir;

	ft_putstr_fd("minishell: ", STDERR);
	ft_putstr_fd(path, STDERR);
	dir = lstat(path, &buf);
	if (dir == 0)
		ft_putstr_fd(": Is a directory\n", STDERR);
	else
		ft_putstr_fd(": No such file or directory\n", STDERR);
	*ret = FAILURE;
}

void	try_path2(t_list_cmd *cmd, char **envp, char **argv, int *ret)
{
	int		i;
	int		cpt;
	char	**path;
	char	*full_path;

	i = 0;
	cpt = 0;
	path = get_paths(envp);
	while (path[i])
	{
		full_path = ft_strjoin(path[i], "/");
		full_path = ft_strjoin_free(full_path, cmd->str);
		if (execve(full_path, argv, envp))
			cpt++;
		free(full_path);
		if (i != cpt)
			*ret = SUCCESS;
		i++;
	}
	ft_free_tab(path);
}

int		try_path(t_list_cmd *cmd, char **envp)
{
	int		ret;
	char	**argv;

	ret = FAILURE;
	if (!(argv = lst_to_strs(cmd)))
		return (FAILURE);
	if (*cmd->str == '/')
	{
		if (execve(cmd->str, argv, envp))
			binary_not_found(cmd->str, &ret);
	}
	else
		try_path2(cmd, envp, argv, &ret);
	ft_free_tab(argv);
	return (ret);
}

int		search_command(t_list_cmd *cmd, t_list *envp)
{
	int		ret;
	int		status;
	pid_t	pid;
	char	**env;

	pid = fork();
	env = lst_to_chartab(envp);
	if (pid == 0)
	{
		if (try_path(cmd, env))
			exit(0);
		ret = FAILURE;
	}
	else
	{
		wait(&status);
		ret = SUCCESS;
	}
	ft_free_tab(env);
	return (ret);
}
