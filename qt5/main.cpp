/***************************************************************************
 *   Copyright (C) 2011~2011 by CSSlayer                                   *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "common.h"

// Qt
#include <QApplication>

// KDE
#ifdef HAVE_KDE
#include <KAboutData>
#include <KApplication>
#include <KCmdLineOptions>
#include <KUniqueApplication>
#include <KDebug>
#endif

// Meego
#ifdef MEEGO_EDITION_HARMATTAN
#include <MApplication>
#endif

// System
#include <stdio.h>

// Twuner
#include "mainwindow.h"

void Usage()
{
    printf("Usage: twuner-qt [options]\n"
           "\t\t-d\tEnable Develope Tool\n"
           "\t\t-h\tShow this help\n"
          );
}

int main(int argc, char *argv[])
{
    bool enableDeveloper = false;

#ifdef HAVE_KDE

    KAboutData aboutData("twuner",                                          // internal name
                         "twuner",                                          // catalog name
                         ki18n("Twuner"),                                   // program name
                         "1.0.0",                                           // app version
                         ki18n("Lightweight, Flexible Microblogging"),      // short description
                         KAboutData::License_GPL_V2,                        // license
                         ki18n("(c) 2009-2012 Shellex Wai (Originally Hotot) / (c) 2015 Draekko\n"),   // copyright
                         KLocalizedString(),
                         "http://www.draekko.org/",                         // home page
                         "https://github.com/shellex/Twuner/issues"         // address for bugs
                        );

    aboutData.addAuthor(ki18n("Draekko"),           ki18n("Twuner Development"),    "draekko.software@gmail.com");

    KCmdLineOptions options;
    options.add("d");
    options.add("dev", ki18n("Enable developer Tool"));
    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    enableDeveloper = args->isSet("dev");

    KApplication a;
#else
#if !defined(Q_OS_WIN32) && !defined(Q_OS_MAC)
    setlocale(LC_ALL, "");
    bindtextdomain("twuner", LOCALEDIR);
    bind_textdomain_codeset("twuner", "UTF-8");
    textdomain("twuner");
#endif
#ifdef MEEGO_EDITION_HARMATTAN
    MApplication a(argc, argv);
#else
    QApplication a(argc, argv);

    int opt;
    while ((opt = getopt(argc, argv, "sdh")) != -1) {
        switch (opt) {
        case 'd':
            enableDeveloper = true;
            break;
        case 'h':
            Usage();
            return 0;
        default:
            Usage();
            exit(EXIT_FAILURE);
            break;
        }
    }
#endif

#endif
    MainWindow w;
    w.setEnableDeveloperTool(enableDeveloper);

#ifdef MEEGO_EDITION_HARMATTAN
    w.setOrientationAngle(M::Angle0);
    w.setOrientationAngleLocked(true);
#endif

    return a.exec();
}
