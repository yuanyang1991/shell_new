#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define EXIT_SUCCESS 0
#define ERROR_FAILURE 1
#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUF_SIZE 64
#define LSH_TOK_DELIM "\t\r\n\a"

void lsh_author()
{
    printf("yuanyang\n");
}

void lsh_help()
{
    int i;
    printf("Stephen Brennan's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");
    printf("Use the man command for information on other programs.\n");
}

int lsh_exit() {
    return 0;   
}

char *builtin_str[] = {"author", "help", "exit"};

char *lsh_read_line()
{
    int buf_size = LSH_RL_BUFSIZE;
    int pos = 0;
    // 一个字符的长度 X 缓冲长度
    char *buffer = malloc(sizeof(char) * buf_size);
    int c;

    // 内存分配失败
    if (!buffer)
    {
        fprintf(stderr, "lsh: allocation error\n");
        exit(ERROR_FAILURE);
    }

    while (1)
    {
        c = getchar();
        if (c == EOF || c == '\n')
        {
            buffer[pos] = '\0';
            return buffer;
        }
        else
        {
            // 复制数据到字符数组
            buffer[pos] = c;
        }
        pos++;

        // 缓存不够，再次分配内存
        if (pos >= buf_size)
        {
            buf_size += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, sizeof(char) * buf_size);
            if (!buffer)
            {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

// 分割字符串
char **lsh_split_line(char *line)
{
    int buf_size = LSH_TOK_BUF_SIZE;
    int position = 0;
    // 为指针数组分配内存
    char **tokens = malloc(buf_size * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "lsh: allocation error \n");
        exit(EXIT_FAILURE);
    }

    // 找到首个分隔符位置
    token = strtok(line, LSH_TOK_DELIM);

    // 找到分隔符，处理剩余字符串
    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        // 检查长度
        if (position >= buf_size)
        {
            buf_size += LSH_TOK_BUF_SIZE;
            tokens = realloc(tokens, buf_size * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "lsh: allocation error \n");
                exit(EXIT_FAILURE);
            }
        }

        // 继续分割
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

void lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    // 开启子进程
    pid = fork();
    if (pid == 0)
    {
        // 子进程
        if (execvp(args[0], args) == -1)
        {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // 子进程开启失败
        perror("lsh");
    }
    else
    {
        // 等待子进程
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

void lsh_loop()
{
    char *line;
    char **args;
    int status;
    do
    {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        char *cmd = args[0];
        if (strcmp(cmd, builtin_str[0]) == 0)
        {
            lsh_author();
        }
        else if (strcmp(cmd, builtin_str[1]) == 0)
        {
            lsh_help();
        }
        else if (strcmp(cmd, builtin_str[2]) == 0)
        {
            break;
        }
        else
        {
            lsh_launch(args);
        }

    } while (1);
}

int main(int argc, char const *argv[])
{
    lsh_loop();
    return EXIT_SUCCESS;
}
