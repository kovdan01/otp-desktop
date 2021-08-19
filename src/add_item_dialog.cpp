#include <add_item_dialog.hpp>

namespace otpd
{

AddItemDialog::AddItemDialog(QWidget* parent)
    : QDialog(parent)
{
    m_ui->setupUi(this);
}

}  // namespace otpd
