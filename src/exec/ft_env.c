#include "../../inc/minishell.h"

int ft_env(t_group *group, t_list_env *env, int fd)
{
	if (group->cmd[1] != NULL) //проверить
	{
		if(ft_strncmp(group->cmd[1], "env", ft_strlen(group->cmd[1])))
		{
			ft_putstr_err("env : ");
			ft_putstr_err(group->cmd[1]);
			ft_putstr_err(": No such file or directory\n");
		}
		else
			ft_putstr_err("env: warning: additional arguments to env are ignored\n");
		return (127); //проверить, не 125 ли
	}
	print_env_list(env, fd);
	return (0);
}