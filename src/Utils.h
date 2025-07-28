
#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
#include <vector>

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QJsonObject>

#if !defined(__clang__) && (defined(__GNUC__) || defined(_MSC_VER))
#include <execution>
#ifdef NDEBUG
#define SUN_PARALLEL_EXECUTION std::execution::par,
#else
#define SUN_PARALLEL_EXECUTION std::execution::seq,
#endif
#else // __clang__
#define SUN_PARALLEL_EXECUTION
#endif

template<typename T>
bool hasCommonElement_unsorted(const std::vector<T>& a, const std::vector<T>& b) {
    return std::any_of(SUN_PARALLEL_EXECUTION
        a.begin(), a.end(), [&b](T element) {
        return std::find(b.begin(), b.end(), element) != b.end();
        });
}

// Assumes both a and b to be sorted
template<typename T>
bool hasCommonElement(const std::vector<T>& a, const std::vector<T>& b) {
    // Search in the larger vector for elements of the smaller vector
    const auto& smaller = (a.size() <= b.size()) ? a : b;
    const auto& larger = (a.size() <= b.size()) ? b : a;

    return std::any_of(SUN_PARALLEL_EXECUTION
        smaller.begin(), smaller.end(), [&larger](const T& element) {
            return std::binary_search(larger.cbegin(), larger.cend(), element);
        });
}

template<typename T>
std::optional<size_t> findIndex(const std::vector<T>& vec, const T& elem) {
    auto it = std::find(vec.begin(), vec.end(), elem);
    if (it != vec.end()) {
        return static_cast<size_t>(std::distance(vec.begin(), it));
    }
    else {
        return std::nullopt; // Not found
    }
}

QList<QStringList> convertJsSelectionClusterNames(const QString& json);

QString jsonToString(const QJsonObject& json);

struct JsonNode {
    QString name = "";
    QString color = "";
    std::optional<int> value = std::nullopt;
    QVector<JsonNode> children = {};

    QJsonObject toJson() const;
    QString toJsonStr() const {
        return jsonToString(toJson());
    }
};
