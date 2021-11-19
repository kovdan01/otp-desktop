#ifndef OTP_DESKTOP_ENTER_PASSWORD_DIALOG_HPP_
#define OTP_DESKTOP_ENTER_PASSWORD_DIALOG_HPP_

#include <ui_enter_password_dialog.h>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class EnterPasswordDialog;
}  // namespace Ui
QT_END_NAMESPACE

namespace otpd
{

class EnterPasswordDialog : public QDialog
{
    Q_OBJECT
public:
    enum Result
    {
        RESULT_OK   = 0,
        RESULT_FAIL = 1,
    };

    explicit EnterPasswordDialog(QWidget* parent);
    ~EnterPasswordDialog() override = default;

    void clear_content();
    void accept() override;
    void reject() override;

private:
    QScopedPointer<Ui::EnterPasswordDialog> m_ui{new Ui::EnterPasswordDialog};
};

}  // namespace otpd

#endif  // OTP_DESKTOP_ENTER_PASSWORD_DIALOG_HPP_
