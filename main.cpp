#include <QApplication>
#include "main_class.h"

static void showMsg(const QString msg) {
    QMessageBox msgbox;
    msgbox.setText(msg);
    msgbox.exec();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (argc != 2) {
        showMsg("使い方: " + QString(argv[0]) + " [ファイルパス]");
        return 1;
    }

    MainClass *mc;
    try {
        mc = new MainClass(argv[1]);
        mc->exec();
    } catch (NoFileException e) {
        showMsg(QString(argv[1]) + ": 指定されたファイルは存在しません。");
        return 1;
    }

    auto r = a.exec();
    delete mc;
    return r;
}
