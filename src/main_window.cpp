#include <main_window.hpp>

#include <QMessageBox>

namespace otpd
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}
    , m_add_item_dialog{new AddItemDialog{this, m_otp_list_model}}
{
    m_ui->setupUi(this);
    this->setFixedSize(600, 480);
    m_add_item_dialog->setFixedSize(480, 240);

    m_ui->otp_list_view->setModel(m_otp_list_model);
    m_otp_item_delegate = new OTPItemDelegate{m_ui->otp_list_view};
    m_ui->otp_list_view->setItemDelegate(m_otp_item_delegate);

    connect(m_otp_item_delegate,
            &OTPItemDelegate::delete_button_clicked,
            this,
            &MainWindow::list_button_clicked);

    connect(m_ui->add_item_button,
            &QPushButton::clicked,
            this,
            &MainWindow::add_item_button_clicked);

    try
    {
        OTPListSingleton::get_instance().load();
    }
    catch (const ParserException& e)
    {
        QMessageBox::critical(this, tr("Oops!"), tr("Corrupted settings file"));
        throw e;
    }
}

void MainWindow::list_button_clicked(const QModelIndex& index)
{
    m_otp_list_model->delete_item(index);
}

void MainWindow::add_item_button_clicked()
{
    m_add_item_dialog->clear_content();
    m_add_item_dialog->show();
}

}  // namespace otpd
