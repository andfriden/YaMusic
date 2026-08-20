#pragma once

#include "../Models/Account.h"

#include <QJsonObject>

class AccountParser
{
public:
    static Account parse(const QJsonObject &json);
};
