#ifndef OTP_DESKTOP_OTP_ITEM_DELEGATE_HPP_
#define OTP_DESKTOP_OTP_ITEM_DELEGATE_HPP_

#include <QStyledItemDelegate>

namespace otpd
{

class OTPItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit OTPItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent* event, QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

signals:
    void delete_button_clicked(const QModelIndex& index);

private:
    QStyle::State m_state;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_OTP_ITEM_DELEGATE_HPP_
