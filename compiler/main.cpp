#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <exception>
#include <memory>

#include "antlr4-runtime.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"
#include "generated/ifccBaseVisitor.h"

#include "IRVisitor.h"
#include "BackEnd.h"
#include "SymbolTableVisitor.h"

using namespace antlr4;
using namespace std;

namespace {
enum class TargetArch {
    X86_64,
    ARM64
};

TargetArch getDefaultTarget() {
#if defined(__aarch64__) || defined(__arm64__)
    return TargetArch::ARM64;
#else
    return TargetArch::X86_64;
#endif
}

void printUsage(const char* programName) {
    cerr << "usage: " << programName << " [--target <x86_64|arm64>] path/to/file.c" << endl;
}

bool parseTarget(const string& value, TargetArch& target) {
    if (value == "x86_64") {
        target = TargetArch::X86_64;
        return true;
    }
    if (value == "arm64") {
        target = TargetArch::ARM64;
        return true;
    }
    return false;
}
}

int main(int argn, const char **argv)
{
    TargetArch target = getDefaultTarget();
    string inputPath;

    for (int i = 1; i < argn; ++i) {
        string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }

        if (arg == "--target") {
            if (i + 1 >= argn) {
                cerr << "error: missing value for --target" << endl;
                printUsage(argv[0]);
                return 1;
            }

            if (!parseTarget(argv[++i], target)) {
                cerr << "error: unsupported target '" << argv[i] << "' (expected x86_64 or arm64)" << endl;
                printUsage(argv[0]);
                return 1;
            }
            continue;
        }

        if (arg.rfind("--target=", 0) == 0) {
            string value = arg.substr(9);
            if (!parseTarget(value, target)) {
                cerr << "error: unsupported target '" << value << "' (expected x86_64 or arm64)" << endl;
                printUsage(argv[0]);
                return 1;
            }
            continue;
        }

        if (!arg.empty() && arg[0] == '-') {
            cerr << "error: unknown option '" << arg << "'" << endl;
            printUsage(argv[0]);
            return 1;
        }

        if (!inputPath.empty()) {
            cerr << "error: multiple input files are not supported" << endl;
            printUsage(argv[0]);
            return 1;
        }
        inputPath = arg;
    }

    if (inputPath.empty()) {
        printUsage(argv[0]);
        return 1;
    }

    stringstream in;
    ifstream lecture(inputPath);
    if (!lecture.good())
    {
        cerr << "error: cannot read file: " << inputPath << endl;
        exit(1);
    }
    in << lecture.rdbuf();

    ANTLRInputStream input(in.str());

    ifccLexer lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();

    ifccParser parser(&tokens);
    tree::ParseTree *tree = parser.axiom();

    if (parser.getNumberOfSyntaxErrors() != 0)
    {
        cerr << "error: syntax error during parsing" << endl;
        exit(1);
    }

    if (lexer.getNumberOfSyntaxErrors() != 0) {
        cerr << "Erreur lexicale" << endl;
        exit(1);
    }
    IRControlFlowGraph* cfg = nullptr;
    try {
        SymbolTableVisitor symbolTableVisitor;
        symbolTableVisitor.visit(tree);

        IRVisitor irVisitor(&symbolTableVisitor.symbolTable);
        cfg = irVisitor.buildIr(tree);

        unique_ptr<BackEnd> backend;
        if (target == TargetArch::X86_64) {
            backend = make_unique<X86BackEnd>(cfg);
        } else {
            backend = make_unique<ARMBackEnd>(cfg);
        }
        backend->generateCode(cout);
    }
    catch (const exception& e) {
        delete cfg;
        cerr << "error: " << e.what() << endl;
        return 1;
    }

    delete cfg;

    return 0;
}
