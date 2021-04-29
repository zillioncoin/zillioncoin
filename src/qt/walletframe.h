/*
 * Qt4 bitcoin GUI.
 *
 * W.J. van der Laan 2011-2012
 * The Bitcoin Developers 2011-2013
 */
#ifndef WALLETFRAME_H
#define WALLETFRAME_H

#include <QFrame>
#include <QTabWidget>

class BitcoinGUI;
class ClientModel;
class WalletModel;
class WalletStack;
class WalletView;

class WalletFrame : public QFrame
{
    Q_OBJECT

public:
    explicit WalletFrame(BitcoinGUI *_gui = 0);
    ~WalletFrame();

    void setClientModel(ClientModel *clientModel);

    bool addWallet(const QString& name, WalletModel *walletModel);
    bool setCurrentWallet(const QString& name);

    void removeAllWallets();

    bool handleURI(const QString &uri);

    void showOutOfSyncWarning(bool fShow);

    void resizeIt();

    void stretchStack(int x, int y, int width, int height);
private:
    BitcoinGUI *gui;
    ClientModel *clientModel;
    WalletStack *walletStack;

    //QTabWidget *test;
    WalletView *currentWalletView();

public slots:
    /** Switch to overview (home) page */
    void gotoOverviewPage();
    /** Switch to history (transactions) page */
    void gotoHistoryPage();
    /** Switch to address book page */
    void gotoAddressBookPage();
    /** Switch to Vanity Gen page */
    void gotoVanityGenPage();
    /** Switch to receive coins page */
    void gotoReceiveCoinsPage();
    /** Switch to send coins page */
    void gotoSendCoinsPage(QString addr = "");
    /** Switch to mining page */
    void gotoMiningPage();
    /** Switch to mining info page */
    void gotoMiningInfoPage();

    /** Switch to market cap page */
    void gotoMarketCapPage();

    /** Switch to zilliongrid page */
    void gotoZillionGridPage();

    /** Show Sign/Verify Message dialog and switch to sign message tab */
    void gotoSignMessageTab(QString addr = "");
    /** Show Sign/Verify Message dialog and switch to verify message tab */
    void gotoVerifyMessageTab(QString addr = "");

    /** Encrypt the wallet */
    void encryptWallet(bool status);
    /** Backup the wallet */
    void backupWallet();
    /** Change encrypted wallet passphrase */
    void changePassphrase();
    /** Ask for passphrase to unlock wallet temporarily */
    void unlockWallet();

    /** Set the encryption status as shown in the UI.
     @param[in] status            current encryption status
     @see WalletModel::EncryptionStatus
     */
    void setEncryptionStatus();

    /** Update the plot on the overview (home) page */
    void updatePlot();

    /** */
    void resizeWhenFinished();
};

#endif // WALLETFRAME_H
