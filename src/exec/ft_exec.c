#include "../../inc/minishell.h"

int	is_redir(t_group *group)
{
	return(group->redir_in || group->redir_out || group->app_out);
}

int	open_redir(t_group *group, int *fd_in, int *fd_out)
{
	if (group->redir_in)
	{
		*fd_in = open(group->redir_in, O_RDONLY);
		if (*fd_in < 0)
			return 1;
	}
	if (group->redir_out)
	{
		*fd_out = open(group->redir_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (*fd_out < 0)
			return 1;
	}
	if (group->app_out)
	{
		*fd_out = open(group->app_out, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (*fd_out < 0)
			return 1;
	}
	return 0;
}

unsigned int	exec_builtin(t_group *group, t_list_env *env, t_parsed *p, int fd_out)
{
	status = ft_do_builtin(group, env, fd_out);
	if(is_built(group->cmd[0]) == B_EXIT)
	{
		free_envp_list(env);
		free_parsed(p);
		clear_history();
		exit(status);
	}
	return (status);
}

unsigned int	ft_cmd_child(t_group *group, char **new_envp, int fd_in, int fd_out)
{
    if (fd_in != STDIN_FILENO)
	{
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }
    if (fd_out != STDOUT_FILENO)
	{
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
    execve(group->cmd[0], group->cmd, new_envp);
    return (1);
}

unsigned int	ft_cmd_parent(int fd_in, int fd_out, pid_t pid)
{
    int	e_status;

	e_status = (int)status;
    waitpid(pid, &e_status, 0);
    if (fd_in != STDIN_FILENO)
        close(fd_in);
    if (fd_out != STDOUT_FILENO)
        close(fd_out);
    if (WIFEXITED(e_status))
        status = WEXITSTATUS(e_status);
    else
        status = 1;
    return (status);
}

unsigned int ft_cmd(t_group *group, t_list_env *env, int fd_in, int fd_out)
{
    pid_t pid;
    char **new_envp;

    new_envp = get_envp(env);
    if (!new_envp)
		return (ft_putstr_err("error : new_envp"), 1);
    if (open_redir(group, &fd_in, &fd_out))
		return (free_tab(new_envp), 1);
	pid = fork();
    if (pid == 0)
	{
        status = ft_cmd_child(group, new_envp, fd_in, fd_out);
        return (free_tab(new_envp), status);
	}
    else if (pid > 0)
        status = ft_cmd_parent(fd_in, fd_out, pid);
    else
		return(ft_putstr_err("fork error"), 1);
    free(new_envp);
    return status;
}

int	do_redir(t_group *group, t_parsed *p, t_list_env *env, int fd_in, int fd_out) //to fix the 5 parameters situation
{
    int new_fd_in;
    int new_fd_out;

	new_fd_in = fd_in;
	new_fd_out = fd_out;
    if (open_redir(group, &new_fd_in, &new_fd_out))
    {
        return (1);
    }
    if (is_built(group->cmd[0]) != 0)
		status = exec_builtin(group, env, p, fd_out);
	else
		status = ft_cmd(group, env, fd_in, fd_out);
    if (new_fd_in != fd_in)
	{
		close(new_fd_in);
		dup2(STDIN_FILENO, fd_in);
	}
    if (new_fd_out != fd_out)
	{
		close(new_fd_out);
		dup2(STDOUT_FILENO, fd_out);
	}
    return (status);
}

unsigned int	ft_exec(t_parsed *p, t_list_env *env)
{
    t_group *curr;
    int fd_in;
	int file_fd;
	int64_t fd_out;
    int pipefd[2];
	char buffer[4096];		//можно ли так делать?
    ssize_t bytes_read;
    // int e_status;

	fd_in = get_hd_fd(p, env);	// передавать ли fd_in параметром (?)
    curr = p->group;
    while (curr)
    {
        if (curr->next)
        {
            if (pipe(pipefd) == -1)
                return (ft_putstr_err("pipe failed"), 1); // не удалось открыть пайп
        }

        if (curr->flag_fail == 0 && curr->cmd && *(curr->cmd))
        {
            if (is_redir(curr))
            {
                fd_out = open(curr->redir_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0)
                    return (ft_putstr_err("open output failed"), 1);
                status = do_redir(curr, p, env, fd_in, fd_out);
                close(fd_out);
                if (curr->next)
                {
                    file_fd = open(curr->redir_out, O_RDONLY);
                    if (file_fd < 0)
                        return ft_putstr_err("open output file failed"), 1;
                    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0)
                        write(pipefd[1], buffer, bytes_read);
                    close(file_fd);
                    close(pipefd[1]);
                    fd_in = pipefd[0];
                }
            }
            else
            {
                if (is_built(curr->cmd[0]) != 0)
				{
					if (curr->next)
						status = exec_builtin(curr, env, p, pipefd[1]);
					else
						status = exec_builtin(curr, env, p, STDOUT_FILENO);
				}
                else
                {
					if (curr->next)
    					status = ft_cmd(curr, env, fd_in, pipefd[1]);
					else
    					status = ft_cmd(curr, env, fd_in, STDOUT_FILENO);
				}
			}
        }
        else
            status = curr->flag_fail;

        if (fd_in != STDIN_FILENO)
            close(fd_in);
        if (pipefd[1] != STDOUT_FILENO)
            close(pipefd[1]);
		if (curr->next)
            fd_in = pipefd[0];
        curr = curr->next;
    }
    if (fd_in != STDIN_FILENO)
        close(fd_in);
    return (status);
}