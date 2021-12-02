#include <add_edit_item_dialog.hpp>
#include <otp_list.hpp>

#include <QFont>
#include <QMessageBox>
#include <QValidator>

#include <string>

namespace otpd
{

AddEditItemDialog::AddEditItemDialog(QWidget* parent, OTPListModel* model)
    : QDialog{parent}
    , m_otp_list_model{model}
{
    m_ui->setupUi(this);
    this->setFixedSize(480, 240);
    m_ui->period_line_edit->setValidator(new QIntValidator(1, 30000, m_ui->period_line_edit));
    QFont secret_base32_font = font();
    secret_base32_font.setCapitalization(QFont::AllUppercase);
    secret_base32_font.setFamily(QStringLiteral("Noto Sans Mono Medium"));
    m_ui->secret_base32_line_edit->setFont(secret_base32_font);
}

void AddEditItemDialog::clear_content()
{
    m_ui->issuer_line_edit->clear();
    m_ui->label_line_edit->clear();
    m_ui->secret_base32_line_edit->clear();
    m_ui->period_line_edit->setText(QStringLiteral("30"));
    m_ui->issuer_line_edit->setFocus();
}

void AddEditItemDialog::display_existing_content()
{
    const TOTP* current_totp_item = m_otp_list_model->get_item(this->index);
    if (current_totp_item == nullptr)
    {
        this->clear_content();
        return;
    }
    m_ui->issuer_line_edit->setText(QString::fromStdString(current_totp_item->issuer()));
    m_ui->label_line_edit->setText(QString::fromStdString(current_totp_item->label()));
    m_ui->secret_base32_line_edit->setText(QString::fromStdString(current_totp_item->secret_base32()));
    m_ui->period_line_edit->setText(QString::fromStdString(std::to_string(current_totp_item->period())));
    m_ui->issuer_line_edit->setFocus();
}

void AddEditItemDialog::accept() try
{
    std::string issuer        = m_ui->issuer_line_edit->text().toStdString();
    std::string label         = m_ui->label_line_edit->text().toStdString();
    std::string secret_base32 = m_ui->secret_base32_line_edit->text().toStdString();

    do
    {
        if (secret_base32.empty())
        {
            QMessageBox::critical(this, tr("Oops!"), tr("Secret must not be empty!"));
            break;
        }

        QString period_string = m_ui->period_line_edit->text();
        const QValidator* period_validator = m_ui->period_line_edit->validator();
        int pos = 0;
        if (period_validator->validate(period_string, pos) != QValidator::Acceptable)
        {
            QMessageBox::critical(this, tr("Oops!"), tr("Period must be an integer between 1 and 30000!"));
            break;
        }
        unsigned period = m_ui->period_line_edit->text().toUInt();

        TOTP totp_item{std::move(issuer), std::move(label), std::move(secret_base32), period};

        switch (this->operation)
        {
        case Operation::ADD:
            m_otp_list_model->add_item(std::move(totp_item));
            break;
        case Operation::EDIT:
            m_otp_list_model->setData(this->index, QVariant::fromValue(totp_item), Qt::DisplayRole);
            break;
        }

    } while (false);

    this->close();
}
OTP_DESKTOP_CATCH_TOTP_EXCEPTION
OTP_DESKTOP_CATCH_CRYPTO_EXCEPTION
OTP_DESKTOP_CATCH_STD_EXCEPTION
OTP_DESKTOP_CATCH_ANY_EXCEPTION

}  // namespace otpd
