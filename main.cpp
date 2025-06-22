#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <algorithm>
#include <getopt.h> // 用于解析命令行选项

// process_file 函数保持不变
void process_file(const std::string& input_file,
                 const std::string& output_file,
                 float mutation_percentage,
                 int max_t,
                 unsigned int seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> value_dist(2, max_t);

    std::ifstream fin(input_file);
    std::ofstream fout(output_file);

    if (!fin || !fout) {
        std::cerr << "Error opening files!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.compare(0, 5, "SITE:") == 0) {
            size_t last_tab = line.rfind('\t');
            if (last_tab != std::string::npos) {
                std::string prefix = line.substr(0, last_tab + 1);
                std::string binary_part = line.substr(last_tab + 1);

                std::vector<size_t> one_positions;
                for (size_t i = 0; i < binary_part.size(); ++i) {
                    if (binary_part[i] == '1') {
                        one_positions.push_back(i);
                    }
                }

                size_t bits_to_mutate = static_cast<size_t>(
                    one_positions.size() * mutation_percentage / 100.0f);

                std::shuffle(one_positions.begin(), one_positions.end(), gen);

                for (size_t i = 0; i < bits_to_mutate && i < one_positions.size(); ++i) {
                    size_t pos = one_positions[i];
                    binary_part[pos] = '0' + value_dist(gen);
                }

                line = prefix + binary_part;
            }
        }

        fout << line << '\n';
    }
}

// 显示帮助信息
void print_help(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  -h, --help                Show this help message and exit\n"
              << "  -i, --input <file>        Input file (required)\n"
              << "  -o, --output <file>       Output file (required)\n"
              << "  -m, --mutation <percent>  Mutation percentage (0-100, required)\n"
              << "  -t, --max-t <value>       Maximum t value (2-9, default: 9)\n"
              << "  -s, --seed <value>        Random seed (unsigned int, default: 1)\n"
              << "\nExample:\n"
              << "  " << program_name << " -i input.txt -o output.txt -m 10 -t 5 -s 123\n"
              << "  " << program_name << " --input input.txt --output output.txt --mutation 5.5\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    // 定义长选项
    static struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"mutation", required_argument, nullptr, 'm'},
        {"max-t", optional_argument, nullptr, 't'},
        {"seed", optional_argument, nullptr, 's'},
        {nullptr, 0, nullptr, 0}
    };

    // 默认值
    std::string input_file, output_file;
    float mutation_percentage = -1.0f; // 未设置时为负值
    int max_t = 9;
    unsigned int seed = 1;

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hi:o:m:t:s:", long_options, &option_index)) != -1) {
        try {
            switch (opt) {
                case 'h':
                    print_help(argv[0]);
                    return EXIT_SUCCESS;
                case 'i':
                    input_file = optarg;
                    break;
                case 'o':
                    output_file = optarg;
                    break;
                case 'm':
                    mutation_percentage = std::stof(optarg);
                    break;
                case 't':
                    max_t = std::stoi(optarg);
                    break;
                case 's':
                    seed = static_cast<unsigned int>(std::stoul(optarg));
                    break;
                default:
                    std::cerr << "Invalid option. Use -h or --help for usage.\n";
                    return EXIT_FAILURE;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing argument for option -" << static_cast<char>(opt)
                      << ": " << e.what() << "\n";
            return EXIT_FAILURE;
        }
    }

    // 验证必需参数
    if (input_file.empty()) {
        std::cerr << "Error: Input file is required. Use -i or --input.\n";
        return EXIT_FAILURE;
    }
    if (output_file.empty()) {
        std::cerr << "Error: Output file is required. Use -o or --output.\n";
        return EXIT_FAILURE;
    }
    if (mutation_percentage < 0) {
        std::cerr << "Error: Mutation percentage is required. Use -m or --mutation.\n";
        return EXIT_FAILURE;
    }

    // 验证参数有效性
    if (mutation_percentage < 0 || mutation_percentage > 100) {
        std::cerr << "Error: Mutation percentage must be between 0 and 100.\n";
        return EXIT_FAILURE;
    }
    if (max_t < 2 || max_t > 9) {
        std::cerr << "Error: Maximum t value must be between 2 and 9.\n";
        return EXIT_FAILURE;
    }

    // 执行处理
    try {
        process_file(input_file, output_file, mutation_percentage, max_t, seed);
        std::cout << "Success! Modified exactly " << mutation_percentage
                  << "% of 1 bits (2-" << max_t << "), kept all 0s and other content identical.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error during processing: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}