/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   micro.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jthuysba <jthuysba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 16:47:42 by jthuysba          #+#    #+#             */
/*   Updated: 2023/12/04 18:08:52 by jthuysba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "micro.h"

int err(char *str)
{
	while (*str)
		write(2, str++, 1);
	return (1);
}

int cd(char **argv, int i)
{
	if (i != 2)
		return (err("error: cd: bad arguments\n"));
	else if (chdir(argv[1]) == -1)
		return (err("error: cd: cannot change directory to "), err(argv[1]), err("\n"));
	return (0);
}

int	exec(char **argv, int i, char **envp)
{
	int fd[2];
	int has_pipe = 0;
	int status;

	if (argv[i] && strcmp(argv[i], "|") == 0)
		has_pipe = 1;

	if (has_pipe && pipe(fd) == -1)
		return (err("error: fatal\n"));
		
	int pid = fork();

	if (pid == 0)
	{
		argv[i] = 0;
		// argv => /bin/ls
		if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[1]) == -1 || close (fd[0]) == -1))
			return (err("error: fatal\n"));
		execve(*argv, argv, envp);
		return (err("error: cannot execute\n"));
	}
	
	waitpid(pid, &status, 0);
	if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return (err("error: fatal\n"));
	
	return WIFEXITED(status) && WEXITSTATUS(status);
}

int main(int argc, char **argv, char **envp)
{
	int status = 0;
	int i = 0;
	
	if (argc <= 1)
		return (0);

	// argv => ./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
	while (argv[i] && argv[++i])
	{
		argv += i;
		i = 0;
		// argv => /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
		
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		
		if (strcmp(*argv, "cd") == 0)
			status = cd(argv, i);
		else if (i)
			status = exec(argv, i, envp);
	}
	
	return (status);
}

/*
	=> ./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
*/