#include <ui_main_window.h>

#include <main_window.hpp>

namespace otpd
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_ui->listView->setModel(m_model_otp_list);

    m_otp_item_delegate = new OTPItemDelegate{m_ui->listView};
    m_ui->listView->setItemDelegate(m_otp_item_delegate);

    connect(m_otp_item_delegate,
            &OTPItemDelegate::delete_button_clicked,
            this,
            &MainWindow::list_button_clicked);
}

void MainWindow::list_button_clicked(const QModelIndex& index)
{
    m_model_otp_list->delete_item(index);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

}  // namespace otpd
