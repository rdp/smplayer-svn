/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2012 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "myapplication.h"
#include "smplayer.h"

int main( int argc, char ** argv ) 
{
	MyApplication a( "smplayer", argc, argv );
	/*
	if (a.isRunning()) { 
		qDebug("Another instance is running. Exiting.");
		return 0;
	}
	*/

	a.setQuitOnLastWindowClosed(false);

#if QT_VERSION >= 0x040400
	// Enable icons in menus
	QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
#endif

	// Sets the config path
	QString config_path;

#ifdef PORTABLE_APP
	config_path = a.applicationDirPath();
#else
	// If a smplayer.ini exists in the app path, will use that path
	// for the config file by default
	if (QFile::exists( a.applicationDirPath() + "/smplayer.ini" ) ) {
		config_path = a.applicationDirPath();
		qDebug("main: using existing %s", QString(config_path + "/smplayer.ini").toUtf8().data());
	}
#endif

	QStringList args = a.arguments();
	int pos = args.indexOf("-config-path");
	if ( pos != -1) {
		if (pos+1 < args.count()) {
			pos++;
			config_path = args[pos];
			// Delete from list
			args.removeAt(pos);
			args.removeAt(pos-1);
		} else {
			printf("Error: expected parameter for -config-path\r\n");
			return SMPlayer::ErrorArgument;
		}
	}

	SMPlayer * smplayer = new SMPlayer(config_path);
	SMPlayer::ExitCode c = smplayer->processArgs( args );
	if (c != SMPlayer::NoExit) {
		return c;
	}
	smplayer->start();

	int r = a.exec();

	delete smplayer;

	return r;
}

