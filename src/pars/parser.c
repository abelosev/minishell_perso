/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aauthier <aauthier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 21:13:51 by abelosev          #+#    #+#             */
/*   Updated: 2024/06/14 04:29:56 by aauthier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

t_group	*unclosed_quotes(t_group *group, t_parser *p)
{
	ft_putstr_err("Invalid syntax: unclosed quotes\n");
	invalid_group(group, 2);
	free_t_parser(p);
	return (group);
}

t_group	*group_list(t_parser *p, t_group *group, t_list_env *env)
{
	p->token_tab = ft_split1(p->line, 1);
	if (p->token_tab == NULL)
	{
		free_t_parser(p);
		return (NULL);
	}
	spaces_between_quotes(&(p->token_tab));
	p->token_list = lexer(p->token_tab);
	if (p->token_list == NULL)
	{
		free_t_parser(p);
		return (NULL);
	}
	if (syntax_pb(p->token_list))
		invalid_group(group, 2);
	else
	{
		free(group);
		group = get_group_list(p->token_list, env);
	}
	return (group);
}

t_group	*parser(char *input, t_list_env *env)
{
	t_parser	*p;
	t_group		*group;

	p = create_init_p();
	group = create_init_group();
	if (quotes_ok(&input) == 0)
		return (unclosed_quotes(group, p));
	p->line = quotes_expand(input, env);
	if (p->line == NULL)
	{
		free_t_parser(p);
		if (input)
		{
			invalid_group(group, 127);
			ft_putstr_err("Command not found\n");
		}
		else
			invalid_group(group, 2);
		return (group);
	}
	group = group_list(p, group, env);
	free_t_parser(p);
	return (group);
}
