#ifndef OTP_DESKTOP_OTP_LIST_MODEL_HPP_
#define OTP_DESKTOP_OTP_LIST_MODEL_HPP_

#include <totp.hpp>

#include <QAbstractListModel>

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

    void add_item(TOTP item);
    void delete_item(const QModelIndex& index);
    const TOTP* get_item(const QModelIndex& index);
    void change_visibility(const QModelIndex& index);

private:
    void timer_hit();
};

}  // namespace otpd

#endif  // OTP_DESKTOP_OTP_LIST_MODEL_HPP_
