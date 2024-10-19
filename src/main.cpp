#include "parsing/argparsing.hpp"
#include "parsing/tsv_parsing.hpp"
#include "model/Sandpile.hpp"

#include <iostream>

int main(int argc, char** argv){
    if (argc < 2) {
        ShowHelpMessage();
        return EXIT_SUCCESS;
    }

    std::expected<Parameters, ParametersParseError> params = ParseArguments(argc, argv);
    if (!params.has_value()) {
        ParametersParseError error = params.error();

        std::cout << "An error occured while parsing arguments:" << std::endl;
        std::cout << error.message << std::endl;

        if (error.argument != nullptr) {
            std::cerr << error.argument << std::endl;
        }

        if (error.argument_value != nullptr) {
            std::cerr << error.argument_value << std::endl;
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

    Sandpile sandpile(grid);
    sandpile.SetOutputDirectory(params->output_directory);
    sandpile.SetOutputFilePrefix(params->output_file_prefix);
    sandpile.SetOutputFileExtension(params->output_file_extension);

    std::expected<uint64_t, SandpileError> run_result 
        = sandpile.Run(params->max_iterations, params->state_saving_frequency);

    if (!run_result.has_value()) {
        std::cout << "An error occured while running the model:" << std::endl;
        std::cerr << run_result.error().message << std::endl;
        
        return EXIT_FAILURE;
    }

    std::cout << "Final grid size: " << grid.GetWidth() << 'x' << grid.GetHeight() << std::endl;
    std::cout << "Calculation took " << run_result.value() << " topplings" << std::endl;

    return EXIT_SUCCESS;
}
