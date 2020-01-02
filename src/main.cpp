#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#ifdef STATIC_KIRIGAMI
#include "3rdparty/kirigami/src/kirigamiplugin.h"
#endif

#ifdef STATIC_MAUIKIT
#include "3rdparty/mauikit/src/mauikit.h"
#endif

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#include <QIcon>
#else
#include <QApplication>
#endif
#include <QtWebView>

#include "buho.h"
#include "linker.h"

#include "models/books/booklet.h"
#include "models/books/books.h"
#include "models/links/links.h"
#include "models/notes/notes.h"

int Q_DECL_EXPORT main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QIcon::setThemeName("Luv");

  qDebug() << "hasThemeIcon(view-pim-notes) :"
           << QIcon::hasThemeIcon("view-pim-notes");
  qDebug() << QIcon::fromTheme("view-pim-notes");

  qDebug() << "searchPaths :" << QIcon::themeSearchPaths();
  qDebug() << "fallbackSearchPaths :" << QIcon::fallbackSearchPaths();

#ifdef Q_OS_ANDROID
  QGuiApplication app(argc, argv);
  QtWebView::initialize();
#else
  QApplication app(argc, argv);
  //    QtWebEngine::initialize();
#endif

  app.setApplicationName(OWL::App);
  app.setApplicationVersion(OWL::version);
  app.setApplicationDisplayName(OWL::App);
  app.setWindowIcon(QIcon(":/buho.png"));

#ifdef STATIC_KIRIGAMI
  KirigamiPlugin::getInstance().registerTypes();
#endif

#ifdef STATIC_MAUIKIT
  MauiKit::getInstance().registerTypes();
#endif

  Buho owl;

  QQmlApplicationEngine engine;
  auto context = engine.rootContext();

  context->setContextProperty("owl", &owl);

  Linker linker;
  context->setContextProperty("linker", &linker);
  qmlRegisterType<Booklet>();
  qmlRegisterType<Notes>("Notes", 1, 0, "Notes");
  qmlRegisterType<Books>("Books", 1, 0, "Books");
  qmlRegisterType<Links>("Links", 1, 0, "Links");

  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}