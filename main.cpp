#include <QApplication>
#include "main_class.h"

#define _tq(str) QString::fromLocal8Bit(str)

static void showMsg(const QString msg) {
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (argc != 2) {
        showMsg(_tq("使い方: ") + QString(argv[0]) + _tq(" [ファイルパス]"));
        return 1;
    }

    MainClass *mc;
    auto filename = _tq(argv[1]);
    try {
        mc = new MainClass(filename);
        mc->exec();
    } catch (NoFileException e) {
        showMsg(filename + _tq(": 指定されたファイルは存在しません。"));
        return 1;
    }

    auto r = a.exec();
    delete mc;
    return r;
}
