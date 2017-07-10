#include "includes/option_parser.h"
#include <algorithm>
#include <iomanip>
#include <set>
#include <sstream>

OptionParser::OptionParser()
    : m_map{std::map<std::string, OptionParser::OptionData *>()},
      m_option_data_list{std::vector<OptionData *>()},
      m_options{std::vector<Option>()},
      m_args{std::vector<std::string>()},
      m_option_index{0},
      m_error_msg{std::string()}
{
}

OptionParser::~OptionParser()
{
    for (const auto &option_data: m_option_data_list)
    {
        delete option_data;
    }
}

bool OptionParser::parse(int argc, char **argv)
{
    for (const auto &option_data : m_option_data_list)
    {
        m_map[option_data->m_name] = option_data;
        std::string long_name = option_data->m_long_name;
        if (long_name.length() > 0)
        {
            m_map[long_name] = option_data;
        }
    }

    bool arg_found = false;

    auto on_arg_found = [this,&arg_found](const std::string &option)
    {
        if (!arg_found)
        {
            arg_found = true;
        }
        m_args.push_back(option);
    };

    std::stringstream stream;
    for (int index = 1; index < argc; ++index)
    {
        std::string option(argv[index]);
        if (is_valid_option_format(option))
        {
            if (arg_found)
            {
                stream << "invalid placement of option " << option;
                break;
            }
            if (is_short_option(option))
            {
                bool error = false;
                for (int i = 1; i < option.length(); ++i)
                {
                    std::string name(1, option[i]);

                    if (is_valid_option(name))
                    {
                        OptionParser::OptionData *data = m_map[name];
                        Option result;
                        result.name = option[i];
                        if (data->m_arg_required)
                        {
                            if (i < option.length() - 1)
                            {
                                stream << "option -" << name << " is not valid";
                                error = true;
                                break;
                            }

                            if (++index < argc)
                            {
                                std::string value = argv[index];
                                if (is_valid_option(value) || value.empty())
                                {
                                    stream << "argument required for option -" << name;
                                    break;
                                }

                                result.value = value;
                            }
                            else
                            {
                                stream << "argument required for option -" << name;
                                break;
                            }
                        }
                        m_options.push_back(result);
                    }
                    else
                    {
                        stream << "option -" << name << " is not valid";
                        error = true;
                        break;
                    }
                }

                if (error)
                {
                    break;
                }
            }
            else if (is_long_option(option))
            {
                // long option
                if (!is_valid_option(option.substr(2)))
                {
                    stream << "option " << option << " is not valid";
                    break;
                }
                OptionParser::OptionData *data = m_map[get_key_from_long_option(option)];
                Option result;
                result.name = data->m_name[0];
                if (data->m_arg_required)
                {
                    std::string value = get_value_from_long_option(option);
                    if (value.empty())
                    {
                        stream << "argument required for option " << option;
                        break;
                    }
                    result.value = value;
                }
                m_options.push_back(result);
            }
            else
            {
                on_arg_found(option);
            }
        }
        else
        {
            on_arg_found(option);
        }
    }

    m_error_msg = stream.str();
    if (!m_error_msg.empty())
    {
        return false;
    }

    bool valid_only_by_itself = false;
    for (const auto &data : m_option_data_list)
    {
        if (data->m_valid_only_by_itself)
        {
            auto itr = std::find_if(std::begin(m_options), std::end(m_options),
                                    [&data](Option &option) -> bool { return data->m_name[0] == option.name; });
            if (itr != m_options.end())
            {
                valid_only_by_itself = true;
                if (m_options.size() > 1)
                {
                    stream << "option -" << data->m_name << " is not valid";
                }
                break;
            }
        }

        if (data->m_required)
        {
            auto itr = std::find_if(std::begin(m_options), std::end(m_options),
                                    [&data](Option &option) -> bool { return data->m_name[0] == option.name; });
            if (itr == m_options.end())
            {
                stream << "option -" << data->m_name << " is required";
                break;
            }
        }
    }

    m_error_msg = stream.str();
    if (m_error_msg.empty())
    {
        if (valid_only_by_itself)
        {
            if (!m_args.empty())
            {
                stream << "arguments not allowed";
            }
        }
    }

    m_error_msg = stream.str();
    bool success = m_error_msg.empty();
    if (success)
    {
        std::sort(std::begin(m_options), std::end(m_options), [](Option &l, Option &r) { return l.name < r.name; });
    }
    return success;
}

bool OptionParser::get_next_option(Option &option)
{
    if (m_option_index == m_options.size())
    {
        return false;
    }

    option = m_options[m_option_index++];
    return true;
}

std::string OptionParser::get_value_from_long_option(const std::string &option)
{
    auto index = option.find("=");
    if (index == std::string::npos)
    {
        return "";
    }
    return option.substr(index + 1);
}

std::string OptionParser::get_key_from_long_option(const std::string &option)
{
    auto index = option.find("=");
    if (index == std::string::npos)
    {
        return option.substr(2);
    }
    return option.substr(2, index);
}

bool OptionParser::is_valid_option(const std::string &option)
{
    return m_map.find(option) != m_map.end();
}

OptionParser::OptionData &OptionParser::new_option(char name)
{
    OptionParser::OptionData *option_data = new OptionParser::OptionData(name);
    m_option_data_list.push_back(option_data);
    return *option_data;
}

std::string OptionParser::get_usage(const std::initializer_list<std::string> &usage_desc)
{
    std::stringstream stream;
    if (usage_desc.size() > 0)
    {
        stream << "Usage:" << std::endl;
        for (const auto &desc: usage_desc)
        {
            stream << "  " << desc << std::endl;
        }
    }

    std::set<std::string> keys = [](std::map<std::string, OptionParser::OptionData *> &map) {
        std::set<std::string> options;
        for (const auto &entry : map)
        {
            OptionParser::OptionData *data = entry.second;
            options.insert(data->m_name);
        }
        return options;
    }(m_map);

    int max_option_name_width = [](std::map<std::string, OptionParser::OptionData *> map) {
        std::map<std::string, std::string> m;
        for (const auto &entry : map)
        {
            m[entry.first] = entry.second->m_long_name;
        }

        int count = 0;
        for (const auto &entry : m)
        {
            count = std::max(count, (int)(entry.first.length() + entry.second.length()));
        }
        return count;
    }(m_map);

    stream << "Options: " << std::endl;
    for (const auto &k : keys)
    {
        OptionParser::OptionData *data = m_map[k];

        std::string name = "-" + data->m_name;
        std::string long_name = data->m_long_name;
        if (long_name.length() > 0)
        {
            name += ", --" + long_name;
        }

        stream << "  " << std::setw(max_option_name_width) << std::left << name;
        stream << "\t" << data->m_description << std::endl;
    }
    return stream.str();
}

OptionParser::OptionData::OptionData(char name)
    : m_name{std::string(1, name)},
      m_long_name{""},
      m_description{""},
      m_required{false},
      m_valid_only_by_itself{false},
      m_arg_required{false},
      m_program_args_can_follow{true}
{
}

OptionParser::OptionData &OptionParser::OptionData::long_name(const std::string &long_name)
{
    m_long_name = long_name;
    return *this;
}

OptionParser::OptionData &OptionParser::OptionData::description(const std::string &desc)
{
    m_description = desc;
    return *this;
}

OptionParser::OptionData &OptionParser::OptionData::required(bool required)
{
    m_required = required;
    return *this;
}

OptionParser::OptionData &OptionParser::OptionData::valid_by_itself_only(bool valid_by_itself_only)
{
    m_valid_only_by_itself = valid_by_itself_only;
    return *this;
}

OptionParser::OptionData &OptionParser::OptionData::arg_required(bool arg_required)
{
    m_arg_required = arg_required;
    return *this;
}

OptionParser::OptionData &OptionParser::OptionData::program_args_can_follow(bool program_args_can_follow)
{
    m_program_args_can_follow = program_args_can_follow;
    return *this;
}
