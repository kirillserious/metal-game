#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("FBX Analyzer");

    program.add_argument("filepath");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        spdlog::error(err.what());
        return 1;
    }

    auto input = program.get("filepath");
    std::cout << input << std::endl;

    return 0;
}