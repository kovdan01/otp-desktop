#include <main_window.hpp>

namespace otpd
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_ui->setupUi(this);

    m_ui->OTPListView->setModel(m_model_otp_list);
    m_otp_item_delegate = new OTPItemDelegate{m_ui->OTPListView};
    m_ui->OTPListView->setItemDelegate(m_otp_item_delegate);

    connect(m_otp_item_delegate,
            &OTPItemDelegate::delete_button_clicked,
            this,
            &MainWindow::list_button_clicked);

    connect(m_ui->AddItemButton,
            &QPushButton::clicked,
            this,
            &MainWindow::add_item_button_clicked);
}

void MainWindow::list_button_clicked(const QModelIndex& index)
{
    m_model_otp_list->delete_item(index);
}

void MainWindow::add_item_button_clicked()
{
    m_add_item_dialog->show();
}

}  // namespace otpd
