#include <main_window.hpp>
#include <totp.hpp>

#include <QApplication>

#include <iostream>

int main(int argc, char* argv[]) try
{
    [[maybe_unused]] const auto& totp_singletone = otpd::TOTPSingleton::get_instance();

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
