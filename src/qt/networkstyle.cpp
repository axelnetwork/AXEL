// Copyright (c) 2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "networkstyle.h"

#include "guiconstants.h"
#include "guiutil.h"

#include <QApplication>

static const struct {
    const char* networkId;
    const char* appName;
    const char* appIcon;
    const char* titleAddText;
    const char* splashImage;
} network_styles[] = {
    {"main", QAPP_APP_NAME_DEFAULT, ":/icons/axel", "", ":/images/splash"},
    {"main-pre-prod", QAPP_APP_NAME_PRE_POROD, ":/icons/axel", QT_TRANSLATE_NOOP("SplashScreen", "[preprod-net]"), ":/images/splash"},
    {"main-custom", QAPP_APP_NAME_CUSTOM, ":/icons/axel", QT_TRANSLATE_NOOP("SplashScreen", "[custom-net]"), ":/images/splash"},
    {"test", QAPP_APP_NAME_TESTNET, ":/icons/axel_testnet", QT_TRANSLATE_NOOP("SplashScreen", "[dev-net]"), ":/images/splash_testnet"},
    {"regtest", QAPP_APP_NAME_TESTNET, ":/icons/axel_testnet", "[regtest]", ":/images/splash_testnet"}};
static const unsigned network_styles_count = sizeof(network_styles) / sizeof(*network_styles);

// titleAddText needs to be const char* for tr()
NetworkStyle::NetworkStyle(const QString& appName, const QString& appIcon, const char* titleAddText, const QString& splashImage) : appName(appName),
                                                                                                                                   appIcon(appIcon),
                                                                                                                                   titleAddText(qApp->translate("SplashScreen", titleAddText)),
                                                                                                                                   splashImage(splashImage)
{
}
//GUIUtil::getThemeImage(
const NetworkStyle* NetworkStyle::instantiate(const QString& networkId,const QString& customName)
{
    for (unsigned x = 0; x < network_styles_count; ++x) {
        if (networkId == network_styles[x].networkId) {
            QString name = network_styles[x].appName;
            std::string strTitleAdd = network_styles[x].titleAddText;
            if (!customName.isEmpty()){
                name += "-";
                name += customName;

                strTitleAdd.clear();
                std::string add = "[custom-net-";
                add += customName.toStdString();
                add += "]";
                strTitleAdd = QT_TRANSLATE_NOOP("SplashScreen", add.c_str());
            }
            return new NetworkStyle(
                name,
                GUIUtil::getThemeImage(network_styles[x].appIcon),
                strTitleAdd.c_str(),
                GUIUtil::getThemeImage(network_styles[x].splashImage));
        }
    }
    return 0;
}
