#include <QtWidgets>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent)
{
    resize(750, 500);
    setupFileMenu();
    setupEditor();

    setCentralWidget(editor);
    setWindowTitle(tr("Tiny+ IDE"));

    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() > 1)
        openFile(arguments.at(1));
}

void MainWindow::setupEditor() {
    QFont font;
    font.setFamily("Consolas");
    font.setFixedPitch(true);
    font.setPointSize(14);

    editor = new QTextEdit(this);
    editor->setFont(font);
    editor->setFrameShape(QFrame::Panel);

    QObject::connect(editor, SIGNAL(textChanged()), this, SLOT(textChanged()));

    new Highlighter(editor->document());
}

void MainWindow::setupFileMenu() {
    QMenu *fileMenu = new QMenu(tr("文件"), this);
    menuBar()->addMenu(fileMenu);
    fileMenu->addAction(tr("新建"), this, SLOT(newFile()), QKeySequence::New);
    fileMenu->addAction(tr("打开"), this, SLOT(openFile()), QKeySequence::Open);
    fileMenu->addAction(tr("保存"), this, SLOT(saveFile()), QKeySequence::Save);
    fileMenu->addAction(tr("另存为"), this, SLOT(saveFileAs()), QKeySequence::SaveAs);

    menuBar()->addAction(tr("编译"), this, SLOT(build()));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (closeFile())
        QApplication::quit();
    else
        event->ignore();
}

/**
 * @return 取消操作返回false，继续操作返回true
 */
bool MainWindow::closeFile() {
    if (editor->isWindowModified()) {
        QMessageBox::StandardButton result = QMessageBox::warning(this, "警告",
                "文件已修改，是否保存？", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (result == QMessageBox::Yes)
            saveFile();
        else if (result == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::newFile() {
    if (closeFile()) {
        filename.clear();
        editor->clear();
        editor->setWindowModified(false);
        setWindowTitle("Tiny+ IDE");
    }
}

void MainWindow::openFile(const QString &path) {
    if (!closeFile())
        return;

    QString fileName = path;
    if (fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "", "Tiny+ Code Files (*.t *.txt)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream toText(&file);
        toText.setCodec("UTF-8");
        editor->setPlainText(toText.readAll());
        file.close();

        filename = fileName;
        setTextModified(false);
    } else
        QMessageBox::warning(this, "警告", "无法读取文件");
}

void MainWindow::saveFile() {
    if (!editor->isWindowModified())
        return;
    if (filename.isEmpty())
        filename = QFileDialog::getSaveFileName(this, tr("保存"), "", "Tiny+ Code Files (*.t *.txt)");
    if (filename.isEmpty())
        return;
    
    QFile file(filename);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << editor->toPlainText();
        out.flush();
        file.close();

        setTextModified(false);
    } else
        QMessageBox::warning(this, "警告", "无法写入文件");
}

void MainWindow::saveFileAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), "", "Tiny+ Code Files (*.t *.txt)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << editor->toPlainText();
        out.flush();
        file.close();

        filename = fileName;
        setTextModified(false);
    } else
        QMessageBox::warning(this, "警告", "无法写入文件");
}

void MainWindow::textChanged() {
    setTextModified(!(filename.isEmpty() && editor->toPlainText().isEmpty()));
}

void MainWindow::setTextModified(bool isModified) {
    editor->setWindowModified(isModified);
    QFileInfo info(filename);
    if (isModified)
        setWindowTitle("*" + (filename.isEmpty() ? "Tiny+ Code" : info.fileName()) + " - Tiny+ IDE");
    else
        setWindowTitle((filename.isEmpty() ? "Tiny+ Code" : info.fileName()) + " - Tiny+ IDE");
}

void MainWindow::build() {
    if (editor->toPlainText().isEmpty()) {
        QMessageBox::information(this, "提示", "没有待编译代码");
        return;
    }
    if (filename.isEmpty())
        saveFileAs();
    else
        saveFile();
    if (filename.isEmpty())
        return;
    if (builder) {
        builder->close();
        delete builder;
    }
    builder = new BuildWindow(filename);
    builder->show();
}
