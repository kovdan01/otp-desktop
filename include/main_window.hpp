#ifndef OTP_DESKTOP_MAIN_WINDOW_HPP_
#define OTP_DESKTOP_MAIN_WINDOW_HPP_

#include <QMainWindow>
#include <qobjectdefs.h>

#include <shit.hpp>

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
    ~MainWindow();

public slots:
    void list_button_clicked(const QModelIndex& index);

private:
    Ui::MainWindow* m_ui;
    OTPListModel* m_model_otp_list = new OTPListModel{this};
    OTPItemDelegate* m_otp_item_delegate;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_MAIN_WINDOW_HPP_
