#include "A.cpp"
#include <iostream>
#include <vector>
#include <cassert>

std::string to_string(std::pair<int, std::string> p) {
    return std::to_string(p.first) + " " + p.second;
}

std::string to_string(SearchingTree<int, std::string>::iterator it) {
    if (it.has_next()) {
        return to_string(*it);
    } else {
        return "end";
    }
}

int main() {
    SearchingTree<int, std::string> a;

    a.insert(0, "zero");
    assert(a.find(0) != a.end());
    a.erase(0);
    assert(a.find(0) == a.end());

    a.insert(1, "one");
    a.insert(1, "not one");
    assert(a.find(1) != a.end());
    assert((*a.find(1)).second == "one");

    a.insert(2, "two");
    a.insert(3, "three");
    a.insert(4, "four");
    a.insert(5, "five");
    a.insert(6, "six");
    a.insert(7, "seven");

    std::vector<std::pair<int, std::string>> expected = {
        {1, "one"},
        {2, "two"},
        {3, "three"},
        {4, "four"},
        {5, "five"},
        {6, "six"},
        {7, "seven"},
    };
    std::vector<std::pair<int, std::string>> actual;
    for (const auto &[key, value] : a) {
        actual.push_back({key, value});
    }
    assert(expected == actual);

    std::vector<std::pair<int, std::string>> expected_range = {
        {2, "two"},
        {3, "three"},
    };
    std::vector<std::pair<int, std::string>> actual_range;
    for (const auto &[key, value] : a.range(2, 4)) {
        actual_range.push_back({key, value});
    }
    assert(expected_range == actual_range);

    std::vector<std::pair<int, std::string>> expected_range_empty = {};
    std::vector<std::pair<int, std::string>> actual_range_empty;
    for (const auto &[key, value] : a.range(8, 10)) {
        actual_range_empty.push_back({key, value});
    }
    assert(expected_range_empty == actual_range_empty);

    std::vector<std::pair<int, std::string>> expected_range_left_half = {
        {1, "one"},
        {2, "two"},
        {3, "three"},
    };
    std::vector<std::pair<int, std::string>> actual_range_left_half;
    for (const auto &[key, value] : a.range(-10, 4)) {
        actual_range_left_half.push_back({key, value});
    }
    assert(expected_range_left_half == actual_range_left_half);

    std::vector<std::pair<int, std::string>> expected_range_right_half = {
        {4, "four"},
        {5, "five"},
        {6, "six"},
        {7, "seven"},
    };
    std::vector<std::pair<int, std::string>> actual_range_right_half;
    for (const auto &[key, value] : a.range(4, 10)) {
        actual_range_right_half.push_back({key, value});
    }
    assert(expected_range_right_half == actual_range_right_half);

    std::cout << "All tests pass" << std::endl;
    return 0;
}
