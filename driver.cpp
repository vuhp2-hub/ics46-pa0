// driver.cpp -- PA 0 harness (provided; do not modify).
//
// It builds YOUR MenuModel from a .menu file, calls its methods (and iterates its pairing
// tables), and prints the analysis report in the exact format the autograder checks. The
// autograder compiles this same driver against your MenuModel.hpp, so just implement the
// class's public API.
//
// Build (local testing):  g++ -std=c++20 -O2 driver.cpp -o analyze
// Run:                    ./analyze model.menu               (basic report)
//                         ./analyze model.menu eaten.chosen  (also the "chosen" report)
#include <iostream>
#include <string>

#include "MenuModel.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <model.menu> [chosen.chosen]\n";
        return 2;
    }

    MenuModel model(argv[1]);

    std::cout << "courses: " << model.numCourses() << "\n";

    std::cout << "dishes_per_course:";
    for (int i = 0; i < model.numCourses(); ++i) std::cout << " " << model.dishes(i);
    std::cout << "\n";

    std::cout << "tables: " << model.numTables() << "\n";

    // Walk the pairing tables via the model's iterator (Unit 0.5) to list each table's scope.
    std::cout << "table_scopes:";
    for (PairingFactor const& table : model) {
        std::cout << " (";
        for (int j = 0; j < table.scopeSize(); ++j) {
            if (j > 0) std::cout << " ";
            std::cout << table.courseAt(j);
        }
        std::cout << ")";
    }
    std::cout << "\n";

    // Each table's size is the length of its score array; also track the largest.
    std::cout << "table_sizes:";
    long long largest = 0;
    for (PairingFactor const& table : model) {
        std::cout << " " << table.numEntries();
        if (table.numEntries() > largest) largest = table.numEntries();
    }
    std::cout << "\n";

    std::cout << "largest_table: " << largest << "\n";
    std::cout << "complete_meals: " << model.completeMeals() << "\n";

    if (argc >= 3) {
        model.readChosen(argv[2]);
        std::cout << "chosen_courses: " << model.chosenCount() << "\n";
        std::cout << "chosen:";
        for (int i = 0; i < model.chosenCount(); ++i)
            std::cout << " (" << model.chosenCourseAt(i) << " " << model.chosenDishAt(i) << ")";
        std::cout << "\n";
        std::cout << "free_courses: " << model.freeCourses() << "\n";
    }
    return 0;
}
