#ifndef VOTECOINSENTRY_H
#define VOTECOINSENTRY_H

#include <QFrame>
#include "sendcoinsdialog.h"

namespace Ui {
    class VoteCoinsEntry;
}
class WalletModel;
//class VoteCoinsRecipient;

/** A single entry in the dialog for sending bitcoins. */
class VoteCoinsEntry : public QFrame
{
    Q_OBJECT

public:
    explicit VoteCoinsEntry(QWidget *parent = 0);
    ~VoteCoinsEntry();

    void setModel(WalletModel *model);
    bool validate();
    SendCoinsRecipient getValue();

    /** Return whether the entry is still empty and unedited */
    bool isClear();

    void setValue(const SendCoinsRecipient &value);
    void setAddress(const QString &address);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907).
     */
    QWidget *setupTabChain(QWidget *prev);

    void setFocus();

public slots:
    void setRemoveEnabled(bool enabled);
    void clear();

signals:
    void removeEntry(VoteCoinsEntry *entry);

private slots:
    void on_deleteButton_clicked();
    void on_payTo_textChanged(const QString &address);
    void on_addressBookButton_clicked();
    void on_pasteButton_clicked();
    void updateDisplayUnit();

private:
    Ui::VoteCoinsEntry *ui;
    WalletModel *model;
};

#endif // VOTECOINSENTRY_H
