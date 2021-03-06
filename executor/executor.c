/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ameteori <ameteori@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/29 15:03:48 by ameteori          #+#    #+#             */
/*   Updated: 2022/05/29 17:47:18 by ameteori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/builtins.h"

int	execute_cmd(t_block_process *block)
{
	char	**paths;
	char	*cmd;

	cmd = NULL;
	paths = get_paths();
	if (!paths)
	{
		ft_putstr_fd("i'm dead bro > ", 2);
		ft_putstr_fd(block->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
		return (1);
	}
	if (!check_if_builtin(block))
	{
		cmd = make_cmd(paths, block->argv);
		if (!cmd)
			return (1);
		if (execve(cmd, block->argv, g_global.local_envp) < 0)
			return (-1);
		return (0);
	}
	total_free(paths);
	return (0);
}

int	child_labour(t_child *child, t_block_process *block, int len)
{
	signal(SIGINT, handler);
	if (child->i > 0)
	{
		if (dup2(child->fd[1 - child->current][0], STDIN_FILENO) < 0)
			strerror(errno);
		close(child->fd[1 - child->current][0]);
	}
	if (child->i < len - 1)
	{
		if (dup2(child->fd[child->current][1], STDOUT_FILENO) < 0)
			strerror(errno);
		close(child->fd[child->current][0]);
		close(child->fd[child->current][1]);
	}
	r_in(block, child);
	r_out(block, child);
	if (block->argv[0])
	{
		if (execute_cmd(block) == 1)
			return (1);
	}
	return (0);
}

int	check_cmd_name(t_list *bp)
{
	t_block_process	*block;

	block = (t_block_process *)bp->content;
	if (block->files_count == 0)
		return (0);
	if (!ft_strcmp(block->argv[0], block->files[0].file_name))
		return (1);
	return (0);
}

int	wait_children2(int len)
{
	int	i;
	int	status;

	i = -1;
	while (++i < len)
		wait(&status);
	if (WIFEXITED(status))
	{
		if (!status)
			return (0);
		if (WEXITSTATUS(status) == 255)
			return (127);
	}
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == 2)
			return (130);
		else if (WTERMSIG(status) == 3)
			return (131);
	}
	return (WEXITSTATUS(status));
}

int	executor(t_list *bp, int flag)
{
	t_child			child;
	t_block_process	*block;

	init_child(&child, bp);
	while (++child.i < child.len)
	{
		flag = 0;
		block = (t_block_process *)bp->content;
		if (check_cmd_name(bp))
			costil(&flag, &block);
		if (pipe(child.fd[child.current]) == -1)
			strerror(errno);
		if (check_if_builtin(block) == 1)
		{
			builtin_labour(&child, block, child.len);
			return (g_global.last_return);
		}
		else
			help_executor(&child, block);
		if (flag == 1)
			block->argv--;
		bp = bp->next;
	}
	close(child.fd[1 - child.current][0]);
	return (wait_children2(child.len));
}
