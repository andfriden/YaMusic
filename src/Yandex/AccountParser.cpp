#include "AccountParser.h"

#include <QJsonObject>

Account AccountParser::parse(const QJsonObject &json)
{
    Account account;

    const QJsonObject result = json.value("result").toObject();
    const QJsonObject accountObject = result.value("account").toObject();

    account.uid = accountObject.value("uid").toInteger();
    account.login = accountObject.value("login").toString();

    account.firstName = accountObject.value("firstName").toString();
    account.lastName = accountObject.value("secondName").toString();
    account.displayName = accountObject.value("displayName").toString();

    account.region = accountObject.value("region").toInt();
    account.regionCode = accountObject.value("regionCode").toString();

    account.serviceAvailable =
        accountObject.value("serviceAvailable").toBool();

    const QJsonObject plusObject =
        result.value("plus").toObject();

    account.hasPlus =
        plusObject.value("hasPlus").toBool();

    return account;
}
