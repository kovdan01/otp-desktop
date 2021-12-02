#ifndef OTP_DESKTOP_OTP_ITEM_DELEGATE_HPP_
#define OTP_DESKTOP_OTP_ITEM_DELEGATE_HPP_

#include <QStyledItemDelegate>

#include <map>

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
    void show_hide_button_clicked(const QModelIndex& index);
    void delete_button_clicked(const QModelIndex& index);
    void edit_button_clicked(const QModelIndex& index);

private:
    std::map<int, QStyle::State> m_show_hide_button_states;
    std::map<int, QStyle::State> m_delete_button_states;
    std::map<int, QStyle::State> m_edit_button_states;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_OTP_ITEM_DELEGATE_HPP_
