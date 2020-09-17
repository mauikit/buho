#include <QIcon>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#ifdef STATIC_KIRIGAMI
#include "3rdparty/kirigami/src/kirigamiplugin.h"
#endif

#ifdef STATIC_MAUIKIT
#include "3rdparty/mauikit/src/mauikit.h"
#include "mauiapp.h"
#else
#include <MauiKit/mauiapp.h>
#endif

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#include <QIcon>
#else
#include <QApplication>
#endif

#include "buho.h"

#include "models/books/booklet.h"
#include "models/books/books.h"
#include "models/notes/notes.h"

#ifdef Q_OS_MACOS
#include <KF5/KI18n/KLocalizedContext>
#else
#include <KI18n/KLocalizedContext>
#endif

int Q_DECL_EXPORT main(int argc, char *argv[]) {
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifdef Q_OS_ANDROID
	QGuiApplication app(argc, argv);
	if (!MAUIAndroid::checkRunTimePermissions({"android.permission.WRITE_EXTERNAL_STORAGE"}))
		return -1;
#else
	QApplication app(argc, argv);
#endif

	app.setApplicationName(OWL::appName);
	app.setApplicationVersion(OWL::version);
	app.setApplicationDisplayName(OWL::displayName);
	app.setOrganizationName(OWL::orgName);
	app.setOrganizationDomain(OWL::orgDomain);
	app.setWindowIcon(QIcon(":/buho.png"));
	MauiApp::instance ()->setIconName ("qrc:/buho.svg");
	MauiApp::instance()->setCredits ({QVariantMap({{"name", "Camilo Higuita"}, {"email", "milo.h@aol.com"}, {"year", "2019-2020"}})});
	MauiApp::instance ()->setDescription ("Buho allows you to take quick notes organize notes as books.");
	MauiApp::instance ()->setHandleAccounts (true);

#ifdef STATIC_KIRIGAMI
	KirigamiPlugin::getInstance().registerTypes();
#endif

#ifdef STATIC_MAUIKIT
	MauiKit::getInstance().registerTypes();
#endif

    Buho buho;

	QQmlApplicationEngine engine;

	engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

	qmlRegisterAnonymousType<Booklet>("Booklet", 1);
	qmlRegisterType<Notes>("Notes", 1, 0, "Notes");
	qmlRegisterType<Books>("Books", 1, 0, "Books");

	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
