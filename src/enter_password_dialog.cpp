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

void EnterPasswordDialog::accept()
{
    std::string password = m_ui->password_line_edit->text().toStdString();
    try
    {
        auto& instance = OTPListSingleton::get_instance();
        instance.set_password(std::move(password));
        instance.load();
        this->clear_content();
        this->done(RESULT_OK);
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("Oops!"), QLatin1String(e.what()));
        this->clear_content();
    }
}

void EnterPasswordDialog::reject()
{
    this->clear_content();
    this->done(RESULT_FAIL);
}

}  // namespace otpd
