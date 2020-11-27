/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hthomas <hthomas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/04 01:10:40 by hthomas           #+#    #+#             */
/*   Updated: 2020/10/28 15:10:26 by hthomas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	simple_quotes(char *input, t_list_cmd **cmd, t_parse *par)
{
	if (!par->in_simple && par->i && !ft_in_charset(input[par->i - 1], WSP))
		add_cmd(&input[par->pos], cmd, par->i - par->pos - 1, F_NO_SP_AFTER);
	else if (par->in_simple)
	{
		if (input[par->i + 1] && !ft_in_charset(input[par->i + 1], WSP))
			add_cmd(&input[par->pos], cmd, par->i - par->pos, F_SIMPLE_QUOTE |\
			F_NO_SP_AFTER);
		else
			add_cmd(&input[par->pos], cmd, par->i - par->pos, F_SIMPLE_QUOTE);
	}
	par->pos = par->i + 1;
	par->in_simple += (par->in_simple == 0 ? 1 : -1);
}

void	double_quotes(char *input, t_list_cmd **cmd, t_parse *par)
{
	if (!par->in_double && par->i && !ft_in_charset(input[par->i - 1], WSP))
		add_cmd(&input[par->pos], cmd, par->i - par->pos - 1, F_NO_SP_AFTER);
	else if (par->in_double)
	{
		if (input[par->i + 1] && !ft_in_charset(input[par->i + 1], WSP))
			add_cmd(&input[par->pos], cmd, par->i - par->pos, F_DOUBLE_QUOTE |\
			F_NO_SP_AFTER);
		else
			add_cmd(&input[par->pos], cmd, par->i - par->pos, F_DOUBLE_QUOTE);
	}
	par->pos = par->i + 1;
	par->in_double += (par->in_double == 0 ? 1 : -1);
}

void	separator(char *input, t_list_cmd **cmd, t_parse *par)
{
	int end;

	// ft_printf("separator !!!\n");
	while (input[par->pos] && ft_in_charset(input[par->pos], WSP)) // or  input[par->pos] <= 32) because this is causing segfaults/leaks when only arrows are pressed
		par->pos++;
	end = par->pos;
	if (!ft_in_charset(input[par->pos], SYMBOLS))
	{
		while (input[end] && !ft_in_charset(input[end], SYMBOLS))
			end++;
		add_cmd(&input[par->pos], cmd, end - par->pos, F_NOTHING);
		if (!input[end])
			end--;
	}
	if (input[par->pos + 1] == '>')
		par->i++;
	add_cmd(&input[end], cmd, par->i - end + 1, get_flags(&input[end]));
	// ft_printf("input:%s\tsep:%d\n", &input[end], get_flags(&input[end]));
	par->pos = end + 1;
	if (!ft_strncmp(&input[end], ">>", 2))
		par->pos++;
}

void	word(char *input, t_list_cmd **cmd, t_parse *par)
{
	while (input[par->pos] && ((ft_in_charset(input[par->pos], WSP)))) // or  input[par->pos] <= 32) because this is causing leaks when only arrows are pressed
		par->pos++;
	if (input[par->i + 1] && !ft_in_charset(input[par->i + 1], WSP))
		add_cmd(&input[par->pos], cmd, par->i - par->pos + 1, F_NO_SP_AFTER);
	else
		add_cmd(&input[par->pos], cmd, par->i - par->pos + 1, F_NOTHING);
	par->pos = par->i + 1;
}

int		is_in_word(char *input, t_parse *par)
{
	return (((!ft_in_charset(input[par->i], WSP) ||\
	escaped(input, par->i)) ||\
	(!ft_in_charset(input[par->i], "\'\"") || escaped(input, par->i)))\
	&& !par->in_simple && !par->in_double);
}

int		next_is_outside_word(char *input, t_parse *par)
{
	return (!input[par->i + 1] ||\
	(ft_in_charset(input[par->i + 1], WSP) && !escaped(input, par->i + 1)) ||\
	(ft_in_charset(input[par->i + 1], "\'\"") &&\
	(!escaped(input, par->i + 1) || par->in_simple)));
}

int		input_to_command(char *input, t_list_cmd **cmd)
{
	t_parse		par;

	init_par(&par);
	while (input[par.i])
	{
		if (input[par.i] == '\'' && (!escaped(input, par.i) ||\
		par.in_simple) && !par.in_double)
			simple_quotes(input, cmd, &par);
		else if (input[par.i] == '\"' && !escaped(input, par.i) &&\
		!par.in_simple)
			double_quotes(input, cmd, &par);
		else if (is_separator(input, par.i) && !escaped(input, par.i) &&\
		!par.in_simple && !par.in_double)
			separator(input, cmd, &par);
		// si je suis sur un mot et hors de quotes
		else if (is_in_word(input, &par) && next_is_outside_word(input, &par))
			word(input, cmd, &par);
		par.i++;
	}
	// ft_putnbr(par.in_simple || par.in_double);
	return (par.in_simple || par.in_double);
}