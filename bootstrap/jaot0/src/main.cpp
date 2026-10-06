#include "jaot/codegen.h"
#include "jaot/ir_lowering.h"
#include "jaot/lexer.h"
#include "jaot/parser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "jaot/semantic.h"

namespace {
    void printHelp() {
        std::cout
            << "jaot0 - JAOT stage 0 bootstrap compiler\n"
            << "\n"
            << "usage: jaot0 <input.jaot> [-S] [-o output.s]\n"
            << "\n"
            << "options:\n"
            << "   -S           emit x86-64 assembly\n"
            << "   -o <file>    write output to file\n"
            << "   --help       show this message\n";
    }

    std::string readFile(const std::string &path) {
        std::ifstream input(path);

        if (!input) {
            throw std::runtime_error("could not open file " + path);
        }

        std::ostringstream contents;
        contents << input.rdbuf();

        return contents.str();
    }
}

int main(int argc, char **argv)
{
    try {
        if (argc == 1) {
            printHelp();
            return 0;
        }

        std::string inputPath;
        std::string outputPath;

        bool assembly = false;

        for (int i = 1; i < argc; i++) {
            const std::string arg = argv[i];

            if (arg == "--help") {
                printHelp();
                return 0;
            }

            if (arg == "-S") {
                assembly = true;
                continue;
            }

            if (arg == "-o") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("-o requires a path");
                }

                outputPath = argv[++i];
                continue;
            }

            if (!arg.empty() && arg[0] == '-') {
                throw std::runtime_error("unkown option: " + arg);
            }

            if (!inputPath.empty()) {
                throw std::runtime_error("only one input file is supported");
            }

            inputPath = arg;
        }

        if (inputPath.empty()) {
            throw std::runtime_error("no input file");
        }

        if (!assembly) {
            throw std::runtime_error("stage 0 currently emits assembly only; use -S");
        }

        const std::string source = readFile(inputPath);

        JAOT::Lexer lexer(source);
        const auto tokens = lexer.lex();

        JAOT::Parser parser(tokens);
        const auto program = parser.parse();

        JAOT::SematicAnalyzer sematicAnalyzer;
        sematicAnalyzer.analyze(program);

        JAOT::IrLowerer irLowerer;
        const JAOT::IR::Program irProgram = irLowerer.lower(program);

        if (outputPath.empty()) {
            outputPath = inputPath + ".s";
        }

        std::ofstream output(outputPath);

        if (!output) {
            throw std::runtime_error("could not open file " + outputPath);
        }

        JAOT::CodeGenerator generator;
        generator.generate(irProgram, output);

        std::cout
            << "wrote "
            << outputPath
            << "\n";

        return 0;
    } catch (const std::exception &error) {
        std::cerr
            << "jaot0: "
            << error.what()
            << "\n";

        return 1;
    }
}