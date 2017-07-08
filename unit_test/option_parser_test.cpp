#include "../src/includes/option_parser.h"
#include <gtest/gtest.h>

struct Arg
{
    size_t argc;
    char **argv;

    Arg(std::initializer_list<std::string> list) : argc(list.size() + 1), argv(nullptr)
    {
        argv = new char *[argc];
        argv[0] = new char[16];
        strcpy(argv[0], "program");
        auto itr = std::begin(list);
        for (int i = 1; i < argc; ++i)
        {
            std::string option = *itr++;
            argv[i] = new char[option.length() + 1];
            strcpy(argv[i], option.c_str());
        }
    }

    ~Arg()
    {
        for (int i = 0; i < argc; ++i)
        {
            delete[] argv[i];
        }
        delete[] argv;
    }
};

struct OptionParserTest : testing::Test
{
    OptionParser parser;

    OptionParserTest() : parser(OptionParser())
    {
        parser.new_option('h')
            .long_name("help")
            .description("prints program usage options")
            .valid_by_itself_only();
        parser.new_option('p')
            .long_name("pretty-print")
            .description("prints the text in nicer format")
            .required();
        parser.new_option('r')
            .long_name("read-stdin")
            .description("read from stdin");
        parser.new_option('f')
            .long_name("file")
            .description("read from file")
            .arg_required();
        parser.new_option('i')
            .long_name("interactive-mode")
            .description("interactive mode");
    }
};

TEST_F(OptionParserTest, no_option)
{
    Arg arg{""};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("option -p is required", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, option_p)
{
    Arg arg{"-p"};
    EXPECT_EQ(true, parser.parse(arg.argc, arg.argv));
}

TEST_F(OptionParserTest, option_f_no_arg)
{
    Arg arg{"-f"};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("argument required for option -f", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, option_f)
{
    Arg arg{"-f", "test_arg"};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("option -p is required", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, option_f_with_option_p)
{
    Arg arg{"-pf", "test_arg"};
    EXPECT_EQ(true, parser.parse(arg.argc, arg.argv));
    Option option;
    parser.get_next_option(option);
    EXPECT_EQ('f', option.name);
    EXPECT_STREQ("test_arg", option.value.c_str());
    parser.get_next_option(option);
    EXPECT_EQ('p', option.name);
}

TEST_F(OptionParserTest, invalid_arg_placement)
{
    Arg arg{"test_arg", "-p"};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("invalid placement of option -p", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, arg_only)
{
    Arg arg{"test_arg"};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("option -p is required", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, arg_only_with_option_p)
{
    Arg arg{"-p", "test_arg"};
    EXPECT_EQ(true, parser.parse(arg.argc, arg.argv));
}

TEST_F(OptionParserTest, option_f_before_p_together)
{
    Arg arg{"-fp", "test_arg"};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("option -f is not valid", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, option_pf_no_arg)
{
    Arg arg{"-pf"};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("argument required for option -f", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, option_h_by_itself)
{
    Arg arg{"-h"};
    EXPECT_EQ(true, parser.parse(arg.argc, arg.argv));
    Option option;
    parser.get_next_option(option);
    EXPECT_EQ('h', option.name);
}

TEST_F(OptionParserTest, option_h_with_another_option)
{
    Arg arg{"-h", "-i"};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("option -h is not valid", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, option_h_with_another_option_together)
{
    Arg arg{"-hi"};
    EXPECT_EQ(false, parser.parse(arg.argc, arg.argv));
    EXPECT_STREQ("option -h is not valid", parser.get_error_message().c_str());
}

TEST_F(OptionParserTest, long_options)
{
    Arg arg{"--pretty-print", "--read-stdin"};
    EXPECT_EQ(true, parser.parse(arg.argc, arg.argv));
    Option option;
    parser.get_next_option(option);
    EXPECT_EQ('p', option.name);
    parser.get_next_option(option);
    EXPECT_EQ('r', option.name);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
