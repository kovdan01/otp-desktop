#include <create_password_dialog.hpp>
#include <enter_password_dialog.hpp>
#include <main_window.hpp>
#include <otp_list.hpp>

#include <QMessageBox>

namespace otpd
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}
    , m_add_edit_item_dialog{new AddEditItemDialog{this, m_otp_list_model.data()}}
    , m_create_password_dialog{new CreatePasswordDialog{this}}
    , m_enter_password_dialog{new EnterPasswordDialog{this}}
{
    m_ui->setupUi(this);
    this->setFixedSize(600, 480);

    m_ui->otp_list_view->setModel(m_otp_list_model.data());
    m_otp_item_delegate.reset(new OTPItemDelegate{m_ui->otp_list_view});
    m_ui->otp_list_view->setItemDelegate(m_otp_item_delegate.data());

    connect(m_otp_item_delegate.data(),
            &OTPItemDelegate::show_hide_button_clicked,
            this,
            &MainWindow::show_hide_button_clicked);

    connect(m_otp_item_delegate.data(),
            &OTPItemDelegate::delete_button_clicked,
            this,
            &MainWindow::delete_button_clicked);

    connect(m_otp_item_delegate.data(),
            &OTPItemDelegate::edit_button_clicked,
            this,
            &MainWindow::edit_button_clicked);

    connect(m_ui->add_item_button,
            &QPushButton::clicked,
            this,
            &MainWindow::add_item_button_clicked);

    connect(m_ui->change_password_button,
            &QPushButton::clicked,
            this,
            &MainWindow::change_password_button_clicked);

    auto& instance = OTPListSingleton::get_instance();
    if (std::filesystem::is_regular_file(instance.settings_path()))
    {
        m_enter_password_dialog->clear_content();
        int code = m_enter_password_dialog->exec();
        switch (code)
        {
        case EnterPasswordDialog::RESULT_OK:
            break;
        case EnterPasswordDialog::RESULT_FAIL:
            throw std::runtime_error("Exit before password enter");
            break;
        }
    }
    else
    {
        m_create_password_dialog->set_caption_create_password();
        m_create_password_dialog->clear_content();
        int code = m_create_password_dialog->exec();
        switch (code)
        {
        case CreatePasswordDialog::RESULT_OK:
            break;
        case CreatePasswordDialog::RESULT_FAIL:
            throw std::runtime_error("Exit before password create");
            break;
        }
    }
}

void MainWindow::edit_button_clicked(const QModelIndex& index)
{
    m_add_edit_item_dialog->operation = AddEditItemDialog::Operation::EDIT;
    m_add_edit_item_dialog->index = index;
    m_add_edit_item_dialog->display_existing_content();
    m_add_edit_item_dialog->show();
}

void MainWindow::delete_button_clicked(const QModelIndex& index) try
{
    m_otp_list_model->delete_item(index);
}
OTP_DESKTOP_CATCH_CRYPTO_EXCEPTION
OTP_DESKTOP_CATCH_STD_EXCEPTION
OTP_DESKTOP_CATCH_ANY_EXCEPTION

void MainWindow::show_hide_button_clicked(const QModelIndex& index) try
{
    m_otp_list_model->change_visibility(index);
}
OTP_DESKTOP_CATCH_STD_EXCEPTION
OTP_DESKTOP_CATCH_ANY_EXCEPTION

void MainWindow::add_item_button_clicked()
{
    m_add_edit_item_dialog->operation = AddEditItemDialog::Operation::ADD;
    m_add_edit_item_dialog->clear_content();
    m_add_edit_item_dialog->show();
}

void MainWindow::change_password_button_clicked()
{
    m_create_password_dialog->set_caption_change_password();
    m_create_password_dialog->clear_content();
    m_create_password_dialog->show();
}

}  // namespace otpd
