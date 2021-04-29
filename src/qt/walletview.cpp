/*
 * Qt4 bitcoin GUI.
 *
 * W.J. van der Laan 2011-2012
 * The Bitcoin Developers 2011-2013
 */
#include "walletview.h"
#include "bitcoingui.h"
#include "transactiontablemodel.h"
#include "addressbookpage.h"
#include "sendcoinsdialog.h"
#include "signverifymessagedialog.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "optionsmodel.h"
#include "transactionview.h"
#include "overviewpage.h"
#include "askpassphrasedialog.h"
#include "ui_interface.h"
#include "miningpage.h"
#include "mininginfopage.h"
#include "guiheader.h"
#include "vanitygenpage.h"
#include "marketcappage.h"
#include "tickerheader.h"

#include "zilliongridpage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#if QT_VERSION < 0x050000
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif
#include <QFileDialog>
#include <QPushButton>
#include <QToolButton>

#include <QDebug>

WalletView::WalletView(QWidget *parent, BitcoinGUI *_gui):
    QStackedWidget(parent),
    gui(_gui),
    clientModel(0),
    walletModel(0)
{
    // Create tabs
    overviewPage = new OverviewPage(this,gui);

    transactionsPage = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setContentsMargins(0,0,0,0);
    vbox->setSpacing(10);

    QHBoxLayout *hbox_buttons = new QHBoxLayout();
    transactionView = new TransactionView(this);
    vbox->addWidget(transactionView);
    QPushButton *exportButton = new QPushButton(tr("&Export"), this);
    exportButton->setToolTip(tr("Export the data in the current tab to a file"));
#ifndef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    exportButton->setIcon(QIcon(":/icons/export"));
#endif
    hbox_buttons->addStretch();
    hbox_buttons->addWidget(exportButton);
    vbox->addLayout(hbox_buttons);
    transactionsPage->setLayout(vbox);

    addressBookPage = new AddressBookPage(AddressBookPage::ForEditing, AddressBookPage::SendingTab);

    receiveCoinsPage = new AddressBookPage(AddressBookPage::ForEditing, AddressBookPage::ReceivingTab);

    sendCoinsPage = new SendCoinsDialog(gui);

    vanityGenPage = new VanityGenPage(this, gui);

    miningPage = new MiningPage(gui);
    miningInfoPage = new MiningInfoPage(gui);

    marketCapPage = new MarketCapPage(gui);

    zillionGridPage = new ZillionGridPage(gui);

    signVerifyMessageDialog = new SignVerifyMessageDialog(gui);

    addWidget(overviewPage);
    addWidget(transactionsPage);
    addWidget(addressBookPage);
    addWidget(receiveCoinsPage);
    addWidget(sendCoinsPage);
    addWidget(vanityGenPage);

    addWidget(miningPage);
    addWidget(miningInfoPage);

    addWidget(marketCapPage);

    addWidget(zillionGridPage);

    connect(marketCapPage, SIGNAL(sendMarketValues(QString,QString,int)), gui->guiHeader, SLOT(setMarketValues(QString,QString,int)));
    connect(marketCapPage, SIGNAL(sendTickerData(QList<QString>)), gui->tickerHeader, SLOT(setTickerData(QList<QString>)));

    connect(zillionGridPage, SIGNAL(sendSimpleGridData(QStringList)), gui->guiHeader, SLOT(sendSimpleGridData(QStringList)));

    connect(gui->guiHeader, SIGNAL(transactionClicked2(QModelIndex)), this, SLOT(gotoHistoryPage()));
    connect(gui->guiHeader, SIGNAL(transactionClicked2(QModelIndex)), this, SLOT(refreshHistoryPage()));
    connect(gui->guiHeader, SIGNAL(transactionClicked2(QModelIndex)), transactionView, SLOT(focusTransaction(QModelIndex)));

    // Double-clicking on a transaction on the transaction history page shows details
    connect(transactionView, SIGNAL(doubleClicked(QModelIndex)), transactionView, SLOT(showDetails()));

    // Clicking on "Send Coins" in the address book sends you to the send coins tab
    connect(addressBookPage, SIGNAL(sendCoins(QString)), this, SLOT(gotoSendCoinsPage(QString)));
    // Clicking on "Verify Message" in the address book opens the verify message tab in the Sign/Verify Message dialog
    connect(addressBookPage, SIGNAL(verifyMessage(QString)), this, SLOT(gotoVerifyMessageTab(QString)));
    // Clicking on "Sign Message" in the receive coins page opens the sign message tab in the Sign/Verify Message dialog
    connect(receiveCoinsPage, SIGNAL(signMessage(QString)), this, SLOT(gotoSignMessageTab(QString)));
    // Clicking on "Export" allows to export the transaction list
    connect(exportButton, SIGNAL(clicked()), transactionView, SLOT(exportClicked()));


    //gotoOverviewPage();
    gotoSendCoinsPage();
    //gotoMiningInfoPage();
}

WalletView::~WalletView()
{
}

void WalletView::setBitcoinGUI(BitcoinGUI *gui)
{
    this->gui = gui;
}

void WalletView::setClientModel(ClientModel *clientModel)
{
    this->clientModel = clientModel;
    if (clientModel)
    {

        gui->guiHeader->setClientModel(clientModel);
        addressBookPage->setOptionsModel(clientModel->getOptionsModel());
        receiveCoinsPage->setOptionsModel(clientModel->getOptionsModel());
    }
}

void WalletView::setWalletModel(WalletModel *walletModel)
{
    this->walletModel = walletModel;
    if (walletModel)
    {
        // Receive and report messages from wallet thread
        connect(walletModel, SIGNAL(message(QString,QString,unsigned int)), gui, SLOT(message(QString,QString,unsigned int)));

        // Put transaction list in tabs
        transactionView->setModel(walletModel);
        gui->guiHeader->setWalletModel(walletModel);
        addressBookPage->setModel(walletModel->getAddressTableModel());
        receiveCoinsPage->setModel(walletModel->getAddressTableModel());
        sendCoinsPage->setModel(walletModel);
        signVerifyMessageDialog->setModel(walletModel);
        miningPage->setModel(walletModel);

        vanityGenPage->setWalletModel(walletModel);

        //to be able to disable ticker through GUI
        gui->setWalletModel(walletModel);

        setEncryptionStatus();
        connect(walletModel, SIGNAL(encryptionStatusChanged(int)), gui, SLOT(setEncryptionStatus(int)));

        // Balloon pop-up for new transaction
        connect(walletModel->getTransactionTableModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(incomingTransaction(QModelIndex,int,int)));

        // Ask for passphrase if needed
        connect(walletModel, SIGNAL(requireUnlock()), this, SLOT(unlockWallet()));
        connect(walletModel, SIGNAL(requireUnlockIndefinite()), this, SLOT(unlockWalletIndefinite()));
    }
}

void WalletView::incomingTransaction(const QModelIndex& parent, int start, int /*end*/)
{
    // Prevent balloon-spam when initial block download is in progress
    if(!walletModel || !clientModel || clientModel->inInitialBlockDownload())
        return;

    TransactionTableModel *ttm = walletModel->getTransactionTableModel();

    QString date = ttm->index(start, TransactionTableModel::Date, parent).data().toString();
    qint64 amount = ttm->index(start, TransactionTableModel::Amount, parent).data(Qt::EditRole).toULongLong();
    QString type = ttm->index(start, TransactionTableModel::Type, parent).data().toString();
    QString address = ttm->index(start, TransactionTableModel::ToAddress, parent).data().toString();

    gui->incomingTransaction(date, walletModel->getOptionsModel()->getDisplayUnit(), amount, type, address);
}

void WalletView::gotoOverviewPage()
{
    setCurrentWidget(overviewPage);
}

void WalletView::gotoHistoryPage()
{
    screenId = 1;
    //gui->menuClicked(0,2);
    //gui->setWalletCategoryChecked(true);
    //gui->getHistoryButton()->setChecked(true);
    //gui->fadeWalletButtons("in");
    //gui->fadeMiningButtons("out");
    setCurrentWidget(transactionsPage);
    gui->stretchStack();

}

void WalletView::gotoVanityGenPage()
{
    //gui->setWalletCategoryChecked(true);
    //gui->getVanityGenButton()->setChecked(true);
    setCurrentWidget(vanityGenPage);
    //
}

void WalletView::gotoAddressBookPage()
{
    //gui->setWalletCategoryChecked(true);
    //gui->getAddressBookButton()->setChecked(true);
    setCurrentWidget(addressBookPage);
}

void WalletView::gotoReceiveCoinsPage()
{
    //gui->setWalletCategoryChecked(true);
    //gui->getReceiveCoinsButton()->setChecked(true);
    setCurrentWidget(receiveCoinsPage);
    receiveCoinsPage->setModel(walletModel->getAddressTableModel());
}

void WalletView::gotoSendCoinsPage(QString addr)
{
    //gui->setWalletCategoryChecked(true);
    //gui->getSendCoinsButton()->setChecked(true);
    setCurrentWidget(sendCoinsPage);

    if (!addr.isEmpty())
        sendCoinsPage->setAddress(addr);
}

void WalletView::gotoMiningPage()
{
    //gui->setMiningCategoryChecked(true);
    //gui->getMiningCPUButton()->setChecked(true);
    setCurrentWidget(miningPage);
}

void WalletView::gotoMiningInfoPage()
{
    //gui->setMiningCategoryChecked(true);
    //gui->getMiningInfoButton()->setChecked(true);
    setCurrentWidget(miningInfoPage);
}

void WalletView::gotoMarketCapPage()
{
    setCurrentWidget(marketCapPage);
}

void WalletView::gotoZillionGridPage()
{
    setCurrentWidget(zillionGridPage);
}

void WalletView::refreshHistoryPage()
{
    //gui->active_main_menu_old = 0;//active_main_menu
    gui->menuClicked(0,2);
}

void WalletView::gotoSignMessageTab(QString addr)
{
    // call show() in showTab_SM()
    signVerifyMessageDialog->showTab_SM(true);

    if (!addr.isEmpty())
        signVerifyMessageDialog->setAddress_SM(addr);
}

void WalletView::gotoVerifyMessageTab(QString addr)
{
    // call show() in showTab_VM()
    signVerifyMessageDialog->showTab_VM(true);

    if (!addr.isEmpty())
        signVerifyMessageDialog->setAddress_VM(addr);
}

bool WalletView::handleURI(const QString& strURI)
{
    // URI has to be valid
    if (sendCoinsPage->handleURI(strURI))
    {
        gotoSendCoinsPage();
        emit showNormalIfMinimized();
        return true;
    }
    else
        return false;
}

void WalletView::showOutOfSyncWarning(bool fShow)
{
    gui->guiHeader->showOutOfSyncWarning(fShow);
}

void WalletView::setEncryptionStatus()
{
    gui->setEncryptionStatus(walletModel->getEncryptionStatus());
}

void WalletView::encryptWallet(bool status)
{
    if(!walletModel)
        return;
    AskPassphraseDialog dlg(status ? AskPassphraseDialog::Encrypt : AskPassphraseDialog::Decrypt, this);
    dlg.setModel(walletModel);
    dlg.exec();

    setEncryptionStatus();
}

void WalletView::backupWallet()
{
#if QT_VERSION < 0x050000
    QString saveDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#else
    QString saveDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif
    QString filename = QFileDialog::getSaveFileName(this, tr("Backup Wallet"), saveDir, tr("Wallet Data (*.dat)"));
    if (!filename.isEmpty()) {
        if (!walletModel->backupWallet(filename)) {
            gui->message(tr("Backup Failed"), tr("There was an error trying to save the wallet data to the new location."),
                      CClientUIInterface::MSG_ERROR);
        }
        else
            gui->message(tr("Backup Successful"), tr("The wallet data was successfully saved to the new location."),
                      CClientUIInterface::MSG_INFORMATION);
    }
}

void WalletView::changePassphrase()
{
    AskPassphraseDialog dlg(AskPassphraseDialog::ChangePass, this);
    dlg.setModel(walletModel);
    dlg.exec();
}

void WalletView::unlockWallet()
{
    if(!walletModel)
        return;
    // Unlock wallet when requested by wallet model
    if (walletModel->getEncryptionStatus() == WalletModel::Locked)
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::Unlock, this);
        dlg.setModel(walletModel);
        dlg.exec();
    }
}

void WalletView::unlockWalletIndefinite()
{
    if(!walletModel)
        return;
    // Unlock wallet when requested by wallet model
    if (walletModel->getEncryptionStatus() == WalletModel::Locked)
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::UnlockIndefinite, this);
        dlg.setModel(walletModel);
        dlg.exec();
    }
}

void WalletView::updatePlot()
{
    miningInfoPage->updatePlot();
    gui->guiHeader->updateNetworkOverview();
}

