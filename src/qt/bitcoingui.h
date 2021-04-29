#ifndef BITCOINGUI_H
#define BITCOINGUI_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMap>
#include <QFrame>

class TransactionTableModel;
class WalletFrame;
class WalletView;
class ClientModel;
class WalletModel;
class WalletStack;
class TransactionView;
class OverviewPage;
class AddressBookPage;
class SendCoinsDialog;
class SignVerifyMessageDialog;
class Notificator;
class RPCConsole;
class BlockExplorer;
class MiningInfoPage;
class MiningPage;
class GuiHeader;
class TickerHeader;

class CWallet;

QT_BEGIN_NAMESPACE
class QLabel;
class QModelIndex;
class QProgressBar;
class QStackedWidget;
class QUrl;
class QListWidget;
class QPushButton;
class QAction;
class QToolButton;
class QTabWidget;
QT_END_NAMESPACE


#include "advancedwidget.h"


/**
  Bitcoin GUI main class. This class represents the main window of the Bitcoin UI. It communicates with both the client and
  wallet models to give the user an up-to-date view of the current core state.
*/
class BitcoinGUI : public QMainWindow
{
    Q_OBJECT

public:
    static const QString DEFAULT_WALLET;

    explicit BitcoinGUI(QWidget *parent = 0);
    ~BitcoinGUI();

    /** Set the client model.
        The client model represents the part of the core that communicates with the P2P network, and is wallet-agnostic.
    */
    void setClientModel(ClientModel *clientModel);
    /** Set the wallet model.
        The wallet model represents a bitcoin wallet, and offers access to the list of transactions, address book and sending
        functionality.
    */

    bool addWallet(const QString& name, WalletModel *walletModel);
    bool setCurrentWallet(const QString& name);

    void removeAllWallets();

    /** Used by WalletView to allow access to needed QActions */
    // Todo: Use Qt signals for these
    QAction * getOverviewAction() { return overviewAction; }
    QAction * getHistoryAction() { return historyAction; }
    QAction * getAddressBookAction() { return addressBookAction; }
    QAction * getReceiveCoinsAction() { return receiveCoinsAction; }
    QAction * getSendCoinsAction() { return sendCoinsAction; }

    QAction * getMiningInfoAction() { return miningInfoAction; }
    QAction * getMiningAction() { return miningAction; }

    /*QToolButton *getSendCoinsButton(){ return sendCoinsButton; }
    QToolButton *getReceiveCoinsButton(){ return receiveCoinsButton; }
    QToolButton *getHistoryButton(){ return historyButton; }
    QToolButton *getAddressBookButton(){ return addressBookButton; }
    QToolButton *getVanityGenButton(){ return vanityGenButton; }

    QToolButton *getMiningInfoButton(){ return miningInfoButton; }
    QToolButton *getMiningCPUButton(){ return miningCPUButton; }*/

    //

    QWidget *sideBarContainer;

    int active_main_menu = 0;
    int active_main_menu_old = 0;

    /*struct selected_menues{
        int menu;
        int sub_menu;
    };*/

    //selected_menues* selected;

    struct menu_descriptor{
        QString icon_path;
        QString title_text;
        QString tooltip_text;
    };

    QVector<menu_descriptor> mainMenuIdentifier;
    QVector<QVector<menu_descriptor> > subMenuIdentifier;

    QList<AdvancedWidget*> mainMenu;
    QList<QLabel*> mainMenuIconLabel;
    QList<QLabel*> mainMenuTextLabel;

    AdvancedWidget *subMenu[4][10];
    QLabel *subMenuIconLabel[4][10];
    QLabel *subMenuTextLabel[4][10];


    //

    QWidget *categoryContainer;

    QToolButton * overviewCategory;
    QToolButton * walletCategory;
    QToolButton * miningCategory;
    QToolButton * settingsCategory;

    //void setMiningCategoryChecked(bool);
    //void setWalletCategoryChecked(bool);

    QWidget *walletButtonContainer;
    QWidget *miningButtonContainer;

    GuiHeader *guiHeader;
    TickerHeader *tickerHeader;
    int ticker_enabled;

    /** FadeIn/Out Wallet Buttons */
    //void fadeWalletButtons(QString way);
    /** FadeIn/Out Mining Buttons */
    //void fadeMiningButtons(QString way);
    /** stretch main ui container depending im subcategories visible or not**/
    void stretchStack();

    /** receives from walletview.cpp **/
    void externCommand(const QString &command);

    QSystemTrayIcon *trayIcon;

    //for ticker disabling:
    void setWalletModel(WalletModel *model);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    ClientModel *clientModel;
    WalletFrame *walletFrame;

    //for ticker disabling:
    WalletModel *walletModel;

    //QWidget *zillionTabCarrier;
    QWidget *backGround;
    QTabWidget *zillionTab;

    QLabel *labelEncryptionIcon;
    QLabel *labelConnectionsIcon;
    QLabel *labelBlocksIcon;
    QLabel *progressBarLabel;
    QProgressBar *progressBar;

    QMenuBar *appMenuBar;
    QAction *overviewAction;
    QAction *historyAction;
    QAction *quitAction;

    QAction *sendCoinsAction;

    QAction *addressBookAction;
    QAction *signMessageAction;
    QAction *verifyMessageAction;
    QAction *aboutAction;
    QAction *receiveCoinsAction;
    QAction *optionsAction;
    QAction *toggleHideAction;
    QAction *encryptWalletAction;
    QAction *backupWalletAction;
    QAction *changePassphraseAction;
    QAction *aboutQtAction;
    QAction *openRPCConsoleAction;
    QAction *openInfoAction;
    QAction *openBlockExplorerAction;


    //QFrame *separatorLineLeft;
    //QFrame *separatorLineBottom;

    QToolButton *sendCoinsButton;
    QToolButton *receiveCoinsButton;
    QToolButton *historyButton;
    QToolButton *addressBookButton;
    QToolButton *vanityGenButton;
    QToolButton *backupButton;

    QAction *miningInfoAction;
    QAction *miningAction;

    QToolButton *miningInfoButton;
    QToolButton *miningCPUButton;

    Notificator *notificator;
    TransactionView *transactionView;
    RPCConsole *rpcConsole;
    BlockExplorer* blockExplorer;

    QMovie *syncIconMovie;
    /** Keep track of previous number of blocks, to detect progress */
    int prevBlocks;

    /** Create Header section that contains logo, wallet info, recent events and network */
    void createHeader();
    /** Create the main categories. */
    void createCategories();
    /** Create the main UI actions. */
    void createSideBar();
    /** Create the menu bar and sub-menus. */
    void createMenuBar();
    /** Create system tray icon and notification */
    void createTrayIcon();
    /** Create system tray menu (or setup the dock menu) */
    void createTrayIconMenu();
    /** Save window size and position */
    void saveWindowGeometry();
    /** Restore window size and position */
    void restoreWindowGeometry();
    /** Enable or disable all wallet-related actions */
    void setWalletActionsEnabled(bool enabled);

public slots:
    /** Set number of connections shown in the UI */
    void setNumConnections(int count);
    /** Set number of blocks shown in the UI */
    void setNumBlocks(int count, int nTotalBlocks);
    /** Set the encryption status as shown in the UI.
       @param[in] status            current encryption status
       @see WalletModel::EncryptionStatus
    */
    void setEncryptionStatus(int status);

    /** Notify the user of an event from the core network or transaction handling code.
       @param[in] title     the message box / notification title
       @param[in] message   the displayed text
       @param[in] style     modality and style definitions (icon and used buttons - buttons only for message boxes)
                            @see CClientUIInterface::MessageBoxFlags
       @param[in] ret       pointer to a bool that will be modified to whether Ok was clicked (modal only)
    */
    void message(const QString &title, const QString &message, unsigned int style, bool *ret = NULL);
    /** Asks the user whether to pay the transaction fee or to cancel the transaction.
       It is currently not possible to pass a return value to another thread through
       BlockingQueuedConnection, so an indirected pointer is used.
       https://bugreports.qt-project.org/browse/QTBUG-10440

      @param[in] nFeeRequired       the required fee
      @param[out] payFee            true to pay the fee, false to not pay the fee
    */
    void askFee(qint64 nFeeRequired, bool *payFee);
    void handleURI(QString strURI);

    /** Show incoming transaction notification for new transactions. */
    void incomingTransaction(const QString& date, int unit, qint64 amount, const QString& type, const QString& address);


    void menuClicked(int index, int index2);
    void menuRollover(int index, int index2);
    void menuRollout(int index, int index2);


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
    void gotoMiningInfoPage();
    /** Switch to mining page */
    void gotoMiningPage();

    /** Switch to marketcap page */
    void gotoMarketCapPage();

    /** Switch to zilliongrid page */
    void gotoZillionGridPage();

    /** Show configuration dialog */
    void optionsClicked();

    void whichTabWasClicked(int);

    //to disable ticker:
    void displayTickerChanged(bool b);

private slots:

    /** Show Sign/Verify Message dialog and switch to sign message tab */
    void gotoSignMessageTab(QString addr = "");
    /** Show Sign/Verify Message dialog and switch to verify message tab */
    void gotoVerifyMessageTab(QString addr = "");

    /** Show about dialog */
    void aboutClicked();
#ifndef Q_OS_MAC
    /** Handle tray icon clicked */
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
#endif

    /** Show window if hidden, unminimize when minimized, rise when obscured or show if hidden and fToggleHidden is true */
    void showNormalIfMinimized(bool fToggleHidden = false);
    /** Simply calls showNormalIfMinimized(true) for use in SLOT() macro */
    void toggleHidden();

    /** called by a timer to check if fRequestShutdown has been set **/
    void detectShutdown();
};

#endif // BITCOINGUI_H
