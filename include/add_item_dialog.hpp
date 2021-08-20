#ifndef OTP_DESKTOP_ADD_ITEM_DIALOG_HPP_
#define OTP_DESKTOP_ADD_ITEM_DIALOG_HPP_

#include <ui_add_item_dialog.h>
#include <otp_list_model.hpp>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AddItemDialog;
}  // namespace Ui
QT_END_NAMESPACE

namespace otpd
{

class AddItemDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddItemDialog(QWidget* parent, OTPListModel* otp_list_model);
    ~AddItemDialog() override = default;

    void clear_content();
    void accept() override;

private:
    QScopedPointer<Ui::AddItemDialog> m_ui{new Ui::AddItemDialog};
    OTPListModel* m_otp_list_model;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_ADD_ITEM_DIALOG_HPP_
