#include "../src/includes/option_parser.h"

int main(int argc, char **argv)
{
    OptionParser parser;
    parser.new_option('h')
        .long_name("help")
        .description("prints program usage options.")
        .valid_by_itself_only();
    parser.new_option('p')
        .long_name("pretty-print")
        .description("prints the text in nicer format")
        .program_args_can_follow();

    if (!parser.parse(argc, argv))
    {
        std::cerr << "Error: " << parser.get_error_message() << std::endl;
        return EXIT_FAILURE;
    }

    Option option;
    while (parser.get_next_option(option))
    {
        switch (option.name)
        {
            case 'h':
            {
                std::cout << parser.get_usage({"Program", "Description"}) << std::endl;
            }
            break;
        }
    }

    return EXIT_SUCCESS;
}
