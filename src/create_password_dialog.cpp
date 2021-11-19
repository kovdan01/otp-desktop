#include <create_password_dialog.hpp>
#include <crypto.hpp>
#include <otp_list.hpp>

#include <QMessageBox>

#include <algorithm>

namespace otpd
{

CreatePasswordDialog::CreatePasswordDialog(QWidget* parent)
    : QDialog(parent)
{
    m_ui->setupUi(this);
    this->setFixedSize(400, 180);
}

void CreatePasswordDialog::clear_content()
{
    m_ui->enter_password_line_edit->clear();
    m_ui->repeat_password_line_edit->clear();
    m_ui->enter_password_line_edit->setFocus();
}

void CreatePasswordDialog::accept()
{
    std::string password1 = m_ui->enter_password_line_edit->text().toStdString();
    std::string password2 = m_ui->repeat_password_line_edit->text().toStdString();
    if (password1 != password2)
    {
        zero_data(password1);
        zero_data(password2);
        this->clear_content();
        QMessageBox::critical(this, tr("Oops!"), tr("Passwords do not match!"));
    }
    else
    {
        auto& instance = OTPListSingleton::get_instance();
        instance.set_password(std::move(password1));
        instance.dump();
        zero_data(password2);
        this->clear_content();
        QMessageBox::information(this, tr("OK"), tr("Password set success"));
        this->done(RESULT_OK);
    }
}

void CreatePasswordDialog::reject()
{
    this->clear_content();
    this->done(RESULT_FAIL);
}

void CreatePasswordDialog::set_caption_create_password()
{
    m_ui->label->setText(tr("It seems that you use the app for the first time. "
                            "Please set up a password to encrypt your data."));
}

void CreatePasswordDialog::set_caption_change_password()
{
    m_ui->label->setText(tr("You are about to change your data encryption password. "
                            "Please type new password for 2 times."));
}

}  // namespace otpd
