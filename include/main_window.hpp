#ifndef OTP_DESKTOP_MAIN_WINDOW_HPP_
#define OTP_DESKTOP_MAIN_WINDOW_HPP_

#include <add_item_dialog.hpp>
#include <shit.hpp>
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
    OTPListModel* m_model_otp_list{new OTPListModel{this}};
    OTPItemDelegate* m_otp_item_delegate;
    AddItemDialog* m_add_item_dialog{new AddItemDialog{this}};
};

}  // namespace otpd

#endif  // OTP_DESKTOP_MAIN_WINDOW_HPP_
