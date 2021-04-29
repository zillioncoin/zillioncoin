/*
 * Qt4 bitcoin GUI.
 *
 * W.J. van der Laan 2011-2012
 * The Bitcoin Developers 2011-2012
 */

#include <QApplication>
#include <QDebug>

#include "bitcoingui.h"

#include "transactiontablemodel.h"
#include "optionsdialog.h"
#include "aboutdialog.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "walletframe.h"
#include "optionsmodel.h"
#include "transactiondescdialog.h"
#include "bitcoinunits.h"
#include "guiconstants.h"
#include "notificator.h"
#include "guiutil.h"
#include "rpcconsole.h"
#include "blockexplorer.h"
#include "ui_interface.h"
#include "wallet.h"
#include "init.h"
#include "guiheader.h"
#include "tickerheader.h"

#include "qgraphicstinteffect.h"
#include "pixmapeffect.h"

#ifdef Q_OS_MAC
#include "macdockiconhandler.h"
#endif

#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QVBoxLayout>
#include <QToolBar>

#include <QToolButton>

#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QStackedWidget>
#include <QDateTime>
#include <QMovie>
#include <QTimer>
#include <QDragEnterEvent>

#if QT_VERSION < 0x050000
#include <QUrl>
#endif
#include <QMimeData>
#include <QStyle>
#include <QSettings>
#include <QDesktopWidget>
#include <QListWidget>
#include <QPropertyAnimation>

#include <iostream>

#include <QFontDatabase>

int MENU_OFFSET;

const QString BitcoinGUI::DEFAULT_WALLET = "~Default";

BitcoinGUI::BitcoinGUI(QWidget *parent) :
    QMainWindow(parent),
    clientModel(0),
    encryptWalletAction(0),
    changePassphraseAction(0),
    aboutQtAction(0),
    trayIcon(0),
    notificator(0),
    rpcConsole(0),
    blockExplorer(0),
    prevBlocks(0),
    ticker_enabled(-1)
{
    restoreWindowGeometry();
    setWindowTitle(tr("ZillionCoin") + " - " + tr("Wallet"));
#ifndef Q_OS_MAC
    QApplication::setWindowIcon(QIcon(":icons/bitcoin"));
    setWindowIcon(QIcon(":icons/bitcoin"));
#else
    setUnifiedTitleAndToolBarOnMac(true);
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

    int id = QFontDatabase::addApplicationFont(":/font/opensans-regular");
    qDebug() << "id" << id << QFontDatabase::applicationFontFamilies(id).at(0);
    id = QFontDatabase::addApplicationFont(":/font/opensans-semibold");
    qDebug() << "id" << id << QFontDatabase::applicationFontFamilies(id).at(0);
    id = QFontDatabase::addApplicationFont(":/font/opensans-bold");
    qDebug() << "id" << id << QFontDatabase::applicationFontFamilies(id).at(0);


    //    QFontDatabase::addApplicationFontFromData(":/font/OpenSans-Regular.ttf");
    //    QFontDatabase::addApplicationFontFromData(":/font/OpenSans-Semibold.ttf");
    //    QFontDatabase::addApplicationFontFromData(":/font/OpenSans-Bold.ttf");



    /*int id = QFontDatabase::addApplicationFont(":/fonts/monospace.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont monospace(family);
    */


    /*zillionTabCarrier = new QWidget(this);

    zillionTabCarrier->setFixedSize(1000,800);
*/

    //QCheckBox { font: bold italic large "Times New Roman" }

    setStyleSheet("QPushButton,QDialogButtonBox::Ok{ "
                  //"background-color: rgba(250,250,250, 128);"
                  "height:40px;"
                  "background-color: #fff4f4f4;"
                  "border-style: solid;"
                  "border-width: 1px;"
                  "border-color: #ffb4b4b4;"
                  "font: bold 16px \"Open Sans Semibold\";"
                  //"font-weight: 00;"
                  "margin: 0px;"
                  "padding-left:10px;"
                  "padding-right:10px;"
                  "}"

                  "QPushButton:pressed { "
                  "height:40px;"
                  "background-color: #ffdddddd;"
                  "border-style: none;"
                  "font: bold 14px;"
                  "padding: 12px;}"

                  /*"background-color: red;"
                                "border-style: outset;"
                                "border-width: 2px;"
                                "border-radius: 10px;"
                                "border-color: beige;"
                                "font: bold 14px;"
                                "padding: 6px;}"*/

                  "QLabel{font: normal 13px \"Open Sans\";}"

                  "QLineEdit{height:35px;border: none; padding-left:5px;font: normal 13px \"Open Sans\";}"
                  "QComboBox{height:35px;border: none; padding-left:5px;font: normal 13px \"Open Sans\";}"
                  "QDoubleSpinBox{height:35px;border: none; padding-left:5px;}"

                  //QLineEdit, QLineEdit:focus {  }

                  "QToolButton{ "
                                    //"background-color: rgba(250,250,250, 128);"
                                    "height:33px;"
                                    "background-color: #fff4f4f4;"
                                    "border-style: solid;"
                                    "border-width: 1px;"
                                    "border-color: #ffb4b4b4;"
                                    "font: bold 16px \"Open Sans Semibold\";"
                                    //"font-weight: 00;"
                                    "margin: 0px;"
                                    "padding-left:10px;"
                                    "padding-right:10px;"
                                    "}"

                  );

    //"min-width: 10em;"

    backGround = new QWidget(this);
    backGround->setGeometry(0,0,0,0);
    //backGround->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #6c3d94, stop: 1 #a13469)");
    backGround->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 1, x2: 1, y2: 0,"
                              //" stop: 0 #b4b8ff, stop: 0.1 #b4b8ff, stop: 0.3 #c3a0f2, stop: 0.5 #fae5ea, stop: 0.7 #ffd3e4, stop: 0.9 #fca4c6, stop: 1 #fca4c6)");
                              //" stop: 0 #FFFFFF, stop: 0.1 #FFFFD5, stop: 0.3 #FFFAAC, stop: 0.5 #F6E093, stop: 0.7 #EDC67A, stop: 0.9 #C4A264, stop: 1 #C4A264)");
                              //" stop: 0 #8B7641, stop: 0.1 #95954F, stop: 0.3 #D3C378, stop: 0.5 #D3C378, stop: 0.7 #95954F, stop: 0.9 #4A3B24, stop: 1 #493E20)");
                              //" stop: 0 #f5f6f6, stop: 0.1 #f5f6f6, stop: 0.3 #dbdce2, stop: 0.5 #b8bac6, stop: 0.7 #dddfe3, stop: 0.9 #f5f6f6, stop: 1 #f5f6f6)");
                              " stop: 0 #b8bac6, stop: 0.1 #b8bac6, stop: 0.3 #dbdce2, stop: 0.5 #f5f6f6, stop: 0.7 #dddfe3, stop: 0.9 #b8bac6, stop: 1 #b8bac6)");


    //#b8bac6
    //dbdce2
    //fae5ea
    //dddfe3
    //fca4c6

    zillionTab = new QTabWidget(this);
    //zillionTab->addTab(new QWidget(), QIcon(":/icons/wallet2"), "   Wallet");
    //zillionTab->addTab(new QWidget(), QIcon(":/icons/mining2"), "   Mining");
    //zillionTab->setIconSize(QSize(32,32));
    zillionTab->setStyleSheet("QTabBar::tab { font-weight:bold; width:105px; }");

    connect(zillionTab, SIGNAL(tabBarClicked(int)), this, SLOT(whichTabWasClicked(int)));

    zillionTab->setVisible(false);



    // Create wallet frame and make it the central widget
    walletFrame = new WalletFrame(this);
    //setCentralWidget(walletFrame);
    walletFrame->setMinimumWidth(800);
    walletFrame->setMinimumHeight(500);
    //walletFrame->setStyleSheet("WalletFrame#walletFrame{background-color:white;}");

    //setStyleSheet("background-color:white;");


    // Accept D&D of URIs
    setAcceptDrops(true);

    // Create application menu bar
    createMenuBar();

    //Header section that contains logo, wallet info, recent events and network
    createHeader();

    // Create main categories on the left side.
    createCategories();

    // Create actions for the toolbar, menu bar and tray/dock icon
    // Needs walletFrame to be initialized

    sideBarContainer = new QWidget(this);
    sideBarContainer->setMinimumWidth(220);
    sideBarContainer->setMinimumHeight(500);

    createSideBar();

    guiHeader->raise();
    tickerHeader->raise();

    // Create system tray icon and notification
    createTrayIcon();

    // Create status bar
    statusBar();

    //statusBar()->->setStyleSheet("background-color:#4dffffff;");

    // Status bar notification icons
    QFrame *frameBlocks = new QFrame();
    frameBlocks->setContentsMargins(0,0,0,0);
    frameBlocks->setMinimumWidth(56);
    frameBlocks->setMaximumWidth(56);
    QHBoxLayout *frameBlocksLayout = new QHBoxLayout(frameBlocks);
    frameBlocksLayout->setContentsMargins(3,0,3,0);
    frameBlocksLayout->setSpacing(3);
    labelEncryptionIcon = new QLabel();
    labelConnectionsIcon = new QLabel();
    labelBlocksIcon = new QLabel();
    frameBlocksLayout->addStretch();
    frameBlocksLayout->addWidget(labelEncryptionIcon);
    frameBlocksLayout->addStretch();
    frameBlocksLayout->addWidget(labelConnectionsIcon);
    frameBlocksLayout->addStretch();
    frameBlocksLayout->addWidget(labelBlocksIcon);
    frameBlocksLayout->addStretch();

    // Progress bar and label for blocks download
    progressBarLabel = new QLabel();
    progressBarLabel->setVisible(false);
    progressBar = new QProgressBar();
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setVisible(false);

    // Override style sheet for progress bar for styles that have a segmented progress bar,
    // as they make the text unreadable (workaround for issue #1071)
    // See https://qt-project.org/doc/qt-4.8/gallery.html
    QString curStyle = QApplication::style()->metaObject()->className();
    if(curStyle == "QWindowsStyle" || curStyle == "QWindowsXPStyle")
    {
        progressBar->setStyleSheet("QProgressBar { background-color: #e8e8e8; border: 1px solid grey; border-radius: 7px; padding: 1px; text-align: center; } QProgressBar::chunk { background: QLinearGradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #FF8000, stop: 1 orange); border-radius: 7px; margin: 0px; }");
    }

    statusBar()->addWidget(progressBarLabel);
    statusBar()->addWidget(progressBar);
    statusBar()->addPermanentWidget(frameBlocks);

    syncIconMovie = new QMovie(":/movies/update_spinner", "mng", this);

    rpcConsole = new RPCConsole(this);
    connect(openRPCConsoleAction, SIGNAL(triggered()), rpcConsole, SLOT(showConsole()));
    connect(openInfoAction, SIGNAL(triggered()), rpcConsole, SLOT(showInfo()));

    blockExplorer = new BlockExplorer(this);
    connect(openBlockExplorerAction, SIGNAL(triggered()), blockExplorer, SLOT(show()));

    // Install event filter to be able to catch status tip events (QEvent::StatusTip)
    this->installEventFilter(this);

    // Initially wallet actions should be disabled
    setWalletActionsEnabled(false);


    //zillionTab->move(4,4+120+20+MENU_OFFSET);
    //walletFrame->move(180+4,4+160+20+MENU_OFFSET);//+90);
    //sideBarContainer->move(0,-10+4+160+20+MENU_OFFSET);//+90);
}

BitcoinGUI::~BitcoinGUI()
{
    saveWindowGeometry();
    if(trayIcon) // Hide tray icon, as deleting will let it linger until quit (on Ubuntu)
        trayIcon->hide();
#ifdef Q_OS_MAC
    delete appMenuBar;
    MacDockIconHandler::instance()->setMainWindow(NULL);
#endif
}

void BitcoinGUI::createHeader()
{
    guiHeader = new GuiHeader(this);
    guiHeader->move(0,20+MENU_OFFSET);

    tickerHeader = new TickerHeader(this);
    tickerHeader->move(0,MENU_OFFSET);

}

void BitcoinGUI::createCategories()
{

}

void BitcoinGUI::createSideBar()
{

    for(int i = 0;i<4;i++){
        QVector<menu_descriptor> tempVector;
        subMenuIdentifier.append(tempVector);
    }

    mainMenuIdentifier.append({":/icons/wallet","WALLET"});
    mainMenuIdentifier.append({":/icons/mining","MINING"});
    mainMenuIdentifier.append({":/icons/market","MARKET"});
    mainMenuIdentifier.append({":/icons/grid","GRID"});

    subMenuIdentifier[0].append({":/icons/send","SEND COINS",tr("Send coins to a ZillionCoin address")});
    subMenuIdentifier[0].append({":/icons/receive","RECEIVE COINS",tr("Show the list of addresses for receiving payments")});
    subMenuIdentifier[0].append({":/icons/transactions","TRANSACTIONS",tr("Browse transaction history")});
    subMenuIdentifier[0].append({":/icons/addressbook","ADDRESS BOOK",tr("Edit the list of stored addresses and labels")});
    subMenuIdentifier[0].append({":/icons/vanitygen","VANITY GEN",tr("Create your own vanity addresses")});
    subMenuIdentifier[0].append({":/icons/backup","BACKUP WALLET",tr("Quickly save a backup file of your wallet")});

    subMenuIdentifier[1].append({":/icons/mininginfo","MINING INFO",tr("Mining Information")});
    subMenuIdentifier[1].append({":/icons/cpu","CPU MINER",tr("Mine ZillionCoins with your CPU")});

    subMenuIdentifier[2].append({":/icons/marketcap","MARKETCAP"});
    subMenuIdentifier[3].append({":/icons/grid","WORLDMAP"});


    int offset_y = 10;//4+160+MENU_OFFSET;

    int offset_y_orig = offset_y;

    QPixmap pix;

    for(int i = 0;i<mainMenuIdentifier.length();i++){

        mainMenu.append(new AdvancedWidget(sideBarContainer));
        mainMenu[i]->setProperty("index", i);
        mainMenu[i]->setProperty("index2", -1);
        mainMenu[i]->resize(180,50);
        mainMenu[i]->setColor(0xda8d21);
        mainMenu[i]->setChecked(true);
        mainMenu[i]->move(0,offset_y);

        connect(mainMenu[i],SIGNAL(released(int,int)),this,SLOT(menuClicked(int,int)));

        mainMenuIconLabel.append(new QLabel(mainMenu[i]));
        pix = QPixmap(mainMenuIdentifier[i].icon_path);
        pix = PixmapEffect::tint(pix,0xFFFFFF);
        mainMenuIconLabel[i]->setPixmap( pix.scaled(32,32,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        mainMenuIconLabel[i]->setAttribute( Qt::WA_TransparentForMouseEvents );
        mainMenuIconLabel[i]->adjustSize();
        mainMenuIconLabel[i]->move(15,9);

        mainMenuTextLabel.append(new QLabel(mainMenu[i]));
        mainMenuTextLabel[i]->setFixedHeight(50);
        mainMenuTextLabel[i]->move(60,0);
        mainMenuTextLabel[i]->setContentsMargins(0,0,0,0);
        mainMenuTextLabel[i]->setAttribute( Qt::WA_TransparentForMouseEvents );

        mainMenuTextLabel[i]->setStyleSheet(".QLabel{color:#FFFFFF; font: bold 22px \"Open Sans\";}");
        mainMenuTextLabel[i]->setText(mainMenuIdentifier[i].title_text);
        mainMenuTextLabel[i]->setAlignment(Qt::AlignVCenter);

        offset_y += 50 + 10;

        for(int i2 = 0; i2<subMenuIdentifier[i].length();i2++){
            subMenu[i][i2] = new AdvancedWidget(sideBarContainer);
            subMenu[i][i2]->setProperty("index", i);
            subMenu[i][i2]->setProperty("index2", i2);
            subMenu[i][i2]->resize(180,35);

            subMenu[i][i2]->setColor(0xFFFFFF);
            subMenu[i][i2]->setChecked(false);

            subMenu[i][i2]->fadeTo(0,0,0);

            if(i == active_main_menu){
                subMenu[i][i2]->move(0,offset_y_orig+60+i*60+i2*35);
            } else{
                subMenu[i][i2]->move(-200,offset_y_orig+60+i*60+i2*35);
            }

            subMenu[i][i2]->setStatusTip(subMenuIdentifier[i][i2].tooltip_text);//;
            subMenu[i][i2]->setToolTip(subMenu[i][i2]->statusTip());

            connect(subMenu[i][i2],SIGNAL(released(int,int)),this,SLOT(menuClicked(int,int)));
            connect(subMenu[i][i2],SIGNAL(rollover(int,int)),this,SLOT(menuRollover(int,int)));
            connect(subMenu[i][i2],SIGNAL(rollout(int,int)),this,SLOT(menuRollout(int,int)));

            subMenuIconLabel[i][i2] = new QLabel(subMenu[i][i2]);
            pix = QPixmap(subMenuIdentifier[i][i2].icon_path);
            pix = PixmapEffect::tint(pix,0x1F1F1F);
            subMenuIconLabel[i][i2]->setPixmap( pix.scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            subMenuIconLabel[i][i2]->setAttribute( Qt::WA_TransparentForMouseEvents );
            subMenuIconLabel[i][i2]->adjustSize();
            subMenuIconLabel[i][i2]->move(21,10);

            subMenuTextLabel[i][i2] = new QLabel(subMenu[i][i2]);
            subMenuTextLabel[i][i2]->setFixedHeight(35);
            subMenuTextLabel[i][i2]->move(50,0);
            subMenuTextLabel[i][i2]->setContentsMargins(0,0,0,0);
            subMenuTextLabel[i][i2]->setAttribute( Qt::WA_TransparentForMouseEvents );

            subMenuTextLabel[i][i2]->setText(subMenuIdentifier[i][i2].title_text);
            subMenuTextLabel[i][i2]->setStyleSheet(".QLabel{color:#1F1F1F; font: bold 14px \"Open Sans\";}");
            subMenuTextLabel[i][i2]->setAlignment(Qt::AlignVCenter);

            if(i == active_main_menu){
                offset_y += 35;
            }
        }

        if(i == active_main_menu){
            if(subMenuIdentifier[i].length() >= 1){
                offset_y += 10;
            }
        }
    }

    //subMenu[1][0]->setColor(0xFF00FF);
    //subMenu[2][0]->setColor(0xFFFF00);

    //subMenu[0][0]->setStatusTip(tr("Send coins to a ZillionCoin address"));
    //subMenu[0][0]->setToolTip(subMenu[0][0]->statusTip());

    for(int i = 0;i<mainMenuIdentifier.length();i++){
        mainMenu[i]->raise();
        mainMenuTextLabel[i]->adjustSize();
        for(int i2 = 0;i2<subMenuIdentifier[i].length();i2++){
            subMenuTextLabel[i][i2]->adjustSize();
        }
    }

    menuClicked(0,0);




    //
    //
    //




    /*QString styleSheetToolButtonNormal = "QToolButton{ padding-top:0px; background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fefbf6, stop: 1 #d7d5d0); border: 1px solid #98958f; border-radius: 5px;}";
    QString styleSheetToolButtonHover = "QToolButton:hover,QToolButton:checked{ padding-top:0px; background-color: #FFFFFF; border: 1px solid #98958f; border-radius: 5px;}";
    QString styleSheetToolButtonChecked = "QToolButton:checked{color:#000000; background-color: #C3C0BB; border: 1px solid #3F3F3F;}";

    walletButtonContainer = new QWidget(this);
    walletButtonContainer->setContentsMargins(0,0,0,0);
    walletButtonContainer->move(10,125+MENU_OFFSET+45+200);

    QVBoxLayout *walletButtonLayout = new QVBoxLayout();

    QSizePolicy sizePolicyWallet(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicyWallet.setHorizontalStretch(0);
    sizePolicyWallet.setVerticalStretch(0);

    sendCoinsButton = new QToolButton(walletButtonContainer);
    sendCoinsButton->setStatusTip(tr("Send coins to a ZillionCoin address"));
    sendCoinsButton->setToolTip(sendCoinsButton->statusTip());
    sendCoinsButton->setCheckable(true);
    sendCoinsButton->setShortcut(QKeySequence(Qt::ALT + Qt::Key_2));
    sizePolicyWallet.setHeightForWidth(sendCoinsButton->sizePolicy().hasHeightForWidth());
    sendCoinsButton->setSizePolicy(sizePolicyWallet);
    sendCoinsButton->setAutoExclusive(true);
    sendCoinsButton->setCheckable(true);

    receiveCoinsButton = new QToolButton(walletButtonContainer);
    receiveCoinsButton->setStatusTip(tr("Show the list of addresses for receiving payments"));
    receiveCoinsButton->setToolTip(receiveCoinsButton->statusTip());
    receiveCoinsButton->setCheckable(true);
    receiveCoinsButton->setShortcut(QKeySequence(Qt::ALT + Qt::Key_3));
    sizePolicyWallet.setHeightForWidth(receiveCoinsButton->sizePolicy().hasHeightForWidth());
    receiveCoinsButton->setSizePolicy(sizePolicyWallet);
    receiveCoinsButton->setAutoExclusive(true);
    receiveCoinsButton->setCheckable(true);

    historyButton = new QToolButton(walletButtonContainer);
    historyButton->setStatusTip(tr("Browse transaction history"));
    historyButton->setToolTip(historyButton->statusTip());
    historyButton->setCheckable(true);
    historyButton->setShortcut(QKeySequence(Qt::ALT + Qt::Key_4));
    sizePolicyWallet.setHeightForWidth(historyButton->sizePolicy().hasHeightForWidth());
    historyButton->setSizePolicy(sizePolicyWallet);
    historyButton->setAutoExclusive(true);
    historyButton->setCheckable(true);

    addressBookButton = new QToolButton(walletButtonContainer);
    addressBookButton->setStatusTip(tr("Edit the list of stored addresses and labels"));
    addressBookButton->setToolTip(addressBookButton->statusTip());
    addressBookButton->setCheckable(true);
    addressBookButton->setShortcut(QKeySequence(Qt::ALT + Qt::Key_5));
    sizePolicyWallet.setHeightForWidth(addressBookButton->sizePolicy().hasHeightForWidth());
    addressBookButton->setSizePolicy(sizePolicyWallet);
    addressBookButton->setAutoExclusive(true);
    addressBookButton->setCheckable(true);

    vanityGenButton = new QToolButton(walletButtonContainer);
    vanityGenButton->setStatusTip(tr("Create your own vanity addresses"));
    vanityGenButton->setToolTip(vanityGenButton->statusTip());
    sizePolicyWallet.setHorizontalStretch(0);
    sizePolicyWallet.setHeightForWidth(vanityGenButton->sizePolicy().hasHeightForWidth());
    vanityGenButton->setSizePolicy(sizePolicyWallet);
    vanityGenButton->setAutoExclusive(true);
    vanityGenButton->setCheckable(true);

    backupButton = new QToolButton(walletButtonContainer);
    backupButton->setStatusTip(tr("Edit the list of stored addresses and labels"));
    backupButton->setToolTip(backupButton->statusTip());
    sizePolicyWallet.setHorizontalStretch(0);
    sizePolicyWallet.setHeightForWidth(backupButton->sizePolicy().hasHeightForWidth());
    backupButton->setSizePolicy(sizePolicyWallet);

    sendCoinsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    sendCoinsButton->setIconSize(QSize(24,24));
    sendCoinsButton->setCursor(Qt::PointingHandCursor);
    sendCoinsButton->setIcon(QIcon(":/icons/send"));

    receiveCoinsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    receiveCoinsButton->setIconSize(QSize(24,24));
    receiveCoinsButton->setCursor(Qt::PointingHandCursor);
    //receiveCoinsButton->setIcon(QIcon(":/icons/receiving_addresses"));

    historyButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    historyButton->setIconSize(QSize(24,24));
    historyButton->setCursor(Qt::PointingHandCursor);
    historyButton->setIcon(QIcon(":/icons/history"));

    addressBookButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addressBookButton->setIconSize(QSize(24,24));
    addressBookButton->setCursor(Qt::PointingHandCursor);
    addressBookButton->setIcon(QIcon(":/icons/address-book"));

    vanityGenButton->setCursor(Qt::PointingHandCursor);

    backupButton->setCursor(Qt::PointingHandCursor);

    walletButtonLayout->addWidget(sendCoinsButton);
    walletButtonLayout->addWidget(receiveCoinsButton);

    walletButtonLayout->addWidget(historyButton);
    walletButtonLayout->addWidget(addressBookButton);

    walletButtonLayout->addWidget(vanityGenButton);
    walletButtonLayout->setContentsMargins(0, 0, 0, 0);
    walletButtonLayout->addStretch();

    walletButtonLayout->addWidget(backupButton,Qt::AlignLeft);
    walletButtonLayout->setContentsMargins(0, 0, 0, 0);
    walletButtonLayout->addStretch();

    walletButtonLayout->addSpacing(1000);//addWidget(line2c,Qt::AlignLeft);

    walletButtonContainer->setFixedWidth(120);
    walletButtonContainer->setFixedHeight(300);
    walletButtonContainer->setLayout(walletButtonLayout);

    sendCoinsButton->setText(" Send coins");
    receiveCoinsButton->setText(" Receive coins");
    historyButton->setText(" Transactions");
    addressBookButton->setText(" Address Book");
    vanityGenButton->setText(" Vanity Gen");
    backupButton->setText(" Backup wallet");

    sendCoinsButton->setStyleSheet(styleSheetToolButtonNormal+styleSheetToolButtonHover+styleSheetToolButtonChecked);
    receiveCoinsButton->setStyleSheet(styleSheetToolButtonNormal+styleSheetToolButtonHover+styleSheetToolButtonChecked);
    historyButton->setStyleSheet(styleSheetToolButtonNormal+styleSheetToolButtonHover+styleSheetToolButtonChecked);
    addressBookButton->setStyleSheet(styleSheetToolButtonNormal+styleSheetToolButtonHover+styleSheetToolButtonChecked);
    vanityGenButton->setStyleSheet(styleSheetToolButtonNormal+styleSheetToolButtonHover+styleSheetToolButtonChecked);
    backupButton->setStyleSheet(styleSheetToolButtonNormal+styleSheetToolButtonHover+styleSheetToolButtonChecked);

    connect(sendCoinsButton, SIGNAL(clicked()), this, SLOT(showNormalIfMinimized()));
    connect(sendCoinsButton, SIGNAL(clicked()), this, SLOT(gotoSendCoinsPage()));
    connect(receiveCoinsButton, SIGNAL(clicked()), this, SLOT(showNormalIfMinimized()));
    connect(receiveCoinsButton, SIGNAL(clicked()), this, SLOT(gotoReceiveCoinsPage()));
    connect(historyButton, SIGNAL(clicked()), this, SLOT(showNormalIfMinimized()));
    connect(historyButton, SIGNAL(clicked()), this, SLOT(gotoHistoryPage()));
    connect(addressBookButton, SIGNAL(clicked()), this, SLOT(showNormalIfMinimized()));
    connect(addressBookButton, SIGNAL(clicked()), this, SLOT(gotoAddressBookPage()));
    connect(vanityGenButton, SIGNAL(clicked()), this, SLOT(showNormalIfMinimized()));
    connect(vanityGenButton, SIGNAL(clicked()), this, SLOT(gotoVanityGenPage()));
    connect(backupButton, SIGNAL(clicked()), walletFrame, SLOT(backupWallet()));

    ////////////////////////////////////////////////////////////////

    miningButtonContainer = new QWidget(this);
    miningButtonContainer->setContentsMargins(0,0,0,0);
    miningButtonContainer->move(10,125+300+MENU_OFFSET+160);

    QVBoxLayout *miningButtonLayout = new QVBoxLayout();

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    miningInfoButton = new QToolButton(miningButtonContainer);
    miningInfoButton->setStatusTip(tr("Mining Information"));
    miningInfoButton->setToolTip(miningInfoButton->statusTip());
    miningInfoButton->setCheckable(true);
    //miningInfoButton->setShortcut(QKeySequence(Qt::ALT + Qt::Key_2));
    sizePolicy.setHeightForWidth(miningInfoButton->sizePolicy().hasHeightForWidth());
    miningInfoButton->setSizePolicy(sizePolicy);
    miningInfoButton->setAutoExclusive(true);
    miningInfoButton->setCheckable(true);


    miningCPUButton = new QToolButton(miningButtonContainer);
    miningCPUButton->setStatusTip(tr("Mine ZillionCoins with your CPU"));
    miningCPUButton->setToolTip(miningCPUButton->statusTip());
    miningCPUButton->setCheckable(true);
    //miningCPUButton->setShortcut(QKeySequence(Qt::ALT + Qt::Key_3));
    sizePolicy.setHeightForWidth(miningCPUButton->sizePolicy().hasHeightForWidth());
    miningCPUButton->setSizePolicy(sizePolicy);
    miningCPUButton->setAutoExclusive(true);
    miningCPUButton->setCheckable(true);

    miningInfoButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    miningInfoButton->setIconSize(QSize(24,24));
    miningInfoButton->setCursor(Qt::PointingHandCursor);
    miningInfoButton->setIcon(QIcon(":/icons/tx_mined"));

    miningCPUButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    miningCPUButton->setIconSize(QSize(24,24));
    miningCPUButton->setCursor(Qt::PointingHandCursor);
    miningCPUButton->setIcon(QIcon(":/icons/tx_mined"));


    miningButtonLayout->addWidget(miningInfoButton);
    miningButtonLayout->addWidget(miningCPUButton);
    miningButtonLayout->addWidget(miningCPUButton);
    miningButtonLayout->addWidget(miningCPUButton);
    miningButtonLayout->addWidget(miningCPUButton);

    miningButtonLayout->setContentsMargins(0, 0, 0, 0);
    miningButtonLayout->addStretch();

    miningButtonContainer->setFixedWidth(120);
    miningButtonContainer->setFixedHeight(120);
    miningButtonContainer->adjustSize();
    miningButtonContainer->setLayout(miningButtonLayout);

    miningInfoButton->setText(" Mining Info");
    miningCPUButton->setText(" CPU Miner");

    miningInfoButton->setStyleSheet(styleSheetToolButtonNormal+styleSheetToolButtonHover+styleSheetToolButtonChecked);
    miningCPUButton->setStyleSheet(styleSheetToolButtonNormal+styleSheetToolButtonHover+styleSheetToolButtonChecked);

    connect(miningInfoButton, SIGNAL(clicked()), this, SLOT(gotoMiningInfoPage()));
    connect(miningCPUButton, SIGNAL(clicked()), this, SLOT(gotoMiningPage()));

    gotoSendCoinsPage();
    */
    //gotoMiningInfoPage();
}

void BitcoinGUI::createMenuBar()
{
#ifdef Q_OS_MAC
    // Create a decoupled menu bar on Mac which stays even if the window is closed
    appMenuBar = new QMenuBar();
#else
    // Get the main window's menu bar on other platforms
    appMenuBar = menuBar();
#endif

    appMenuBar->isNativeMenuBar() ? MENU_OFFSET = 0 : MENU_OFFSET = 18;

    appMenuBar->raise();

    quitAction = new QAction(QIcon(":/menu/quit"), tr("E&xit"), this);
    quitAction->setStatusTip(tr("Quit application"));
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    quitAction->setMenuRole(QAction::QuitRole);
    aboutAction = new QAction(QIcon(":/icons/bitcoin"), tr("&About ZillionCoin"), this);
    aboutAction->setStatusTip(tr("Show information about ZillionCoin"));
    aboutAction->setMenuRole(QAction::AboutRole);
    aboutQtAction = new QAction(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"), tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show information about Qt"));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
    optionsAction = new QAction(QIcon(":/icons/options"), tr("&Options..."), this);
    optionsAction->setStatusTip(tr("Modify configuration options for ZillionCoin"));
    optionsAction->setMenuRole(QAction::PreferencesRole);
    toggleHideAction = new QAction(QIcon(":/icons/bitcoin"), tr("&Show / Hide"), this);
    toggleHideAction->setStatusTip(tr("Show or hide the main Window"));

    encryptWalletAction = new QAction(QIcon(":/icons/lock_closed"), tr("&Encrypt Wallet..."), this);
    encryptWalletAction->setStatusTip(tr("Encrypt the private keys that belong to your wallet"));
    encryptWalletAction->setCheckable(true);
    backupWalletAction = new QAction(QIcon(":/menu/filesave"), tr("&Backup Wallet..."), this);
    backupWalletAction->setStatusTip(tr("Backup wallet to another location"));
    changePassphraseAction = new QAction(QIcon(":/icons/key"), tr("&Change Passphrase..."), this);
    changePassphraseAction->setStatusTip(tr("Change the passphrase used for wallet encryption"));
    signMessageAction = new QAction(QIcon(":/menu/sign"), tr("Sign &message..."), this);
    signMessageAction->setStatusTip(tr("Sign messages with your ZillionCoin addresses to prove you own them"));
    verifyMessageAction = new QAction(QIcon(":/menu/verify"), tr("&Verify message..."), this);
    verifyMessageAction->setStatusTip(tr("Verify messages to ensure they were signed with specified ZillionCoin addresses"));

    openRPCConsoleAction = new QAction(QIcon(":/icons/console"), tr("&Debug Console"), this);
    openRPCConsoleAction->setStatusTip(tr("Open debugging and diagnostic console"));

    openInfoAction = new QAction(QIcon(":/icons/info"), tr("&Information"), this);
    openInfoAction->setStatusTip(tr("Show debugging and diagnostic information"));

    openBlockExplorerAction = new QAction(QIcon(":/icons/explorer"), tr("Blockchain Explorer"), this);
    openBlockExplorerAction->setStatusTip(tr("Open blockchain explorer"));

    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutClicked()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(optionsAction, SIGNAL(triggered()), this, SLOT(optionsClicked()));
    connect(toggleHideAction, SIGNAL(triggered()), this, SLOT(toggleHidden()));
    connect(encryptWalletAction, SIGNAL(triggered(bool)), walletFrame, SLOT(encryptWallet(bool)));
    connect(backupWalletAction, SIGNAL(triggered()), walletFrame, SLOT(backupWallet()));
    connect(changePassphraseAction, SIGNAL(triggered()), walletFrame, SLOT(changePassphrase()));
    connect(signMessageAction, SIGNAL(triggered()), this, SLOT(gotoSignMessageTab()));
    connect(verifyMessageAction, SIGNAL(triggered()), this, SLOT(gotoVerifyMessageTab()));


    // Configure the menus
    QMenu *file = appMenuBar->addMenu(tr("&File"));
    file->addAction(backupWalletAction);
    file->addAction(signMessageAction);
    file->addAction(verifyMessageAction);
    file->addSeparator();
    file->addAction(quitAction);

    QMenu *settings = appMenuBar->addMenu(tr("&Settings"));
    settings->addAction(encryptWalletAction);
    settings->addAction(changePassphraseAction);
    settings->addSeparator();
    settings->addAction(optionsAction);

    QMenu *tools = appMenuBar->addMenu(tr("&Tools"));
    tools->addAction(openInfoAction);
    tools->addAction(openRPCConsoleAction);
    tools->addAction(openBlockExplorerAction);

    QMenu *help = appMenuBar->addMenu(tr("&About"));
    help->addAction(aboutAction);
    help->addAction(aboutQtAction);

}

void BitcoinGUI::setClientModel(ClientModel *clientModel)
{
    this->clientModel = clientModel;
    if(clientModel)
    {
        // Replace some strings and icons, when using the testnet
        if(clientModel->isTestNet())
        {
            setWindowTitle(windowTitle() + QString(" ") + tr("[testnet]"));
#ifndef Q_OS_MAC
            QApplication::setWindowIcon(QIcon(":icons/bitcoin_testnet"));
            setWindowIcon(QIcon(":icons/bitcoin_testnet"));
#else
            MacDockIconHandler::instance()->setIcon(QIcon(":icons/bitcoin_testnet"));
#endif
            if(trayIcon)
            {
                // Just attach " [testnet]" to the existing tooltip
                trayIcon->setToolTip(trayIcon->toolTip() + QString(" ") + tr("[testnet]"));
                trayIcon->setIcon(QIcon(":/icons/toolbar_testnet"));
            }

            toggleHideAction->setIcon(QIcon(":/icons/toolbar_testnet"));
            aboutAction->setIcon(QIcon(":/icons/toolbar_testnet"));
        }

        // Create system tray menu (or setup the dock menu) that late to prevent users from calling actions,
        // while the client has not yet fully loaded
        createTrayIconMenu();

        // Keep up to date with client
        setNumConnections(clientModel->getNumConnections());
        connect(clientModel, SIGNAL(numConnectionsChanged(int)), this, SLOT(setNumConnections(int)));

        setNumBlocks(clientModel->getNumBlocks(), clientModel->getNumBlocksOfPeers());
        connect(clientModel, SIGNAL(numBlocksChanged(int,int)), this, SLOT(setNumBlocks(int,int)));

        // Receive and report messages from network/worker thread
        connect(clientModel, SIGNAL(message(QString,QString,unsigned int)), this, SLOT(message(QString,QString,unsigned int)));

        rpcConsole->setClientModel(clientModel);
        walletFrame->setClientModel(clientModel);
    }
}

bool BitcoinGUI::addWallet(const QString& name, WalletModel *walletModel)
{
    setWalletActionsEnabled(true);
    return walletFrame->addWallet(name, walletModel);
}

bool BitcoinGUI::setCurrentWallet(const QString& name)
{
    return walletFrame->setCurrentWallet(name);
}

void BitcoinGUI::removeAllWallets()
{
    setWalletActionsEnabled(false);
    walletFrame->removeAllWallets();
}

void BitcoinGUI::setWalletActionsEnabled(bool enabled)
{
    //    //overviewAction->setEnabled(enabled);
    //    //sendCoinsAction->setEnabled(enabled);
    //    //receiveCoinsAction->setEnabled(enabled);
    //    //historyAction->setEnabled(enabled);
    //    encryptWalletAction->setEnabled(enabled);
    //    //backupWalletAction->setEnabled(enabled);
    //    changePassphraseAction->setEnabled(enabled);
    //    signMessageAction->setEnabled(enabled);
    //    verifyMessageAction->setEnabled(enabled);
    //    //addressBookAction->setEnabled(enabled);
}

void BitcoinGUI::createTrayIcon()
{
#ifndef Q_OS_MAC
    trayIcon = new QSystemTrayIcon(this);

    trayIcon->setToolTip(tr("ZillionCoin client"));
    trayIcon->setIcon(QIcon(":/icons/toolbar"));
    trayIcon->show();
#endif

    notificator = new Notificator(QApplication::applicationName(), trayIcon);
}

void BitcoinGUI::createTrayIconMenu()
{
    QMenu *trayIconMenu;
#ifndef Q_OS_MAC
    // return if trayIcon is unset (only on non-Mac OSes)
    if (!trayIcon)
        return;

    trayIconMenu = new QMenu(this);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
#else
    // Note: On Mac, the dock icon is used to provide the tray's functionality.
    MacDockIconHandler *dockIconHandler = MacDockIconHandler::instance();
    dockIconHandler->setMainWindow((QMainWindow *)this);
    trayIconMenu = dockIconHandler->dockMenu();
#endif

    // Configuration of the tray icon (or dock icon) icon menu
    trayIconMenu->addAction(toggleHideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(optionsAction);
    trayIconMenu->addAction(openInfoAction);
    trayIconMenu->addAction(openRPCConsoleAction);
    trayIconMenu->addAction(openBlockExplorerAction);
#ifndef Q_OS_MAC // This is built-in on Mac
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
#endif
}

#ifndef Q_OS_MAC
void BitcoinGUI::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger)
    {
        // Click on system tray icon triggers show/hide of the main window
        toggleHideAction->trigger();
    }
}
#endif

void BitcoinGUI::saveWindowGeometry()
{
    QSettings settings;
    settings.setValue("nWindowPos", pos());
    settings.setValue("nWindowSize", size());
}

void BitcoinGUI::restoreWindowGeometry()
{
    QSettings settings;
    QPoint pos = settings.value("nWindowPos").toPoint();
    QSize size = settings.value("nWindowSize", QSize(850, 550)).toSize();
    if (!pos.x() && !pos.y())
    {
        QRect screen = QApplication::desktop()->screenGeometry();
        pos.setX((screen.width()-size.width())/2);
        pos.setY((screen.height()-size.height())/2);
    }
    resize(size);
    move(pos);
}

void BitcoinGUI::optionsClicked()
{
    if(!clientModel || !clientModel->getOptionsModel())
        return;
    OptionsDialog dlg;
    dlg.setModel(clientModel->getOptionsModel());
    dlg.exec();
}

void BitcoinGUI::whichTabWasClicked(int i)
{
    if(i == 0){
        gotoSendCoinsPage();
    }
    if(i == 1){
        gotoMiningInfoPage();
    }
}

void BitcoinGUI::displayTickerChanged(bool b)
{
    qDebug() << "displayTickerChanged ---------------------------------" << b;
    b ? ticker_enabled = 1: ticker_enabled = -1;
    /*update();
    repaint();
    qApp->processEvents();
    updateGeometry();*/
    qDebug() << this->size();
    QSize size_ = this->size();
    adjustSize();
    resize(size_);
    qDebug() << "......" << this->sizeHint();


}

void BitcoinGUI::aboutClicked()
{
    AboutDialog dlg;
    dlg.setModel(clientModel);
    dlg.exec();
}

/*void BitcoinGUI::setWalletCategoryChecked(bool state){
    //walletCategory->setChecked(state);
}

void BitcoinGUI::setMiningCategoryChecked(bool state){
    //miningCategory->setChecked(state);
}*/

//void BitcoinGUI::fadeWalletButtons(QString way)
//{
//    /*QPropertyAnimation *animation = new QPropertyAnimation(walletButtonContainer, "pos");
//    animation->setDuration(0);
//    animation->setStartValue(QPoint(10, walletButtonContainer->y()));
//    if(way == "in"){
//        animation->setEndValue(QPoint(10,125+MENU_OFFSET+45));
//    } else{
//        animation->setEndValue(QPoint(10,125+MENU_OFFSET-50));
//    }
//    animation->setEasingCurve(QEasingCurve::InCubic);
//    animation->start();*/
//}

//void BitcoinGUI::fadeMiningButtons(QString way)
//{
//    /*QPropertyAnimation *animation2 = new QPropertyAnimation(miningButtonContainer, "pos");
//    animation2->setDuration(0);
//    animation2->setStartValue(QPoint(10, miningButtonContainer->y()));
//    if(way == "in"){
//        animation2->setEndValue(QPoint(10,125+MENU_OFFSET+45));
//    } else{
//        animation2->setEndValue(QPoint(10,125+MENU_OFFSET-50));
//    }
//    animation2->setEasingCurve(QEasingCurve::InCubic);
//    animation2->start();*/
//}

void BitcoinGUI::stretchStack(){
    /*if(screenId == 0){
        walletFrame->stretchStack(10,125,this->width()-105,this->height()-10-125-40);
    } else{*/

    //walletFrame->stretchStack(10,0,this->width()-10-10,this->height()-10-125-40-40-60);
    walletFrame->resize(this->width()-10-180-10,this->height()-10-125-40-40-50-12+90-40);
    walletFrame->stretchStack(10,0,this->width()-10-10-180-10,this->height()-10-125-40-40-50-12+90-40);
    //}

}

void BitcoinGUI::externCommand(const QString &command)
{
    rpcConsole->externCommand(command);
}

void BitcoinGUI::setWalletModel(WalletModel *model)
{
    this->walletModel = model;

   if(model && model->getOptionsModel()){
       connect(model->getOptionsModel(), SIGNAL(displayTickerChanged(bool)), this, SLOT(displayTickerChanged(bool)));
        qDebug() << "model->getOptionsModel()->getDisplayTicker()" << model->getOptionsModel()->getDisplayTicker();
        displayTickerChanged(model->getOptionsModel()->getDisplayTicker());
   }
}

void BitcoinGUI::gotoOverviewPage()
{
    screenId = 0;
    //fadeWalletButtons("out");
    //fadeMiningButtons("out");
    stretchStack();
    if (walletFrame) walletFrame->gotoOverviewPage();
}

void BitcoinGUI::gotoHistoryPage()
{
    screenId = 1;
    //fadeWalletButtons("in");
    //fadeMiningButtons("out");
    stretchStack();
    if (walletFrame) walletFrame->gotoHistoryPage();
    zillionTab->setCurrentIndex(0);
}

void BitcoinGUI::gotoAddressBookPage()
{
    screenId = 1;
    //fadeWalletButtons("in");
    //fadeMiningButtons("out");
    stretchStack();
    if (walletFrame) walletFrame->gotoAddressBookPage();
    zillionTab->setCurrentIndex(0);
}

void BitcoinGUI::gotoVanityGenPage()
{
    screenId = 1;
    //fadeWalletButtons("in");
    //fadeMiningButtons("out");
    stretchStack();
    if (walletFrame) walletFrame->gotoVanityGenPage();
    zillionTab->setCurrentIndex(0);
}

void BitcoinGUI::gotoReceiveCoinsPage()
{
    if (walletFrame) walletFrame->gotoReceiveCoinsPage();
}

void BitcoinGUI::gotoSendCoinsPage(QString addr)
{
    screenId = 1;
    //fadeWalletButtons("in");
    //fadeMiningButtons("out");
    stretchStack();
    if (walletFrame) walletFrame->gotoSendCoinsPage(addr);
    zillionTab->setCurrentIndex(0);
}

void BitcoinGUI::gotoSignMessageTab(QString addr)
{
    if (walletFrame) walletFrame->gotoSignMessageTab(addr);
}

void BitcoinGUI::gotoVerifyMessageTab(QString addr)
{
    if (walletFrame) walletFrame->gotoVerifyMessageTab(addr);
}

void BitcoinGUI::gotoMiningInfoPage()
{
    screenId = 2;
    //fadeWalletButtons("out");
    //fadeMiningButtons("in");

    stretchStack();
    if (walletFrame) walletFrame->gotoMiningInfoPage();

    zillionTab->setCurrentIndex(1);
}

void BitcoinGUI::gotoMiningPage()
{
    if (walletFrame) walletFrame->gotoMiningPage();
}

void BitcoinGUI::gotoMarketCapPage()
{
    if (walletFrame) walletFrame->gotoMarketCapPage();
}

void BitcoinGUI::gotoZillionGridPage()
{
    if (walletFrame) walletFrame->gotoZillionGridPage();
}

void BitcoinGUI::setNumConnections(int count)
{
    QString icon;
    switch(count)
    {
    case 0: icon = ":/icons/connect_0"; break;
    case 1: case 2: case 3: icon = ":/icons/connect_1"; break;
    case 4: case 5: case 6: icon = ":/icons/connect_2"; break;
    case 7: case 8: case 9: icon = ":/icons/connect_3"; break;
    default: icon = ":/icons/connect_4"; break;
    }
    labelConnectionsIcon->setPixmap(QIcon(icon).pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
    labelConnectionsIcon->setToolTip(tr("%n active connection(s) to ZillionCoin network", "", count));
}

void BitcoinGUI::setNumBlocks(int count, int nTotalBlocks)
{
    // Prevent orphan statusbar messages (e.g. hover Quit in main menu, wait until chain-sync starts -> garbelled text)
    statusBar()->clearMessage();

    // if(fDebug){ printf("NumBlocks: %d : %d\n", count, nTotalBlocks); }
    if(GetBoolArg("-chart", true) && count > 0 && nTotalBlocks > 0)
    {
        walletFrame->updatePlot();
    }

    // Acquire current block source
    enum BlockSource blockSource = clientModel->getBlockSource();

    //qDebug() << "count: " << count << "totalBlocks: " << nTotalBlocks;

    switch (blockSource) {

    case BLOCK_SOURCE_NETWORK:
        progressBarLabel->setText(tr("Synchronizing with network..."));
        break;
    case BLOCK_SOURCE_DISK:
        progressBarLabel->setText(tr("Importing blocks from disk..."));
        break;
    case BLOCK_SOURCE_REINDEX:
        progressBarLabel->setText(tr("Reindexing blocks on disk..."));
        break;
    case BLOCK_SOURCE_NONE:
        // Case: not Importing, not Reindexing and no network connection
        progressBarLabel->setText(tr("No block source available..."));
        break;
    }

    QString tooltip;

    QDateTime lastBlockDate = clientModel->getLastBlockDate();
    QDateTime currentDate = QDateTime::currentDateTime();
    int secs = lastBlockDate.secsTo(currentDate);

    if(count < nTotalBlocks)
    {
        tooltip = tr("Processed %1 of %2 (estimated) blocks of transaction history.").arg(count).arg(nTotalBlocks);
    }
    else
    {
        tooltip = tr("Processed %1 blocks of transaction history.").arg(count);
    }

    // Set icon state: spinning if catching up, tick otherwise
    if(secs < 90*60 && count >= nTotalBlocks)
    {
        tooltip = tr("Up to date") + QString(".<br>") + tooltip;
        labelBlocksIcon->setPixmap(QIcon(":/icons/synced").pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));

        walletFrame->showOutOfSyncWarning(false);

        progressBarLabel->setVisible(false);
        progressBar->setVisible(false);
    }
    else
    {
        // Represent time from last generated block in human readable text
        QString timeBehindText;
        if(secs < 48*60*60)
        {
            timeBehindText = tr("%n hour(s)","",secs/(60*60));
        }
        else if(secs < 14*24*60*60)
        {
            timeBehindText = tr("%n day(s)","",secs/(24*60*60));
        }
        else
        {
            timeBehindText = tr("%n week(s)","",secs/(7*24*60*60));
        }

        progressBarLabel->setVisible(true);
        progressBar->setFormat(tr("%1 behind").arg(timeBehindText));
        progressBar->setMaximum(1000000000);
        progressBar->setValue(clientModel->getVerificationProgress() * 1000000000.0 + 0.5);
        progressBar->setVisible(true);

        tooltip = tr("Catching up...") + QString("<br>") + tooltip;
        labelBlocksIcon->setMovie(syncIconMovie);
        if(count != prevBlocks)
            syncIconMovie->jumpToNextFrame();
        prevBlocks = count;

        walletFrame->showOutOfSyncWarning(true);

        tooltip += QString("<br>");
        tooltip += tr("Last received block was generated %1 ago.").arg(timeBehindText);
        tooltip += QString("<br>");
        tooltip += tr("Transactions after this will not yet be visible.");
    }

    // Don't word-wrap this (fixed-width) tooltip
    tooltip = QString("<nobr>") + tooltip + QString("</nobr>");

    labelBlocksIcon->setToolTip(tooltip);
    progressBarLabel->setToolTip(tooltip);
    progressBar->setToolTip(tooltip);
}

void BitcoinGUI::message(const QString &title, const QString &message, unsigned int style, bool *ret)
{
    QString strTitle = tr("ZillionCoin"); // default title
    // Default to information icon
    int nMBoxIcon = QMessageBox::Information;
    int nNotifyIcon = Notificator::Information;

    // Override title based on style
    QString msgType;
    switch (style) {
    case CClientUIInterface::MSG_ERROR:
        msgType = tr("Error");
        break;
    case CClientUIInterface::MSG_WARNING:
        msgType = tr("Warning");
        break;
    case CClientUIInterface::MSG_INFORMATION:
        msgType = tr("Information");
        break;
    default:
        msgType = title; // Use supplied title
    }
    if (!msgType.isEmpty())
        strTitle += " - " + msgType;

    // Check for error/warning icon
    if (style & CClientUIInterface::ICON_ERROR) {
        nMBoxIcon = QMessageBox::Critical;
        nNotifyIcon = Notificator::Critical;
    }
    else if (style & CClientUIInterface::ICON_WARNING) {
        nMBoxIcon = QMessageBox::Warning;
        nNotifyIcon = Notificator::Warning;
    }

    // Display message
    if (style & CClientUIInterface::MODAL) {
        // Check for buttons, use OK as default, if none was supplied
        QMessageBox::StandardButton buttons;
        if (!(buttons = (QMessageBox::StandardButton)(style & CClientUIInterface::BTN_MASK)))
            buttons = QMessageBox::Ok;

        QMessageBox mBox((QMessageBox::Icon)nMBoxIcon, strTitle, message, buttons);
        int r = mBox.exec();
        if (ret != NULL)
            *ret = r == QMessageBox::Ok;
    }
    else
        notificator->notify((Notificator::Class)nNotifyIcon, strTitle, message);
}

void BitcoinGUI::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
#ifndef Q_OS_MAC // Ignored on Mac
    if(e->type() == QEvent::WindowStateChange)
    {
        if(clientModel && clientModel->getOptionsModel()->getMinimizeToTray())
        {
            QWindowStateChangeEvent *wsevt = static_cast<QWindowStateChangeEvent*>(e);
            if(!(wsevt->oldState() & Qt::WindowMinimized) && isMinimized())
            {
                QTimer::singleShot(0, this, SLOT(hide()));
                e->ignore();
            }
        }
    }
#endif
}

void BitcoinGUI::closeEvent(QCloseEvent *event)
{
    if(clientModel)
    {
#ifndef Q_OS_MAC // Ignored on Mac
        if(!clientModel->getOptionsModel()->getMinimizeToTray() &&
                !clientModel->getOptionsModel()->getMinimizeOnClose())
        {
            QApplication::quit();
        }
#endif
    }
    QMainWindow::closeEvent(event);
}

void BitcoinGUI::askFee(qint64 nFeeRequired, bool *payFee)
{
    QString strMessage = tr("This transaction is over the size limit. You can still send it for a fee of %1, "
                            "which goes to the nodes that process your transaction and helps to support the network. "
                            "Do you want to pay the fee?").arg(BitcoinUnits::formatWithUnit(BitcoinUnits::BTC, nFeeRequired));
    QMessageBox::StandardButton retval = QMessageBox::question(
                this, tr("Confirm transaction fee"), strMessage,
                QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes);
    *payFee = (retval == QMessageBox::Yes);
}

void BitcoinGUI::incomingTransaction(const QString& date, int unit, qint64 amount, const QString& type, const QString& address)
{
    // On new transaction, make an info balloon
    message((amount)<0 ? tr("Sent transaction") : tr("Incoming transaction"),
            tr("Date: %1\n"
               "Amount: %2\n"
               "Type: %3\n"
               "Address: %4\n")
            .arg(date)
            .arg(BitcoinUnits::formatWithUnit(unit, amount, true))
            .arg(type)
            .arg(address), CClientUIInterface::MSG_INFORMATION);
}


void BitcoinGUI::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept only URIs
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void BitcoinGUI::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        int nValidUrisFound = 0;
        QList<QUrl> uris = event->mimeData()->urls();
        foreach(const QUrl &uri, uris)
        {
            if (walletFrame->handleURI(uri.toString()))
                nValidUrisFound++;
        }

        // if valid URIs were found
        if (nValidUrisFound)
            walletFrame->gotoSendCoinsPage();
        else
            message(tr("URI handling"), tr("URI can not be parsed! This can be caused by an invalid ZillionCoin address or malformed URI parameters."),
                    CClientUIInterface::ICON_WARNING);
    }

    event->acceptProposedAction();
}

bool BitcoinGUI::eventFilter(QObject *object, QEvent *event)
{
    // Catch status tip events
    if (event->type() == QEvent::StatusTip)
    {
        // Prevent adding text from setStatusTip(), if we currently use the status bar for displaying other stuff
        if (progressBarLabel->isVisible() || progressBar->isVisible())
            return true;
    }
    return QMainWindow::eventFilter(object, event);
}

void BitcoinGUI::resizeEvent(QResizeEvent *event)
{
    //Pos check

    if(ticker_enabled == -1){
        guiHeader->move(0,0+MENU_OFFSET);
        tickerHeader->setVisible(false);
        zillionTab->move(4,4+120+MENU_OFFSET);
        walletFrame->move(180+4,4+160+MENU_OFFSET);//+90);
        sideBarContainer->move(0,-10+4+160+MENU_OFFSET);//+90);
    }else{
        guiHeader->move(0,20+MENU_OFFSET);
        zillionTab->move(4,4+120+20+MENU_OFFSET);
        walletFrame->move(180+4,4+160+20+MENU_OFFSET);//+90);
        sideBarContainer->move(0,-10+4+160+20+MENU_OFFSET);//+90);
        tickerHeader->setVisible(true);
    }

    tickerHeader->resize(this->width(),20);
    guiHeader->resize(this->width());

    //qDebug() << tickerHeader->height() << guiHeader->height();


    zillionTab->resize(this->width()-8,this->height()-180+3);

    //separatorLineLeft->setFixedHeight(walletFrame->y()+walletFrame->height()-120);
    //separatorLineBottom->setFixedWidth(this->width());//walletFrame->width());
    //separatorLineBottom->move(0,zillionTab->y()+zillionTab->height()+5);

    walletFrame->resize(this->width()-10-180-10,this->height()-10-125-40-40-50-12+90-40);
    walletFrame->stretchStack(10,0,this->width()-10-10-180-10,this->height()-10-125-40-40-50-12+90-40);
    //walletFrame->resizeIt();

    backGround->resize(this->width(),this->height());

}

void BitcoinGUI::handleURI(QString strURI)
{
    // URI has to be valid
    if (!walletFrame->handleURI(strURI))
        message(tr("URI handling"), tr("URI can not be parsed! This can be caused by an invalid ZillionCoin address or malformed URI parameters."),
                CClientUIInterface::ICON_WARNING);
}

void BitcoinGUI::setEncryptionStatus(int status)
{
    switch(status)
    {
    case WalletModel::Unencrypted:
        labelEncryptionIcon->hide();
        encryptWalletAction->setChecked(false);
        changePassphraseAction->setEnabled(false);
        encryptWalletAction->setEnabled(true);
        break;
    case WalletModel::Unlocked:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(":/icons/lock_open").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>unlocked</b>"));
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    case WalletModel::Locked:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(":/icons/lock_closed").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>locked</b>"));
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    }
}

void BitcoinGUI::showNormalIfMinimized(bool fToggleHidden)
{
    // activateWindow() (sometimes) helps with keyboard focus on Windows
    if (isHidden())
    {
        show();
        activateWindow();
    }
    else if (isMinimized())
    {
        showNormal();
        activateWindow();
    }
    else if (GUIUtil::isObscured(this))
    {
        raise();
        activateWindow();
    }
    else if(fToggleHidden)
        hide();
}

void BitcoinGUI::toggleHidden()
{
    showNormalIfMinimized(true);
}

void BitcoinGUI::detectShutdown()
{
    if (ShutdownRequested())
        QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
}







void BitcoinGUI::menuClicked(int index, int index2)
{
    //qDebug() << "menuClicked" << index << index2;

    QPixmap pix;

    int offset_y = 10;

    active_main_menu = index;

    for(int i = 0;i<mainMenuIdentifier.length();i++){
        mainMenu[i]->moveTo(0,offset_y,200,100,QEasingCurve::InQuad);
        offset_y += 50 + 10;
        if(i == active_main_menu){
            for(int i2 = 0;i2<subMenuIdentifier[i].length();i2++){
                //subMenuTextLabel[i][i2]->adjustSize();
                if(active_main_menu != active_main_menu_old){
                    subMenu[i][i2]->move(-200,offset_y);
                    subMenu[i][i2]->moveTo(0,subMenu[i][i2]->y(),200,200+i2*30,QEasingCurve::OutQuad);
                } else{
                    subMenu[i][i2]->move(0,offset_y);
                }
                offset_y += 35;
            }
            if(subMenuIdentifier[i].length() >= 1){
                offset_y += 10;
            }
        } else{
            for(int i2 = 0;i2<subMenuIdentifier[i].length();i2++){
                subMenu[i][i2]->moveTo(-200,subMenu[i][i2]->y(),200,((subMenuIdentifier[i].length()-1)*30) -(i2*30),QEasingCurve::InQuad);
            }
        }
    }

    for(int i = 0;i<mainMenuIdentifier.length();i++){
        for(int i2 = 0;i2<subMenuIdentifier[i].length();i2++){
            /* qDebug() << i << i2;
            if(i == index && i2 != index2){
                qDebug() << "fade this" << i << i2;*/
            subMenu[i][i2]->setChecked(false);
            //subMenu[i][i2]->fadeTo(0,100,0);
            //subMenu[i][i2]->update();
            pix = QPixmap(subMenuIdentifier[i][i2].icon_path);
            pix = PixmapEffect::tint(pix,0x1F1F1F);
            subMenuIconLabel[i][i2]->setPixmap( pix.scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            subMenuTextLabel[i][i2]->setStyleSheet(".QLabel{color:#1F1F1F; font: bold 14px \"Open Sans\";}");
            //}
        }
    }

    if(index2 == -1){
        index2 = 0;
    }

    if(index2 >= 0){
        subMenu[index][index2]->setChecked(true);
        pix = QPixmap(subMenuIdentifier[index][index2].icon_path);
        pix = PixmapEffect::tint(pix,0xda8d21);
        subMenuIconLabel[index][index2]->setPixmap( pix.scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        subMenuTextLabel[index][index2]->setStyleSheet(".QLabel{color:#da8d21; font: bold 14px \"Open Sans\";}");

        if(index == 0){
            if(index2 == 0){
                gotoSendCoinsPage();
            }
            if(index2 == 1){
                gotoReceiveCoinsPage();
            }
            if(index2 == 2){
                gotoHistoryPage();
            }
            if(index2 == 3){
                gotoAddressBookPage();
            }
            if(index2 == 4){
                gotoVanityGenPage();
            }
            if(index2 == 5){
                walletFrame->backupWallet();
            }
        }

        if(index == 1){
            if(index2 == 0){
                gotoMiningInfoPage();
            }
            if(index2 == 1){
                gotoMiningPage();
            }
        }

        if(index == 2){
            if(index2 == 0){
                gotoMarketCapPage();
            }
        }
        if(index == 3){
            if(index2 == 0){
                gotoZillionGridPage();
            }
        }
    }

    active_main_menu_old = active_main_menu;
}

void BitcoinGUI::menuRollover(int index, int index2)
{
    QPixmap pix;
    for(int i2 = 0;i2<subMenuIdentifier[index].length();i2++){
        pix = QPixmap(subMenuIdentifier[index][i2].icon_path);
        if(i2 == index2){
            pix = PixmapEffect::tint(pix,0xda8d21);
            subMenuIconLabel[index][i2]->setPixmap( pix.scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            subMenuTextLabel[index][i2]->setStyleSheet(".QLabel{color:#da8d21; font: bold 14px \"Open Sans\";}");
        } else{
            if(!subMenu[index][i2]->checked){
                pix = PixmapEffect::tint(pix,0x1F1F1F);
                subMenuIconLabel[index][i2]->setPixmap( pix.scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                subMenuTextLabel[index][i2]->setStyleSheet(".QLabel{color:#1F1F1F; font: bold 14px \"Open Sans\";}");
            }
        }
    }
}

void BitcoinGUI::menuRollout(int index, int index2)
{
    if(!subMenu[index][index2]->checked){
        QPixmap pix = QPixmap(subMenuIdentifier[index][index2].icon_path);
        pix = PixmapEffect::tint(pix,0x1F1F1F);
        subMenuIconLabel[index][index2]->setPixmap( pix.scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        subMenuTextLabel[index][index2]->setStyleSheet(".QLabel{color:#1F1F1F; font: bold 14px \"Open Sans\";}");
    }
}

