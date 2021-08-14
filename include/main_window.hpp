#ifndef OTP_DESKTOP_MAIN_WINDOW_HPP_
#define OTP_DESKTOP_MAIN_WINDOW_HPP_

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace otpd
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_MAIN_WINDOW_HPP_
