#ifndef OTP_DESKTOP_ADD_ITEM_DIALOG_HPP_
#define OTP_DESKTOP_ADD_ITEM_DIALOG_HPP_

#include <ui_add_edit_item_dialog.h>
#include <otp_list_model.hpp>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AddEditItemDialog;
}  // namespace Ui
QT_END_NAMESPACE

namespace otpd
{

class AddEditItemDialog : public QDialog
{
    Q_OBJECT
public:
    enum class Operation
    {
        ADD,
        EDIT,
    };

    explicit AddEditItemDialog(QWidget* parent, OTPListModel* otp_list_model);
    ~AddEditItemDialog() override = default;

    void clear_content();
    void display_existing_content();
    void accept() override;

    Operation operation;
    QModelIndex index;

private:
    QScopedPointer<Ui::AddEditItemDialog> m_ui{new Ui::AddEditItemDialog};
    OTPListModel* m_otp_list_model;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_ADD_ITEM_DIALOG_HPP_
