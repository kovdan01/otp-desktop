#include <enter_password_dialog.hpp>
#include <otp_list.hpp>

#include <QMessageBox>

namespace otpd
{

EnterPasswordDialog::EnterPasswordDialog(QWidget* parent)
    : QDialog(parent)
{
    m_ui->setupUi(this);
    this->setFixedSize(400, 112);
}

void EnterPasswordDialog::clear_content()
{
    m_ui->password_line_edit->clear();
    m_ui->password_line_edit->setFocus();
}

void EnterPasswordDialog::accept() try
{
    try
    {
        std::string password = m_ui->password_line_edit->text().toStdString();
        auto& instance = OTPListSingleton::get_instance();
        instance.set_password(std::move(password));
        instance.load();
        this->clear_content();
        this->done(RESULT_OK);
    }
    catch (...)
    {
        this->clear_content();
        throw;
    }
}
OTP_DESKTOP_CATCH_PARSER_EXCEPTION
OTP_DESKTOP_CATCH_CRYPTO_EXCEPTION
OTP_DESKTOP_CATCH_STD_EXCEPTION
OTP_DESKTOP_CATCH_ANY_EXCEPTION

void EnterPasswordDialog::reject() try
{
    try
    {
        this->clear_content();
        this->done(RESULT_FAIL);
    }
    catch (...)
    {
        this->clear_content();
        throw;
    }
}
OTP_DESKTOP_CATCH_STD_EXCEPTION
OTP_DESKTOP_CATCH_ANY_EXCEPTION

}  // namespace otpd
