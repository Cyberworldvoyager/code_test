#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

string int128_to_string(__int128 value) {
    if (value == 0) return "0";

    bool negative = value < 0;
    if (negative) value = -value;

    string str;
    while (value > 0) {
        str += '0' + static_cast<char>(value % 10);
        value /= 10;
    }
    if (negative) str += '-';

    reverse(str.begin(), str.end());
    return str;
}

class TradeProcessor {
private:
    // 每个交易品种的统计信息(这里可以补充）
    struct SymbolStats {
        int64_t last_timestamp = -1;  // 上次交易时间戳
        int64_t max_time_gap = 0;     // 最大时间间隔
        __int128 total_volume = 0;    // 总交易量（使用128位防止溢出）
        __int128 sum_price = 0;       // 总交易额（数量×价格累加）
        int64_t max_price = 0;        // 最高交易价格
    };

    map<std::string, SymbolStats> symbol_data;  // 使用map以排序

public:
    void process(const std::string& input_file, const std::string& output_file) {
        process_input(input_file);
        write_output(output_file);
    }

private:

    void process_input(const std::string& filename) {
        ifstream file(filename);
        string line;

        while (std::getline(file, line)) {
            process_line(line);
        }
    }

    void process_line(const std::string& line) {
        stringstream ss(line);
        string part;
        vector<std::string> tokens;

        while (std::getline(ss, part, ',')) {
            tokens.push_back(part);
        }
        if (tokens.size() != 4) return;  // 忽略格式错误行



        int64_t timestamp = stoll(tokens[0]);
        string symbol = tokens[1];
        __int128 quantity = static_cast<__int128>(stoll(tokens[2]));
        int64_t price = stoll(tokens[3]);


        SymbolStats& stats = symbol_data[symbol];


        if (stats.last_timestamp != -1) {
            int64_t gap = timestamp - stats.last_timestamp;
            stats.max_time_gap = std::max(stats.max_time_gap, gap);
        } else {
            stats.max_time_gap = 0;  // 首次交易时间间隔为0
        }
        stats.last_timestamp = timestamp;
        stats.total_volume += quantity;
        stats.sum_price += quantity * static_cast<__int128>(price);
        stats.max_price = max(stats.max_price, price);
    }

    void write_output(const std::string& filename) {
        std::ofstream file(filename);

        for (const auto& [symbol, stats] : symbol_data) {
            __int128 weighted_avg = stats.total_volume > 0
                                    ? stats.sum_price / stats.total_volume
                                    : 0;

            file << symbol << ","
                 << stats.max_time_gap << ","
                 << int128_to_string(stats.total_volume) << ","
                 << int128_to_string(weighted_avg) << ","
                 << stats.max_price << "\n";
        }
    }
};

int main(int argc, char* argv[]) {
    cout<<12345<<endl;
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input.csv output.csv\n";
        return 1;
    }

    TradeProcessor processor;
    processor.process(argv[1], argv[2]);

    return 0;
}
/*
int main(int argc, char* argv[]) {

    TradeProcessor processor;
    processor.process("testdata.csv","output.csv");

    return 0;
}*/