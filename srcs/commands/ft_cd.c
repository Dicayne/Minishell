/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoreau <vmoreau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:16:24 by vmoreau           #+#    #+#             */
/*   Updated: 2020/10/26 20:47:00 by vmoreau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*find_var_env(t_list *envp, char *var)
{
	int		i;

	i = 0;
	while (envp)
	{
		if (!ft_strncmp(envp->content, var, ft_strlen(var)))
			return (envp->content);
		i++;
	}
	return (NULL);
}

char	*ft_cd(t_list_cmd *args, t_list *envp)
{
	char		*ret;
	struct stat	stats;

	if (!args || !args->str)
		chdir(&find_var_env(envp, "HOME=")[5]);
	else if (stat(args->str, &stats) != 0)
	{
		ret = ft_strdup("cd: no such file or directory: ");
		ret = ft_strjoin_free(ret, args->str);
		ret = ft_strjoin_free(ret, "\n");
		return (ret);
	}
	else if (chdir(args->str))
	{
		ret = (ft_strdup("cd: not a directory: "));
		ret = ft_strjoin_free(ret, args->str);
		ret = ft_strjoin_free(ret, "\n");
		return (ret);
	}
	return (ft_strdup(""));
}