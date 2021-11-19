#ifndef OTP_DESKTOP_MAIN_WINDOW_HPP_
#define OTP_DESKTOP_MAIN_WINDOW_HPP_

#include <add_item_dialog.hpp>
#include <create_password_dialog.hpp>
#include <enter_password_dialog.hpp>
#include <otp_item_delegate.hpp>
#include <otp_list_model.hpp>
#include <ui_main_window.h>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}  // namespace Ui
QT_END_NAMESPACE

namespace otpd
{

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

public slots:
    void show_hide_button_clicked(const QModelIndex& index);
    void delete_button_clicked(const QModelIndex& index);
    void add_item_button_clicked();
    void change_password_button_clicked();

private:
    QScopedPointer<Ui::MainWindow> m_ui{new Ui::MainWindow};
    QScopedPointer<OTPListModel> m_otp_list_model{new OTPListModel{this}};
    QScopedPointer<OTPItemDelegate> m_otp_item_delegate;
    QScopedPointer<AddItemDialog> m_add_item_dialog;
    QScopedPointer<CreatePasswordDialog> m_create_password_dialog;
    QScopedPointer<EnterPasswordDialog> m_enter_password_dialog;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_MAIN_WINDOW_HPP_
