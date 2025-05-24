/**
 * @file main.cpp
 * @brief задача написать программу, которая будет высчитывать значение интеграла через метод трапеций
 * Особенность этой задачи: использовать потоки(threads) для ускорения вычислений (нужно разделить задачу для вычисления части интеграла на каждый поток)
 *
 * на вход программе при запуске(это в argv[]) подается 3 числа: a, b, n, tn где [a, b] - интервал (целочисленный, неотрицательный и не больше 50 для задачи), n - количество разбиений, tn - threads number - количество потоков для подсчёта
 * примечание: n - количество разбиений в тестах будет явно нацело делиться на tn - количество потоков.
 *
 * на выход программе выведите значение интеграла через метод трапеций (вывод в стандартный поток std::cout, в конце выведите '\n'), вывод в поток с точностью до 4 знака
 * в функции trapezoidalIntegral есть переменная const std::function<double (double)> &f, при вызове подсчётов передайте ссылку на функцию из задания
 * реализовать подсчёт интеграла S(a, b) = (1+e^x)^0.5 dx
 * 
 *
 * литература:
 * https://ru.wikipedia.org/wiki/Метод_трапеций
 * https://habr.com/ru/articles/420867/
 */

#include <cmath>
#include <functional>
#include <numeric>
#include <vector>
#include <thread>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <mutex>


class Integral {
private:
    int a, b, n, tn;
    std::mutex mtx;

public:
    Integral(int& argc, char** argv) {
        if (argc != 5) {
            throw std::invalid_argument("Wrong number of arguments");
        }
        a  = std::stoi(argv[1]);
        b  = std::stoi(argv[2]);
        n  = std::stoi(argv[3]);
        tn = std::stoi(argv[4]);
    }


    static double integralFunction(double x) {
        return std::sqrt(1 + std::exp(x));
    }


    double calculateIntegral() {
        double total = 0.0;
        std::vector<std::thread> threads;
        int n_per_thread = n / tn;
        double h = static_cast<double>(b - a) / n;

        for (int t = 0; t < tn; ++t) {
            int start = t * n_per_thread;
            int end = (t == tn - 1) ? n : (t + 1) * n_per_thread;

            threads.emplace_back([&, start, end, h, t]() {
                double local_sum = 0.0;
                for (int i = start; i < end; ++i) {
                    double x_i = a + i * h;
                    double x_next = a + (i + 1) * h;
                    local_sum += (integralFunction(x_i) + integralFunction(x_next)) * h / 2.0;
                }

                std::lock_guard<std::mutex> lock(mtx);
                total += local_sum;
            });
    }

    for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        return total;
    }

};



int main(int argc, char** argv)
{
    auto i = Integral(argc, argv);
    std::cout << std::fixed << std::setprecision (4);
    std::cout << i.calculateIntegral() << std::endl;
    return 0;
}