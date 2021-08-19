#include <main_window.hpp>
#include <otp_list.hpp>
#include <totp.hpp>

#include <QApplication>

#include <iostream>

int main(int argc, char* argv[]) try
{
    [[maybe_unused]] const auto& totp_singleton = otpd::TOTPSingleton::get_instance();
    [[maybe_unused]] const auto& otp_list_singleton = otpd::OTPListSingleton::get_instance();

    QApplication application(argc, argv);

    otpd::MainWindow window;
    window.show();
    return QApplication::exec();
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
catch (...)
{
    std::cerr << "Unknown error!" << std::endl;
}
