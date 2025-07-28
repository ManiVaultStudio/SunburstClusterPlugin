#include "Utils.h"

#include <QJsonArray>
#include <QJsonDocument>

QJsonObject JsonNode::toJson() const {
    QJsonObject obj;

    obj["name"] = name;
    obj["color"] = color;

    if (value.has_value() && children.isEmpty()) {
        obj["value"] = value.value();
    }

    if (!children.isEmpty()) {
        QJsonArray childArray;
        for (const auto& child : children) {
            childArray.append(child.toJson());
        }
        obj["children"] = childArray;
    }

    return obj;
}

QString jsonToString(const QJsonObject& json) {
    const QJsonDocument doc(json);
    const QString strJson(doc.toJson(QJsonDocument::Indented));
    return strJson;
}

QList<QStringList> convertJsSelectionClusterNames(const QString& json) {
    QList<QStringList> clusterPaths;

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isArray()) 
        return clusterPaths;

    QJsonArray pathsArray = doc.array();

    for (const QJsonValue& pathVal : pathsArray) {
        if (!pathVal.isArray()) continue;
        QJsonArray path = pathVal.toArray();

        QStringList pathList;
        for (const QJsonValue& step : path) {
            pathList.append(step.toString());
        }

        clusterPaths.append(pathList);
    }

    return clusterPaths;
}
