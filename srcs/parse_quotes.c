/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hthomas <hthomas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/04 01:10:40 by hthomas           #+#    #+#             */
/*   Updated: 2020/10/20 09:05:20 by hthomas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	init_par(t_parse *par)
{
	par->in_simple = 0;
	par->in_double = 0;
	par->i = 0;
	par->pos = 0;
}

void	add_substr_to_cmd(char *input, t_list_cmd **cmd, int size, int flags)
{
	char *str;

	if (size <= 0)
		return ;
	while (!(flags & F_SIMPLE_QUOTES) && !(flags & F_DOUBLE_QUOTES) && ft_in_charset(*input, WHITESPACES))
		input++;
	str = ft_strndup(input, size);
	c_lst_add_back(cmd, c_lst_new(str, flags));
	free(str); //! todo
}

void	simple_quotes(char *input, t_list_cmd **cmd, t_parse *par)
{
	// ft_printf("simple quotes\n");
	if (!par->in_simple && par->i && !ft_in_charset(input[par->i - 1], WHITESPACES))
		add_substr_to_cmd(&input[par->pos], cmd, par->i - par->pos - 1, F_NO_SPACE_AFTER);
	else if (par->in_simple)
	{
		if (input[par->i + 1] && !ft_in_charset(input[par->i + 1], WHITESPACES))
			add_substr_to_cmd(&input[par->pos], cmd, par->i - par->pos, F_SIMPLE_QUOTES | F_NO_SPACE_AFTER);
		else
			add_substr_to_cmd(&input[par->pos], cmd, par->i - par->pos, F_SIMPLE_QUOTES);
	}
	par->pos = par->i + 1;
	par->in_simple += (par->in_simple == 0 ? 1 : -1);
}

void	double_quotes(char *input, t_list_cmd **cmd, t_parse *par)
{
	if (!par->in_double && par->i && !ft_in_charset(input[par->i - 1], WHITESPACES))
		add_substr_to_cmd(&input[par->pos], cmd, par->i - par->pos - 1, F_NO_SPACE_AFTER);
	else if (par->in_double)
	{
		if (input[par->i + 1] && !ft_in_charset(input[par->i + 1], WHITESPACES))
			add_substr_to_cmd(&input[par->pos], cmd, par->i - par->pos, F_DOUBLE_QUOTES | F_NO_SPACE_AFTER);
		else
			add_substr_to_cmd(&input[par->pos], cmd, par->i - par->pos, F_DOUBLE_QUOTES);
	}
	par->pos = par->i + 1;
	par->in_double += (par->in_double == 0 ? 1 : -1);
}

void	separator(char *input, t_list_cmd **cmd, t_parse *par)
{
		int end;

		while (input[par->pos] && ft_in_charset(input[par->pos], WHITESPACES)) // or  input[par->pos] <= 32) because this is causing segfaults/leaks when only arrows are pressed
			par->pos++;
		end = par->pos;
		if (!ft_in_charset(input[par->pos], SEPARATORS))
		{
			while (input[end] && !ft_in_charset(input[end], SEPARATORS))
				end++;
			add_substr_to_cmd(&input[par->pos], cmd, end - par->pos, F_NOTHING);
			if(!input[end])
				end--;
		}
		if (input[par->pos + 1] == '>')
			par->i++;
		add_substr_to_cmd(&input[end], cmd, par->i - end + 1, F_SEPARATOR);
		par->pos = end + 1;
		if (!ft_strncmp(&input[end], ">>", 2))
			par->pos++;
}

void	end_word(char *input, t_list_cmd **cmd, t_parse *par)
{
	// ft_printf("end_word:%d %d %s|\n", par->pos, par->i, &input[par->i]);
	// si je suis sur le dernier caractere du mot
	if (!input[par->i + 1] || (ft_in_charset(input[par->i + 1], WHITESPACES) && !escaped(input, par->i + 1)) || (ft_in_charset(input[par->i + 1], "\'\"") && !escaped(input, par->i + 1)))
	{
		// ft_printf("in\n");
		while (input[par->pos] && ((ft_in_charset(input[par->pos], WHITESPACES)))) // or  input[par->pos] <= 32) because this is causing leaks when only arrows are pressed
			par->pos++;
		add_substr_to_cmd(&input[par->pos], cmd, par->i - par->pos + 1, F_NOTHING);
		par->pos = par->i + 1;
	}
}
