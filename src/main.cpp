#include "parsing/argparsing.hpp"
#include "parsing/tsv_parsing.hpp"
#include "model/Sandpile.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char** argv){
    if (argc < 2) {
        ShowHelpMessage();
        return EXIT_SUCCESS;
    }

    std::expected<Parameters, ParametersParseError> params = ParseArguments(argc, argv);
    if (!params.has_value()) {
        ParametersParseError error = params.error();

        std::cout << "An error occured while parsing arguments:\n" << error.message << std::endl;
        if (error.argument != nullptr) {
            std::cerr << error.argument << std::endl;
        }

        std::cout << "Use --help to see information about supported commands";

        return EXIT_FAILURE;
    }

    if (params->need_help) {
        ShowHelpMessage();
        return EXIT_SUCCESS;
    }

    Grid grid;
    std::optional<TsvParsingError> tsv_parsing_result = FillGrid(grid, params->input_file);
    
    if (tsv_parsing_result.has_value()) {
        std::cout << "An error occured while processing the input file:" << std::endl;
        std::cerr << tsv_parsing_result.value().message;

        return EXIT_FAILURE;
    }

    Sandpile sandpile(grid, *params);
    std::optional<SandpileError> run_result = sandpile.Run();

    if (run_result.has_value()) {
        std::cout << "An error occured while running the model:" << std::endl;
        std::cerr << run_result.value().message;
        
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
