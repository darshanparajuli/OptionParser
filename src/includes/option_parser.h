#ifndef OPTIONS_PARSER_H
#define OPTIONS_PARSER_H

#include <iostream>
#include <map>
#include <vector>

struct Option
{
    char name;
    std::string value;

    friend std::ostream &operator<<(std::ostream &os, const Option &option)
    {
        return os << "Option{name:" << option.name << ", value:" << option.value << "}";
    }
};

class OptionParser
{
private:
    class OptionData
    {
    private:
        friend class OptionParser;

        std::string m_name;
        std::string m_long_name;
        std::string m_description;
        bool m_required;
        bool m_valid_only_by_itself;
        bool m_arg_required;
        bool m_program_args_can_follow;

    public:
        explicit OptionData(char name);

        OptionData &long_name(const std::string &long_name);

        OptionData &description(const std::string &desc);

        OptionData &required(bool required = true);

        OptionData &valid_by_itself_only(bool valid_by_itself_only = true);

        OptionData &arg_required(bool arg_required = true);

        OptionData &program_args_can_follow(bool program_args_can_follow = true);
    };

    std::map<std::string, OptionData *> m_map;
    std::vector<OptionData *> m_option_data_list;
    std::vector<Option> m_options;
    std::vector<std::string> m_args;
    int m_option_index;
    std::string m_error_msg;

public:
    OptionParser();

    ~OptionParser();

    OptionData &new_option(char name);

    bool parse(int argc, char **argv);

    bool get_next_option(Option &option);

    inline std::string get_error_message() const { return m_error_msg; }

    inline std::vector<std::string> get_args() const { return m_args; }

    std::string get_usage(const std::initializer_list<std::string> &usage_desc);

    inline bool has_options() const { return !m_options.empty(); }

    inline bool has_args() const { return !m_args.empty(); }

private:
    inline bool is_long_option(const std::string &option) { return option.length() > 2 && option.find("--") == 0; }

    inline bool is_short_option(const std::string &option)
    {
        return option.length() >= 2 && option.find("-") == 0 && option[1] != '-';
    }

    std::string get_value_from_long_option(const std::string &option);

    std::string get_key_from_long_option(const std::string &option);

    inline bool long_option_has_value(const std::string &option) { return option.find("=") != std::string::npos; }

    bool is_valid_option(const std::string &option);

    inline bool is_valid_option_format(const std::string &option)
    {
        return is_short_option(option) || is_long_option(option);
    }
};

#endif
