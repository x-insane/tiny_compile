#ifndef TINY_COMPILE_MAIN_WINDOW_H
#define TINY_COMPILE_MAIN_WINDOW_H

#include <QMainWindow>
#include "component/highlighter.h"

QT_BEGIN_NAMESPACE
    class QTextEdit;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void newFile();
    void openFile(const QString &path = QString());
    void saveFile();
    void saveFileAs();
    void textChanged();
    void build();

private:
    void setupEditor();
    void setupFileMenu();
    void setTextModified(bool isModified);
    bool closeFile();

private:
    QString filename;
    QTextEdit *editor;
};

#endif //TINY_COMPILE_MAIN_WINDOW_H
