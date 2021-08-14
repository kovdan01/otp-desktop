#include <ui_main_window.h>

#include <main_window.hpp>

namespace otpd
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

}  // namespace otpd
