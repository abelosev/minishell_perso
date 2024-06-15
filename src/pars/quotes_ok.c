/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_ok.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aauthier <aauthier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 20:32:23 by abelosev          #+#    #+#             */
/*   Updated: 2024/06/15 21:18:57 by aauthier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

void	double_detected(char **str, int *res, int *start, int *i)
{
	*res = 0;
	*start = *i;
	while ((*str)[*i])
	{
		(*i)++;
		if ((*str)[*i] == '"')
		{
			*res = 1;
			(*str)[*start] = 29;
			if (*start > 0 && (*str)[(*start) - 1] == '$')
				(*str)[*start] = 30;
			(*str)[*i] = 31;
			break ;
		}
	}
}

void	single_detected(char **str, int *res, int *start, int *i)
{
	*res = 0;
	*start = *i;
	while ((*str)[*i])
	{
		(*i)++;
		if ((*str)[*i] == '\'')
		{
			*res = 1;
			(*str)[*start] = 30;
			(*str)[*i] = 31;
			break ;
		}
	}
}

int	quotes_ok(char **str)
{
	int	i;
	int	res;
	int	start;

	i = 0;
	res = 1;
	while ((*str)[i])
	{
		if ((*str)[i] == '"')
			double_detected(str, &res, &start, &i);
		else if ((*str)[i] == '\'')
			single_detected(str, &res, &start, &i);
		if ((*str)[i])
			i++;
	}
	return (res);
}
