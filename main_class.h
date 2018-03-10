#ifndef MAIN_CLASS_H
#define MAIN_CLASS_H

#include <QDebug>
#include <QObject>
#include <QException>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QtWebEngine>
#include <QWebEngineView>

class NoFileException : QException {};

class MainClass : QObject {
    Q_OBJECT
    private:
        QFileSystemWatcher watcher;
        QString abspath;
        QUrl fileurl;
        QWebEngineView eng;

        int timerId;

    public:
        MainClass(QString const &filename, QObject *parent = Q_NULLPTR) : QObject(parent) {
            QtWebEngine::initialize();

            timerId = startTimer(500);
            auto fileInfo = QFileInfo(filename);
            if (!fileInfo.exists()) {
                throw NoFileException();
            }
            abspath = fileInfo.absoluteFilePath();
            fileurl = QUrl::fromLocalFile(abspath);
            watcher.addPath(abspath);
            connect(&watcher, SIGNAL( fileChanged(const QString&) ), this, SLOT( engReload() ));
            connect(&eng, SIGNAL( titleChanged(const QString&) ), this, SLOT( engTitle(const QString &) ));
        }

        ~MainClass() {
            killTimer(timerId);
        }

        void exec() {
            eng.setZoomFactor(1.5);
            eng.load(fileurl);
            eng.show();
        }

        void window_repaint() {
            static int a = 1;
            // qDebug() << "Repainting...";
            // workaround: eng.children() returns const pointer, but resize() is not a const function. I know this is dirty hack but no other way.
            //             by the way, children()[2] is QtWebEngineCore::RenderWidgetHostViewQtDelegateWidget.
            // auto renderWidget = const_cast<QWidget *>(static_cast<QWidget const *>(eng.children()[2]));
            // qDebug() << renderWidget->metaObject()->className();
            // renderWidget->repaint();
            // workaround: the parent QWebEngineView also must be repainted.
            // eng.repaint();

            // no other way found; very very dirty hack.
            a = -a;
            eng.resize(eng.width(), eng.height()+a);
            // qDebug() << eng.title();
        }

    protected:
        void timerEvent(QTimerEvent *e) {
            if (e->timerId() == timerId) {
                window_repaint();
            }
        }

    public slots:
        void engTitle(QString const &title) {
            eng.setWindowTitle(title);
        }

        void engReload() {
            // qDebug() << "reload.";
            eng.reload();

            // workaround: QFileSystemWatcher remove path even if the file is modified.
            if (watcher.files().isEmpty()) {
                // workaround: while saving sometimes path isn't judged to exist.
                for (int i = 0; i < 3; i++) {
                    if (!watcher.files().isEmpty()) break;
                    watcher.addPath(abspath);
                }
            }
            // workaround: Qt doesn't fire repaint() when switching workspace, so then the window will be filled with background color
            //             and never repainted until some event occurs (like resizing). Though we can resize the window programmatically
            //             using resize(width, height) function but resizing to the same size doesn't seem to fire repaint() and resizing
            //             1px smaller (or larger) makes the window flicker and likely cause another unexpected behavior.
            //             so I decided to force repaint here. Of course, the window will be still background color until next
            //             engReload() triggered. however it is much more easier than resizing the window manually every time it get freezed,
            //             or resizing window programmatically with the risk of causing another serious bug. when the window freezed, all you
            //             have to do is to modify the target file.
            window_repaint();
        }
};

#endif // MAIN_CLASS_H
