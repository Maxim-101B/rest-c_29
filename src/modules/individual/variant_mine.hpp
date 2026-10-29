#pragma once
// ============================================================================
// ВАРИАНТ 29: Модуль расчета амортизации основных средств (Asset Depreciation Engine)
// ============================================================================

#include <string>
#include <cmath>
#include "../../json.hpp"

namespace TaskVariant {

    // ------------------------------------------------------------------------
    // Структура входных данных
    // ------------------------------------------------------------------------
    struct Input {
        double initial_asset_cost;     // первоначальная стоимость
        double salvage_value;          // ликвидационная стоимость
        int    useful_life_years;      // срок полезного использования (лет)
        int    current_year_of_service; // текущий год эксплуатации
    };

    // ------------------------------------------------------------------------
    // Структура выходных данных
    // ------------------------------------------------------------------------
    struct Output {
        double annual;                 // annual_depreciation_linear
        double accumulated;            // accumulated_depreciation
        double book_value;             // book_value
    };

    // ------------------------------------------------------------------------
    // Проверка входных данных
    // ------------------------------------------------------------------------
    inline std::string validate(const Input& in) {
        if (in.initial_asset_cost <= 0.0)
            return "initial_asset_cost must be > 0.0";
        if (in.salvage_value < 0.0 || in.salvage_value > in.initial_asset_cost * 0.9)
            return "salvage_value must be in [0.0, initial_asset_cost * 0.9]";
        if (in.useful_life_years < 1 || in.useful_life_years > 50)
            return "useful_life_years must be in [1, 50]";
        if (in.current_year_of_service < 1 || in.current_year_of_service > in.useful_life_years)
            return "current_year_of_service must be in [1, useful_life_years]";
        return "";
    }

    // ------------------------------------------------------------------------
    // Основные расчёты (ваши формулы)
    // ------------------------------------------------------------------------
    inline Output calculate(const Input& in) {
        Output out{};

        out.annual = (in.initial_asset_cost - in.salvage_value) / in.useful_life_years;
        out.accumulated = out.annual * in.current_year_of_service;
        out.book_value = in.initial_asset_cost - out.accumulated;

        return out;
    }

    // ------------------------------------------------------------------------
    // Разбор JSON-тела запроса в Input
    // ------------------------------------------------------------------------
    inline Input parse(const std::string& body) {
        Input in{};

        auto get_d = [&](const std::string& key) -> double {
            auto p = body.find("\"" + key + "\"");
            if (p == std::string::npos) return 0.0;
            p = body.find(':', p);
            if (p == std::string::npos) return 0.0;
            try { return std::stod(body.substr(p + 1)); }
            catch (...) { return 0.0; }
        };

        in.initial_asset_cost      = get_d("initial_asset_cost");
        in.salvage_value           = get_d("salvage_value");
        in.useful_life_years       = (int)get_d("useful_life_years");
        in.current_year_of_service = (int)get_d("current_year_of_service");
        
        return in;
    }

    // ------------------------------------------------------------------------
    // Формирование JSON-ответа из Output
    // ------------------------------------------------------------------------
    inline std::string to_json(const Output& out) {
        JObj o;
        o.num("annual", out.annual);
        o.num("accumulated", out.accumulated);
        o.num("book_value", out.book_value);
        return o.str();
    }

} // namespace TaskVariant