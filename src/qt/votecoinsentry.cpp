#include "votecoinsentry.h"
#include "ui_votecoinsentry.h"

#include "guiutil.h"
#include "bitcoinunits.h"
#include "addressbookpage.h"
#include "walletmodel.h"
#include "optionsmodel.h"
#include "addresstablemodel.h"

#include "net.h"
#include <QApplication>
#include <QClipboard>

VoteCoinsEntry::VoteCoinsEntry(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::VoteCoinsEntry),
    model(nullptr)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    ui->payToLayout->setSpacing(4);
#endif
#if QT_VERSION >= 0x040700
    /* Do not move this to the XML file, Qt before 4.7 will choke on it */
    //ui->addAsLabel->setPlaceholderText(tr("Enter a label for this address to add it to your address book"));
    ui->payTo->setPlaceholderText(tr("Enter a BunnyCoin voting address (starts with BUNN)"));
#endif
    setFocusPolicy(Qt::TabFocus);
    setFocusProxy(ui->payTo);

    GUIUtil::setupAddressWidget(ui->payTo, this);
}

VoteCoinsEntry::~VoteCoinsEntry()
{
    delete ui;
}

void VoteCoinsEntry::on_pasteButton_clicked()
{
    // Paste text from clipboard into recipient field
    ui->payTo->setText(QApplication::clipboard()->text());
}

void VoteCoinsEntry::on_addressBookButton_clicked()
{
    if(!model)
        return;
    AddressBookPage dlg(AddressBookPage::ForSending, AddressBookPage::SendingTab, this);
    dlg.setModel(model->getAddressTableModel());
    if(dlg.exec())
    {
        ui->payTo->setText(dlg.getReturnValue());
        //ui->payAmount->setFocus();
    }
}

void VoteCoinsEntry::on_payTo_textChanged(const QString &address)
{
    if(!model)
        return;
    // Fill in label from address book, if address has an associated label
    QString associatedLabel = model->getAddressTableModel()->labelForAddress(address);
    //if(!associatedLabel.isEmpty())
    //    ui->addAsLabel->setText(associatedLabel);
}

void VoteCoinsEntry::setModel(WalletModel *model)
{
    this->model = model;

    if(model && model->getOptionsModel())
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

    clear();
}

void VoteCoinsEntry::setRemoveEnabled(bool enabled)
{
    ui->deleteButton->setEnabled(enabled);
}

void VoteCoinsEntry::clear()
{
    ui->payTo->clear();
    //ui->addAsLabel->clear();
    //ui->payAmount->clear();
    ui->payTo->setFocus();
    // update the display unit, to not use the default ("BUN")
    updateDisplayUnit();
}

void VoteCoinsEntry::on_deleteButton_clicked()
{
    emit removeEntry(this);
}

bool VoteCoinsEntry::validate()
{
    // Check input validity
    bool retval = true;

    /*if(!ui->payAmount->validate())
    {
        retval = false;
    }
    else
    {
        if(ui->payAmount->value() <= 0)
        {
            // Cannot send 0 coins or less
            ui->payAmount->setValid(false);
            retval = false;
        }
    }*/

    if(!ui->payTo->hasAcceptableInput() ||
       (model && !model->validateAddress(ui->payTo->text())))
    {
        ui->payTo->setValid(false);
        retval = false;
    }


    if(!ui->payTo->text().startsWith("BUNN")){ // BUNN
        ui->payTo->setValid(false);
        retval = false;
    }

    return retval;
}

SendCoinsRecipient VoteCoinsEntry::getValue()
{
    SendCoinsRecipient rv;

    rv.address = ui->payTo->text();
    //rv.label = ui->addAsLabel->text();
    rv.amount =  ui->payAmount->currentIndex()+1;
            //itemData();
            //value();

    return rv;
}

/*QWidget *VoteCoinsEntry::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, ui->payTo);
    QWidget::setTabOrder(ui->payTo, ui->addressBookButton);
    QWidget::setTabOrder(ui->addressBookButton, ui->pasteButton);
    QWidget::setTabOrder(ui->pasteButton, ui->deleteButton);
    //QWidget::setTabOrder(ui->deleteButton, ui->addAsLabel);
    //return ui->payAmount->setupTabChain(ui->addAsLabel);
    //return ui->deleteButton->setupTabChain(ui->deleteButton);
    return NULL;
}*/

void VoteCoinsEntry::setValue(const SendCoinsRecipient &value)
{
    ui->payTo->setText(value.address);
    //ui->addAsLabel->setText(value.label);
    //ui->payAmount->setValue(value.amount);
}

void VoteCoinsEntry::setAddress(const QString &address)
{
    ui->payTo->setText(address);
    //ui->payAmount->setFocus();
}

bool VoteCoinsEntry::isClear()
{
    return ui->payTo->text().isEmpty();
}

void VoteCoinsEntry::setFocus()
{
    ui->payTo->setFocus();
}

void VoteCoinsEntry::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        // Update payAmount with the current unit
        //ui->payAmount->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
    }
}
