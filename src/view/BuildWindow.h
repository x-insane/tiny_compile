#ifndef TINY_COMPILE_BUILD_WINDOW_H
#define TINY_COMPILE_BUILD_WINDOW_H

#include <QMainWindow>
#include <QtWidgets>

class BuildWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit BuildWindow(QString filename, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QTabWidget *tab = nullptr;
    QTextBrowser *src = nullptr;
    QTextBrowser *token = nullptr;
    QTextBrowser *tree = nullptr;
    QTextBrowser *code = nullptr;
    QTextBrowser *code_statement = nullptr;
    QTextBrowser *output = nullptr;
    QTextBrowser *symbol = nullptr;
};

#endif //TINY_COMPILE_BUILD_WINDOW_H
