#ifndef OTP_DESKTOP_SHIT_HPP_
#define OTP_DESKTOP_SHIT_HPP_

#include <otp_list.hpp>

#include <QAbstractListModel>
#include <QStyledItemDelegate>

namespace otpd
{

class OTPListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit OTPListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& index) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex&) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    void delete_item(const QModelIndex& index);

private:
    void timer_hit();
};

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

#endif  // OTP_DESKTOP_SHIT_HPP_
