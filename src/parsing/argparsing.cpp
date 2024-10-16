#include "argparsing.hpp"
#include "utils.hpp"

#include <iostream>
#include <filesystem>

const char* kInputFileLongArg = "--input";
const char* kInputFileShortArg = "-i";
const char* kOutputDirectoryLongArg = "--output";
const char* kOutputDirectoryShortArg = "-o";
const char* kMaxIterLongArg = "--max-iter";
const char* kMaxIterShortArg = "-m";
const char* kFrequencyLongArg = "--freq";
const char* kFrequencyShortArg = "-f";
const char* kHelpLongArg = "--help";
const char* kHelpShortArg = "-h";

const char* kMissingArgumentMsg = "Unspecified argument value (unexpected end of argument sequence)";

bool SetFlag(Parameters& parameters, std::string_view argument) {
    if (argument == kHelpLongArg || argument == kHelpShortArg) {
        parameters.need_help = true;
        return true;
    }

    return false;
}

std::expected<Parameters, ParametersParseError> ParseArguments(int argc, char** argv) {
    Parameters parameters;

    for (size_t i = 1; i < argc; ++i) {
        std::string_view argument = std::string_view{argv[i]};
        if (SetFlag(parameters, argument)) {
            continue;
        }

        std::string_view raw_value;
        std::string_view argument_name;

        if (!argument.contains('=') && i == argc - 1) {
            return std::unexpected{ParametersParseError(kMissingArgumentMsg)};
        }
        
        if (argument.contains('=')) {
            raw_value = argument.substr(argument.find('='));
            argument_name = argument.substr(0, argument.find('='));
        } else {
            raw_value = std::string_view{argv[++i]};
            argument_name = argument;
        }

        std::optional<ParametersParseError> parsing_result = ParseOption(parameters, argument_name, raw_value);

        if (parsing_result.has_value()) {
            return std::unexpected{parsing_result.value()};
        }
    }
    
    std::optional<ParametersParseError> validation_result = ValidateParameters(parameters);

    if (validation_result.has_value()) {
        return std::unexpected{validation_result.value()};
    }

    return parameters;
}

std::optional<ParametersParseError> ParseOption(
    Parameters& parameters, 
    std::string_view argument_name, 
    std::string_view raw_value)
{
    if (argument_name == kInputFileLongArg || argument_name == kInputFileShortArg) {
        parameters.input_file = raw_value.data();
        return std::nullopt;
    } else if (argument_name == kOutputDirectoryLongArg || argument_name == kOutputDirectoryShortArg) {
        parameters.output_directory = raw_value.data();
        return std::nullopt;
    }

    std::expected<int64_t, const char*> number = ParseInt<uint64_t>(raw_value);

    if (!number.has_value()) {
        return ParametersParseError("Cannot parse integer value", argument_name.data());
    }

    if (argument_name == kMaxIterLongArg || argument_name == kMaxIterShortArg) {
        parameters.max_iterations = number.value();
    } else if (argument_name == kFrequencyLongArg || argument_name == kFrequencyShortArg) {
        parameters.state_saving_frequency = number.value();
    } else {
        return ParametersParseError("Unknown argument", argument_name.data());
    }

    return std::nullopt;
}

std::optional<ParametersParseError> ValidateParameters(const Parameters& parameters) {
    if (parameters.need_help) {
        return std::nullopt;
    }

    if (parameters.input_file != nullptr && !std::filesystem::exists(parameters.input_file)) {
        return ParametersParseError("Cannot find input file");
    } else if (parameters.input_file == nullptr) {
        return ParametersParseError("No input file is specified");
    } else if (parameters.output_directory == nullptr) {
        return ParametersParseError("No output directory is specified");
    }

    return std::nullopt;
}

std::expected<const char*, const char*> GetParameterInfo(std::string_view parameter) {
    if (parameter == kInputFileLongArg || parameter == kInputFileShortArg) {
        return "--input=<amount> | -i <amount>           [string]                 "
            "Path to the .tsv file with the sandpile initial state";
    } else if (parameter == kOutputDirectoryLongArg || parameter == kOutputDirectoryShortArg) {
        return "--output=<seconds> | -o <second>         [string]                 "
            "Path to the directory where to save states of the sandpile";
    } else if (parameter == kMaxIterLongArg || parameter == kMaxIterShortArg) {
        return "--max-iter=<timestamp> | -m <timestamp>  [int, >= 0, default=inf] "
            "Maximal amount of iterations.";
    } else if (parameter == kFrequencyLongArg || parameter == kFrequencyShortArg) {
        return "--freq=<timestamp> | -f <timestamp>      [int, >= 0, default=0]   "
            "Frequency of states to save. If zero, only the final state is saved";
    } else if (parameter == kHelpLongArg || parameter == kHelpShortArg) {
        return "--help | -h                              [flag]                   "
            "Show help and exit";
    }

    return std::unexpected{"Cannot get parameter info: unknown parameter"};
}

void ShowHelpMessage() {
    std::cout << "Usage: Sandpile [OPTIONS]" << std::endl << 
        "Possible options:" << std::endl << "\t";
    std::cout << *GetParameterInfo(kInputFileShortArg) << std::endl << '\t';
    std::cout << *GetParameterInfo(kOutputDirectoryShortArg) << std::endl << '\t';
    std::cout << *GetParameterInfo(kMaxIterShortArg) << std::endl << '\t';
    std::cout << *GetParameterInfo(kFrequencyShortArg) << std::endl << '\t';
    std::cout << *GetParameterInfo(kHelpShortArg) << std::endl << '\t';
}
