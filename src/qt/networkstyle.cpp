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
const NetworkStyle* NetworkStyle::instantiate(const QString& networkId)
{
    for (unsigned x = 0; x < network_styles_count; ++x) {
        if (networkId == network_styles[x].networkId) {
            return new NetworkStyle(
                network_styles[x].appName,
                GUIUtil::getThemeImage(network_styles[x].appIcon),
                network_styles[x].titleAddText,
                GUIUtil::getThemeImage(network_styles[x].splashImage));
        }
    }
    return 0;
}
