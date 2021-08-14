#include <main_window.hpp>

#include <QApplication>

#include <iostream>

int main(int argc, char* argv[]) try
{
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
