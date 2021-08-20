#include <shit.hpp>

#include <QApplication>
#include <QListView>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QTimer>

#include <chrono>

Q_DECLARE_METATYPE(otpd::TOTP)

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
    return OTPListSingleton::get_instance().entries().size();
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

bool OTPListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    std::size_t row = static_cast<std::size_t>(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
        OTPListSingleton::get_instance().entries()[row] = value.value<TOTP>();
        emit dataChanged(index, index);
        return true;
    default:
        break;
    }

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
    catch (const ParserException& e)
    {
        otp_list_singleton.entries().pop_back();
        throw e;
    }

    this->endInsertRows();
}

void OTPListModel::delete_item(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    std::size_t row = static_cast<std::size_t>(index.row());
    auto& otp_list_singleton = OTPListSingleton::get_instance();
    std::vector<TOTP>& entries = otp_list_singleton.entries();
    entries.erase(entries.begin() + row);
    otp_list_singleton.dump();
    emit dataChanged(index, index);
}

void OTPListModel::timer_hit()
{
    const std::vector<TOTP>& entries = OTPListSingleton::get_instance().entries();
    if (!entries.empty())
    {
        QModelIndex first = this->index(0);
        QModelIndex last = this->index(entries.size() - 1);
        emit dataChanged(first, last);
    }
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    QTimer::singleShot(1000 - (now % 1000), this, &OTPListModel::timer_hit);
}

OTPItemDelegate::OTPItemDelegate(QObject* parent)
    : QStyledItemDelegate{parent}
{
}

QRect create_label_rect(const QStyleOptionViewItem& option)
{
    QRect label_rect = option.rect;
    label_rect.setWidth(150);
    label_rect.setHeight(30);
    return label_rect;
}

QRect create_otp_rect(const QStyleOptionViewItem& option)
{
    QRect otp_rect = option.rect;
    otp_rect.setX(otp_rect.x() + 150);
    otp_rect.setWidth(150);
    otp_rect.setHeight(30);
    return otp_rect;
}

QRect create_delete_button_rect(const QStyleOptionViewItem& option)
{
    QRect delete_button_rect(option.rect);
    delete_button_rect.setX(delete_button_rect.x() + 300);
    delete_button_rect.setWidth(100);
    delete_button_rect.setHeight(30);
    return delete_button_rect;
}

QStyleOptionButton create_delete_button(const QRect& delete_button_rect)
{
    QStyleOptionButton delete_button;
    delete_button.rect = delete_button_rect;
    delete_button.text = QStringLiteral("Delete");
    return delete_button;
}

void OTPItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QRect label_rect = create_label_rect(option);
    QRect otp_rect = create_otp_rect(option);
    QRect delete_button_rect = create_delete_button_rect(option);
    QStyleOptionButton delete_button = create_delete_button(delete_button_rect);
    delete_button.state = m_state | QStyle::State_Enabled;
    QApplication::style()->drawControl(QStyle::CE_PushButton, &delete_button, painter);

    TOTP totp = index.data(Qt::DisplayRole).value<TOTP>();
    const std::string& label = totp.label();
    painter->drawText(label_rect, QString::fromStdString(label));
    std::string otp = totp.generate();
    painter->drawText(otp_rect, QString::fromStdString(otp));
}

QSize OTPItemDelegate::sizeHint(const QStyleOptionViewItem& /* option */, const QModelIndex& /* index */) const
{
    const auto* p = qobject_cast<QListView*>(this->parent());
    return QSize(p->viewport()->size().width(), 30);
}

bool OTPItemDelegate::editorEvent(
    QEvent* event,
    QAbstractItemModel* /* model */,
    const QStyleOptionViewItem& option,
    const QModelIndex& index)
{
    switch (event->type())
    {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        break;
    default:
        // ignoring other mouse event and reseting button's state
        m_state = QStyle::State_Raised;
        return true;
    }

    QRect delete_button_rect = create_delete_button_rect(option);

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    if (!delete_button_rect.contains(mouseEvent->pos()))
    {
        m_state = QStyle::State_Raised;
        return true;
    }

    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        m_state = QStyle::State_Sunken;
        break;
    case QEvent::MouseButtonRelease:
        m_state = QStyle::State_Raised;
        emit this->delete_button_clicked(index);
        break;
    default:
        assert(false);
        break;
    }
    return true;
}

}  // namespace otpd
