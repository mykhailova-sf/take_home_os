#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <map>
#include <functional>

class ComputationComponent {
public:
    ComputationComponent(char symbol) : symbol(symbol) {}

    void compute(int x) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Моделюємо час обчислень
        result = x * x; // Просте обчислення
        std::cout << "Component " << symbol << " finished with result " << result << std::endl;
    }

    char getSymbol() const { return symbol; }
    int getResult() const { return result; }

private:
    char symbol;
    int result = 0;
};

class ComputationManager {
public:
    void createGroup(int x) {
        int groupIdx = groups.size();
        groups.push_back(x);
        std::cout << "New group " << groupIdx << " with x = " << x << std::endl;
    }

    void addComponentToGroup(int groupIdx, char symbol) {
        if (groupIdx >= groups.size()) {
            std::cout << "Group index out of range." << std::endl;
            return;
        }

        ComputationComponent component(symbol);
        components.push_back(component);
        groupComponents[groupIdx].push_back(&components.back());
        std::cout << "Computation component " << symbol << " added to group " << groupIdx << std::endl;
    }

    void run() {
        std::cout << "Computing..." << std::endl;
        std::vector<std::thread> threads;

        for (auto& group : groupComponents) {
            for (auto* comp : group.second) {
                threads.push_back(std::thread([this, comp, x = groups[group.first]]() {
                    comp->compute(x);
                    finishedComponents.push_back(comp);
                }));
            }
        }

        for (auto& t : threads) {
            t.join();
        }

        std::cout << "Computation finished." << std::endl;
    }

    void printSummary() {
        std::cout << "Summary:" << std::endl;
        for (size_t i = 0; i < finishedComponents.size(); ++i) {
            std::cout << "Component " << finishedComponents[i]->getSymbol()
                      << " finished with result " << finishedComponents[i]->getResult() << std::endl;
        }
    }

    void printHelp() {
        std::cout << "\nAvailable commands:\n" << std::endl;
        std::cout << "  group <x>           : Create a new group with parameter x" << std::endl;
        std::cout << "  new <symbol> <group_idx> : Add a new computation component with a symbol to a specific group" << std::endl;
        std::cout << "  > run                : Run the computations for all groups and components" << std::endl;
        std::cout << "  > summary            : Show the summary of results for all components" << std::endl;
        std::cout << "  help                 : Show this help message" << std::endl;
        std::cout << "  exit                 : Exit the program" << std::endl;
    }

private:
    std::vector<int> groups;  // Містить x для кожної групи
    std::vector<ComputationComponent> components;
    std::map<int, std::vector<ComputationComponent*>> groupComponents;  // Групи і їх компоненти
    std::vector<ComputationComponent*> finishedComponents;
};

int main() {
    ComputationManager manager;

    // Вивести перелік команд при запуску
    manager.printHelp();

    // Приклад вводу команд
    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command.substr(0, 5) == "group") {
            int x = std::stoi(command.substr(6));
            manager.createGroup(x);
        }
        else if (command.substr(0, 3) == "new") {
            char symbol = command[4];
            int groupIdx = std::stoi(command.substr(6));
            manager.addComponentToGroup(groupIdx, symbol);
        }
        else if (command == "run") {
            manager.run();
        }
        else if (command == "summary") {
            manager.printSummary();
        }
        else if (command == "help") {
            manager.printHelp();
        }
        else if (command == "exit") {
            break;
        }
        else {
            std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
        }
    }

    return 0;
}