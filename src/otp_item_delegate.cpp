#include <totp.hpp>
#include <otp_item_delegate.hpp>

#include <QApplication>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>

#include <chrono>

namespace otpd
{

OTPItemDelegate::OTPItemDelegate(QObject* parent)
    : QStyledItemDelegate{parent}
{
}

QRect create_issuer_rect(const QStyleOptionViewItem& option)
{
    QRect issuer_rect = option.rect;
    issuer_rect.setX(issuer_rect.x() + 5);
    issuer_rect.setY(issuer_rect.y() + 10);
    issuer_rect.setWidth(100);
    issuer_rect.setHeight(30);
    return issuer_rect;
}

QRect create_label_rect(const QStyleOptionViewItem& option)
{
    QRect label_rect = option.rect;
    label_rect.setX(label_rect.x() + 105);
    label_rect.setY(label_rect.y() + 10);
    label_rect.setWidth(200);
    label_rect.setHeight(30);
    return label_rect;
}

QRect create_otp_rect(const QStyleOptionViewItem& option)
{
    QRect otp_rect = option.rect;
    otp_rect.setX(otp_rect.x() + 305);
    otp_rect.setY(otp_rect.y() + 10);
    otp_rect.setWidth(150);
    otp_rect.setHeight(30);
    return otp_rect;
}

QRect create_show_hide_button_rect(const QStyleOptionViewItem& option)
{
    QRect show_hide_button_rect(option.rect);
    show_hide_button_rect.setX(show_hide_button_rect.x() + 455);
    show_hide_button_rect.setY(show_hide_button_rect.y() + 10);
    show_hide_button_rect.setWidth(50);
    show_hide_button_rect.setHeight(30);
    return show_hide_button_rect;
}

QStyleOptionButton create_show_hide_button(const QRect& show_hide_button_rect, bool visible)
{
    QStyleOptionButton show_hide_button;
    show_hide_button.rect = show_hide_button_rect;
    show_hide_button.text = (visible ? QStringLiteral("Hide") : QStringLiteral("Show"));
    return show_hide_button;
}

QRect create_delete_button_rect(const QStyleOptionViewItem& option)
{
    QRect delete_button_rect(option.rect);
    delete_button_rect.setX(delete_button_rect.x() + 505);
    delete_button_rect.setY(delete_button_rect.y() + 10);
    delete_button_rect.setRight(delete_button_rect.right() - 5);
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

QRect create_progress_bar_rect(const QStyleOptionViewItem& option)
{
    QRect progress_bar_rect(option.rect);
    progress_bar_rect.setLeft(progress_bar_rect.left() + 5);
    progress_bar_rect.setRight(progress_bar_rect.right() - 5);
    progress_bar_rect.setHeight(10);
    return progress_bar_rect;
}

QStyleOptionProgressBar create_progress_bar(const QRect& progress_bar_rect, double progress)
{
    QStyleOptionProgressBar progress_bar;
    progress_bar.rect = progress_bar_rect;
    progress_bar.minimum = 0;
    progress_bar.maximum = progress_bar_rect.width();
    progress_bar.progress = progress * progress_bar.maximum;
    return progress_bar;
}

void OTPItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int row = index.row();
    TOTP totp = index.data(Qt::DisplayRole).value<TOTP>();

    QRect issuer_rect = create_issuer_rect(option);
    const std::string& issuer = totp.issuer();
    painter->drawText(issuer_rect, Qt::AlignVCenter, QString::fromStdString(issuer));

    QRect label_rect = create_label_rect(option);
    const std::string& label = totp.label();
    painter->drawText(label_rect, Qt::AlignVCenter, QString::fromStdString(label));

    QRect otp_rect = create_otp_rect(option);
    std::string otp = totp.generate();
    QFont otp_font;
    otp_font.setBold(true);
    otp_font.setFamily(QStringLiteral("DejaVu Sans Mono"));
    otp_font.setLetterSpacing(QFont::PercentageSpacing, 200);
    painter->setFont(otp_font);
    painter->drawText(otp_rect, Qt::AlignVCenter | Qt::AlignHCenter, totp.visible ? QString::fromStdString(otp) : QStringLiteral("******"));
    painter->setFont(QFont{});

    QRect show_hide_button_rect = create_show_hide_button_rect(option);
    QStyleOptionButton show_hide_button = create_show_hide_button(show_hide_button_rect, totp.visible);
    show_hide_button.state = (m_show_hide_button_states.contains(row) ? m_show_hide_button_states.at(row) : QStyle::State_None ) | QStyle::State_Enabled;
    QApplication::style()->drawControl(QStyle::CE_PushButton, &show_hide_button, painter);

    QRect delete_button_rect = create_delete_button_rect(option);
    QStyleOptionButton delete_button = create_delete_button(delete_button_rect);
    delete_button.state = (m_delete_button_states.contains(row) ? m_delete_button_states.at(row) : QStyle::State_None ) | QStyle::State_Enabled;
    QApplication::style()->drawControl(QStyle::CE_PushButton, &delete_button, painter);

    unsigned period = totp.period();
    auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double progress = static_cast<double>(period - (now % period)) / period;
    QRect progress_bar_rect = create_progress_bar_rect(option);
    QStyleOptionProgressBar progress_bar = create_progress_bar(progress_bar_rect, progress);
    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progress_bar, painter);
}

QSize OTPItemDelegate::sizeHint(const QStyleOptionViewItem& /* option */, const QModelIndex& /* index */) const
{
    const auto* p = qobject_cast<QListView*>(this->parent());
    return QSize(p->viewport()->size().width(), 40);
}

bool OTPItemDelegate::editorEvent(
    QEvent* event,
    QAbstractItemModel* /* model */,
    const QStyleOptionViewItem& option,
    const QModelIndex& index)
{
    int row = index.row();

    switch (event->type())
    {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        break;
    default:
        // ignoring other mouse event and reseting button's state
        m_show_hide_button_states[row] = QStyle::State_Raised;
        m_delete_button_states[row] = QStyle::State_Raised;
        return true;
    }

    QRect show_hide_button_rect = create_show_hide_button_rect(option);
    QRect delete_button_rect = create_delete_button_rect(option);

    QPoint pos = static_cast<QMouseEvent*>(event)->pos();
    if (delete_button_rect.contains(pos))
    {
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
            m_delete_button_states[row] = QStyle::State_Sunken;
            break;
        case QEvent::MouseButtonRelease:
            m_delete_button_states[row] = QStyle::State_Raised;
            emit this->delete_button_clicked(index);
            break;
        default:
            assert(false);
            break;
        }
    }
    else if (show_hide_button_rect.contains(pos))
    {
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
            m_show_hide_button_states[row] = QStyle::State_Sunken;
            break;
        case QEvent::MouseButtonRelease:
            m_show_hide_button_states[row] = QStyle::State_Raised;
            emit this->show_hide_button_clicked(index);
            break;
        default:
            assert(false);
            break;
        }
    }
    else
    {
        m_show_hide_button_states[row] = QStyle::State_Raised;
        m_delete_button_states[row] = QStyle::State_Raised;
    }

    return true;
}

}  // namespace otpd
