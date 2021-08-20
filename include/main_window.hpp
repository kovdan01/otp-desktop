#ifndef OTP_DESKTOP_MAIN_WINDOW_HPP_
#define OTP_DESKTOP_MAIN_WINDOW_HPP_

#include <add_item_dialog.hpp>
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
    void list_button_clicked(const QModelIndex& index);
    void add_item_button_clicked();

private:
    QScopedPointer<Ui::MainWindow> m_ui{new Ui::MainWindow};
    OTPListModel* m_otp_list_model{new OTPListModel{this}};
    OTPItemDelegate* m_otp_item_delegate;
    AddItemDialog* m_add_item_dialog;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_MAIN_WINDOW_HPP_
