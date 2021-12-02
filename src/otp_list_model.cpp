#include <otp_list_model.hpp>
#include <otp_list.hpp>

#include <QApplication>
#include <QListView>
#include <QMessageBox>
#include <QTimer>

#include <chrono>

namespace otpd
{

OTPListModel::OTPListModel(QObject* parent)
    : QAbstractListModel{parent}
{
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    QTimer::singleShot(1000 - (now % 1000), this, &OTPListModel::timer_hit);
}

int OTPListModel::rowCount(const QModelIndex& /* index */) const
{
    return static_cast<int>(OTPListSingleton::get_instance().entries().size());
}

QVariant OTPListModel::data(const QModelIndex& index, int role) const
{
    QVariant data;

    if (!index.isValid())
        return data;

    std::size_t row = static_cast<std::size_t>(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
        data.setValue(OTPListSingleton::get_instance().entries()[row]);
        break;
    default:
        break;
    }

    return data;
}

Qt::ItemFlags OTPListModel::flags(const QModelIndex& index) const
{
    return index.isValid() ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::ItemFlags();
}

bool OTPListModel::setData(const QModelIndex& index, const QVariant& value, int role) try
{
    if (!index.isValid())
        return false;

    std::size_t row = static_cast<std::size_t>(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    {
        auto& otp_list_singleton = OTPListSingleton::get_instance();
        std::vector<TOTP>& entries = otp_list_singleton.entries();
        std::vector<TOTP> entries_copy = entries;
        entries[row] = value.value<TOTP>();
        try
        {
            otp_list_singleton.dump();
        }
        catch (...)
        {
            entries = entries_copy;
            return false;
        }
        emit dataChanged(index, index);
        return true;
    }
    default:
        break;
    }

    return false;
}
catch (...)
{
    return false;
}

void OTPListModel::add_item(TOTP item)
{
    QModelIndex parent{};
    int row = this->rowCount(parent) + 1;
    this->beginInsertRows(parent, row, row);
    auto& otp_list_singleton = OTPListSingleton::get_instance();
    otp_list_singleton.entries().emplace_back(std::move(item));
    try
    {
        otp_list_singleton.dump();
    }
    catch (...)
    {
        otp_list_singleton.entries().pop_back();
        throw;
    }

    this->endInsertRows();
}

const TOTP* OTPListModel::get_item(const QModelIndex& index)
{
    if (!index.isValid())
        return nullptr;

    std::size_t row = static_cast<std::size_t>(index.row());
    auto& otp_list_singleton = OTPListSingleton::get_instance();
    return &otp_list_singleton.entries()[row];
}

void OTPListModel::delete_item(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    std::size_t row = static_cast<std::size_t>(index.row());
    auto& otp_list_singleton = OTPListSingleton::get_instance();
    std::vector<TOTP>& entries = otp_list_singleton.entries();
    std::vector<TOTP> entries_copy = entries;
    entries.erase(entries.begin() + row);
    try
    {
        otp_list_singleton.dump();
    }
    catch (...)
    {
        entries = entries_copy;
        throw;
    }

    emit dataChanged(index, index);
}

void OTPListModel::change_visibility(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    std::size_t row = static_cast<std::size_t>(index.row());
    OTPListSingleton::get_instance().entries()[row].visible ^= 1;
    emit dataChanged(index, index);
}

void OTPListModel::timer_hit()
{
    const std::vector<TOTP>& entries = OTPListSingleton::get_instance().entries();
    if (!entries.empty())
    {
        QModelIndex first = this->index(0);
        QModelIndex last = this->index(static_cast<int>(entries.size()) - 1);
        emit dataChanged(first, last);
    }
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    QTimer::singleShot(1000 - (now % 1000), this, &OTPListModel::timer_hit);
}

}  // namespace otpd
