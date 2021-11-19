#ifndef OTP_DESKTOP_CREATE_PASSWORD_DIALOG_HPP_
#define OTP_DESKTOP_CREATE_PASSWORD_DIALOG_HPP_

#include <ui_create_password_dialog.h>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CreatePasswordDialog;
}  // namespace Ui
QT_END_NAMESPACE

namespace otpd
{

class CreatePasswordDialog : public QDialog
{
    Q_OBJECT
public:
    enum Result
    {
        RESULT_OK   = 0,
        RESULT_FAIL = 1,
    };

    explicit CreatePasswordDialog(QWidget* parent);
    ~CreatePasswordDialog() override = default;

    void clear_content();
    void accept() override;
    void reject() override;

    void set_caption_create_password();
    void set_caption_change_password();

private:
    QScopedPointer<Ui::CreatePasswordDialog> m_ui{new Ui::CreatePasswordDialog};
};

}  // namespace otpd

#endif  // OTP_DESKTOP_CREATE_PASSWORD_DIALOG_HPP_
