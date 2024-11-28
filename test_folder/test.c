#include "shell.h"
#include "unity.h"

void test_handle_cd_empty_null_direction(void);
void test_handle_cd_back_char(void);

void test_check_redir(void);
void test_check_pipe(void);
void test_parse_command(void);
void test_parse_command_env_var(void);

// void test_read_command();

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_handle_cd_empty_null_direction);
    RUN_TEST(test_handle_cd_back_char);
    RUN_TEST(test_check_redir);
    RUN_TEST(test_check_pipe);
    RUN_TEST(test_parse_command);
    RUN_TEST(test_parse_command_env_var);

    return UNITY_END();
}

void test_handle_cd_empty_null_direction(void)
{
    char* args[] = {"cd", NULL};
    char PWD[MAX_CWD_BUFFER];
    char OLDPWD[MAX_CWD_BUFFER];
    handle_cd(args, PWD, OLDPWD);
    char* home = getenv("HOME");

    TEST_ASSERT_EQUAL_STRING(home, PWD);
}

void test_handle_cd_back_char(void)
{
    char PWD[MAX_CWD_BUFFER];
    char OLDPWD[MAX_CWD_BUFFER];

    char CWD[MAX_CWD_BUFFER];
    getcwd(CWD, MAX_CWD_BUFFER);

    char* args1[] = {"cd", NULL};
    handle_cd(args1, PWD, OLDPWD);

    char* home = getenv("HOME");
    TEST_ASSERT_EQUAL_STRING(home, PWD);

    char* args2[] = {"cd", "-", NULL};
    handle_cd(args2, PWD, OLDPWD);

    TEST_ASSERT_EQUAL_STRING(CWD, PWD);
}

void test_check_redir(void)
{

    int SUCCESS = 0;
    char* args1[] = {"ls", ">", "file.txt", NULL};
    char* args2[] = {"ls", "<", "file.txt", NULL};
    TEST_ASSERT_EQUAL_INT(SUCCESS, check_redir(args1));
    TEST_ASSERT_EQUAL_INT(SUCCESS, check_redir(args2));
}

void test_check_pipe(void)
{

    char* args[] = {"ls", "|", "wc", "|", "wc", "|", "wc", NULL};

    // The amount of commands is the amount of pipes + 1

    TEST_ASSERT_EQUAL_INT(4, check_pipe(args));
}

void test_parse_command(void)
{
    char* expected[MAX_ARGS] = {"ls", "-l", NULL};
    char cmd[MAX_CMD_LEN];
    strcpy(cmd, "ls -l");
    char* args[MAX_ARGS];
    parse_command(cmd, args);
    TEST_ASSERT_EQUAL_STRING(args[0], expected[0]);
    TEST_ASSERT_EQUAL_STRING(args[1], expected[1]);

    strcpy(cmd, "ls -l");
    TEST_ASSERT_EQUAL_INT(2, parse_command(cmd, args));
}

void test_parse_command_env_var(void)
{

    char* home = getenv("HOME");
    char* expected[MAX_ARGS] = {"ls", "-l", home, NULL};
    char cmd[MAX_CMD_LEN];
    strcpy(cmd, "ls -l $HOME");
    char* args[MAX_ARGS];
    parse_command(cmd, args);

    TEST_ASSERT_EQUAL_STRING(args[0], expected[0]);
    TEST_ASSERT_EQUAL_STRING(args[1], expected[1]);
    TEST_ASSERT_EQUAL_STRING(args[2], expected[2]);

    strcpy(cmd, "ls -l $HOME");
    TEST_ASSERT_EQUAL_INT(3, parse_command(cmd, args));
}
