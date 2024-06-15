#include "../../inc/parsing.h"
#include "../../inc/minishell.h"

char	*uniq_name(char *prefix)
{
	char	*res;
	char	*tmp;

	res = ft_strjoin("/tmp/", prefix);
	if(!res)
		return (NULL);
	while(access(res, F_OK) == 0)
	{
		tmp = ft_strjoin(res, prefix);
		free(res);
		if(!tmp)
			return (NULL);
		res = tmp;
	}
	return (res);
}

void	close_hd(int file_fd, int prev_fd)
{
	close (file_fd);
	signal(SIGINT, SIG_IGN);
	dup2(prev_fd, STDIN_FILENO);
	close(prev_fd);
}

void	do_hd(t_group *group, t_list_env *env, int file_fd)
{
	char	*line;
	char	*tmp;

	line = readline ("> ");
	while (line && errno != EINTR && ft_strcmp(line, group->app_in))
	{
		tmp = ft_expand(line, env);
		if(!tmp || !(*tmp))
		{
			free(line);
			if(tmp)
				free(tmp);
			continue ;
		}
		ft_putstr_fd(tmp, file_fd);
		free (line);
		line = readline ("> ");
		if (!line && errno != EINTR && errno != EBADF)
			ft_putstr_err("\n");
		if (errno == EINTR || errno == EBADF)
		{
			free (line);
			status = 130;
		}
		ft_putstr_fd("\n", file_fd);
	}
}

char	*heredoc(t_group *group, t_list_env *env)
{
	char	*file_name;
	int		file_fd;
	int		prev_fd;

	prev_fd = STDIN_FILENO;				//to delete later
	
	file_name = uniq_name(group->app_in);
	prev_fd = dup(STDIN_FILENO);
	file_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(file_fd < 0)
	{
		free (file_name);
		return (NULL);
	}
	signal(SIGINT, ft_sigint_hd);
	do_hd(group, env, file_fd);
	close_hd(file_fd, prev_fd);
	return (file_name);
}
