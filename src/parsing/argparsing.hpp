#pragma once

#include <cstdint>
#include <expected>
#include <string_view>
#include <optional>

struct Parameters {
    const char* input_file = nullptr;
    const char* output_directory = nullptr;
    uint64_t max_iterations = 0;
    uint64_t state_saving_frequency = 0;

    bool need_help = false;
};

struct ParametersParseError {
    const char* message = nullptr;
    const char* argument = nullptr;
    const char* argument_value = nullptr;
};

std::expected<Parameters, ParametersParseError> ParseArguments(int argc, char** argv);

std::expected<const char*, const char*> GetParameterInfo(std::string_view parameter);

void ShowHelpMessage();

std::optional<ParametersParseError> ParseOption(
    Parameters& parameters, 
    std::string_view argument, 
    std::string_view raw_value);

std::optional<ParametersParseError> ValidateParameters(const Parameters& parameters);
